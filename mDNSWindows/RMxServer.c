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
    
$Log: RMxServer.c,v $
Revision 1.6  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.5  2004/04/09 21:03:14  bradley
Changed port numbers to use network byte order for consistency with other platforms.

Revision 1.4  2004/04/08 21:14:19  bradley
Changed resolve callback to use correct calling conventions (previously hidden by a cast).

Revision 1.3  2004/04/08 09:43:43  bradley
Changed callback calling conventions to __stdcall so they can be used with C# delegates.

Revision 1.2  2004/03/16 22:09:03  bradley
Skip socket creation failures to handle local IPv6 addresses being returned by Windows even when
they are not actually supported by the OS; Log a message and only fail if no sockets can be created.

Revision 1.1  2004/01/30 02:35:13  bradley
Rendezvous Message Exchange implementation for DNS-SD IPC on Windows.

*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSD.h"
#include	"DNSSDDirect.h"
#include	"RMxCommon.h"

#include	"RMxServer.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[RMxServer] "

#if 0
#pragma mark == Structures ==
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================

typedef void ( *DNSServiceRefReleaseCallBack )( DNSServiceRef inRef );

// DNSServiceRef

typedef struct	_DNSServiceRef_t	_DNSServiceRef_t;
struct	_DNSServiceRef_t
{
	RMxOpCode			opcode;
	bool				sessionClosing;
	RMxSessionRef		session;
	DNSServiceRef		directObj;
	DNSRecordRef		records;
};

// DNSRecordRef

typedef struct	_DNSRecordRef_t		_DNSRecordRef_t;
struct	_DNSRecordRef_t
{
	DNSRecordRef		next;
	uint32_t			id;
	DNSServiceRef		owner;
	DNSRecordRef		directObj;
};

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// RMx

DEBUG_LOCAL OSStatus	RMxServerRunInitialize( void );
DEBUG_LOCAL void		RMxServerRunFinalize( void );
DEBUG_LOCAL void		RMxServerMessageCallBack( RMxMessage *inMessage );

// General

DEBUG_LOCAL void	DNSServiceRefDeallocate_server( DNSServiceRef inRef );
DEBUG_LOCAL void	DNSServiceCheckVersion_server( RMxMessage *inMessage );

// Properties

DEBUG_LOCAL void		DNSServiceCopyProperty_server( RMxMessage *inMessage );
DEBUG_LOCAL OSStatus	DNSServiceCopyPropertySendData_server( DNSPropertyCode inCode, RMxMessage *inMessage );

// Domain Enumeration

DEBUG_LOCAL void	DNSServiceEnumerateDomains_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceEnumerateDomainsCallBack_server(
		DNSServiceRef		sdRef,
		DNSServiceFlags		flags,
		uint32_t			interfaceIndex,
		DNSServiceErrorType	errorCode,
		const char *		replyDomain,  
		void *				context );

// Service Registration

DEBUG_LOCAL void	DNSServiceRegister_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceRegisterCallBack_server(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,  
		const char *		inType,  
		const char *		inDomain,  
		void *				inContext );

DEBUG_LOCAL void	DNSServiceAddRecord_server( RMxMessage *inMessage );
DEBUG_LOCAL void	DNSServiceUpdateRecord_server( RMxMessage *inMessage );
DEBUG_LOCAL void	DNSServiceRemoveRecord_server( RMxMessage *inMessage );

// Service Discovery

DEBUG_LOCAL void	DNSServiceBrowse_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceBrowseCallBack_server(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		uint32_t			inInterfaceIndex,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,  
		const char *		inType,  
		const char *		inDomain,  
		void *				inContext );

DEBUG_LOCAL void	DNSServiceResolve_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceResolveCallBack_server(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,  
		const char *			inHostName,  
		uint16_t				inPort, 
		uint16_t				inTXTSize, 
		const char *			inTXT, 
		void *					inContext );

// Special Purpose

DEBUG_LOCAL void	DNSServiceCreateConnection_server( RMxMessage *inMessage );

DEBUG_LOCAL void	DNSServiceRegisterRecord_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceRegisterRecordCallBack_server( 
		DNSServiceRef		inRef,
		DNSRecordRef		inRecordRef,
		DNSServiceFlags		inFlags,
		DNSServiceErrorType	inErrorCode,
		void *				inContext );

DEBUG_LOCAL void	DNSServiceQueryRecord_server( RMxMessage *inMessage );
DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceQueryRecordCallBack_server(
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

DEBUG_LOCAL void	DNSServiceReconfirmRecord_server( RMxMessage *inMessage );

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

DEBUG_LOCAL RMxServerFlags		gRMxServerFlags				= kRMxServerFlagsNone;
DEBUG_LOCAL SocketRef			gRMxServerSockets[ 2 ] 		= { kInvalidSocketRef, kInvalidSocketRef };
DEBUG_LOCAL HANDLE				gRMxServerSocketEvents[ 2 ]	= { NULL, NULL };
DEBUG_LOCAL HANDLE				gRMxServerQuitDoneEvent		= NULL;

#if 0
#pragma mark -
#pragma mark == RMx ==
#endif

//===========================================================================================================================
//	RMxServerInitialize
//===========================================================================================================================

OSStatus	RMxServerInitialize( RMxServerFlags inFlags )
{
	OSStatus		err;
	
	err = RMxInitialize();
	require_noerr( err, exit );
	
	// Set up the quit done event to allow for waiting until the run loop is quit. A manual-reset event is used so 
	// multiple calls to the stopping code will all see that the quit event has been signaled (not just the first). 
	// The event is also created as signaled so even if run is not called, a stop or finalize will still run successfully.
	// The event will be explicitly reset to an unsignaled state when the run loop starts.
	
	gRMxServerQuitDoneEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	err = translate_errno( gRMxServerQuitDoneEvent, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );
	
	gRMxServerFlags = inFlags;
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxServerFinalize
//===========================================================================================================================

void	RMxServerFinalize( void )
{
	BOOL		ok;
	
	RMxServerStop( kRMxServerStopFlagsNone );
	
	// Release the quit event.
	
	if( gRMxServerQuitDoneEvent )
	{
		ok = CloseHandle( gRMxServerQuitDoneEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		gRMxServerQuitDoneEvent = NULL;
	}
	
	RMxFinalize();
}

//===========================================================================================================================
//	RMxServerRun
//===========================================================================================================================

OSStatus	RMxServerRun( void )
{
	OSStatus		err;
	DWORD			waitCount;
	HANDLE			waitHandles[ 3 ];
	
	// Initialize the server run loop and set up the list of objects to wait for.
	
	err = RMxServerRunInitialize();
	require_noerr( err, exit );
	
	waitCount = 0;
	waitHandles[ waitCount++ ] = gRMxServerSocketEvents[ 0 ];
	waitHandles[ waitCount++ ] = gRMxServerSocketEvents[ 1 ];
	waitHandles[ waitCount++ ] = gRMxStateChangeEvent;
	check( waitCount == sizeof_array( waitHandles ) );
	
	// Main event loop. Process connection requests and state changes (i.e. quit).
	
	while( gRMxState == kRMxStateRun )
	{
		DWORD		result;
		
		result = WaitForMultipleObjects( waitCount, waitHandles, FALSE, INFINITE );
		if( ( result ==   WAIT_OBJECT_0 ) ||				
			( result == ( WAIT_OBJECT_0 + 1 ) ) )	// Socket Events
		{
			SocketRef					sock;
			struct sockaddr_storage		addr;
			socklen_t					addrSize;
			DWORD						index;
			
			index = result - WAIT_OBJECT_0;
			check( ( index == 0 ) || IsValidSocket( gRMxServerSockets[ index ] ) );
			
			// The socket event is only set up to notify on connection requests so try to accept the connection.
			// Note: It is possible for accept to fail if the client aborts the connection after the network stack
			// sees the connection request, but before accept is called. See UNPv1 section 15.6 for more information.
			
			addrSize = sizeof( addr );
			sock = accept( gRMxServerSockets[ index ], (struct sockaddr *) &addr, &addrSize );
			if( IsValidSocket( sock ) )
			{
				if( ( gRMxServerFlags & kRMxServerFlagsAllowRemote ) || SOCKADDR_IS_IP_LOOPBACK( &addr ) )
				{
					dlog( kDebugLevelNotice, "\n" DEBUG_NAME "accepting connection from %##a\n", &addr );
					
					err = RMxSessionOpen( NULL, kRMxSessionFlagsNone, sock, RMxServerMessageCallBack, NULL, NULL, 
						kRMxOpCodeInvalid, NULL );
					check_noerr( err );
				}
				else
				{
					dlog( kDebugLevelNotice, DEBUG_NAME "remote connection attempt from %##a when local-only\n", &addr );
					
					err = close_compat( sock );
					check_translated_errno( err == 0, errno_compat(), kUnknownErr );
				}
			}
			else
			{
				err = errno_compat();
				dlog( kDebugLevelWarning, DEBUG_NAME "connection abort before accept (%d %m)\n", err, err );
			}
		}
		else if( result == ( WAIT_OBJECT_0 + 2 ) )	// State Change Event
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "state change (%d)\n", gRMxState );
			break;
		}
		else
		{
			err = (OSStatus) GetLastError();
			dlog( kDebugLevelAlert, DEBUG_NAME "wait error: 0x%08X, %d (%m)\n", result, err, err );
			err = (OSStatus) result;
			goto exit;
		}
	}
	err = kNoErr;
	
exit:
	RMxServerRunFinalize();
	return( err );
}

//===========================================================================================================================
//	RMxServerRunInitialize
//===========================================================================================================================

DEBUG_LOCAL OSStatus	RMxServerRunInitialize( void )
{
	OSStatus					err;
	struct addrinfo				hints;
	struct addrinfo *			addrList;
	struct addrinfo *			addr;
	int							nAddrs;
	BOOL						ok;
	int							option;
	
	addrList = NULL;
	
	// Set up a socket for each address (only expected to be a single IPv4 and optionally a single IPv6 address). 
	// Windows does not support IPv4-mapped IPv6 addresses so we have to create a separate socket for each type.

	memset( &hints, 0, sizeof( hints ) );
	hints.ai_flags		= AI_PASSIVE;
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;
	
	err = getaddrinfo( NULL, kRMxServerPortString, &hints, &addrList );
	require_noerr( err, exit );
	
	nAddrs = 0;
	for( addr = addrList; addr; addr = addr->ai_next )
	{
		dlog( kDebugLevelTrace, DEBUG_NAME "setting up %s socket\n", 
			( addr->ai_family == AF_INET ) ? "AF_INET" : ( addr->ai_family == AF_INET6 ) ? "AF_INET6" : "<unknown>" );
		if( nAddrs > 1 )
		{
			dlog( kDebugLevelWarning, DEBUG_NAME "cannot handle more than 2 listening sockets (unexpected)\n" );
			break;
		}
		
		gRMxServerSockets[ nAddrs ] = socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
		err = translate_errno( IsValidSocket( gRMxServerSockets[ nAddrs ] ), errno_compat(), kNoResourcesErr );
		if( err != kNoErr )
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "%s socket not supported (%d %m)\n", 
				( addr->ai_family == AF_INET ) ? "AF_INET" : ( addr->ai_family == AF_INET6 ) ? "AF_INET6" : "<unknown>", 
				err, err );
			continue;
		}
		
		// Enable the address-reuse option so the server can be stopped and re-started without the TIME_WAIT delay.
		
		option = 1;
		err = setsockopt( gRMxServerSockets[ nAddrs ], SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof( option ) );
		err = translate_errno( err == 0, errno_compat(), kInUseErr );
		require_noerr( err, exit );
		
		// Set up to signal an event when a connection can be accepted so we can wait for it with WaitForMultipleObjects.
		// Note: WSAEventSelect makes the socket non-blocking so it does not need to made non-blocking separately.
		
		gRMxServerSocketEvents[ nAddrs ] = CreateEvent( NULL, FALSE, FALSE, NULL );
		err = translate_errno( gRMxServerSocketEvents[ nAddrs ], errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		err = WSAEventSelect( gRMxServerSockets[ nAddrs ], gRMxServerSocketEvents[ nAddrs ], FD_ACCEPT );
		err = translate_errno( err == 0, errno_compat(), kInUseErr );
		require_noerr( err, exit );
		
		// Bind to the server port and enable listening for connections.
		
		err = bind( gRMxServerSockets[ nAddrs ], addr->ai_addr, (int) addr->ai_addrlen );
		err = translate_errno( err == 0, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		err = listen( gRMxServerSockets[ nAddrs ], SOMAXCONN );
		err = translate_errno( err == 0, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		++nAddrs;
	}
	require_action( nAddrs > 0, exit, err = kUnknownErr );
	
	// Create a dummy event (which will never be signaled) to simplify WaitForMultipleObjects usage,
	
	if( !gRMxServerSocketEvents[ 1 ] )
	{
		gRMxServerSocketEvents[ 1 ] = CreateEvent( NULL, FALSE, FALSE, NULL );
		err = translate_errno( gRMxServerSocketEvents[ 1 ], errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
	}
	
	// Reset the quit event to an unsignaled state so other code can wait for the run loop to quit.
	
	gRMxState = kRMxStateRun;
	ok = ResetEvent( gRMxServerQuitDoneEvent );
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );
	
exit:
	if( addrList )
	{
		freeaddrinfo( addrList );
	}
	if( err != kNoErr )
	{
		RMxServerRunFinalize();
	}
	return( err );
}

//===========================================================================================================================
//	RMxServerRunFinalize
//
//	Note: This routine is safe to call multiple times (for easier cleanup).
//===========================================================================================================================

DEBUG_LOCAL void	RMxServerRunFinalize( void )
{
	OSStatus		err;
	BOOL			ok;
	
	// Release the socket events
	
	if( gRMxServerSocketEvents[ 0 ] )
	{
		ok = CloseHandle( gRMxServerSocketEvents[ 0 ] );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		gRMxServerSocketEvents[ 0 ] = NULL;
	}
	if( gRMxServerSocketEvents[ 1 ] )
	{
		ok = CloseHandle( gRMxServerSocketEvents[ 1 ] );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		gRMxServerSocketEvents[ 1 ] = NULL;
	}
	
	// Close the sockets.
	
	if( IsValidSocket( gRMxServerSockets[ 0 ] ) )
	{
		err = close_compat( gRMxServerSockets[ 0 ] );
		check_translated_errno( err == 0, errno_compat(), kUnknownErr );
		gRMxServerSockets[ 0 ] = kInvalidSocketRef;
	}
	if( IsValidSocket( gRMxServerSockets[ 1 ] ) )
	{
		err = close_compat( gRMxServerSockets[ 1 ] );
		check_translated_errno( err == 0, errno_compat(), kUnknownErr );
		gRMxServerSockets[ 1 ] = kInvalidSocketRef;
	}
	
	// Signal the quit event to indicate the run loop has quit.
	
	ok = SetEvent( gRMxServerQuitDoneEvent );
	check_translated_errno( ok, errno_compat(), kUnknownErr );
}

//===========================================================================================================================
//	RMxServerStop
//===========================================================================================================================

OSStatus	RMxServerStop( RMxServerStopFlags inFlags )
{
	BOOL		ok;
	DWORD		result;
	
	// Signal a state changed event to trigger everything to stop. Note: This is a manual-reset event so it will 
	// remain signaled to allow all running threads to wake up and exit cleanly.
	
	gRMxState = kRMxStateStop;
	if( gRMxStateChangeEvent )
	{
		ok = SetEvent( gRMxStateChangeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
	}
	
	// Wait for the quit event indicating the run loop has quit. Give up after 10 seconds to handle a hung thread.
	
	if( !( inFlags & kRMxServerStopFlagsNoWait ) )
	{
		if( gRMxServerQuitDoneEvent )
		{
			result = WaitForSingleObject( gRMxServerQuitDoneEvent, 10 * 1000 );
			check_translated_errno( result == WAIT_OBJECT_0, errno_compat(), result );
		}
	}
	
	return( kNoErr );
}

//===========================================================================================================================
//	RMxServerMessageCallBack
//===========================================================================================================================

DEBUG_LOCAL void	RMxServerMessageCallBack( RMxMessage *inMessage )
{
	DNSServiceRef		obj;
	OSStatus			err;
	
	check( inMessage );
	check( inMessage->session );
	
	switch( inMessage->opcode )
	{
		// General
		
		case kRMxOpCodeInvalid:
			
			// The session is closing so delete the object (if still valid). Invalidate the session because the session
			// is already in the process of closing so we don't want the deallocate code closing it again.
			
			obj = (DNSServiceRef) inMessage->context;
			if( obj )
			{
				obj->sessionClosing = true;
				DNSServiceRefDeallocate_server( obj );
			}
			break;
		
		case kRMxOpCodeCheckVersion:
			DNSServiceCheckVersion_server( inMessage );
			break;
		
		// Properties
		
		case kRMxOpCodeCopyProperty:
			DNSServiceCopyProperty_server( inMessage );
			break;
		
		// Domain Enumeration
		
		case kRMxOpCodeEnumerateDomains:
			DNSServiceEnumerateDomains_server( inMessage );
			break;
		
		// Service Registration
		
		case kRMxOpCodeRegister:
			DNSServiceRegister_server( inMessage );
			break;
		
		case kRMxOpCodeAddRecord:
			DNSServiceAddRecord_server( inMessage );
			break;
		
		case kRMxOpCodeUpdateRecord:
			DNSServiceUpdateRecord_server( inMessage );
			break;
		
		case kRMxOpCodeRemoveRecord:
			DNSServiceRemoveRecord_server( inMessage );
			break;
		
		// Service Discovery
		
		case kRMxOpCodeBrowse:
			DNSServiceBrowse_server( inMessage );
			break;
		
		case kRMxOpCodeResolve:
			DNSServiceResolve_server( inMessage );
			break;
		
		// Special Purpose
		
		case kRMxOpCodeCreateConnection:
			DNSServiceCreateConnection_server( inMessage );
			break;
		
		case kRMxOpCodeRegisterRecord:
			DNSServiceRegisterRecord_server( inMessage );
			break;
		
		case kRMxOpCodeQueryRecord:
			DNSServiceQueryRecord_server( inMessage );
			break;
		
		case kRMxOpCodeReconfirmRecord:
			DNSServiceReconfirmRecord_server( inMessage );
			break;
		
		default:
			dlog( kDebugLevelWarning, DEBUG_NAME "message with unknown opcode received (%d)\n", inMessage->opcode );
			err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, kUnsupportedErr, "" );
			check_noerr( err );
			break;
	}
}

#if 0
#pragma mark -
#pragma mark == DNS-SD General ==
#endif

//===========================================================================================================================
//	DNSServiceRefDeallocate_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRefDeallocate_server( DNSServiceRef inRef )
{
	OSStatus		err;
	
	check( inRef );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: ref=%#p\n", __ROUTINE__, inRef );
		
	// Release the direct object if active.
	
	if( inRef->directObj )
	{
		DNSServiceRefDeallocate_direct( inRef->directObj );
	}
	
	// Close the session.
	
	if( inRef->session && !inRef->sessionClosing )
	{
		inRef->session->callback		= NULL;
		inRef->session->message.context	= NULL;
		
		err = RMxSessionClose( inRef->session, kEndingErr );
		check_noerr( err );
	}
	
	// Release any extra records.
	
	while( inRef->records )
	{
		DNSRecordRef		record;
		
		record = inRef->records;
		inRef->records = record->next;
		
		free( record );
	}
	
	// Release the object.
	
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceCheckVersion_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceCheckVersion_server( RMxMessage *inMessage )
{
	OSStatus		err;
	uint32_t		clientCurrentVersion;
	uint32_t		clientOldestClientVersion;
	uint32_t		clientOldestServerVersion;
	
	check( inMessage );
	check( inMessage->session );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "www", 
		&clientCurrentVersion, &clientOldestClientVersion, &clientOldestServerVersion );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: clientCurrent=%v, clientOldestClient=%v, clientOldestServer=%v\n", 
		__ROUTINE__, clientCurrentVersion, clientOldestClientVersion, clientOldestServerVersion );
	
	err = RMxCheckVersion( clientCurrentVersion, clientOldestClientVersion, clientOldestServerVersion, 
						   kRMxCurrentVersion, kRMxOldestClientVersion, kRMxOldestServerVersion );
		
	// Send the response and our version information. Cause the session to quit since this is a one-shot session.
	
exit:
	err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, kNoErr, "wwww", 
		err, kRMxCurrentVersion, kRMxOldestClientVersion, kRMxOldestServerVersion );
	check_noerr( err );
	
	inMessage->session->quit = true;
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Properties ==
#endif

//===========================================================================================================================
//	DNSServiceCopyProperty_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceCopyProperty_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSPropertyCode		code;
	
	check( inMessage );
	check( inMessage->session );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "w", &code );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: code='%C'\n", __ROUTINE__, code );
	
		
	// Send the response and our version information. Cause the session to quit since this is a one-shot session.
	
exit:
	err = DNSServiceCopyPropertySendData_server( code, inMessage );
	check_noerr( err );
	
	inMessage->session->quit = true;
}

//===========================================================================================================================
//	DNSServiceCopyPropertySendData_server
//===========================================================================================================================

DEBUG_LOCAL OSStatus	DNSServiceCopyPropertySendData_server( DNSPropertyCode inCode, RMxMessage *inMessage )
{
	OSStatus		err;
	
	switch( inCode )
	{
		case kDNSPropertyCodeVersion:
			
			// Note: This manually builds the "n" data type using a "w" size followed by the data to avoid a temporary buffer.
			
			err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, kNoErr, "ww wwww", kNoErr, inCode, 
				4 + 4 + 4, kRMxCurrentVersion, kRMxOldestClientVersion, kRMxOldestServerVersion );
			require_noerr( err, exit );
			break;
		
		default:
			dlog( kDebugLevelWarning, DEBUG_NAME "%s: unknown property code (%C)\n", __ROUTINE__, inCode );
			err = kDNSServiceErr_Unsupported;
			err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, kNoErr, "wn", err, 0, NULL );
			require_noerr( err, exit );
			break;
	}
	err = kNoErr;
	
exit:
	return( err );
}

#if 0
#pragma mark DNSServiceReleaseProperty_server
#endif

// Note: DNSServiceReleaseProperty_server is not needed on the server side because memory is released immediately after sending.

#if 0
#pragma mark -
#pragma mark == DNS-SD Domain Enumeration ==
#endif

//===========================================================================================================================
//	DNSServiceEnumerateDomains_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceEnumerateDomains_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "ww", &flags, &interfaceIndex );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: flags=0x%08X, ifi=%d\n", __ROUTINE__, flags, interfaceIndex );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceEnumerateDomains_direct( &obj->directObj, flags, interfaceIndex, 
		DNSServiceEnumerateDomainsCallBack_server, obj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceEnumerateDomainsCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceEnumerateDomainsCallBack_server(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		uint32_t			inInterfaceIndex,
		DNSServiceErrorType	inErrorCode,
		const char *		inDomain,  
		void *				inContext )
{
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
	
	check( inDomain );
	obj = (DNSServiceRef) inContext;
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeEnumerateDomains, kNoErr, "wwws", 
		inFlags, inInterfaceIndex, inErrorCode, inDomain );
	check_noerr( err );
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Service Registration ==
#endif

//===========================================================================================================================
//	DNSServiceRegister_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegister_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		name;
	const char *		type;
	const char *		domain;
	const char *		host;
	uint16_t			port;
	const void *		txt;
	size_t				txtSize;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwsssshn", 
		&flags, &interfaceIndex, &name, NULL, &type, NULL, &domain, NULL, &host, NULL, &port, &txt, &txtSize );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME
		"%s: flags=0x%08X, ifi=%ld, name=\"%s\", type=\"%s\", domain=\"%s\", host=\"%s\", port=%d, txtSize=%d\n", 
		__ROUTINE__, flags, interfaceIndex, name, type, domain, host, ntohs( port ), txtSize );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceRegister_direct( &obj->directObj, flags, interfaceIndex, name, type, domain, host, port, 
		(uint16_t) txtSize, txt, DNSServiceRegisterCallBack_server, obj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceRegisterCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceRegisterCallBack_server(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,  
		const char *		inType,  
		const char *		inDomain,  
		void *				inContext )
{
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
	
	check( inDomain );
	obj = (DNSServiceRef) inContext;
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeRegister, kNoErr, "wwsss", 
		inFlags, inErrorCode, inName, inType, inDomain );
	check_noerr( err );
}

//===========================================================================================================================
//	DNSServiceAddRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceAddRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSRecordRef		obj;
	DNSServiceRef		ref;
	uint32_t			id;
	DNSServiceFlags		flags;
	uint16_t			rrType;
	uint8_t *			rData;
	size_t				rDataSize;
	uint32_t			ttl;
	DNSRecordRef *		p;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	ref = (DNSServiceRef) inMessage->context;
	require_action( ref, exit, err = kNotInitializedErr );
	require_action( ref->opcode == kRMxOpCodeRegister, exit, err = kTypeErr );
	check( ref->directObj );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwhnw", &id, &flags, &rrType, &rData, &rDataSize, &ttl );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: id=%d, flags=0x%08X, rrType=%d, rDataSize=%d, ttl=%d\n", 
		__ROUTINE__, id, flags, rrType, rDataSize, ttl );
	
	// Allocate and initialize the object and add it to the list of records.
	
	obj = (DNSRecordRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->id 		= id;
	
	obj->next 		= ref->records;
	ref->records	= obj;
	
	err = DNSServiceAddRecord_direct( ref->directObj, &obj->directObj, flags, rrType, (uint16_t) rDataSize, rData, ttl );
	require_noerr( err, exit );
	
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		for( p = &ref->records; *p; p = &( *p )->next )
		{
			if( *p == obj )
			{
				*p = obj->next;
				free( obj );
				break;
			}
		}
	}
}

//===========================================================================================================================
//	DNSServiceUpdateRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceUpdateRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		ref;
	uint32_t			id;
	DNSServiceFlags		flags;
	uint8_t *			rData;
	size_t				rDataSize;
	uint32_t			ttl;
	DNSRecordRef		obj;
	
	check( inMessage );
	check( inMessage->session );
	ref = (DNSServiceRef) inMessage->context;
	require_action( ref, exit, err = kNotInitializedErr );
	require_action( ( ref->opcode == kRMxOpCodeRegister ) || ( ref->opcode == kRMxOpCodeCreateConnection ), exit, err = kTypeErr );
	check( ref->directObj );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwnw", &id, &flags, &rData, &rDataSize, &ttl );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: id=%d, flags=0x%08X, rDataSize=%d, ttl=%d\n", __ROUTINE__, id, flags, rDataSize, ttl );
	
	// Find the record with the specified ID and update it.
	
	if( id == kDNSRecordIndexDefaultTXT )
	{
		obj = NULL;
	}
	else
	{
		for( obj = ref->records; obj; obj = obj->next )
		{
			if( obj->id == id )
			{
				break;
			}
		}
		require_action( obj, exit, err = kNotFoundErr );
		check( obj->directObj );
	}
	
	err = DNSServiceUpdateRecord_direct( ref->directObj, obj->directObj, flags, (uint16_t) rDataSize, rData, ttl );
	require_noerr( err, exit );
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
}

//===========================================================================================================================
//	DNSServiceRemoveRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRemoveRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		ref;
	uint32_t			id;
	DNSServiceFlags		flags;
	DNSRecordRef *		p;
	DNSRecordRef		obj;
	DNSRecordRef		directObj;
	
	check( inMessage );
	check( inMessage->session );
	ref = (DNSServiceRef) inMessage->context;
	require_action( ref, exit, err = kNotInitializedErr );
	require_action( ( ref->opcode == kRMxOpCodeRegister ) || ( ref->opcode == kRMxOpCodeCreateConnection ), exit, err = kTypeErr );
	check( ref->directObj );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "ww", &id, &flags );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: id=%d, flags=0x%08X\n", __ROUTINE__, id, flags );
	
	// Find the record with the specified ID, remove it from the list, and free it.
	
	for( p = &ref->records; *p; p = &( *p )->next )
	{
		if( ( *p )->id == id )
		{
			break;
		}
	}
	obj = *p;
	require_action( obj, exit, err = kNotFoundErr );
	*p = obj->next;
	
	directObj = obj->directObj;
	check( directObj );
	free( obj );
	
	err = DNSServiceRemoveRecord_direct( ref->directObj, directObj, flags );
	require_noerr( err, exit );
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Service Discovery ==
#endif

//===========================================================================================================================
//	DNSServiceBrowse_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceBrowse_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		type;
	const char *		domain;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwss", &flags, &interfaceIndex, &type, NULL, &domain, NULL );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: flags=0x%08X, ifi=%d, type=\"%s\", domain=\"%s\"\n", 
		__ROUTINE__, flags, interfaceIndex, type, domain );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceBrowse_direct( &obj->directObj, flags, interfaceIndex, type, domain, DNSServiceBrowseCallBack_server, obj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceBrowseCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceBrowseCallBack_server(
		DNSServiceRef		inRef,
		DNSServiceFlags		inFlags,
		uint32_t			inInterfaceIndex,
		DNSServiceErrorType	inErrorCode,
		const char *		inName,  
		const char *		inType,  
		const char *		inDomain,  
		void *				inContext )
{
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
	
	check( inName );
	check( inType );
	check( inDomain );
	obj = (DNSServiceRef) inContext;
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeBrowse, kNoErr, "wwwsss", 
		inFlags, inInterfaceIndex, inErrorCode, inName, inType, inDomain );
	check_noerr( err );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	DNSServiceResolve_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceResolve_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		name;
	const char *		type;
	const char *		domain;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwsss", 
		&flags, &interfaceIndex, &name, NULL, &type, NULL, &domain, NULL );
	require_noerr( err, exit );

	dlog( kDebugLevelTrace, DEBUG_NAME "%s: flags=0x%08X, ifi=%d, name=\"%s\", type=\"%s\", domain=\"%s\"\n", 
		__ROUTINE__, flags, interfaceIndex, name, type, domain );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceResolve_direct( &obj->directObj, flags, interfaceIndex, name, type, domain, 
		DNSServiceResolveCallBack_server, obj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceResolveCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceResolveCallBack_server(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,  
		const char *			inHostName,  
		uint16_t				inPort, 
		uint16_t				inTXTSize, 
		const char *			inTXT, 
		void *					inContext )
{
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
	
	check( inFullName );
	check( inHostName );
	obj = (DNSServiceRef) inContext;
	dlog( kDebugLevelTrace, DEBUG_NAME "%s: ref=%#p, directRef=%#p\n", __ROUTINE__, obj, inRef );
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeResolve, kNoErr, "wwwsshn", 
		inFlags, inInterfaceIndex, inErrorCode, inFullName, inHostName, inPort, inTXTSize, inTXT );
	check_noerr( err );
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Special Purpose ==
#endif

//===========================================================================================================================
//	DNSServiceCreateConnection_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceCreateConnection_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "%s\n", __ROUTINE__ );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceCreateConnection_direct( &obj->directObj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceRegisterRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSRecordRef		obj;
	DNSServiceRef		ref;
	uint32_t			id;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		name;
	uint16_t			rrType;
	uint16_t			rrClass;
	uint8_t *			rData;
	size_t				rDataSize;
	uint32_t			ttl;
	DNSRecordRef *		p;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	ref = (DNSServiceRef) inMessage->context;
	require_action( ref, exit, err = kNotInitializedErr );
	require_action( ref->opcode == kRMxOpCodeCreateConnection, exit, err = kTypeErr );
	check( ref->directObj );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwwshhnw", 
		&id, &flags, &interfaceIndex, &name, NULL, &rrType, &rrClass, &rData, &rDataSize, &ttl );
	require_noerr( err, exit );
	
	dlog( kDebugLevelTrace, DEBUG_NAME
		"%s: id=%d, flags=0x%08X, ifi=%d, name=\"%s\", rrType=%d, rrClass=%d, rDataSize=%d, ttl=%d\n", 
		__ROUTINE__, id, flags, interfaceIndex, name, rrType, rrClass, rDataSize, ttl );
	
	// Allocate and initialize the object and add it to the list of records.
	
	obj = (DNSRecordRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->id 		= id;
	obj->owner		= ref;
	
	obj->next 		= ref->records;
	ref->records	= obj;
	
	err = DNSServiceRegisterRecord_direct( ref->directObj, &obj->directObj, flags, interfaceIndex, name, rrType, rrClass, 
		(uint16_t) rDataSize, rData, ttl, DNSServiceRegisterRecordCallBack_server, obj );
	require_noerr( err, exit );
	
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		for( p = &ref->records; *p; p = &( *p )->next )
		{
			if( *p == obj )
			{
				*p = obj->next;
				free( obj );
				break;
			}
		}
	}
}

//===========================================================================================================================
//	DNSServiceRegisterRecordCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceRegisterRecordCallBack_server( 
		DNSServiceRef		inRef,
		DNSRecordRef		inRecordRef,
		DNSServiceFlags		inFlags,
		DNSServiceErrorType	inErrorCode,
		void *				inContext )
{
	DNSRecordRef		record;
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
	DEBUG_UNUSED( inRecordRef );
	
	record = (DNSRecordRef) inContext;
	check( record );
	obj = record->owner;
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeRegisterRecord, kNoErr, "www", inFlags, inErrorCode, record->id );
	check_noerr( err );
}

//===========================================================================================================================
//	DNSServiceQueryRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceQueryRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		name;
	uint16_t			rrType;
	uint16_t			rrClass;
	
	obj = NULL;
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwshh", 
		&flags, &interfaceIndex, &name, NULL, &rrType, &rrClass );
	require_noerr( err, exit );

	dlog( kDebugLevelTrace, DEBUG_NAME "%s: flags=0x%08X, ifi=%d, name=\"%s\", rrType=%d, rrClass=%d\n", 
		__ROUTINE__, flags, interfaceIndex, name, rrType, rrClass );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );
	
	obj->opcode  = inMessage->opcode;
	obj->session = inMessage->session;
	
	err = DNSServiceQueryRecord_direct( &obj->directObj, flags, interfaceIndex, name, rrType, rrClass, 
		DNSServiceQueryRecordCallBack_server, obj );
	require_noerr( err, exit );
	
	// Success!
	
	inMessage->session->message.context = obj;
	obj = NULL;
	
exit:
	if( err != kNoErr )
	{
		err = RMxSessionSendMessage( inMessage->session, inMessage->opcode, err, "" );
		check_noerr( err );
	}
	if( obj )
	{
		DNSServiceRefDeallocate_server( obj );
	}
}

//===========================================================================================================================
//	DNSServiceQueryRecordCallBack_server
//===========================================================================================================================

DEBUG_LOCAL void CALLBACK_COMPAT
	DNSServiceQueryRecordCallBack_server(
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
	DNSServiceRef		obj;
	OSStatus			err;
	
	DEBUG_UNUSED( inRef );
		
	obj = (DNSServiceRef) inContext;
	check( obj );
	check( obj->session );
	
	err = RMxSessionSendMessage( obj->session, kRMxOpCodeQueryRecord, kNoErr, "wwwshhnw", 
		inFlags, inInterfaceIndex, inErrorCode, inName, inRRType, inRRClass, inRDataSize, inRData, inTTL );
	check_noerr( err );
}

//===========================================================================================================================
//	DNSServiceReconfirmRecord_server
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceReconfirmRecord_server( RMxMessage *inMessage )
{
	OSStatus			err;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	const char *		name;
	uint16_t			rrType;
	uint16_t			rrClass;
	uint8_t *			rData;
	size_t				rDataSize;
	
	check( inMessage );
	check( inMessage->session );
	require_action( !inMessage->session->message.context, exit, err = kTypeErr );
	
	// Extract parameters from the message.
	
	err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwshhn", 
		&flags, &interfaceIndex, &name, NULL, &rrType, &rrClass, &rData, &rDataSize );
	require_noerr( err, exit );

	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "%s: flags=0x%08X, ifi=%d, name=\"%s\", rrType=%d, rrClass=%d, rDataSize=%d\n", 
		__ROUTINE__, flags, interfaceIndex, name, rrType, rrClass, rDataSize );
	
	// Trigger the reconfirm then cause the session to quit.
	
	DNSServiceReconfirmRecord_direct( flags, interfaceIndex, name, rrType, rrClass, (uint16_t) rDataSize, rData );
	require_noerr( err, exit );
	
exit:
	inMessage->session->quit = true;
	return;
}

#ifdef	__cplusplus
	}
#endif
