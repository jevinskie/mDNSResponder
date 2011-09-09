/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@

    Change History (most recent first):
    
$Log: Service.c,v $
Revision 1.1  2004/01/30 02:58:39  bradley
mDNSResponder Windows Service. Provides global Rendezvous support with an IPC interface.

*/

#include	<stdio.h>
#include	<stdlib.h>

#define	_WIN32_WINNT		0x0400

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSDDirect.h"
#include	"RMxServer.h"

#include	"Resource.h"

#include	"mDNSClientAPI.h"

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME					"[Server] "
#define	kServiceName				"Rendezvous"
#define	kServiceDependencies		"Tcpip\0winmgmt\0\0"

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

int 				main( int argc, char *argv[] );
static void			Usage( void );
static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent );
static OSStatus		InstallService( const char *inName, const char *inDisplayName, const char *inDescription, const char *inPath );
static OSStatus		RemoveService( const char *inName );
static OSStatus		SetServiceDescription( SC_HANDLE inSCM, const char *inServiceName, const char *inDescription );
static void			ReportStatus( int inType, const char *inFormat, ... );
static OSStatus		RunDirect( int argc, char *argv[] );

static void WINAPI	ServiceMain( DWORD argc, LPSTR argv[] );
static OSStatus		ServiceSetupEventLogging( void );
static void WINAPI	ServiceControlHandler( DWORD inControl );

static OSStatus		ServiceRun( int argc, char *argv[] );
static void			ServiceStop( void );

static OSStatus		ServiceSpecificInitialize( int argc, char *argv[] );
static OSStatus		ServiceSpecificRun( int argc, char *argv[] );
static OSStatus		ServiceSpecificStop( void );
static void			ServiceSpecificFinalize( int argc, char *argv[] );

#include	"mDNSClientAPI.h"

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

DEBUG_LOCAL BOOL						gServiceQuietMode		= FALSE;
DEBUG_LOCAL SERVICE_TABLE_ENTRY			gServiceDispatchTable[] = 
{
	{ kServiceName,	ServiceMain }, 
	{ NULL, 		NULL }
};
DEBUG_LOCAL SERVICE_STATUS				gServiceStatus;
DEBUG_LOCAL SERVICE_STATUS_HANDLE		gServiceStatusHandle 	= NULL;
DEBUG_LOCAL HANDLE						gServiceEventSource		= NULL;
DEBUG_LOCAL bool						gServiceAllowRemote		= false;
DEBUG_LOCAL int							gServiceCacheEntryCount	= 0;	// 0 means to use the DNS-SD default.

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	main
//===========================================================================================================================

int	main( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			ok;
	BOOL			start;
	int				i;
	
	debug_initialize( kDebugOutputTypeMetaConsole );
	debug_set_property( kDebugPropertyTagPrintLevel, kDebugLevelVerbose );
	
	// Install a Console Control Handler to handle things like control-c signals.
	
	ok = SetConsoleCtrlHandler( ConsoleControlHandler, TRUE );
	err = translate_errno( ok, (OSStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	// Default to automatically starting the service dispatcher if no extra arguments are specified.
	
	start = ( argc <= 1 );
	
	// Parse arguments.
	
	for( i = 1; i < argc; ++i )
	{
		if( strcmp( argv[ i ], "-install" ) == 0 )			// Install
		{
			char		desc[ 256 ];
			
			desc[ 0 ] = 0;
			LoadStringA( GetModuleHandle( NULL ), IDS_SERVICE_DESCRIPTION, desc, sizeof( desc ) );
			err = InstallService( kServiceName, kServiceName, desc, argv[ 0 ] );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "install service failed (%d)\n", err );
				goto exit;
			}
		}
		else if( strcmp( argv[ i ], "-remove" ) == 0 )		// Remove
		{
			err = RemoveService( kServiceName );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "remove service failed (%d)\n", err );
				goto exit;
			}
		}
		else if( strcmp( argv[ i ], "-start" ) == 0 )		// Start
		{
			start = TRUE;
		}
		else if( strcmp( argv[ i ], "-server" ) == 0 )		// Server
		{
			err = RunDirect( argc, argv );
			if( err )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "run service directly failed (%d)\n", err );
			}
			goto exit;
		}
		else if( strcmp( argv[ i ], "-q" ) == 0 )			// Quiet Mode (toggle)
		{
			gServiceQuietMode = !gServiceQuietMode;
		}
		else if( strcmp( argv[ i ], "-remote" ) == 0 )		// Allow Remote Connections
		{
			gServiceAllowRemote = true;
		}
		else if( strcmp( argv[ i ], "-cache" ) == 0 )		// Number of mDNS cache entries
		{
			if( i <= argc )
			{
				ReportStatus( EVENTLOG_ERROR_TYPE, "-cache used, but number of cache entries not specified\n" );
				err = kParamErr;
				goto exit;
			}
			gServiceCacheEntryCount = atoi( argv[ ++i ] );
		}
		else if( ( strcmp( argv[ i ], "-help" ) == 0 ) || 	// Help
				 ( strcmp( argv[ i ], "-h" ) == 0 ) )
		{
			Usage();
			err = 0;
			break;
		}
		else
		{
			Usage();
			err = kParamErr;
			break;
		}
	}
	
	// Start the service dispatcher if requested. This does not return until all services have terminated. If any 
	// global initialization is needed, it should be done before starting the service dispatcher, but only if it 
	// will take less than 30 seconds. Otherwise, use a separate thread for it and start the dispatcher immediately.
	
	if( start )
	{
		ok = StartServiceCtrlDispatcher( gServiceDispatchTable );
		err = translate_errno( ok, (OSStatus) GetLastError(), kInUseErr );
		if( err != kNoErr )
		{
			ReportStatus( EVENTLOG_ERROR_TYPE, "start service dispatcher failed (%d)\n", err );
			goto exit;
		}
	}
	err = 0;
	
exit:
	dlog( kDebugLevelTrace, DEBUG_NAME "exited (%d %m)\n", err, err );
	return( (int) err );
}

//===========================================================================================================================
//	Usage
//===========================================================================================================================

static void	Usage( void )
{
	fprintf( stderr, "\n" );
	fprintf( stderr, "mDNSResponder 1.0d1\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "    <no args>    Runs the service normally\n" );
	fprintf( stderr, "    -install     Creates the service and starts it\n" );
	fprintf( stderr, "    -remove      Stops the service and deletes it\n" );
	fprintf( stderr, "    -start       Starts the service dispatcher after processing all other arguments\n" );
	fprintf( stderr, "    -server      Runs the service directly as a server (for debugging)\n" );
	fprintf( stderr, "    -q           Toggles Quiet Mode (no events or output)\n" );
	fprintf( stderr, "    -remote      Allow remote connections\n" );
	fprintf( stderr, "    -cache n     Number of mDNS cache entries (defaults to %d)\n", kDNSServiceCacheEntryCountDefault );
	fprintf( stderr, "    -h[elp]      Display Help/Usage\n" );
	fprintf( stderr, "\n" );
}

//===========================================================================================================================
//	ConsoleControlHandler
//===========================================================================================================================

static BOOL WINAPI	ConsoleControlHandler( DWORD inControlEvent )
{
	BOOL			handled;
	OSStatus		err;
	
	handled = FALSE;
	switch( inControlEvent )
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			err = ServiceSpecificStop();
			require_noerr( err, exit );
			
			handled = TRUE;
			break;
		
		default:
			break;
	}
	
exit:
	return( handled );
}

//===========================================================================================================================
//	InstallService
//===========================================================================================================================

static OSStatus	InstallService( const char *inName, const char *inDisplayName, const char *inDescription, const char *inPath )
{
	OSStatus		err;
	SC_HANDLE		scm;
	SC_HANDLE		service;
	BOOL			ok;
	TCHAR			fullPath[ MAX_PATH ];
	TCHAR *			namePtr;
	DWORD			size;
	
	scm		= NULL;
	service = NULL;
	
	// Get a full path to the executable since a relative path may have been specified.
	
	size = GetFullPathName( inPath, sizeof( fullPath ), fullPath, &namePtr );
	err = translate_errno( size > 0, (OSStatus) GetLastError(), kPathErr );
	require_noerr( err, exit );
	
	// Create the service and start it.
	
	scm = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	err = translate_errno( scm, (OSStatus) GetLastError(), kOpenErr );
	require_noerr( err, exit );
	
	service = CreateService( scm, inName, inDisplayName, SERVICE_ALL_ACCESS, SERVICE_WIN32_SHARE_PROCESS, 
							 SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, inPath, NULL, NULL, kServiceDependencies, 
							 NULL, NULL );
	err = translate_errno( service, (OSStatus) GetLastError(), kDuplicateErr );
	require_noerr( err, exit );
	
	if( inDescription )
	{
		err = SetServiceDescription( scm, inName, inDescription );
		check_noerr( err );
	}
	
	ok = StartService( service, 0, NULL );
	err = translate_errno( ok, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	ReportStatus( EVENTLOG_SUCCESS, "installed service \"%s\"/\"%s\" at \"%s\"\n", inName, inDisplayName, inPath );
	err = kNoErr;
	
exit:
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( scm )
	{
		CloseServiceHandle( scm );
	}
	return( err );
}

//===========================================================================================================================
//	RemoveService
//===========================================================================================================================

static OSStatus	RemoveService( const char *inName )
{
	OSStatus			err;
	SC_HANDLE			scm;
	SC_HANDLE			service;
	BOOL				ok;
	SERVICE_STATUS		status;
	
	scm		= NULL;
	service = NULL;
	
	// Open a connection to the service.
	
	scm = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
	err = translate_errno( scm, (OSStatus) GetLastError(), kOpenErr );
	require_noerr( err, exit );
	
	service = OpenService( scm, inName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE );
	err = translate_errno( service, (OSStatus) GetLastError(), kNotFoundErr );
	require_noerr( err, exit );
	
	// Stop the service, if it is not already stopped, then delete it.
	
	ok = QueryServiceStatus( service, &status );
	err = translate_errno( ok, (OSStatus) GetLastError(), kAuthenticationErr );
	require_noerr( err, exit );
	
	if( status.dwCurrentState != SERVICE_STOPPED )
	{
		ok = ControlService( service, SERVICE_CONTROL_STOP, &status );
		check_translated_errno( ok, (OSStatus) GetLastError(), kAuthenticationErr );
	}
	
	ok = DeleteService( service );
	err = translate_errno( ok, (OSStatus) GetLastError(), kDeletedErr );
	require_noerr( err, exit );
		
	ReportStatus( EVENTLOG_SUCCESS, "Removed service \"%s\"\n", inName );
	err = ERROR_SUCCESS;
	
exit:
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( scm )
	{
		CloseServiceHandle( scm );
	}
	return( err );
}

//===========================================================================================================================
//	SetServiceDescription
//===========================================================================================================================

static OSStatus	SetServiceDescription( SC_HANDLE inSCM, const char *inServiceName, const char *inDescription )
{
	OSStatus				err;
	SC_LOCK					lock;
	SC_HANDLE				service;
	SERVICE_DESCRIPTION		description;
	BOOL					ok;
	
	check( inServiceName );
	check( inDescription );
	
	lock 	= NULL;
	service	= NULL;
	
	// Open the database (if not provided) and lock it to prevent other access while re-configuring.
	
	if( !inSCM )
	{
		inSCM = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
		err = translate_errno( inSCM, (OSStatus) GetLastError(), kOpenErr );
		require_noerr( err, exit );
	}
	
	lock = LockServiceDatabase( inSCM );
	err = translate_errno( lock, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	// Open a handle to the service. 

	service = OpenService( inSCM, inServiceName, SERVICE_CHANGE_CONFIG );
	err = translate_errno( service, (OSStatus) GetLastError(), kNotFoundErr );
	require_noerr( err, exit );
	
	// Change the description.
	
	description.lpDescription = (char *) inDescription;
	ok = ChangeServiceConfig2( service, SERVICE_CONFIG_DESCRIPTION, &description );
	err = translate_errno( ok, (OSStatus) GetLastError(), kParamErr );
	require_noerr( err, exit );
	
	err = ERROR_SUCCESS;
	
exit:
	// Close the service and release the lock.
	
	if( service )
	{
		CloseServiceHandle( service );
	}
	if( lock )
	{
		UnlockServiceDatabase( lock ); 
	}
	return( err );
}

//===========================================================================================================================
//	ReportStatus
//===========================================================================================================================

static void	ReportStatus( int inType, const char *inFormat, ... )
{
	if( !gServiceQuietMode )
	{
		va_list		args;
		
		va_start( args, inFormat );
		if( gServiceEventSource )
		{
			char				s[ 1024 ];
			BOOL				ok;
			const char *		array[ 1 ];
			
			vsprintf( s, inFormat, args );
			array[ 0 ] = s;
			ok = ReportEvent( gServiceEventSource, (WORD) inType, 0, 0x20000001L, NULL, 1, 0, array, NULL );
			check_translated_errno( ok, GetLastError(), kUnknownErr );
		}
		else
		{
			int		n;
			
			n = vfprintf( stderr, inFormat, args );
			check( n >= 0 );
		}
		va_end( args );
	}
}

//===========================================================================================================================
//	RunDirect
//===========================================================================================================================

static OSStatus	RunDirect( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			initialized;
	
	initialized = FALSE;
	
	err = ServiceSpecificInitialize( argc, argv );
	require_noerr( err, exit );
	initialized = TRUE;
	
	// Run the service. This does not return until the service quits or is stopped.
	
	ReportStatus( EVENTLOG_SUCCESS, "Running \"%s\" service directly\n", kServiceName );
	
	err = ServiceSpecificRun( argc, argv );
	require_noerr( err, exit );
	
	// Clean up.
	
exit:
	if( initialized )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	ServiceMain
//===========================================================================================================================

static void WINAPI ServiceMain( DWORD argc, LPSTR argv[] )
{
	OSStatus		err;
	BOOL			ok;
	char			desc[ 256 ];
	
	err = ServiceSetupEventLogging();
	require_noerr( err, exit );
	
	// Initialize the service status and register the service control handler with the name of the service.
	
	gServiceStatus.dwServiceType 				= SERVICE_WIN32_SHARE_PROCESS;
	gServiceStatus.dwCurrentState 				= 0;
	gServiceStatus.dwControlsAccepted 			= SERVICE_ACCEPT_STOP;
	gServiceStatus.dwWin32ExitCode 				= NO_ERROR;
	gServiceStatus.dwServiceSpecificExitCode 	= NO_ERROR;
	gServiceStatus.dwCheckPoint 				= 0;
	gServiceStatus.dwWaitHint 					= 0;
	
	gServiceStatusHandle = RegisterServiceCtrlHandler( argv[ 0 ], ServiceControlHandler );
	err = translate_errno( gServiceStatusHandle, (OSStatus) GetLastError(), kInUseErr );
	require_noerr( err, exit );
	
	// Setup the description. This should be done by the installer, but it doesn't support that yet.
			
	desc[ 0 ] = '\0';
	LoadStringA( GetModuleHandle( NULL ), IDS_SERVICE_DESCRIPTION, desc, sizeof( desc ) );
	err = SetServiceDescription( NULL, kServiceName, desc );
	check_noerr( err );
	
	// Mark the service as starting.
	
	gServiceStatus.dwCurrentState 	= SERVICE_START_PENDING;
	gServiceStatus.dwCheckPoint	 	= 0;
	gServiceStatus.dwWaitHint 		= 5000;	// 5 seconds
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Run the service. This does not return until the service quits or is stopped.
	
	err = ServiceRun( (int) argc, argv );
	if( err != kNoErr )
	{
		gServiceStatus.dwWin32ExitCode				= ERROR_SERVICE_SPECIFIC_ERROR;
		gServiceStatus.dwServiceSpecificExitCode 	= (DWORD) err;
	}
	
	// Service-specific work is done so mark the service as stopped.
	
	gServiceStatus.dwCurrentState = SERVICE_STOPPED;
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Note: The service status handle should not be closed according to Microsoft documentation.
	
exit:
	if( gServiceEventSource )
	{
		ok = DeregisterEventSource( gServiceEventSource );
		check_translated_errno( ok, GetLastError(), kUnknownErr );
		gServiceEventSource = NULL;
	}
}

//===========================================================================================================================
//	ServiceSetupEventLogging
//===========================================================================================================================

static OSStatus	ServiceSetupEventLogging( void )
{
	OSStatus			err;
	HKEY				key;
	const char *		s;
	DWORD				typesSupported;
	char				path[ MAX_PATH ];
	DWORD 				n;
	
	key = NULL;
	
	// Add/Open source name as a sub-key under the Application key in the EventLog registry key.

	s = "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" kServiceName;
	err = RegCreateKey( HKEY_LOCAL_MACHINE, s, &key );
	require_noerr( err, exit );
	
	// Add the name to the EventMessageFile subkey.
	
	path[ 0 ] = '\0';
	GetModuleFileName( NULL, path, MAX_PATH );
	n = (DWORD)( strlen( path ) + 1 );
	err = RegSetValueEx( key, "EventMessageFile", 0, REG_EXPAND_SZ, (const LPBYTE) path, n );
	require_noerr( err, exit );
	
	// Set the supported event types in the TypesSupported subkey.
	
	typesSupported = 0 
					 | EVENTLOG_SUCCESS
					 | EVENTLOG_ERROR_TYPE
					 | EVENTLOG_WARNING_TYPE
					 | EVENTLOG_INFORMATION_TYPE
					 | EVENTLOG_AUDIT_SUCCESS
					 | EVENTLOG_AUDIT_FAILURE; 
	err = RegSetValueEx( key, "TypesSupported", 0, REG_DWORD, (const LPBYTE) &typesSupported, sizeof( DWORD ) );
	require_noerr( err, exit );
	
	// Set up the event source.
	
	gServiceEventSource = RegisterEventSource( NULL, kServiceName );
	err = translate_errno( gServiceEventSource, (OSStatus) GetLastError(), kParamErr );
	require_noerr( err, exit );
		
exit:
	if( key )
	{
		RegCloseKey( key );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceControlHandler
//===========================================================================================================================

static void WINAPI	ServiceControlHandler( DWORD inControl )
{
	BOOL		setStatus;
	BOOL		ok;
	
	setStatus = TRUE;
	switch( inControl )
	{
		case SERVICE_CONTROL_STOP:
			dlog( kDebugLevelNotice, DEBUG_NAME "ServiceControlHandler: SERVICE_CONTROL_STOP\n" );
			
			ServiceStop();
			setStatus = FALSE;
			break;
		
		default:
			dlog( kDebugLevelNotice, DEBUG_NAME "ServiceControlHandler: event (0x%08X)\n", inControl );
			break;
	}
	
	if( setStatus && gServiceStatusHandle )
	{
		ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
		check_translated_errno( ok, GetLastError(), kUnknownErr );
	}
}

//===========================================================================================================================
//	ServiceRun
//===========================================================================================================================

static OSStatus	ServiceRun( int argc, char *argv[] )
{
	OSStatus		err;
	BOOL			initialized;
	BOOL			ok;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	initialized = FALSE;
	
	// Initialize the service-specific stuff and mark the service as running.
	
	err = ServiceSpecificInitialize( argc, argv );
	require_noerr( err, exit );
	initialized = TRUE;
	
	gServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
	check_translated_errno( ok, GetLastError(), kParamErr );
	
	// Run the service-specific stuff. This does not return until the service quits or is stopped.
	
	ReportStatus( EVENTLOG_INFORMATION_TYPE, "mDNSResponder started\n" );
	err = ServiceSpecificRun( argc, argv );
	ReportStatus( EVENTLOG_INFORMATION_TYPE, "mDNSResponder stopped (%d)\n", err );
	require_noerr( err, exit );
	
	// Service stopped. Clean up and we're done.
	
exit:
	if( initialized )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceStop
//===========================================================================================================================

static void	ServiceStop( void )
{
	BOOL			ok;
	OSStatus		err;
	
	// Signal the event to cause the service to exit.
	
	if( gServiceStatusHandle )
	{
		gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		ok = SetServiceStatus( gServiceStatusHandle, &gServiceStatus );
		check_translated_errno( ok, GetLastError(), kParamErr );
	}
		
	err = ServiceSpecificStop();
	check_noerr( err );
}

#if 0
#pragma mark -
#pragma mark == Service Specific ==
#endif

//===========================================================================================================================
//	ServiceSpecificInitialize
//===========================================================================================================================

static OSStatus	ServiceSpecificInitialize( int argc, char *argv[] )
{
	OSStatus						err;
	DNSServiceInitializeFlags		initFlags;
	RMxServerFlags					flags;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	initFlags = kDNSServiceInitializeFlagsAdvertise | kDNSServiceInitializeFlagsNoServerCheck;
	err = DNSServiceInitialize_direct( initFlags, gServiceCacheEntryCount );
	require_noerr( err, exit );
	
	flags = 0;
	if( gServiceAllowRemote )
	{
		flags |= kRMxServerFlagsAllowRemote;
	}
	err = RMxServerInitialize( flags );
	require_noerr( err, exit );

exit:
	if( err != kNoErr )
	{
		ServiceSpecificFinalize( argc, argv );
	}
	return( err );
}

//===========================================================================================================================
//	ServiceSpecificRun
//===========================================================================================================================

static OSStatus	ServiceSpecificRun( int argc, char *argv[] )
{
	OSStatus		err;
	
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );

	err = RMxServerRun();
	require_noerr( err, exit );

exit:
	return( err );
}

//===========================================================================================================================
//	ServiceSpecificStop
//===========================================================================================================================

static OSStatus	ServiceSpecificStop( void )
{
	OSStatus		err;

	err = RMxServerStop( kRMxServerStopFlagsNoWait );
	require_noerr( err, exit );

exit:
	return( err );
}

//===========================================================================================================================
//	ServiceSpecificFinalize
//===========================================================================================================================

static void	ServiceSpecificFinalize( int argc, char *argv[] )
{
	DEBUG_UNUSED( argc );
	DEBUG_UNUSED( argv );
	
	RMxServerFinalize();
	DNSServiceFinalize_direct();
}
