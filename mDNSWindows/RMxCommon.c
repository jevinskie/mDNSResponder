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
    
$Log: RMxCommon.c,v $
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

#include	<process.h>

#include	"RMxCommon.h"

#ifdef	__cplusplus
	extern "C" {
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME						"[RMxCommon] "

#define	kRMxSessionOpenValidFlags		( kRMxSessionFlagsNoThread | kRMxSessionFlagsNoClose )

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// Session

DEBUG_LOCAL unsigned WINAPI	RMxSessionThread( LPVOID inParam );
DEBUG_LOCAL OSStatus		RMxSessionInitServer( RMxSessionRef inSession );
DEBUG_LOCAL OSStatus		RMxSessionInitClient( RMxSessionRef inSession, const char *inServer );
DEBUG_LOCAL OSStatus		RMxSessionConnect( RMxSessionRef inSession, const struct sockaddr *inAddr, size_t inAddrSize );
DEBUG_LOCAL OSStatus
	RMxSessionSendMessageVAList( 
		RMxSessionRef 	inSession, 
		RMxOpCode 		inOpCode, 
		OSStatus 		inStatus, 
		const char *	inFormat, 
		va_list			inArgs1, 
		va_list			inArgs2 );

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

// General

DEBUG_LOCAL CRITICAL_SECTION		gRMxLock;
DEBUG_LOCAL bool					gRMxLockInitialized		= false;
DEBUG_LOCAL RMxSessionRef			gRMxSessionList			= NULL;
RMxState							gRMxState				= kRMxStateInvalid;
HANDLE								gRMxStateChangeEvent	= NULL;

#if 0
#pragma mark -
#pragma mark == General ==
#endif

//===========================================================================================================================
//	RMxInitialize
//===========================================================================================================================

OSStatus	RMxInitialize( void )
{
	OSStatus		err;
	WSADATA			wsaData;
	
	// Set up WinSock.
	
	err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	require_noerr( err, exit );
	require_action( ( LOBYTE( wsaData.wVersion ) == 2 ) && ( HIBYTE( wsaData.wVersion ) == 2 ), exit, err = kUnsupportedErr );
	
	// Set up the global locked used to protect things like the session list.
	
	InitializeCriticalSection( &gRMxLock );
	gRMxLockInitialized = true;
	
	// Set up the state and state changed event. A manual-reset event is used so all threads wake up when it is signaled.
	
	gRMxState = kRMxStateInvalid;
	gRMxStateChangeEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	err = translate_errno( gRMxStateChangeEvent, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxFinalize
//===========================================================================================================================

void	RMxFinalize( void )
{
	BOOL			ok;
	OSStatus		err;
	
	// Signal a state changed event to trigger everything to stop. Note: This is a manual-reset event so it will 
	// remain signaled to allow all running threads to wake up and exit cleanly.
	
	gRMxState = kRMxStateStop;
	if( gRMxStateChangeEvent )
	{
		ok = SetEvent( gRMxStateChangeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
	}
	
	// Close any open sessions.
	
	while( gRMxSessionList )
	{
		err = RMxSessionClose( gRMxSessionList, kEndingErr );
		check( ( err == kNoErr ) || ( err == kNotFoundErr ) );
	}
	
	// Release the state changed event.
	
	if( gRMxStateChangeEvent )
	{
		ok = CloseHandle( gRMxStateChangeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		gRMxStateChangeEvent = NULL;
	}
	
	// Release the lock.
	
	if( gRMxLockInitialized )
	{
		gRMxLockInitialized = false;
		DeleteCriticalSection( &gRMxLock );
	}
	
	// Tear down WinSock.
	
	WSACleanup();
}

//===========================================================================================================================
//	RMxLock
//===========================================================================================================================

void	RMxLock( void )
{
	check( gRMxLockInitialized );
	if( gRMxLockInitialized )
	{
		EnterCriticalSection( &gRMxLock );
	}
}

//===========================================================================================================================
//	RMxUnlock
//===========================================================================================================================

void	RMxUnlock( void )
{
	check( gRMxLockInitialized );
	if( gRMxLockInitialized )
	{
		LeaveCriticalSection( &gRMxLock );
	}
}

#if 0
#pragma mark -
#pragma mark == Messages ==
#endif

//===========================================================================================================================
//	RMxMessageInitialize
//===========================================================================================================================

void	RMxMessageInitialize( RMxMessage *inMessage )
{
	check( inMessage );
	if( inMessage )
	{
		inMessage->sendSize 	= 0;
		inMessage->sendData 	= NULL;
		inMessage->recvSize 	= 0;
		inMessage->recvData 	= NULL;
		inMessage->bufferSize 	= 0;
		inMessage->buffer 		= NULL;
	}
}

//===========================================================================================================================
//	RMxMessageRelease
//===========================================================================================================================

void	RMxMessageRelease( RMxMessage *inMessage )
{
	check( inMessage );
	if( inMessage )
	{
		// Assert if a malloc'd buffer was used for a small message that could have used the inline message buffer.
		
		check(  !inMessage->recvData 									|| 
			   ( inMessage->recvSize > sizeof( inMessage->storage ) ) 	|| 
			   ( inMessage->recvData == inMessage->storage ) );
		
		// Free the memory if it was malloc'd (non-null and not the inline message buffer).
		
		if( inMessage->recvData && ( inMessage->recvData != inMessage->storage ) )
		{
			free( inMessage->recvData );
		}
		inMessage->recvData = NULL;
	}
}

#if 0
#pragma mark -
#pragma mark == Sessions ==
#endif

//===========================================================================================================================
//	RMxSessionOpen
//===========================================================================================================================

OSStatus
	RMxSessionOpen( 
		const char *		inServer, 
		RMxSessionFlags		inFlags, 
		SocketRef 			inSock, 
		RMxMessageCallBack 	inCallBack, 
		void *				inContext, 
		RMxSessionRef *		outSession, 
		RMxOpCode 			inMessageOpCode, 
		const char *		inMessageFormat, 
		... )
{
	OSStatus			err;
	RMxSessionRef		session;
	DWORD				result;
	bool				locked;
	
	session	= NULL;
	locked	= false;
	dlog( kDebugLevelNotice, DEBUG_NAME "opening %s session to %s\n", IsValidSocket( inSock ) ? "server" : "client", 
		inServer ? inServer : "<local>" );
	require_action( gRMxState == kRMxStateRun, exit, err = kStateErr );
	require_action( ( inFlags & ~kRMxSessionOpenValidFlags ) == 0, exit, err = kFlagErr );
	
	// Allocate and initialize the object and add it to the session list.
	
	session = (RMxSessionRef) calloc( 1, sizeof( *session ) );
	require_action( session, exit, err = kNoMemoryErr );
	
	session->flags					= inFlags;				
	session->sock 					= kInvalidSocketRef;	
	session->callback 				= inCallBack;
	RMxMessageInitialize( &session->message );
	session->message.context 		= inContext;
	session->message.session		= session;
	session->messageRecvBuffer		= session->message.storage;
	session->messageRecvBufferPtr	= session->messageRecvBuffer;
	session->messageRecvRemaining	= kRMxMessageHeaderSize;
	session->messageRecvHeaderDone 	= false;
	
	RMxLock();
	session->next 	= gRMxSessionList;
	gRMxSessionList	= session;
	RMxUnlock();
	
	session->closeEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	err = translate_errno( session->closeEvent, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );
	
	// Set up the sockets and socket events.
	
	if( IsValidSocket( inSock ) )
	{
		// Server: accepted a connection from a client.
		
		session->flags |= kRMxSessionFlagsServer;
		session->sock 	= inSock;
		inSock 			= kInvalidSocketRef;
		
		err = RMxSessionInitServer( session );
		require_noerr( err, exit );
	}
	else
	{
		// Client: initiate a connection to the server.
		
		err = RMxSessionInitClient( session, inServer );
		require_noerr( err, exit );
	}
	
	// Create thread with _beginthreadex() instead of CreateThread() to avoid memory leaks when using static run-time 
	// libraries. See <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp>.
	// Create the thread suspended then resume it so the thread handle and ID are valid before the thread starts running.
	
	RMxLock();
	locked = true;
	if( !( inFlags & kRMxSessionFlagsNoThread ) )
	{
		session->thread = (HANDLE) _beginthreadex_compat( NULL, 0, RMxSessionThread, session, CREATE_SUSPENDED, &session->threadID );
		err = translate_errno( session->thread, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		result = ResumeThread( session->thread );
		err = translate_errno( result != (DWORD) -1, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
	}
	
	// Send the optional message. Note: we have to do 2 va_start/va_end because va_copy is not widely supported yet.
	
	if( inMessageFormat )
	{
		va_list		args1;
		va_list		args2;
		
		va_start( args1, inMessageFormat );
		va_start( args2, inMessageFormat );
		err = RMxSessionSendMessageVAList( session, inMessageOpCode, kNoErr, inMessageFormat, args1, args2 );
		va_end( args1 );
		va_end( args2 );
		require_noerr( err, exit );
	}
	
	// Success!
	
	if( outSession )
	{
		*outSession	= session;
	}
	session = NULL;
	
exit:
	if( locked )
	{
		RMxUnlock();
	}
	if( session )
	{
		RMxSessionClose( session, err );
	}
	if( IsValidSocket( inSock ) )
	{
		close_compat( inSock );
	}
	return( err );
}

//===========================================================================================================================
//	RMxSessionClose
//===========================================================================================================================

OSStatus	RMxSessionClose( RMxSessionRef inSession, OSStatus inReason )
{
	OSStatus			err;
	bool				locked;
	RMxSessionRef *		p;
	bool				sameThread;
	bool				deferClose;
	BOOL				ok;
	DWORD 				threadID;
	DWORD				result;
	
	DEBUG_USE_ONLY( inReason );
	
	check( inSession );
	
	// Find the session in the list.
	
	RMxLock();
	locked = true;
	
	for( p = &gRMxSessionList; *p; p = &( *p )->next )
	{
		if( *p == inSession )
		{
			break;
		}
	}
	require_action( *p, exit, err = kNotFoundErr );
	
	// If we're being called from the same thread as the session (e.g. message callback is closing the session) then 
	// we must defer the close until the thread is done because the thread is still using the session object.
	
	deferClose	= false;
	threadID	= GetCurrentThreadId();
	sameThread	= inSession->thread && ( threadID == inSession->threadID );
	if( sameThread && !( inSession->flags & kRMxSessionFlagsThreadDone ) )
	{
		inSession->flags &= ~kRMxSessionFlagsNoClose;
		deferClose = true;
	}
	
	// If the thread we're not being called from the session thread, but the thread has already marked itself as
	// as done (e.g. session closed from something like a peer disconnect and at the same time the client also 
	// tried to close) then we only want to continue with the close if the thread is not going to close itself.
	
	if( !sameThread && ( inSession->flags & kRMxSessionFlagsThreadDone ) && !( inSession->flags & kRMxSessionFlagsNoClose ) )
	{
		deferClose = true;
	}
	
	// Signal a close so the thread exits.
	
	inSession->quit = true;
	if( inSession->closeEvent )
	{
		ok = SetEvent( inSession->closeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
	}	
	if( deferClose )
	{
		err = kNoErr;
		goto exit;
	}
	inSession->flags |= kRMxSessionFlagsNoClose;
	
	// Remove the session from the list.
	
	*p = inSession->next;
	
	RMxUnlock();
	locked = false;
	
	// Wait for the thread to exit. Give up after 10 seconds to handle a hung thread.
	
	if( inSession->thread && ( threadID != inSession->threadID ) )
	{
		result = WaitForSingleObject( inSession->thread, 10 * 1000 );
		check_translated_errno( result == WAIT_OBJECT_0, (OSStatus) GetLastError(), result );
	}
	
	// Release the thread.
	
	if( inSession->thread )
	{
		ok = CloseHandle( inSession->thread );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		inSession->thread = NULL;
	}
	
	// Release the socket event.
	
	if( inSession->sockEvent )
	{
		ok = CloseHandle( inSession->sockEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		inSession->sockEvent = NULL;
	}
	
	// Close the socket.
	
	if( IsValidSocket( inSession->sock ) )
	{
		err = close_compat( inSession->sock );
		err = translate_errno( err == 0, errno_compat(), kUnknownErr );
		check_noerr( err );
		inSession->sock = kInvalidSocketRef;
	}
	
	// Release the close event.
	
	if( inSession->closeEvent )
	{
		ok = CloseHandle( inSession->closeEvent );
		check_translated_errno( ok, errno_compat(), kUnknownErr );
		inSession->closeEvent = NULL;
	}
	
	// Release the memory used by the object.
	
	RMxMessageRelease( &inSession->message );
	free( inSession );
	err = kNoErr;
	
	dlog( kDebugLevelNotice, DEBUG_NAME "session closed (%d %m)\n", inReason, inReason );
	
exit:
	if( locked )
	{
		RMxUnlock();
	}
	return( err );
}

//===========================================================================================================================
//	RMxSessionThread
//===========================================================================================================================

DEBUG_LOCAL unsigned WINAPI	RMxSessionThread( LPVOID inParam )
{
	OSStatus			err;
	RMxSessionRef		session;
	bool				safeToClose;
	
	session = (RMxSessionRef) inParam;
	check( session );
	
	// Process messages until the session is told to close or the remote site disconnects.
	
	for( ;; )
	{
		if( session->quit || ( gRMxState != kRMxStateRun ) )
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session state exit (quit=%d, state=%d)\n", session->quit, gRMxState );
			err = kEndingErr;
			break;
		}
		
		err = RMxSessionRecvMessage( session, INFINITE );
		if( err == kNoErr )
		{
			if( session->callback )
			{
				session->callback( &session->message );
			}
			RMxMessageRelease( &session->message );
		}
		else
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session closing (%d %m)\n", err, err );
			break;
		}
	}
	
	// Tell the callback the session is closing.
	
	if( session->callback )
	{
		session->message.opcode 	= kRMxOpCodeInvalid;
		session->message.status		= kNoErr;
		session->message.sendSize	= 0;
		session->message.sendData	= NULL;
		session->message.recvSize	= 0;
		session->message.recvData	= NULL;
		session->callback( &session->message );
	}
	
	// Mark the thread as done and close the session (unless asked not to).
	
	RMxLock();
	session->flags |= kRMxSessionFlagsThreadDone;
	safeToClose = !( session->flags & kRMxSessionFlagsNoClose );
	RMxUnlock();
	if( safeToClose )
	{
		RMxSessionClose( session, err );
	}
	
	// Call _endthreadex() explicitly instead of just exiting normally to avoid memory leaks when using static run-time
	// libraries. See <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp>.
	
	_endthreadex_compat( (unsigned) err );
	return( (unsigned) err );	
}

//===========================================================================================================================
//	RMxSessionAccept
//===========================================================================================================================

DEBUG_LOCAL OSStatus	RMxSessionInitServer( RMxSessionRef inSession )
{
	OSStatus		err;
	
	inSession->sockEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inSession->sockEvent, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );
	
	err = WSAEventSelect( inSession->sock, inSession->sockEvent, FD_READ | FD_CLOSE );
	err = translate_errno( err == 0, errno_compat(), kNoResourcesErr );
	require_noerr( err, exit );
	
	inSession->waitCount = 0;
	inSession->waitHandles[ inSession->waitCount++ ] = inSession->sockEvent;
	inSession->waitHandles[ inSession->waitCount++ ] = inSession->closeEvent;
	inSession->waitHandles[ inSession->waitCount++ ] = gRMxStateChangeEvent;
	check( inSession->waitCount == sizeof_array( inSession->waitHandles ) );
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxSessionInitClient
//===========================================================================================================================

DEBUG_LOCAL OSStatus	RMxSessionInitClient( RMxSessionRef inSession, const char *inServer )
{
	OSStatus				err;
	struct addrinfo			hints;
	struct addrinfo *		addrList;
	struct addrinfo *		addr;
	BOOL					ok;
	
	addrList = NULL;
	
	// Initiate a connection to the server (if we're the client).
	
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype 	= SOCK_STREAM;
	hints.ai_protocol 	= IPPROTO_TCP;
	
	err = getaddrinfo( inServer, kRMxServerPortString, &hints, &addrList );
	require_noerr( err, exit );
	
	for( addr = addrList; addr; addr = addr->ai_next )
	{
		check( addr->ai_addr && ( addr->ai_addrlen > 0 ) );
		
		if( inSession->quit || ( gRMxState != kRMxStateRun ) )
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session state exit while connecting (quit=%d, state=%d)\n", 
				inSession->quit, gRMxState );
			err = kEndingErr;
			break;
		}
		
		// Clean up for any previous iteration that failed. Normal cleanup will occur when closing the session.
		
		if( IsValidSocket( inSession->sock ) )
		{
			err = close_compat( inSession->sock );
			check_translated_errno( err == 0, errno_compat(), kUnknownErr );
			inSession->sock = kInvalidSocketRef;
		}
		
		if( inSession->sockEvent )
		{
			ok = CloseHandle( inSession->sockEvent );
			check_translated_errno( ok, errno_compat(), kUnknownErr );
			inSession->sockEvent = NULL;
		}
		
		// Set up the socket and try to connect.
		
		dlog( kDebugLevelTrace, DEBUG_NAME "connecting %s socket to %s/%##a\n", 
			( addr->ai_family == AF_INET ) ? "AF_INET" : ( addr->ai_family == AF_INET6 ) ? "AF_INET6" : "<unknown>", 
			inServer ? inServer : "<local>", addr->ai_addr );
		
		inSession->sock = socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
		err = translate_errno( IsValidSocket( inSession->sock ), errno_compat(), kNoResourcesErr );
		if( err != kNoErr )
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "%s socket not supported...skipping (%d %m)\n", 
				( addr->ai_family == AF_INET ) ? "AF_INET" : ( addr->ai_family == AF_INET6 ) ? "AF_INET6" : "<unknown>", 
				err, err );
			continue;
		}
		
		inSession->sockEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		err = translate_errno( inSession->sockEvent, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		err = WSAEventSelect( inSession->sock, inSession->sockEvent, FD_READ | FD_CONNECT | FD_CLOSE );
		err = translate_errno( err == 0, errno_compat(), kNoResourcesErr );
		require_noerr( err, exit );
		
		inSession->waitCount = 0;
		inSession->waitHandles[ inSession->waitCount++ ] = inSession->sockEvent;
		inSession->waitHandles[ inSession->waitCount++ ] = inSession->closeEvent;
		inSession->waitHandles[ inSession->waitCount++ ] = gRMxStateChangeEvent;
		check( inSession->waitCount == sizeof_array( inSession->waitHandles ) );
		
		err = RMxSessionConnect( inSession, addr->ai_addr, addr->ai_addrlen );
		if( err == kNoErr )
		{
			break;
		}
	}
	require_action( addr, exit, err = kConnectionErr );
	
exit:
	if( addrList )
	{
		freeaddrinfo( addrList );
	}
	return( err );
}

//===========================================================================================================================
//	RMxSessionConnect
//===========================================================================================================================

DEBUG_LOCAL OSStatus	RMxSessionConnect( RMxSessionRef inSession, const struct sockaddr *inAddr, size_t inAddrSize )
{
	OSStatus		err;
	DWORD			result;
	
	check( inSession );
	check( inSession->sock );
	check( inSession->waitCount > 0 );
	check( inAddr && ( inAddrSize > 0 ) );
	
	// Start the connection process. This returns immediately. If the error is 0, it means the connection has 
	// successfully completed (usually only if the host is local). Otherwise, it returns an error to indicate 
	// that the connection process has started and we must use wait for it to complete.
	
	err = connect( inSession->sock, inAddr, (int) inAddrSize );
	if( err == 0 ) goto exit;
	
	// Wait for the connection to complete, timeout, or be canceled.
	
	result = WaitForMultipleObjects( inSession->waitCount, inSession->waitHandles, FALSE, kRMxClientTimeout );
	if( result == WAIT_OBJECT_0 )		// Socket Event
	{
		WSANETWORKEVENTS		events;
		
		// Check the result of the FD_CONNECT event to see if the connection was successful or not.
		
		err = WSAEnumNetworkEvents( inSession->sock, NULL, &events );
		require_noerr( err, exit );
		require_action( events.lNetworkEvents & FD_CONNECT, exit, err = kSelectorErr );
		err = events.iErrorCode[ FD_CONNECT_BIT ];
	}
	else if( result == WAIT_TIMEOUT )
	{
		err = kTimeoutErr;
	}
	else
	{
		err = kEndingErr;
	}
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxSessionSendMessage
//
//	Warning: Assumes the RMx lock is held.
//===========================================================================================================================

OSStatus	RMxSessionSendMessage( RMxSessionRef inSession, RMxOpCode inOpCode, OSStatus inStatus, const char *inFormat, ... )
{
	OSStatus		err;
	va_list			args1;
	va_list			args2;
	
	// Note: 2 va_list's need to be provided as the va_list needs to be processed twice (once to preflight, once for 
	// real) and this is illegal without C99 va_copy, but some environments do not yet support va_copy (e.g. Visual C++).
	
	va_start( args1, inFormat );
	va_start( args2, inFormat );
	err = RMxSessionSendMessageVAList( inSession, inOpCode, inStatus, inFormat, args1, args2 );
	va_end( args1 );
	va_end( args2 );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxSessionSendMessageVAList
//
//	Warning: Assumes the RMx lock is held.
//===========================================================================================================================

DEBUG_LOCAL OSStatus
	RMxSessionSendMessageVAList( 
		RMxSessionRef 	inSession, 
		RMxOpCode 		inOpCode, 
		OSStatus 		inStatus, 
		const char *	inFormat, 
		va_list			inArgs1, 
		va_list			inArgs2 )
{
	OSStatus		err;
	RMxMessage		msg;
	uint8_t *		bufferStorage;
	uint8_t *		buffer;
	size_t			bodySize;
	size_t			headerSize;
	size_t			size;
	int				n;
	
	bufferStorage = NULL;
	check( inSession );
	check( IsValidSocket( inSession->sock ) );
	check( inFormat );
	
	// Calculate the size of the data section of the message and set up the buffer for the entire message.
	// If the entire message will fit in the inline message buffer, use it. Otherwise, allocate a buffer for it.
	
	err = RMxPackedSizeVAList( &bodySize, inFormat, inArgs1 );
	require_noerr( err, exit );
	
	size = kRMxMessageHeaderSize + bodySize;
	if( size <= sizeof( msg.storage ) )
	{
		buffer = msg.storage;
	}
	else
	{
		bufferStorage = (uint8_t *) malloc( size );
		require_action( bufferStorage, exit, err = kNoMemoryErr );
		buffer = bufferStorage;
	}
	
	// Build the message header.
	
	err = RMxPack( buffer, kRMxMessageHeaderSize, &headerSize, "wwwwww", 
		kRMxSignatureVersion1, 		// signature
		inOpCode, 					// opcode
		kRMxFlagsNone, 				// flags
		0, 							// xid
		inStatus, 					// status
		(uint32_t) bodySize );		// size
	require_noerr( err, exit );
	check( headerSize == kRMxMessageHeaderSize );
		
	// Build the message body.
	
	err = RMxPackVAList( buffer + kRMxMessageHeaderSize, size - kRMxMessageHeaderSize, &size, inFormat, inArgs2 );
	require_noerr( err, exit );
	check( size == bodySize );
	
	// Send the entire message.
	
	size = headerSize + size;
	n = send( inSession->sock, (const char *) buffer, (int) size, 0 );
	err = translate_errno( n == (int) size, errno_compat(), kWriteErr );
	require_noerr( err, exit );
	
exit:
	if( bufferStorage )
	{
		free( bufferStorage );
	}
	return( err );
}

//===========================================================================================================================
//	RMxSessionRecvMessage
//
//	Note: This routine maintains state within the session data structure and can resume partial message reception.
//===========================================================================================================================

OSStatus	RMxSessionRecvMessage( RMxSessionRef inSession, DWORD inTimeout )
{
	OSStatus		err;
	DWORD			result;
	int				n;
	
	for( ;; )
	{
		if( inSession->quit || ( gRMxState != kRMxStateRun ) )
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session recv state exit (quit=%d, state=%d)\n", inSession->quit, gRMxState );
			err = kEndingErr;
			goto exit;
		}
		
		// Wait for data to become available or another event to occur.
		
		result = WaitForMultipleObjects( inSession->waitCount, inSession->waitHandles, FALSE, inTimeout );
		if( result == WAIT_OBJECT_0 )				// Socket Event (i.e. data available to read)
		{
			n = recv( inSession->sock, (char *) inSession->messageRecvBufferPtr, inSession->messageRecvRemaining, 0 );
			if( n > 0 )
			{
				inSession->messageRecvBufferPtr += n;
				inSession->messageRecvRemaining -= n;
				if( inSession->messageRecvRemaining == 0 )
				{
					// Complete chunk ready. If we haven't read the header yet, it's the header. Otherwise, it's the data. 
					
					if( !inSession->messageRecvHeaderDone )
					{
						// Parse the buffer into the message header structure and mark the header as complete.
						
						err = RMxUnpack( inSession->messageRecvBuffer, kRMxMessageHeaderSize, 
							"wwwwww", 
							&inSession->message.signature, 
							&inSession->message.opcode, 
							&inSession->message.flags, 
							&inSession->message.xid, 
							&inSession->message.status, 
							&inSession->message.recvSize );
						require_noerr( err, exit );
						require_action( inSession->message.signature == kRMxSignatureVersion1, exit, err = kMismatchErr );
						require_action( inSession->message.opcode != kRMxOpCodeInvalid, exit, err = kMismatchErr );
						inSession->messageRecvHeaderDone = true;
						
						// Set up to read the data section (if any). Use the inline message buffer if the data will fit.
						
						if( inSession->message.recvSize > 0 )
						{
							if( inSession->message.recvSize <= sizeof( inSession->message.storage ) )
							{
								inSession->message.recvData = inSession->message.storage;
							}
							else
							{
								inSession->message.recvData = (uint8_t *) malloc( inSession->message.recvSize );
								require_action( inSession->message.recvData, exit, err = kNoMemoryErr );
							}
							inSession->messageRecvBufferPtr = inSession->message.recvData;
							inSession->messageRecvRemaining = (int) inSession->message.recvSize;
						}
					}			
				}
				if( inSession->messageRecvHeaderDone && ( inSession->messageRecvRemaining == 0 ) )
				{
					// Complete message ready. Reset state for next message. Exit to allow message to be processed.
					
					inSession->messageRecvBufferPtr		= inSession->messageRecvBuffer;
					inSession->messageRecvRemaining		= kRMxMessageHeaderSize;
					inSession->messageRecvHeaderDone 	= false;
					break;
				}
			}
			else
			{
				err = errno_compat();
				dlog( kDebugLevelNotice, DEBUG_NAME "session recv peer disconnected (%d/%d %m)\n", n, err, err );
				err = kConnectionErr;
				goto exit;
			}
		}
		else if( result == ( WAIT_OBJECT_0 + 1 ) )	// Close Event
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session recv close signaled\n" );
			err = kEndingErr;
			goto exit;
		}
		else if( result == ( WAIT_OBJECT_0 + 2 ) )	// State Change Event
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session recv state change (%d)\n", gRMxState );
			err = kEndingErr;
			goto exit;
		}
		else if( result == WAIT_TIMEOUT )			// Timeout
		{
			dlog( kDebugLevelNotice, DEBUG_NAME "session recv timeout\n" );
			err = kTimeoutErr;
			goto exit;
		}
		else
		{
			err = errno_compat();
			dlog( kDebugLevelAlert, DEBUG_NAME "session recv message wait error: 0x%08X, %d (%m)\n", result, err, err );
			err = (OSStatus) result;
			goto exit;
		}
	}
	err = kNoErr;
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Utilities ==
#endif

//===========================================================================================================================
//	RMxCheckVersion
//===========================================================================================================================

OSStatus
	RMxCheckVersion( 
		uint32_t inClientCurrentVersion, uint32_t inClientOldestClientVersion, uint32_t inClientOldestServerVersion, 
		uint32_t inServerCurrentVersion, uint32_t inServerOldestClientVersion, uint32_t inServerOldestServerVersion )
{
	OSStatus			err;
	const char *		message;
	
	DEBUG_USE_ONLY( inClientOldestClientVersion );
	DEBUG_USE_ONLY( inServerOldestServerVersion );
	DEBUG_USE_ONLY( message );
	
	// Determine if the version information on both sides is compatible.
	
	if( inClientCurrentVersion == inServerCurrentVersion )
	{	
		message 	= "versions exactly match";
		err  		= kNoErr;
	}
	else if( inClientCurrentVersion > inServerCurrentVersion )
	{
		if( inClientOldestServerVersion <= inServerCurrentVersion )
		{
			message = "client newer, but compatible";
			err  	= kNoErr;
		}
		else
		{
			message = "server too old for client";
			err		= kIncompatibleErr;
		}
	}
	else
	{
		if( inServerOldestClientVersion <= inClientCurrentVersion )
		{
			message = "server newer, but compatible";
			err		= kNoErr;
		}
		else
		{
			message = "client too old for server";
			err		= kIncompatibleErr;
		}
	}
	dlog( kDebugLevelNotice, DEBUG_NAME "%s (client=%v/%v/%v vs server=%v/%v/%v)\n", message, 
		inClientCurrentVersion, inClientOldestClientVersion, inClientOldestServerVersion, 
		inServerCurrentVersion, inServerOldestClientVersion, inServerOldestServerVersion );

	return( err );
}

//===========================================================================================================================
//	RMxPackedSize
//===========================================================================================================================

OSStatus	RMxPackedSize( size_t *outSize, const char *inFormat, ... )
{
	OSStatus		err;
	va_list			args;
	
	va_start( args, inFormat );
	err = RMxPackedSizeVAList( outSize, inFormat, args );
	va_end( args );
	
	return( err );
}

//===========================================================================================================================
//	RMxPackedSizeVAList
//===========================================================================================================================

OSStatus	RMxPackedSizeVAList( size_t *outSize, const char *inFormat, va_list inArgs )
{
	OSStatus			err;
	size_t				size;
	char				c;
	const uint8_t *		src;
	uint32_t			tempU32;
	const uint8_t *		p;
	
	check_compile_time_code( sizeof( unsigned int ) >= 4 );
	
	size = 0;
	
	// Loop thru each character in the format string, decode it, and add the size required to pack it.
	
	for( c = *inFormat; c != '\0'; c = *( ++inFormat ) )
	{
		switch( c )
		{
			case 'b':	// Byte (8-bit)
				
				va_arg( inArgs, unsigned int );
				size += 1;
				break;
			
			case 'h':	// Half-word (16-bit)
				
				va_arg( inArgs, unsigned int );
				size += 2;
				break;
			
			case 'w':	// Word (32-bit)
				
				va_arg( inArgs, unsigned int );
				size += 4;
				break;
			
			case 's':	// UTF-8 String, null terminated
				
				src = va_arg( inArgs, const uint8_t * );
				check( src );
				
				p = src;
				while( *p++ != 0 ) {}
				size += ( p - src );
				break;
			
			case 'n':	// N bytes of raw data; 1st arg is size, 2nd arg is ptr; stored with 32-bit length prefix.
				
				tempU32 = (uint32_t) va_arg( inArgs, unsigned int );				
				src = va_arg( inArgs, const uint8_t * );
				check( src || ( tempU32 == 0 ) );
				
				size += ( 4 + tempU32 );
				break;
			
			case ' ':	// Ignore spaces (they're just for format string readability).
				break;
			
			default:	// Unknown format specifier
				
				err = kUnsupportedErr;
				goto exit;
		}
	}
	
	// Success!
	
	if( outSize )
	{
		*outSize = size;
	}
	err = kNoErr;
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxPack
//===========================================================================================================================

OSStatus	RMxPack( void *inBuffer, size_t inMaxSize, size_t *outSize, const char *inFormat, ... )
{
	OSStatus		err;
	va_list			args;
	
	va_start( args, inFormat );
	err = RMxPackVAList( inBuffer, inMaxSize, outSize, inFormat, args );
	va_end( args );
	
	return( err );
}

//===========================================================================================================================
//	RMxPackVAList
//===========================================================================================================================

OSStatus	RMxPackVAList( void *inBuffer, size_t inMaxSize, size_t *outSize, const char *inFormat, va_list inArgs )
{
	OSStatus			err;
	char				c;
	const uint8_t *		src;
	uint8_t *			dst;
	uint8_t *			end;
	uint8_t				tempU8;
	uint16_t			tempU16;
	uint32_t			tempU32;
	const uint8_t *		p;
	size_t				size;
	
	check_compile_time_code( sizeof( unsigned int ) >= 4 );
	
	dst = (uint8_t *) inBuffer;
	end = dst + inMaxSize;
	
	// Loop thru each character in the format string, decode it, and pack the data appropriately.
	
	for( c = *inFormat; c != '\0'; c = *( ++inFormat ) )
	{
		switch( c )
		{
			case 'b':	// Byte (8-bit)
				
				check( ( end - dst ) >= 1 );
				tempU8 = (uint8_t) va_arg( inArgs, unsigned int );
				*dst++ = tempU8;
				break;
			
			case 'h':	// Half-word (16-bit)
				
				check( ( end - dst ) >= 2 );
				tempU16	= (uint16_t) va_arg( inArgs, unsigned int );
				*dst++ 	= (uint8_t)( ( tempU16 >> 8 ) & 0xFF );
				*dst++ 	= (uint8_t)(   tempU16        & 0xFF );
				break;
			
			case 'w':	// Word (32-bit)
				
				check( ( end - dst ) >= 4 );
				tempU32	= (uint32_t) va_arg( inArgs, unsigned int );
				*dst++  = (uint8_t)( ( tempU32 >> 24 ) & 0xFF );
				*dst++  = (uint8_t)( ( tempU32 >> 16 ) & 0xFF );
				*dst++  = (uint8_t)( ( tempU32 >>  8 ) & 0xFF );
				*dst++  = (uint8_t)(   tempU32         & 0xFF );
				break;
			
			case 's':	// UTF-8 String, null terminated
				
				src = va_arg( inArgs, const uint8_t * );
				check( src );
				
				p = src;
				while( *p++ != 0 ) {}
				size = (size_t)( p - src );
				check( ( end - dst ) >= (ptrdiff_t) size );
				
				while( size-- > 0 )
				{
					*dst++ = *src++;
				}
				break;
			
			case 'n':	// N bytes of raw data; 1st arg is size, 2nd arg is ptr; stored with 32-bit length prefix.
				
				tempU32 = (uint32_t) va_arg( inArgs, unsigned int );
				check( ( end - dst ) >= (ptrdiff_t)( 4 + tempU32 ) );
				
				src = va_arg( inArgs, const uint8_t * );
				check( src || ( tempU32 == 0 ) );
				
				*dst++ = (uint8_t)( ( tempU32 >> 24 ) & 0xFF );
				*dst++ = (uint8_t)( ( tempU32 >> 16 ) & 0xFF );
				*dst++ = (uint8_t)( ( tempU32 >>  8 ) & 0xFF );
				*dst++ = (uint8_t)(   tempU32         & 0xFF );
				while( tempU32-- > 0 )
				{
					*dst++ = *src++;
				}
				break;
			
			case ' ':	// Ignore spaces (they're just for format string readability).
				break;
			
			default:	// Unknown format specifier
				
				err = kUnsupportedErr;
				goto exit;
		}
	}
	
	// Success!
	
	if( outSize )
	{
		*outSize = (size_t)( dst - ( (uint8_t *) inBuffer ) );
	}
	err = kNoErr;
	
exit:
	return( err );
}

//===========================================================================================================================
//	RMxUnpack
//===========================================================================================================================

OSStatus	RMxUnpack( const void *inData, size_t inSize, const char *inFormat, ... )
{
	OSStatus		err;
	va_list			args;
	
	va_start( args, inFormat );
	err = RMxUnpackVAList( inData, inSize, inFormat, args );
	va_end( args );
	
	return( err );
}

//===========================================================================================================================
//	DNSUnpackVAList
//===========================================================================================================================

OSStatus	RMxUnpackVAList( const void *inData, size_t inSize, const char *inFormat, va_list inArgs )
{
	OSStatus				err;
	char					c;
	const uint8_t *			src;
	const uint8_t *			end;
	uint8_t *				b;
	uint16_t *				h;
	uint32_t *				w;
	uint16_t				tempU16;
	uint32_t				tempU32;
	const uint8_t *			p;
	size_t					size;
	const uint8_t **		ptrArg;
	size_t *				sizeArg;
	
	check_compile_time_code( sizeof( unsigned int ) >= 4 );
	
	src = (const uint8_t *) inData;
	end = src + inSize;
	
	// Loop thru each character in the format string, decode it, and unpack the data appropriately.
	
	for( c = *inFormat; c != '\0'; c = *( ++inFormat ) )
	{
		switch( c )
		{
			case 'b':	// Byte (8-bit)
				
				require_action( ( end - src ) >= 1, exit, err = kSizeErr );
				b  = va_arg( inArgs, uint8_t * );
				if( b )
				{
					*b = *src;
				}
				++src;
				break;
			
			case 'h':	// Half-word (16-bit)
				
				require_action( ( end - src ) >= 2, exit, err = kSizeErr );
				tempU16  = (uint16_t)( *src++ << 8 );
				tempU16 |= (uint16_t)( *src++ );
				h 		 = va_arg( inArgs, uint16_t * );
				if( h )
				{
					*h = tempU16;
				}
				break;
			
			case 'w':	// Word (32-bit)
				
				require_action( ( end - src ) >= 4, exit, err = kSizeErr );
				tempU32  = (uint32_t)( *src++ << 24 );
				tempU32 |= (uint32_t)( *src++ << 16 );
				tempU32 |= (uint32_t)( *src++ << 8 );
				tempU32 |= (uint32_t)( *src++ );
				w 		 = va_arg( inArgs, uint32_t * );
				if( w )
				{
					*w = tempU32;
				}
				break;
			
			case 's':	// UTF-8 String, null terminated; 1st arg=ptr to ptr, 2nd arg=ptr to size, excluding null terminator.
				
				p = src;
				while( ( ( end - p ) > 0 ) && ( *p != 0 ) )
				{
					++p;
				}
				require_action( ( end - p ) > 0, exit, err = kSizeErr );
				size = (size_t)( p - src );
				
				ptrArg = va_arg( inArgs, const uint8_t ** );
				if( ptrArg )
				{
					*ptrArg = src;
				}
				
				sizeArg	= va_arg( inArgs, size_t * );
				if( sizeArg )
				{
					*sizeArg = size;
				}
				
				src = p + 1;
				break;
			
			case 'n':	// N bytes of raw data; 1st arg is ptr to ptr, 2nd arg is ptr to size.
				
				require_action( ( end - src ) >= 4, exit, err = kSizeErr );
				tempU32  = (uint32_t)( *src++ << 24 );
				tempU32 |= (uint32_t)( *src++ << 16 );
				tempU32 |= (uint32_t)( *src++ << 8 );
				tempU32 |= (uint32_t)( *src++ );
				require_action( ( end - src ) >= (ptrdiff_t) tempU32, exit, err = kSizeErr );
				size = (size_t) tempU32;
				
				ptrArg = va_arg( inArgs, const uint8_t ** );
				if( ptrArg )
				{
					*ptrArg = src;
				}
				
				sizeArg	= va_arg( inArgs, size_t * );
				if( sizeArg )
				{
					*sizeArg = size;
				}
				
				src += size;
				break;
			
			case ' ':	// Ignore spaces (they're just for format string readability).
				break;
			
			default:	// Unknown format specifier
				
				err = kUnsupportedErr;
				goto exit;
		}
	}

	// Success!
	
	err = kNoErr;
	
exit:
	return( err );
}

#if( DEBUG )
//===========================================================================================================================
//	RMxPackUnpackTest
//===========================================================================================================================

OSStatus	RMxPackUnpackTest( void );

OSStatus	RMxPackUnpackTest( void )
{
	static const uint8_t		data[] = 
	{
		0xAA, 
		0xBB, 0xCC, 
		0x11, 0x22, 0x33, 0x44, 
		0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x70, 0x65, 0x6F, 0x70, 0x6C, 0x65, 0x00, 	// hello people\0
		0x00, 0x00, 0x00, 0x05, 0x74, 0x65, 0x73, 0x74, 0x73							// tests
	};
	OSStatus		err;
	uint8_t			buffer[ 128 ];
	size_t			size;
	uint8_t			b;
	uint16_t		h;
	uint32_t		w;
	char *			s;
	size_t			sSize;
	uint8_t *		d;
	size_t			dSize;
	
	check_compile_time_code( sizeof( data ) >= 29 );
	
	// simple API test.
	//
	
	printf( "\nsimple API test\n" );
	
	err = RMxPackedSize( &size, "bhwsn ", 0xAA, 0xBBCC, 0x11223344, "hello people", 5, "tests" );
	require_noerr( err, exit );
	require_action( size == 29, exit, err = kSizeErr );
	
	err = RMxPack( buffer, 29, &size, " bhwsn", 0xAA, 0xBBCC, 0x11223344, "hello people", 5, "tests" );
	require_noerr( err, exit );
	require_action( size == 29, exit, err = kSizeErr );
	require_action( memcmp( buffer, data, size ) == 0, exit, err = kMismatchErr );
	
	err = RMxUnpack( data, sizeof( data ), "bhw sn", &b, &h, &w, &s, &sSize, &d, &dSize );
	require_noerr( err, exit );
	require_action( b == 0xAA, exit, err = kMismatchErr );
	require_action( h == 0xBBCC, exit, err = kMismatchErr );
	require_action( w == 0x11223344, exit, err = kMismatchErr );
	require_action( sSize == 12, exit, err = kSizeErr );
	require_action( strcmp( s, "hello people" ) == 0, exit, err = kMismatchErr );
	require_action( dSize == 5, exit, err = kSizeErr );
	require_action( memcmp( d, "tests", 5 ) == 0, exit, err = kMismatchErr );
	
	printf( "\nsimple API test done\n\n" );
	
	// Done
	
	printf( "\n\nALL TESTS PASSED\n\n" );

exit:
	return( err );
}
#endif	// DEBUG

#ifdef	__cplusplus
	}
#endif
