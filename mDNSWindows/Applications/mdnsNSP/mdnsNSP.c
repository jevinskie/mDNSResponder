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
    
$Log: mdnsNSP.c,v $
Revision 1.2  2004/04/08 09:43:43  bradley
Changed callback calling conventions to __stdcall so they can be used with C# delegates.

Revision 1.1  2004/01/30 03:00:33  bradley
Rendezvous NameSpace Provider (NSP). Hooks into the Windows name resolution system to resolve
Rendezvous name lookups using Multicast DNS so .local names work in all Windows apps.

*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	<guiddef.h>
#include	<ws2spi.h>

#include	"DNSSD.h"

#if 0
#pragma mark == Structures ==
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================

typedef struct	Query *		QueryRef;
typedef struct	Query		Query;
struct	Query
{
	QueryRef			next;
	int					refCount;
	DWORD				querySetFlags;
	WSAQUERYSETW *		querySet;
	size_t				querySetSize;
	HANDLE				dataEvent;
	HANDLE				cancelEvent;
	HANDLE				waitHandles[ 2 ];
	DWORD				waitCount;
	DNSServiceRef		resolver;
	char				name[ kDNSServiceMaxDomainName ];
	size_t				nameSize;
	uint32_t			addr;
	bool				addrValid;
};

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// DLL Exports

BOOL WINAPI		DllMain( HINSTANCE inInstance, DWORD inReason, LPVOID inReserved );

// NSP SPIs

int	WSPAPI	NSPCleanup( LPGUID inProviderID );

DEBUG_LOCAL int WSPAPI
	NSPLookupServiceBegin(
		LPGUID					inProviderID,
		LPWSAQUERYSETW			inQuerySet,
		LPWSASERVICECLASSINFOW	inServiceClassInfo,
		DWORD					inFlags,   
		LPHANDLE				outLookup );

DEBUG_LOCAL int WSPAPI
	NSPLookupServiceNext(  
		HANDLE			inLookup,
		DWORD			inFlags,
		LPDWORD			ioBufferLength,
		LPWSAQUERYSETW	outResults );

DEBUG_LOCAL int WSPAPI	NSPLookupServiceEnd( HANDLE inLookup );

DEBUG_LOCAL int WSPAPI
	NSPSetService(
		LPGUID					inProviderID,						
		LPWSASERVICECLASSINFOW	inServiceClassInfo,   
		LPWSAQUERYSETW			inRegInfo,				  
		WSAESETSERVICEOP		inOperation,			   
		DWORD					inFlags );

DEBUG_LOCAL int WSPAPI	NSPInstallServiceClass( LPGUID inProviderID, LPWSASERVICECLASSINFOW inServiceClassInfo );
DEBUG_LOCAL int WSPAPI	NSPRemoveServiceClass( LPGUID inProviderID, LPGUID inServiceClassID );
DEBUG_LOCAL int WSPAPI	NSPGetServiceClassInfo(	LPGUID inProviderID, LPDWORD ioBufSize, LPWSASERVICECLASSINFOW ioServiceClassInfo );

// Private

#define	NSPLock()		EnterCriticalSection( &gLock );
#define	NSPUnlock()		LeaveCriticalSection( &gLock );

DEBUG_LOCAL OSStatus	QueryCreate( const WSAQUERYSETW *inQuerySet, DWORD inQuerySetFlags, QueryRef *outRef );
DEBUG_LOCAL OSStatus	QueryRetain( QueryRef inRef );
DEBUG_LOCAL OSStatus	QueryRelease( QueryRef inRef );

DEBUG_LOCAL void CALLBACK_COMPAT
	QueryRecordCallback(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		uint32_t			inInterfaceIndex,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,    
		uint16_t			inRRType,
		uint16_t			inRRClass,
		uint16_t			inRDataSize,
		const void *		inRData,
		uint32_t			inTTL,
		void *				inContext );

DEBUG_LOCAL OSStatus
	QueryCopyQuerySet( 
		QueryRef 				inRef, 
		const WSAQUERYSETW *	inQuerySet, 
		DWORD 					inQuerySetFlags, 
		WSAQUERYSETW **			outQuerySet, 
		size_t *				outSize );

DEBUG_LOCAL void
	QueryCopyQuerySetTo( 
		QueryRef 				inRef, 
		const WSAQUERYSETW *	inQuerySet, 
		DWORD 					inQuerySetFlags, 
		WSAQUERYSETW *			outQuerySet );

DEBUG_LOCAL size_t	QueryCopyQuerySetSize( QueryRef inRef, const WSAQUERYSETW *inQuerySet, DWORD inQuerySetFlags );

#if( DEBUG )
	void	DebugDumpQuerySet( DebugLevel inLevel, const WSAQUERYSETW *inQuerySet );
	
	#define	dlog_query_set( LEVEL, SET )		DebugDumpQuerySet( LEVEL, SET )
#else
	#define	dlog_query_set( LEVEL, SET )
#endif

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

// {B600E6E9-553B-4a19-8696-335E5C896153}
// GUID		kRendezvousNSPGUID = { 0xb600e6e9, 0x553b, 0x4a19, { 0x86, 0x96, 0x33, 0x5e, 0x5c, 0x89, 0x61, 0x53 } };

DEBUG_LOCAL LONG					gRefCount			= 0;
DEBUG_LOCAL CRITICAL_SECTION		gLock;
DEBUG_LOCAL bool					gLockInitialized 	= false;
DEBUG_LOCAL bool					gDNSSDInitialized	= false;
DEBUG_LOCAL QueryRef				gQueryList	 		= NULL;

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	DllMain
//===========================================================================================================================

BOOL APIENTRY	DllMain( HINSTANCE inInstance, DWORD inReason, LPVOID inReserved )
{
	DEBUG_USE_ONLY( inInstance );
	DEBUG_UNUSED( inReserved );
	
	switch( inReason )
	{
		case DLL_PROCESS_ATTACH:			
			debug_initialize( kDebugOutputTypeWindowsEventLog, "Rendezvous NSP", inInstance );
			debug_set_property( kDebugPropertyTagPrintLevel, kDebugLevelInfo );
			dlog( kDebugLevelTrace, "\n" );
			dlog( kDebugLevelVerbose, "%s: process attach\n", __ROUTINE__ );
			break;
		
		case DLL_PROCESS_DETACH:
			dlog( kDebugLevelVerbose, "%s: process detach\n", __ROUTINE__ );
			break;
		
		case DLL_THREAD_ATTACH:
			dlog( kDebugLevelVerbose, "%s: thread attach\n", __ROUTINE__ );
			break;
		
		case DLL_THREAD_DETACH:
			dlog( kDebugLevelVerbose, "%s: thread detach\n", __ROUTINE__ );
			break;
		
		default:
			dlog( kDebugLevelNotice, "%s: unknown reason code (%d)\n", __ROUTINE__, inReason );
			break;
	}
	return( TRUE );
}

//===========================================================================================================================
//	NSPStartup
//
//	This function is called when our namespace DLL is loaded. It sets up the NSP functions we implement and initializes us.
//===========================================================================================================================

int WSPAPI	NSPStartup( LPGUID inProviderID, LPNSP_ROUTINE outRoutines )
{
	OSStatus		err;
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s (GUID=%U, refCount=%ld)\n", __ROUTINE__, inProviderID, gRefCount );
	
	// Only initialize if this is the first time NSPStartup is called. 
	
	if( InterlockedIncrement( &gRefCount ) != 1 )
	{
		err = NO_ERROR;
		goto exit;
	}
	
	// Initialize our internal state.
	
	InitializeCriticalSection( &gLock );
	gLockInitialized = true;
	
	// Set the size to exclude NSPIoctl because we don't implement it.
	
	outRoutines->cbSize					= FIELD_OFFSET( NSP_ROUTINE, NSPIoctl );
	outRoutines->dwMajorVersion			= 4;
	outRoutines->dwMinorVersion			= 4;
	outRoutines->NSPCleanup				= NSPCleanup;
	outRoutines->NSPLookupServiceBegin	= NSPLookupServiceBegin;
	outRoutines->NSPLookupServiceNext	= NSPLookupServiceNext;
	outRoutines->NSPLookupServiceEnd	= NSPLookupServiceEnd;
	outRoutines->NSPSetService			= NSPSetService;
	outRoutines->NSPInstallServiceClass	= NSPInstallServiceClass;
	outRoutines->NSPRemoveServiceClass	= NSPRemoveServiceClass;
	outRoutines->NSPGetServiceClassInfo	= NSPGetServiceClassInfo;
	
	err = NO_ERROR;
	
exit:
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	if( err != NO_ERROR )
	{
		NSPCleanup( inProviderID );
		SetLastError( (DWORD) err );
		return( SOCKET_ERROR );
	}
	return( NO_ERROR );
}

//===========================================================================================================================
//	NSPCleanup
//
//	This function is called when our namespace DLL is unloaded. It cleans up anything we set up in NSPStartup.
//===========================================================================================================================

int	WSPAPI	NSPCleanup( LPGUID inProviderID )
{
	DEBUG_USE_ONLY( inProviderID );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s (GUID=%U, refCount=%ld)\n", __ROUTINE__, inProviderID, gRefCount );
	
	// Only initialize if this is the first time NSPStartup is called.
	
	if( InterlockedDecrement( &gRefCount ) != 0 )
	{
		goto exit;
	}
	
	// Stop any outstanding queries.
	
	if( gLockInitialized )
	{
		NSPLock();
	}
	while( gQueryList )
	{
		check_string( gQueryList->refCount == 1, "NSPCleanup with outstanding queries!" );
		QueryRelease( gQueryList );
	}
	if( gLockInitialized )
	{
		NSPUnlock();
	}
	
	// Shut down DNS-SD and release our resources.
	
	if( gDNSSDInitialized )
	{
		gDNSSDInitialized = false;
		DNSServiceFinalize();
	}
	if( gLockInitialized )
	{
		gLockInitialized = false;
		DeleteCriticalSection( &gLock );
	}
	
exit:
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	return( NO_ERROR );
}

//===========================================================================================================================
//	NSPLookupServiceBegin
//
//	This function maps to the WinSock WSALookupServiceBegin function. It starts the lookup process and returns a HANDLE 
//	that can be used in subsequent operations. Subsequent calls only need to refer to this query by the handle as 
//	opposed to specifying the query parameters each time.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI
	NSPLookupServiceBegin(
		LPGUID					inProviderID,
		LPWSAQUERYSETW			inQuerySet,
		LPWSASERVICECLASSINFOW	inServiceClassInfo,
		DWORD					inFlags,   
		LPHANDLE				outLookup )
{
	OSStatus		err;
	QueryRef		obj;
	LPCWSTR			name;
	size_t			size;
	LPCWSTR			p;
	DWORD           type;
	DWORD			n;
	DWORD			i;
	INT				family;
	INT				protocol;
	
	DEBUG_UNUSED( inProviderID );
	DEBUG_UNUSED( inServiceClassInfo );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	
	obj = NULL;
	require_action( inQuerySet, exit, err = WSAEINVAL );
	name = inQuerySet->lpszServiceInstanceName;
	require_action_quiet( name, exit, err = WSAEINVAL );
	require_action( outLookup, exit, err = WSAEINVAL );
	
	dlog( kDebugLevelTrace, "%s (flags=0x%08X, name=\"%S\")\n", __ROUTINE__, inFlags, name );
	dlog_query_set( kDebugLevelVerbose, inQuerySet );
	
	// Check if we can handle this type of request and if we support any of the protocols being requested.
	// We only support the DNS namespace, TCP and UDP protocols, and IPv4. Only blob results are supported.
	
	require_action_quiet( inFlags & LUP_RETURN_BLOB, exit, err = WSASERVICE_NOT_FOUND );
	
	type = inQuerySet->dwNameSpace;
	require_action_quiet( ( type == NS_DNS ) || ( type == NS_ALL ), exit, err = WSASERVICE_NOT_FOUND );
	
	n = inQuerySet->dwNumberOfProtocols;
	if( n > 0 )
	{
		require_action( inQuerySet->lpafpProtocols, exit, err = WSAEINVAL );
		for( i = 0; i < n; ++i )
		{
			family = inQuerySet->lpafpProtocols[ i ].iAddressFamily;
			protocol = inQuerySet->lpafpProtocols[ i ].iProtocol;
			if( ( family == AF_INET ) && ( ( protocol == IPPROTO_UDP ) || ( protocol == IPPROTO_TCP ) ) )
			{
				break;
			}
		}
		require_action_quiet( i < n, exit, err = WSASERVICE_NOT_FOUND );
	}
	
	// Check if the name ends in ".local" and if not, exit with an error since we only resolve .local names.
	// The name may or may not end with a "." (fully qualified) so handle both cases. DNS is also case 
	// insensitive the check for .local has to be case insensitive (.LoCaL is equivalent to .local). This
	// manually does the wchar_t strlen and stricmp to avoid needing any special wchar_t versions of the 
	// libraries. It is probably faster to do the inline compare than invoke functions to do it anyway.
	
	for( p = name; *p; ++p ) {}		// Find end of string
	size = (size_t)( p - name );
	require_action_quiet( size > sizeof_string( ".local" ), exit, err = WSASERVICE_NOT_FOUND );
	
	p = name + ( size - 1 );
	p = ( *p == '.' ) ? ( p - sizeof_string( ".local" ) ) : ( ( p - sizeof_string( ".local" ) ) + 1 );
	require_action_quiet( ( ( p[ 0 ] == '.' )						 &&
						  ( ( p[ 1 ] == 'L' ) || ( p[ 1 ] == 'l' ) ) &&
						  ( ( p[ 2 ] == 'O' ) || ( p[ 2 ] == 'o' ) ) &&
						  ( ( p[ 3 ] == 'C' ) || ( p[ 3 ] == 'c' ) ) &&
						  ( ( p[ 4 ] == 'A' ) || ( p[ 4 ] == 'a' ) ) &&
						  ( ( p[ 5 ] == 'L' ) || ( p[ 5 ] == 'l' ) ) ), 
						  exit, err = WSASERVICE_NOT_FOUND );
	
	// The name ends in .local so start the resolve operation. Lazy initialize DNS-SD if needed.
		
	NSPLock();
	if( !gDNSSDInitialized )
	{
		err = DNSServiceInitialize( kDNSServiceInitializeFlagsNoServerCheck, 0 );
		require_noerr( err, exit );
		gDNSSDInitialized = true;
	}
	
	err = QueryCreate( inQuerySet, inFlags, &obj );
	NSPUnlock();
	require_noerr( err, exit );
	
	*outLookup = (HANDLE) obj;
	
exit:
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	if( err != NO_ERROR )
	{
		SetLastError( (DWORD) err );
		return( SOCKET_ERROR );
	}
	return( NO_ERROR );
}

//===========================================================================================================================
//	NSPLookupServiceNext
//
//	This function maps to the Winsock call WSALookupServiceNext. This routine takes a handle to a previously defined 
//	query and attempts to locate a service matching the criteria defined by the query. If so, that instance is returned 
//	in the lpqsResults parameter.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI
	NSPLookupServiceNext(  
		HANDLE			inLookup,
		DWORD			inFlags,
		LPDWORD			ioSize,
		LPWSAQUERYSETW	outResults )
{
	OSStatus		err;
	QueryRef		obj;
	DWORD			waitResult;
	size_t			size;
	
	DEBUG_USE_ONLY( inFlags );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	
	obj = NULL;
	NSPLock();
	err = QueryRetain( (QueryRef) inLookup );
	require_noerr( err, exit );
	obj = (QueryRef) inLookup;
	require_action( ioSize, exit, err = WSAEINVAL );
	require_action( outResults, exit, err = WSAEINVAL );
	
	dlog( kDebugLevelTrace, "%s (lookup=%#p, flags=0x%08X, *ioSize=%d)\n", __ROUTINE__, inLookup, inFlags, *ioSize );
	
	// Wait for data or a cancel. Release the lock while waiting. This is safe because we've retained the query.

	NSPUnlock();
	waitResult = WaitForMultipleObjects( obj->waitCount, obj->waitHandles, FALSE, 5 * 1000 );
	NSPLock();
	require_action_quiet( waitResult != ( WAIT_OBJECT_0 + 1 ), exit, err = WSA_E_CANCELLED );
	err = translate_errno( waitResult == WAIT_OBJECT_0, (OSStatus) GetLastError(), WSASERVICE_NOT_FOUND );
	require_noerr_quiet( err, exit );
	require_action_quiet( obj->addrValid, exit, err = WSA_E_NO_MORE );
	
	// Copy the externalized query results to the callers buffer (if it fits).
	
	size = QueryCopyQuerySetSize( obj, obj->querySet, obj->querySetFlags );
	require_action( size <= (size_t) *ioSize, exit, err = WSAEFAULT );
	
	QueryCopyQuerySetTo( obj, obj->querySet, obj->querySetFlags, outResults );
	outResults->dwOutputFlags = RESULT_IS_ADDED;
	obj->addrValid = false;
	
exit:
	if( obj )
	{
		QueryRelease( obj );
	}
	NSPUnlock();
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	if( err != NO_ERROR )
	{
		SetLastError( (DWORD) err );
		return( SOCKET_ERROR );
	}
	return( NO_ERROR );
}

//===========================================================================================================================
//	NSPLookupServiceEnd
//
//	This function maps to the Winsock call WSALookupServiceEnd. Once the user process has finished is query (usually 
//	indicated when WSALookupServiceNext returns the error WSA_E_NO_MORE) a call to this function is made to release any 
//	allocated resources associated with the query.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI	NSPLookupServiceEnd( HANDLE inLookup )
{
	OSStatus		err;

	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	
	dlog( kDebugLevelTrace, "%s (lookup=%#p)\n", __ROUTINE__, inLookup );
	
	NSPLock();
	err = QueryRelease( (QueryRef) inLookup );
	NSPUnlock();
	require_noerr( err, exit );
	
exit:
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	if( err != NO_ERROR )
	{
		SetLastError( (DWORD) err );
		return( SOCKET_ERROR );
	}
	return( NO_ERROR );
}

//===========================================================================================================================
//	NSPSetService
//
//	This function maps to the Winsock call WSASetService. This routine is called when the user wants to register or 
//	deregister an instance of a server with our service. For registration, the user needs to associate the server with a 
//	service class. For deregistration the service class is required along with the servicename. The inRegInfo parameter 
//	contains a WSAQUERYSET structure defining the server (such as protocol and address where it is).
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI
	NSPSetService(
		LPGUID					inProviderID,						
		LPWSASERVICECLASSINFOW	inServiceClassInfo,   
		LPWSAQUERYSETW			inRegInfo,				  
		WSAESETSERVICEOP		inOperation,			   
		DWORD					inFlags )
{
	DEBUG_UNUSED( inProviderID );
	DEBUG_UNUSED( inServiceClassInfo );
	DEBUG_UNUSED( inRegInfo );
	DEBUG_UNUSED( inOperation );
	DEBUG_UNUSED( inFlags );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s\n", __ROUTINE__ );
	
	// We don't allow services to be registered so always return an error.
	
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	return( WSAEINVAL );
}

//===========================================================================================================================
//	NSPInstallServiceClass
//
//	This function maps to the Winsock call WSAInstallServiceClass. This routine is used to install a service class which 
//	is used to define certain characteristics for a group of services. After a service class is registered, an actual
//	instance of a server may be registered.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI	NSPInstallServiceClass( LPGUID inProviderID, LPWSASERVICECLASSINFOW inServiceClassInfo )
{
	DEBUG_UNUSED( inProviderID );
	DEBUG_UNUSED( inServiceClassInfo );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s\n", __ROUTINE__ );
	
	// We don't allow service classes to be installed so always return an error.

	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	return( WSA_INVALID_PARAMETER );
}

//===========================================================================================================================
//	NSPRemoveServiceClass
//
//	This function maps to the Winsock call WSARemoveServiceClass. This routine removes a previously registered service 
//	class. This is accomplished by connecting to the namespace service and writing the GUID which defines the given 
//	service class.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI	NSPRemoveServiceClass( LPGUID inProviderID, LPGUID inServiceClassID )
{
	DEBUG_UNUSED( inProviderID );
	DEBUG_UNUSED( inServiceClassID );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s\n", __ROUTINE__ );
	
	// We don't allow service classes to be installed so always return an error.
	
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	return( WSATYPE_NOT_FOUND );
}

//===========================================================================================================================
//	NSPGetServiceClassInfo
//
//	This function maps to the Winsock call WSAGetServiceClassInfo. This routine returns the information associated with 
//	a given service class.
//===========================================================================================================================

DEBUG_LOCAL int WSPAPI	NSPGetServiceClassInfo(	LPGUID inProviderID, LPDWORD ioSize, LPWSASERVICECLASSINFOW ioServiceClassInfo )
{
	DEBUG_UNUSED( inProviderID );
	DEBUG_UNUSED( ioSize );
	DEBUG_UNUSED( ioServiceClassInfo );
	
	dlog( kDebugLevelTrace, "%s begin (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	dlog( kDebugLevelTrace, "%s\n", __ROUTINE__ );
	
	// We don't allow service classes to be installed so always return an error.
	
	dlog( kDebugLevelTrace, "%s end   (ticks=%d)\n", __ROUTINE__, GetTickCount() );
	return( WSATYPE_NOT_FOUND );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	QueryCreate
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL OSStatus	QueryCreate( const WSAQUERYSETW *inQuerySet, DWORD inQuerySetFlags, QueryRef *outRef )
{
	OSStatus		err;
	QueryRef		obj;
	char			name[ kDNSServiceMaxDomainName ];
	int				n;
	QueryRef *		p;
	
	obj = NULL;
	check( inQuerySet );
	check( inQuerySet->lpszServiceInstanceName );
	check( outRef );
	
	// Convert the wchar_t name to UTF-8.
	
	n = WideCharToMultiByte( CP_UTF8, 0, inQuerySet->lpszServiceInstanceName, -1, name, sizeof( name ), NULL, NULL );
	err = translate_errno( n > 0, (OSStatus) GetLastError(), WSAEINVAL );
	require_noerr( err, exit );
	
	// Allocate the object and append it to the list. Append immediately so releases of partial objects work.
	
	obj = (QueryRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = WSA_NOT_ENOUGH_MEMORY );
	
	obj->refCount = 1;
	
	for( p = &gQueryList; *p; p = &( *p )->next ) {}	// Find the end of the list.
	*p = obj;
	
	// Set up events to signal when data is ready and when cancelling.
	
	obj->dataEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	require_action( obj->dataEvent, exit, err = WSA_NOT_ENOUGH_MEMORY );
	
	obj->cancelEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	require_action( obj->cancelEvent, exit, err = WSA_NOT_ENOUGH_MEMORY );
	
	obj->waitCount = 0;
	obj->waitHandles[ obj->waitCount++ ] = obj->dataEvent;
	obj->waitHandles[ obj->waitCount++ ] = obj->cancelEvent;
	check( obj->waitCount == sizeof_array( obj->waitHandles ) );
	
	// Copy the QuerySet so it can be returned later.
	
	obj->querySetFlags = inQuerySetFlags;
	inQuerySetFlags = ( inQuerySetFlags & ~( LUP_RETURN_ADDR | LUP_RETURN_BLOB ) ) | LUP_RETURN_NAME;
	err = QueryCopyQuerySet( obj, inQuerySet, inQuerySetFlags, &obj->querySet, &obj->querySetSize );
	require_noerr( err, exit );
	
	// Start the query.
	
	err = DNSServiceQueryRecord( &obj->resolver, 0, 0, name, kDNSServiceDNSType_A, kDNSServiceDNSClass_IN, 
		QueryRecordCallback, obj );
	require_noerr( err, exit );
		
	// Success!
	
	*outRef	= obj;
	obj 	= NULL;
	err 	= NO_ERROR;

exit:
	if( obj )
	{
		QueryRelease( obj );
	}
	return( err );
}

//===========================================================================================================================
//	QueryRetain
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL OSStatus	QueryRetain( QueryRef inRef )
{
	OSStatus		err;
	QueryRef		obj;
	
	for( obj = gQueryList; obj; obj = obj->next )
	{
		if( obj == inRef )
		{
			break;
		}
	}
	require_action( obj, exit, err = WSA_INVALID_HANDLE );
	
	++inRef->refCount;
	err = NO_ERROR;
	
exit:
	return( err );
}

//===========================================================================================================================
//	QueryRelease
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL OSStatus	QueryRelease( QueryRef inRef )
{
	OSStatus		err;
	QueryRef *		p;
	BOOL			ok;
		
	// Find the item in the list.
	
	for( p = &gQueryList; *p; p = &( *p )->next )
	{
		if( *p == inRef )
		{
			break;
		}
	}
	require_action( *p, exit, err = WSA_INVALID_HANDLE );
	
	// Signal a cancel to unblock any threads waiting for results.
	
	if( inRef->cancelEvent )
	{
		ok = SetEvent( inRef->cancelEvent );
		check_translated_errno( ok, GetLastError(), WSAEINVAL );
	}
	
	// Stop the query.
	
	if( inRef->resolver )
	{
		DNSServiceRefDeallocate( inRef->resolver );
		inRef->resolver = NULL;
	}
	
	// Decrement the refCount. Fully release if it drops to 0. If still referenced, just exit.
	
	if( --inRef->refCount != 0 )
	{
		err = NO_ERROR;
		goto exit;
	}
	*p = inRef->next;
	
	// Release resources.
	
	if( inRef->cancelEvent )
	{
		ok = CloseHandle( inRef->cancelEvent );
		check_translated_errno( ok, GetLastError(), WSAEINVAL );
	}
	if( inRef->dataEvent )
	{
		ok = CloseHandle( inRef->dataEvent );
		check_translated_errno( ok, GetLastError(), WSAEINVAL );
	}
	if( inRef->querySet )
	{
		free( inRef->querySet );
	}
	free( inRef );
	err = NO_ERROR;
	
exit:
	return( err );
}

//===========================================================================================================================
//	QueryRecordCallback
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	QueryRecordCallback(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		uint32_t			inInterfaceIndex,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,    
		uint16_t			inRRType,
		uint16_t			inRRClass,
		uint16_t			inRDataSize,
		const void *		inRData,
		uint32_t			inTTL,
		void *				inContext )
{
	QueryRef			obj;
	const char *		src;
	char *				dst;
	BOOL				ok;
	
	DEBUG_UNUSED( inFlags );
	DEBUG_UNUSED( inInterfaceIndex );
	DEBUG_UNUSED( inTTL );

	NSPLock();
	obj = (QueryRef) inContext;
	check( obj );
	require_noerr( inErrorCode, exit );
	require_quiet( inFlags & kDNSServiceFlagsAdd, exit );
	require( inRRClass   == kDNSServiceDNSClass_IN, exit );
	require( inRRType    == kDNSServiceDNSType_A, exit );
	require( inRDataSize == 4, exit );
	
	dlog( kDebugLevelTrace, "%s (flags=0x%08X, name=%s, rrType=%d, rDataSize=%d)\n", 
		__ROUTINE__, inFlags, inName, inRRType, inRDataSize );
		
	// Copy the name if needed.
	
	if( obj->name[ 0 ] == '\0' )
	{
		src = inName;
		dst = obj->name;
		while( *src != '\0' )
		{
			*dst++ = *src++;
		}
		*dst = '\0';
		obj->nameSize = (size_t)( dst - obj->name );
		check( obj->nameSize < sizeof( obj->name ) );
	}
	
	// Copy the data.
	
	memcpy( &obj->addr, inRData, inRDataSize );
	obj->addrValid = true;
	
	// Signal that a result is ready.
	
	check( obj->dataEvent );
	ok = SetEvent( obj->dataEvent );
	check_translated_errno( ok, GetLastError(), WSAEINVAL );
	
	// Stop the resolver after the first response.
	
	DNSServiceRefDeallocate( inRef );
	obj->resolver = NULL;

exit:
	NSPUnlock();
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	QueryCopyQuerySet
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL OSStatus
	QueryCopyQuerySet( 
		QueryRef 				inRef, 
		const WSAQUERYSETW *	inQuerySet, 
		DWORD 					inQuerySetFlags, 
		WSAQUERYSETW **			outQuerySet, 
		size_t *				outSize )
{
	OSStatus			err;
	size_t				size;
	WSAQUERYSETW *		qs;
	
	check( inQuerySet );
	check( outQuerySet );
	
	size  = QueryCopyQuerySetSize( inRef, inQuerySet, inQuerySetFlags );
	qs = (WSAQUERYSETW *) calloc( 1, size );
	require_action( qs, exit, err = WSA_NOT_ENOUGH_MEMORY  );
	
	QueryCopyQuerySetTo( inRef, inQuerySet, inQuerySetFlags, qs );
	
	*outQuerySet = qs;
	if( outSize )
	{
		*outSize = size;
	}
	qs = NULL;
	err = NO_ERROR;
	
exit:
	if( qs )
	{
		free( qs );
	}
	return( err );	
}

//===========================================================================================================================
//	QueryCopyQuerySetTo
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL void
	QueryCopyQuerySetTo( 
		QueryRef 				inRef, 
		const WSAQUERYSETW *	inQuerySet, 
		DWORD 					inQuerySetFlags, 
		WSAQUERYSETW *			outQuerySet )
{
	uint8_t *		dst;
	LPCWSTR			s;
	LPWSTR			q;
	DWORD			n;
	DWORD			i;
	
#if( DEBUG )
	size_t			debugSize;
	
	debugSize = QueryCopyQuerySetSize( inRef, inQuerySet, inQuerySetFlags );
#endif

	check( inQuerySet );
	check( outQuerySet );

	dst = (uint8_t *) outQuerySet;
	
	// Copy the static portion of the results.
	
	*outQuerySet = *inQuerySet;
	dst += sizeof( *inQuerySet );
	
	if( inQuerySetFlags & LUP_RETURN_NAME )
	{
		s = inQuerySet->lpszServiceInstanceName;
		if( s )
		{
			outQuerySet->lpszServiceInstanceName = (LPWSTR) dst;
			q = (LPWSTR) dst;
			while( ( *q++ = *s++ ) != 0 ) {}
			dst = (uint8_t *) q;
		}
	}
	else
	{
		outQuerySet->lpszServiceInstanceName = NULL;
	}
	
	if( inQuerySet->lpServiceClassId )
	{
		outQuerySet->lpServiceClassId  = (LPGUID) dst;
		*outQuerySet->lpServiceClassId = *inQuerySet->lpServiceClassId;
		dst += sizeof( *inQuerySet->lpServiceClassId );
	}
	
	if( inQuerySet->lpVersion )
	{
		outQuerySet->lpVersion  = (LPWSAVERSION) dst;
		*outQuerySet->lpVersion = *inQuerySet->lpVersion;
		dst += sizeof( *inQuerySet->lpVersion );
	}
	
	s = inQuerySet->lpszComment;
	if( s )
	{
		outQuerySet->lpszComment = (LPWSTR) dst;
		q = (LPWSTR) dst;
		while( ( *q++ = *s++ ) != 0 ) {}
		dst = (uint8_t *) q;
	}
	
	if( inQuerySet->lpNSProviderId )
	{
		outQuerySet->lpNSProviderId  = (LPGUID) dst;
		*outQuerySet->lpNSProviderId = *inQuerySet->lpNSProviderId;
		dst += sizeof( *inQuerySet->lpNSProviderId );
	}
	
	s = inQuerySet->lpszContext;
	if( s )
	{
		outQuerySet->lpszContext = (LPWSTR) dst;
		q = (LPWSTR) dst;
		while( ( *q++ = *s++ ) != 0 ) {}
		dst = (uint8_t *) q;
	}
		
	n = inQuerySet->dwNumberOfProtocols;
	if( n > 0 )
	{
		check( inQuerySet->lpafpProtocols );
		
		outQuerySet->lpafpProtocols = (LPAFPROTOCOLS) dst;
		for( i = 0; i < n; ++i )
		{
			outQuerySet->lpafpProtocols[ i ] = inQuerySet->lpafpProtocols[ i ];
			dst += sizeof( *inQuerySet->lpafpProtocols );
		}
	}
		
	s = inQuerySet->lpszQueryString;
	if( s )
	{
		outQuerySet->lpszQueryString = (LPWSTR) dst;
		q = (LPWSTR) dst;
		while( ( *q++ = *s++ ) != 0 ) {}
		dst = (uint8_t *) q;
	}
	
	// Copy the address(es).
	
	if( ( inQuerySetFlags & LUP_RETURN_ADDR ) && inRef->addrValid )
	{
		struct sockaddr_in *		addr;
		
		outQuerySet->dwNumberOfCsAddrs								= 1;
		outQuerySet->lpcsaBuffer 									= (LPCSADDR_INFO) dst;
		dst 													   += sizeof( *outQuerySet->lpcsaBuffer );
		
		outQuerySet->lpcsaBuffer[ 0 ].LocalAddr.lpSockaddr 			= NULL;
		outQuerySet->lpcsaBuffer[ 0 ].LocalAddr.iSockaddrLength		= 0;
		
		outQuerySet->lpcsaBuffer[ 0 ].RemoteAddr.lpSockaddr 		= (LPSOCKADDR) dst;
		outQuerySet->lpcsaBuffer[ 0 ].RemoteAddr.iSockaddrLength	= sizeof( struct sockaddr_in );
		
		addr 														= (struct sockaddr_in *) dst;
		memset( addr, 0, sizeof( *addr ) );
		addr->sin_family											= AF_INET;
		memcpy( &addr->sin_addr, &inRef->addr, 4 );
		dst 													   += sizeof( *addr );
		
		outQuerySet->lpcsaBuffer[ 0 ].iSocketType 					= AF_INET;		// Emulate Tcpip NSP
		outQuerySet->lpcsaBuffer[ 0 ].iProtocol						= IPPROTO_UDP;	// Emulate Tcpip NSP
	}
	else
	{
		outQuerySet->dwNumberOfCsAddrs	= 0;
		outQuerySet->lpcsaBuffer 		= NULL;
	}
	
	// Copy the hostent blob.
	
	if( ( inQuerySetFlags & LUP_RETURN_BLOB ) && inRef->addrValid )
	{
		uint8_t *				base;
		struct hostent *		he;
		uintptr_t *				p;
		
		outQuerySet->lpBlob	 = (LPBLOB) dst;
		dst 				+= sizeof( *outQuerySet->lpBlob );
		
		base = dst;
		he	 = (struct hostent *) dst;
		dst += sizeof( *he );
		
		he->h_name = (char *)( dst - base );
		memcpy( dst, inRef->name, inRef->nameSize + 1 );
		dst += ( inRef->nameSize + 1 );
		
		he->h_aliases 	= (char **)( dst - base );
		p	  			= (uintptr_t *) dst;
		*p++  			= 0;
		dst 		 	= (uint8_t *) p;
		
		he->h_addrtype 	= AF_INET;
		he->h_length	= 4;
		
		he->h_addr_list	= (char **)( dst - base );
		p	  			= (uintptr_t *) dst;
		dst 		   += ( 2 * sizeof( *p ) );
		*p++			= (uintptr_t)( dst - base );
		*p++			= 0;
		p	  			= (uintptr_t *) dst;
		*p++			= (uintptr_t) inRef->addr;
		dst 		 	= (uint8_t *) p;
		
		outQuerySet->lpBlob->cbSize 	= (ULONG)( dst - base );
		outQuerySet->lpBlob->pBlobData	= (BYTE *) base;
	}
	dlog_query_set( kDebugLevelVerbose, outQuerySet );
	
	check( (size_t)( dst - ( (uint8_t *) outQuerySet ) ) == debugSize );
}

//===========================================================================================================================
//	QueryCopyQuerySetSize
//
//	Warning: Assumes the NSP lock is held.
//===========================================================================================================================

DEBUG_LOCAL size_t	QueryCopyQuerySetSize( QueryRef inRef, const WSAQUERYSETW *inQuerySet, DWORD inQuerySetFlags )
{
	size_t		size;
	LPCWSTR		s;
	LPCWSTR		p;
	
	check( inRef );
	check( inQuerySet );
	
	// Calculate the size of the static portion of the results.
	
	size = sizeof( *inQuerySet );
	
	if( inQuerySetFlags & LUP_RETURN_NAME )
	{
		s = inQuerySet->lpszServiceInstanceName;
		if( s )
		{
			for( p = s; *p; ++p ) {}
			size += (size_t)( ( ( p - s ) + 1 ) * sizeof( *p ) );
		}
	}
	
	if( inQuerySet->lpServiceClassId )
	{
		size += sizeof( *inQuerySet->lpServiceClassId );
	}
	
	if( inQuerySet->lpVersion )
	{
		size += sizeof( *inQuerySet->lpVersion );
	}
	
	s = inQuerySet->lpszComment;
	if( s )
	{
		for( p = s; *p; ++p ) {}
		size += (size_t)( ( ( p - s ) + 1 ) * sizeof( *p ) );
	}
	
	if( inQuerySet->lpNSProviderId )
	{
		size += sizeof( *inQuerySet->lpNSProviderId );
	}
	
	s = inQuerySet->lpszContext;
	if( s )
	{
		for( p = s; *p; ++p ) {}
		size += (size_t)( ( ( p - s ) + 1 ) * sizeof( *p ) );
	}
	
	size += ( inQuerySet->dwNumberOfProtocols * sizeof( *inQuerySet->lpafpProtocols ) );
	
	s = inQuerySet->lpszQueryString;
	if( s )
	{
		for( p = s; *p; ++p ) {}
		size += (size_t)( ( ( p - s ) + 1 ) * sizeof( *p ) );
	}
	
	// Calculate the size of the address(es).
	
	if( ( inQuerySetFlags & LUP_RETURN_ADDR ) && inRef->addrValid )
	{
		size += sizeof( *inQuerySet->lpcsaBuffer );
		size += sizeof( struct sockaddr_in );
	}
	
	// Calculate the size of the hostent blob.
	
	if( ( inQuerySetFlags & LUP_RETURN_BLOB ) && inRef->addrValid )
	{
		size += sizeof( *inQuerySet->lpBlob );	// Blob ptr/size structure
		size += sizeof( struct hostent );		// Old-style hostent structure
		size += ( inRef->nameSize + 1 );		// Name and null terminator
		size += 4;								// Alias list terminator (0 offset)
		size += 4;								// Offset to address.
		size += 4;								// Address list terminator (0 offset)
		size += 4;								// IPv4 address
	}
	return( size );
}

#if 0
#pragma mark -
#endif

#if( DEBUG )
//===========================================================================================================================
//	DebugDumpQuerySet
//===========================================================================================================================

#define	DebugSocketFamilyToString( FAM )	( ( FAM ) == AF_INET )  ? "AF_INET"  : \
											( ( FAM ) == AF_INET6 ) ? "AF_INET6" : ""

#define	DebugSocketProtocolToString( PROTO )	( ( PROTO ) == IPPROTO_UDP ) ? "IPPROTO_UDP" : \
												( ( PROTO ) == IPPROTO_TCP ) ? "IPPROTO_TCP" : ""

#define	DebugNameSpaceToString( NS )			( ( NS ) == NS_DNS ) ? "NS_DNS" : ( ( NS ) == NS_ALL ) ? "NS_ALL" : ""

void	DebugDumpQuerySet( DebugLevel inLevel, const WSAQUERYSETW *inQuerySet )
{
	DWORD		i;
	
	check( inQuerySet );

	// Fixed portion of the QuerySet.
		
	dlog( inLevel, "QuerySet:\n" );
	dlog( inLevel, "    dwSize:                  %d (expected %d)\n", inQuerySet->dwSize, sizeof( *inQuerySet ) );
	if( inQuerySet->lpszServiceInstanceName )
	{
		dlog( inLevel, "    lpszServiceInstanceName: %S\n", inQuerySet->lpszServiceInstanceName );
	}
	else
	{
		dlog( inLevel, "    lpszServiceInstanceName: <null>\n" );
	}
	if( inQuerySet->lpServiceClassId )
	{
		dlog( inLevel, "    lpServiceClassId:        %U\n", inQuerySet->lpServiceClassId );
	}
	else
	{
		dlog( inLevel, "    lpServiceClassId:        <null>\n" );
	}
	if( inQuerySet->lpVersion )
	{
		dlog( inLevel, "    lpVersion:\n" );
		dlog( inLevel, "        dwVersion:               %d\n", inQuerySet->lpVersion->dwVersion );
		dlog( inLevel, "        dwVersion:               %d\n", inQuerySet->lpVersion->ecHow );
	}
	else
	{
		dlog( inLevel, "    lpVersion:               <null>\n" );
	}
	if( inQuerySet->lpszComment )
	{
		dlog( inLevel, "    lpszComment:             %S\n", inQuerySet->lpszComment );
	}
	else
	{
		dlog( inLevel, "    lpszComment:             <null>\n" );
	}
	dlog( inLevel, "    dwNameSpace:             %d %s\n", inQuerySet->dwNameSpace, 
		DebugNameSpaceToString( inQuerySet->dwNameSpace ) );
	if( inQuerySet->lpNSProviderId )
	{
		dlog( inLevel, "    lpNSProviderId:          %U\n", inQuerySet->lpNSProviderId );
	}
	else
	{
		dlog( inLevel, "    lpNSProviderId:          <null>\n" );
	}
	if( inQuerySet->lpszContext )
	{
		dlog( inLevel, "    lpszContext:             %S\n", inQuerySet->lpszContext );
	}
	else
	{
		dlog( inLevel, "    lpszContext:             <null>\n" );
	}
	dlog( inLevel, "    dwNumberOfProtocols:     %d\n", inQuerySet->dwNumberOfProtocols );
	dlog( inLevel, "    lpafpProtocols:          %s\n", inQuerySet->lpafpProtocols ? "" : "<null>" );
	for( i = 0; i < inQuerySet->dwNumberOfProtocols; ++i )
	{
		if( i != 0 )
		{
			dlog( inLevel, "\n" );
		}
		dlog( inLevel, "        iAddressFamily:          %d %s\n", inQuerySet->lpafpProtocols[ i ].iAddressFamily, 
			DebugSocketFamilyToString( inQuerySet->lpafpProtocols[ i ].iAddressFamily ) );
		dlog( inLevel, "        iProtocol:               %d %s\n", inQuerySet->lpafpProtocols[ i ].iProtocol, 
			DebugSocketProtocolToString( inQuerySet->lpafpProtocols[ i ].iProtocol ) );
	}
	if( inQuerySet->lpszQueryString )
	{
		dlog( inLevel, "    lpszQueryString:         %S\n", inQuerySet->lpszQueryString );
	}
	else
	{
		dlog( inLevel, "    lpszQueryString:         <null>\n" );
	}
	dlog( inLevel, "    dwNumberOfCsAddrs:       %d\n", inQuerySet->dwNumberOfCsAddrs );
	dlog( inLevel, "    lpcsaBuffer:             %s\n", inQuerySet->lpcsaBuffer ? "" : "<null>" );
	for( i = 0; i < inQuerySet->dwNumberOfCsAddrs; ++i )
	{
		if( i != 0 )
		{
			dlog( inLevel, "\n" );
		}
		if( inQuerySet->lpcsaBuffer[ i ].LocalAddr.lpSockaddr && 
			( inQuerySet->lpcsaBuffer[ i ].LocalAddr.iSockaddrLength > 0 ) )
		{
			dlog( inLevel, "        LocalAddr:               %##a\n", 
				inQuerySet->lpcsaBuffer[ i ].LocalAddr.lpSockaddr );
		}
		else
		{
			dlog( inLevel, "        LocalAddr:               <null/empty>\n" );
		}
		if( inQuerySet->lpcsaBuffer[ i ].RemoteAddr.lpSockaddr && 
			( inQuerySet->lpcsaBuffer[ i ].RemoteAddr.iSockaddrLength > 0 ) )
		{
			dlog( inLevel, "        RemoteAddr:              %##a\n", 
				inQuerySet->lpcsaBuffer[ i ].RemoteAddr.lpSockaddr );
		}
		else
		{
			dlog( inLevel, "        RemoteAddr:              <null/empty>\n" );
		}
		dlog( inLevel, "        iSocketType:             %d\n", inQuerySet->lpcsaBuffer[ i ].iSocketType );
		dlog( inLevel, "        iProtocol:               %d\n", inQuerySet->lpcsaBuffer[ i ].iProtocol );
	}
	dlog( inLevel, "    dwOutputFlags:           %d\n", inQuerySet->dwOutputFlags );
	
	// Blob portion of the QuerySet.
	
	if( inQuerySet->lpBlob )
	{
		dlog( inLevel, "    lpBlob:\n" );
		dlog( inLevel, "        cbSize:                  %ld\n", inQuerySet->lpBlob->cbSize );
		dlog( inLevel, "        pBlobData:               %#p\n", inQuerySet->lpBlob->pBlobData );
		dloghex( inLevel, 12, NULL, 0, 0, NULL, 0, 
			inQuerySet->lpBlob->pBlobData, inQuerySet->lpBlob->pBlobData, inQuerySet->lpBlob->cbSize, 
			kDebugFlagsNone, NULL, 0 );
	}
	else
	{
		dlog( inLevel, "    lpBlob:                  <null>\n" );
	}
}
#endif
