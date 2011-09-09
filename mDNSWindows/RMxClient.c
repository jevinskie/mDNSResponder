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
    
$Log: RMxClient.c,v $
Revision 1.3  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.2  2004/04/09 21:03:14  bradley
Changed port numbers to use network byte order for consistency with other platforms.

Revision 1.1  2004/01/30 02:35:13  bradley
Rendezvous Message Exchange implementation for DNS-SD IPC on Windows.

*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSD.h"
#include	"RMxCommon.h"

#include	"RMxClient.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[RMxClient] "

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
	RMxSessionRef									session;
	void *											context;
	DNSRecordRef									records;
	
	union
	{
		struct	// EnumerateDomains
		{
			DNSServiceDomainEnumReply				callback;
		
		}	domain;
				
		struct	// Register
		{
			DNSServiceRegisterReply					callback;
			uint32_t								lastID;
		
		}	reg;
		
		struct	// Browse
		{
			DNSServiceBrowseReply					callback;
		
		}	browse;
		
		struct	// Resolve
		{
			DNSServiceFlags							flags;
			DNSServiceResolveReply					callback;
		
		}	resolve;

		struct	// CreateConnection
		{
			DNSServiceRegisterRecordReply			callback;
			uint32_t								lastID;
		
		}	connection;
		
		struct	// QueryRecord
		{
			DNSServiceQueryRecordReply				callback;
		
		}	query;
	
	}	u;
};

// DNSRecordRef

typedef struct	_DNSRecordRef_t		_DNSRecordRef_t;
struct	_DNSRecordRef_t
{
	DNSRecordRef						next;
	uint32_t							id;
	DNSServiceRegisterRecordReply		callback;
	void *								context;
};

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// RMx

DEBUG_LOCAL void	RMxClientMessageCallBack( RMxMessage *inMessage );

// Properties

DEBUG_LOCAL DNSServiceErrorType
	DNSServiceCopyPropertyDecodeData_client( 
		DNSPropertyCode		inCode, 
		const void *		inData, 
		size_t				inSize, 
		DNSPropertyData *	outData );

// Domain Enumeration

DEBUG_LOCAL void	DNSServiceEnumerateDomainsReply_client( RMxMessage *inMessage );

// Service Discovery

DEBUG_LOCAL void	DNSServiceBrowseReply_client( RMxMessage *inMessage );

DEBUG_LOCAL void	DNSServiceResolveReply_client( RMxMessage *inMessage );

// Service Registration

DEBUG_LOCAL void	DNSServiceRegisterReply_client( RMxMessage *inMessage );

// Special Purpose

DEBUG_LOCAL DNSRecordRef	DNSServiceConnectionRecordRemove_client( DNSServiceRef inRef, DNSRecordRef inRecordRef );
DEBUG_LOCAL void			DNSServiceRegisterRecordReply_client( RMxMessage *inMessage );
DEBUG_LOCAL void			DNSServiceQueryRecordReply_client( RMxMessage *inMessage );

#if 0
#pragma mark -
#pragma mark == RMx ==
#endif

//===========================================================================================================================
//	RMxClientInitialize
//===========================================================================================================================

OSStatus	RMxClientInitialize( void )
{
	OSStatus		err;
	
	// Initialize the lower-level layer and indicate it is running.
	
	err = RMxInitialize();
	require_noerr( err, exit );
	
	gRMxState = kRMxStateRun;
	
exit:
	if( err != kNoErr )
	{
		RMxClientFinalize();
	}
	return( err );
}

//===========================================================================================================================
//	RMxClientFinalize
//===========================================================================================================================

void	RMxClientFinalize( void )
{
	RMxFinalize();
}

//===========================================================================================================================
//	RMxClientMessageCallBack
//===========================================================================================================================

DEBUG_LOCAL void	RMxClientMessageCallBack( RMxMessage *inMessage )
{
	check( inMessage );
	
	switch( inMessage->opcode )
	{
		case kRMxOpCodeInvalid:			
			// The session is closing. We don't delete the DNS-SD object here because the client deletes DNS-SD objects.
			break;
		
		case kRMxOpCodeEnumerateDomains:
			DNSServiceEnumerateDomainsReply_client( inMessage );
			break;
		
		case kRMxOpCodeRegister:
			DNSServiceRegisterReply_client( inMessage );
			break;

		case kRMxOpCodeBrowse:
			DNSServiceBrowseReply_client( inMessage );
			break;
		
		case kRMxOpCodeResolve:
			DNSServiceResolveReply_client( inMessage );
			break;
		
		case kRMxOpCodeRegisterRecord:
			DNSServiceRegisterRecordReply_client( inMessage );
			break;
		
		case kRMxOpCodeQueryRecord:
			DNSServiceQueryRecordReply_client( inMessage );
			break;
		
		default:
			dlog( kDebugLevelWarning, DEBUG_NAME "message with unknown opcode received (%d)\n", inMessage->opcode );
			break;
	}
}

#if 0
#pragma mark -
#pragma mark == DNS-SD General ==
#endif

//===========================================================================================================================
//	DNSServiceRefDeallocate_client
//===========================================================================================================================

void	DNSServiceRefDeallocate_client( DNSServiceRef inRef )
{
	OSStatus		err;
	
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "Deallocate ref=%#p\n", inRef );
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	
	// Close the session (if not already closed).
	
	if( inRef->session )
	{
		inRef->session->callback		= NULL;
		inRef->session->message.context	= NULL;
		
		err = RMxSessionClose( inRef->session, kEndingErr );
		check_noerr( err );
	}
	
	// Release any outstanding individual records.
	
	while( inRef->records )
	{
		DNSRecordRef		record;
		
		record = inRef->records;
		inRef->records = record->next;
		
		free( record );
	}
	
	// Release the object itself.
	
	free( inRef );

exit:
	return;
}

//===========================================================================================================================
//	DNSServiceCheckVersion_client
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCheckVersion_client( const char *inServer )
{	
	DNSServiceErrorType		err;
	RMxSessionRef			session;
	OSStatus				errorCode;
	uint32_t				serverCurrentVersion;
	uint32_t				serverOldestClientVersion;
	uint32_t				serverOldestServerVersion;
	
	session = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "CheckVersion\n" );
	
	// Open a session to the server and send the request. Specify no thread since we are going to manually read the message.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoThread, kInvalidSocketRef, NULL, NULL, &session, 
		kRMxOpCodeCheckVersion, "www", kRMxCurrentVersion, kRMxOldestClientVersion, kRMxOldestServerVersion );
	require_noerr( err, exit );
	
	// Receive the respons, parse it, and check the versions.
	
	err = RMxSessionRecvMessage( session, kRMxClientTimeout );
	require_noerr( err, exit );
	check( session->message.recvData || ( session->message.recvSize == 0 ) );
	
	err = RMxUnpack( session->message.recvData, session->message.recvSize, "wwww", 
		&errorCode, &serverCurrentVersion, &serverOldestClientVersion, &serverOldestServerVersion );
	require_noerr( err, exit );
	
	err = RMxCheckVersion( kRMxCurrentVersion, kRMxOldestClientVersion, kRMxOldestServerVersion, 
						   serverCurrentVersion, serverOldestClientVersion, serverOldestServerVersion );
	check( err == errorCode );
	if( ( err == kNoErr ) && ( errorCode != kNoErr ) )
	{
		dlog( kDebugLevelWarning, DEBUG_NAME "client/server disagree on versions\n" );
		err = errorCode;
	}
	
exit:
	if( session )
	{
		RMxSessionClose( session, kEndingErr );
	}
	return( err );
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Properties ==
#endif

//===========================================================================================================================
//	DNSServiceCopyProperty_client
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCopyProperty_client( const char *inServer, DNSPropertyCode inCode, DNSPropertyData *outData )
{	
	DNSServiceErrorType		err;
	RMxSessionRef			session;
	OSStatus				errorCode;
	DNSPropertyCode			code;
	const void *			data;
	size_t					size;
	
	session = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "CopyProperty server=%s, code='%C'\n", inServer ? inServer : "<local>", inCode );
	require_action( outData, exit, err = kDNSServiceErr_BadParam );
	
	// Open a session to the server and send the request. Specify no thread since we are going to manually read the message.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoThread, kInvalidSocketRef, NULL, NULL, &session, 
		kRMxOpCodeCopyProperty, "w", inCode );
	require_noerr( err, exit );
	
	// Receive the response, parse it, and check the versions.
	
	err = RMxSessionRecvMessage( session, kRMxClientTimeout );
	require_noerr( err, exit );
	check( session->message.recvData || ( session->message.recvSize == 0 ) );
	
	err = RMxUnpack( session->message.recvData, session->message.recvSize, "wwn", &errorCode, &code, &data, &size );
	require_noerr( err, exit );
	err = errorCode;
	require_noerr_quiet( err, exit );
	
	// Decode the data and fill in the results.
	
	err = DNSServiceCopyPropertyDecodeData_client( code, data, size, outData );
	require_noerr( err, exit );
	
exit:
	if( session )
	{
		RMxSessionClose( session, kEndingErr );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceCopyProperty_client
//===========================================================================================================================

DEBUG_LOCAL DNSServiceErrorType
	DNSServiceCopyPropertyDecodeData_client( 
		DNSPropertyCode		inCode, 
		const void *		inData, 
		size_t				inSize, 
		DNSPropertyData *	outData )
{
	OSStatus		err;
	
	check( outData );
	
	switch( inCode )
	{
		case kDNSPropertyCodeVersion:
			outData->u.version.clientCurrentVersion			= kRMxCurrentVersion;
			outData->u.version.clientOldestServerVersion	= kRMxOldestServerVersion;
			
			err = RMxUnpack( inData, inSize, "www", 
				&outData->u.version.serverCurrentVersion, &outData->u.version.serverOldestClientVersion, NULL );
			require_noerr( err, exit );
			break;
		
		default:
			dlog( kDebugLevelError, DEBUG_NAME "CopyPropertyDecodeData unknown property code (%C)\n", inCode );
			err = kDNSServiceErr_Unsupported;
			goto exit;
	}
	err = kDNSServiceErr_NoError;

exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceReleaseProperty_client
//===========================================================================================================================

DNSServiceErrorType	DNSServiceReleaseProperty_client( DNSPropertyData *inData )
{
	OSStatus		err;
	
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "ReleaseProperty\n" );
	require_action( inData, exit, err = kDNSServiceErr_BadParam );
	
	switch( inData->code )
	{
		case kDNSPropertyCodeVersion:
			// Data is embedded directly in the structure so there is nothing to release.
			break;
			
		default:
			dlog( kDebugLevelError, DEBUG_NAME "ReleaseProperty unknown property code (%C)\n", inData->code );
			err = kDNSServiceErr_Unsupported;
			goto exit;
	}
	err = kDNSServiceErr_NoError;
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Domain Enumeration ==
#endif

//===========================================================================================================================
//	DNSServiceEnumerateDomains_client
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceEnumerateDomains_client(
		DNSServiceRef *					outRef,
		const char *					inServer, 
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "EnumerateDomains flags=0x%08X, ifi=%d, server=%s\n", 
		inFlags, inInterfaceIndex, inServer ? inServer : "<local>" );
	require_action( outRef, exit, err = kDNSServiceErr_BadParam );
	require_action( ( inFlags == kDNSServiceFlagsBrowseDomains ) || 
					( inFlags == kDNSServiceFlagsRegistrationDomains ), 
					exit, err = kDNSServiceErr_BadFlags );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->context			= inContext;
	obj->u.domain.callback 	= inCallBack;
	
	// Open a session to the server and send the request.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeEnumerateDomains, "ww", inFlags, inInterfaceIndex );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceEnumerateDomainsReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceEnumerateDomainsReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	uint32_t				interfaceIndex;
	DNSServiceErrorType		errorCode;
	const char *			domain;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwws", &flags, &interfaceIndex, &errorCode, &domain, NULL );
		check_noerr( err );
		if( err == kNoErr )
		{
			dlog( kDebugLevelTrace, DEBUG_NAME "EnumerateDomains reply flags=0x%08X, ifi=%d, err=%d, domain=\"%s\"\n", 
				flags, interfaceIndex, errorCode, domain );
			
			obj->u.domain.callback( obj, flags, interfaceIndex, errorCode, domain, obj->context );
		}
	}
	if( err != kNoErr )
	{
		obj->u.domain.callback( obj, 0, 0, err, "", obj->context );
	}
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Service Registration ==
#endif

//===========================================================================================================================
//	DNSServiceRegister_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegister_client(
		DNSServiceRef *			outRef,
		const char *			inServer, 
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,
		const char *			inType,
		const char *			inDomain,
		const char *			inHost,
		uint16_t				inPort,
		uint16_t				inTXTSize,
		const void *			inTXT,
		DNSServiceRegisterReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME 
		"Resolve flags=0x%08X, ifi=%d, name=\"%s\", type=\"%s\", domain=\"%s\", host=\"%s\", port=%d, txtSize=%d\n", 
		inFlags, inInterfaceIndex, inName ? inName : "<default>", inType, inDomain ? inDomain : "<default>", inHost, 
		ntohs( inPort ), inTXTSize );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( ( inFlags == 0 ) || ( inFlags == kDNSServiceFlagsNoAutoRename ), exit, err = kDNSServiceErr_BadFlags );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	require_action( inTXT || ( inTXTSize == 0 ), exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->context		= inContext;
	obj->u.reg.callback	= inCallBack;
	
	// Open a session to the server and send the request.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeRegister, "wwsssshn", inFlags, inInterfaceIndex, inName ? inName : "", inType, 
		inDomain ? inDomain : "", inHost, inPort, inTXTSize, inTXT );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	DNSServiceErrorType		errorCode;
	const char *			name;
	const char *			type;
	const char *			domain;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwsss", 
			&flags, &errorCode, &name, NULL, &type, NULL, &domain, NULL );
		check_noerr( err );
		if( err == kNoErr )
		{
			dlog( kDebugLevelTrace, DEBUG_NAME "Register reply flags=0x%08X, err=%d, name=\"%s\", type=\"%s\", domain=\"%s\"\n", 
				flags, errorCode, name, type, domain );
			
			obj->u.reg.callback( obj, flags, errorCode, name, type, domain, obj->context );
		}
	}
	if( err != kNoErr )
	{
		obj->u.reg.callback( obj, 0, err, "", "", "", obj->context );
	}
}

//===========================================================================================================================
//	DNSServiceAddRecord_client
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceAddRecord_client(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	DNSRecordRef			obj;
	
	obj = NULL;
	RMxLock();
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "AddRecord flags=0x%08X, rrType=%d, rrDataSize=%d, ttl=%d\n", 
		inFlags, inRRType, inRDataSize, inTTL );
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inRef->session, exit, err = kDNSServiceErr_NotInitialized );
	require_action( outRecordRef, exit, err = kDNSServiceErr_BadParam );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the object.
	
	obj = (DNSRecordRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	// Send the message. Use an ID that should be unique for the session and avoid the reserved ID.
	
	obj->id = ++inRef->u.reg.lastID;
	if( obj->id == kDNSRecordIndexDefaultTXT )
	{
		obj->id = ++inRef->u.reg.lastID;
	}
	
	err = RMxSessionSendMessage( inRef->session, kRMxOpCodeAddRecord, kNoErr, "wwhnw", 
		obj->id, inFlags, inRRType, inRDataSize, inRData, inTTL );
	require_noerr( err, exit );
	
	// Success!
	
	*outRecordRef	= obj;
	obj 			= NULL;
	
exit:
	if( obj )
	{
		free( obj );
	}
	RMxUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceUpdateRecord_client
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceUpdateRecord_client(
	    DNSServiceRef	inRef,
	    DNSRecordRef	inRecordRef,
	    DNSServiceFlags	inFlags,
	    uint16_t 		inRDataSize,
	    const void *	inRData,
	    uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	
	RMxLock();
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "UpdateRecord flags=0x%08X, rrDataSize=%d, ttl=%d\n", inFlags, inRDataSize, inTTL );
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inRef->session, exit, err = kDNSServiceErr_NotInitialized );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
	
	err = RMxSessionSendMessage( inRef->session, kRMxOpCodeUpdateRecord, kNoErr, "wwnw", 
		inRecordRef ? inRecordRef->id : kDNSRecordIndexDefaultTXT, inFlags, inRDataSize, inRData, inTTL );
	require_noerr( err, exit );
	
exit:
	RMxUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceRemoveRecord_client
//===========================================================================================================================

DNSServiceErrorType	DNSServiceRemoveRecord_client( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags )
{
	DNSServiceErrorType		err;
	
	RMxLock();
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "RemoveRecord flags=0x%08X\n", inFlags );
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inRef->session, exit, err = kDNSServiceErr_NotInitialized );
	require_action( inRecordRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	
	err = RMxSessionSendMessage( inRef->session, kRMxOpCodeRemoveRecord, kNoErr, "ww", inRecordRef->id, inFlags );
	DNSServiceConnectionRecordRemove_client( inRef, inRecordRef );
	free( inRecordRef );
	require_noerr( err, exit );
	
exit:
	RMxUnlock();
	return( err );
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Service Discovery ==
#endif

//===========================================================================================================================
//	DNSServiceBrowse_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceBrowse_client(
		DNSServiceRef *			outRef,
		const char *			inServer, 
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "Browse flags=0x%08X, ifi=%d, type=\"%s\", domain=\"%s\"\n", 
		inFlags, inInterfaceIndex, inType, inDomain ? inDomain : "<default>" );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->context			= inContext;
	obj->u.browse.callback 	= inCallBack;
	
	// Open a session to the server and send the request.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeBrowse, "wwss", inFlags, inInterfaceIndex, inType, inDomain ? inDomain : "" );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceBrowseReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceBrowseReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	uint32_t				interfaceIndex;
	DNSServiceErrorType		errorCode;
	const char *			name;
	const char *			type;
	const char *			domain;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwwsss", 
			&flags, &interfaceIndex, &errorCode, &name, NULL, &type, NULL, &domain, NULL );
		check_noerr( err );
		if( err == kNoErr )
		{
			dlog( kDebugLevelTrace, DEBUG_NAME 
				"Browse reply flags=0x%08X, ifi=%d, err=%d, name=\"%s\", type=\"%s\", domain=\"%s\"\n", 
				flags, interfaceIndex, errorCode, name, type, domain );
				
			obj->u.browse.callback( obj, flags, interfaceIndex, errorCode, name, type, domain, obj->context );
		}
	}
	if( err != kNoErr )
	{
		obj->u.browse.callback( obj, 0, 0, err, "", "", "", obj->context );
	}
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	DNSServiceResolve_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceResolve_client(
		DNSServiceRef *			outRef,
		const char *			inServer, 
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,     
		const char *			inType,  
		const char *			inDomain,   
		DNSServiceResolveReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "Resolve flags=0x%08X, ifi=%d, name=\"%s\", type=\"%s\", domain=\"%s\"\n", 
		inFlags, inInterfaceIndex, inName, inType, inDomain ? inDomain : "<default>" );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->context			= inContext;
	obj->u.resolve.flags	= inFlags;
	obj->u.resolve.callback = inCallBack;
		
	// Open a session to the server and send the request.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeResolve, "wwsss", inFlags, inInterfaceIndex, inName, inType, inDomain ? inDomain : "" );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceResolveReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceResolveReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	uint32_t				interfaceIndex;
	DNSServiceErrorType		errorCode;
	const char *			name;
	const char *			host;
	uint16_t				port;
	const char *			txt;
	size_t					txtSize;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwwsshn", 
			&flags, &interfaceIndex, &errorCode, &name, NULL, &host, NULL, &port, &txt, &txtSize );
		check_noerr( err );
		if( err == kNoErr )
		{
			dlog( kDebugLevelTrace, DEBUG_NAME
				"Resolve reply flags=0x%08X, ifi=%d, err=%d, name=\"%s\", host=\"%s\", port=%d, txtSize=%d\n", 
				flags, interfaceIndex, errorCode, name, host, ntohs( port ), (int) txtSize );
			
			obj->u.resolve.callback( obj, flags, interfaceIndex, errorCode, name, host, port, (uint16_t) txtSize, txt, 
			obj->context );
		}
	}
	if( err != kNoErr )
	{
		obj->u.resolve.callback( obj, 0, 0, err, "", "", 0, 0, NULL, obj->context );
	}
}

#if 0
#pragma mark -
#pragma mark == DNS-SD Special Purpose ==
#endif

//===========================================================================================================================
//	DNSServiceCreateConnection_client
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCreateConnection_client( DNSServiceRef *outRef, const char *inServer )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "CreateConnection (server=\"%s\")\n", inServer ? inServer : "<local>" );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeCreateConnection, "" );
	require_noerr( err, exit );
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceConnectionRecordRemove_client
//
//	Warning: Assumes the RMx lock is held (or is not needed due to a single thread having exclusive access).
//===========================================================================================================================

DEBUG_LOCAL DNSRecordRef	DNSServiceConnectionRecordRemove_client( DNSServiceRef inRef, DNSRecordRef inRecordRef )
{
	DNSRecordRef *		p;
	
	for( p = &inRef->records; *p; p = &( *p )->next )
	{
		if( *p == inRecordRef )
		{
			break;
		}
	}
	inRecordRef = *p;
	if( inRecordRef )
	{
		*p = inRecordRef->next;
	}
	return( inRecordRef );
}

//===========================================================================================================================
//	DNSServiceRegisterRecord_client
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegisterRecord_client(
		DNSServiceRef					inRef,
		DNSRecordRef *					outRecordRef,
		DNSServiceFlags					inFlags,
		uint32_t						inInterfaceIndex,
		const char *					inName,   
		uint16_t						inRRType,
		uint16_t						inRRClass,
		uint16_t						inRDataSize,
		const void *					inRData,
		uint32_t						inTTL,
		DNSServiceRegisterRecordReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	DNSRecordRef			obj;
	
	DEBUG_UNUSED( inContext );
	
	obj = NULL;
	RMxLock();
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME 
		"RegisterRecord flags=0x%08X, ifi=%d, name=\"%s\" rrType=0x%04X, rrClass=0x%04X, rDataSize=%d, ttl=%d\n", 
		inFlags, inInterfaceIndex, inName, inRRType, inRRClass, inRDataSize, inTTL );
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inRef->session, exit, err = kDNSServiceErr_NotInitialized );
	require_action( outRecordRef, exit, err = kDNSServiceErr_BadParam );
	require_action( ( inFlags == kDNSServiceFlagsShared ) || ( inFlags == kDNSServiceFlagsUnique ), 
					exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadFlags );
	
	// Allocate and initialize the object.
	
	obj = (DNSRecordRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->callback = inCallBack;
	obj->context  = inContext;
	
	// Set up a unique ID for the session, avoiding the reserved ID 0, add the record, then send the message.
	
	obj->id = ++inRef->u.connection.lastID;
	if( obj->id == kDNSRecordIndexDefaultTXT )
	{
		obj->id = ++inRef->u.connection.lastID;
	}
	obj->next = inRef->records;
	inRef->records = obj;
	
	err = RMxSessionSendMessage( inRef->session, kRMxOpCodeRegisterRecord, kNoErr, "wwwshhnw", 
		obj->id, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, inRDataSize, inRData, inTTL );
	require_noerr( err, exit );
	
	// Success!
	
	*outRecordRef	= obj;
	obj 			= NULL;
	
exit:
	if( obj )
	{
		DNSServiceConnectionRecordRemove_client( inRef, obj );
		free( obj );
	}
	RMxUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterRecordReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterRecordReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	DNSServiceErrorType		errorCode;
	uint32_t				id;
	DNSRecordRef			record;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	record = NULL;
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "www", &flags, &errorCode, &id );
		check_noerr( err );
		if( err == kNoErr )
		{
			RMxLock();
			for( record = obj->records; record; record = record->next )
			{
				if( record->id == id )
				{
					break;
				}
			}
			RMxUnlock();
			if( !record )
			{
				dlog( kDebugLevelError, DEBUG_NAME "RegisterRecord reply with unknown record ID (%d)\n", id );
				err = kNotFoundErr;
			}
			if( err == kNoErr )
			{
				dlog( kDebugLevelTrace, DEBUG_NAME "RegisterRecord reply id=%d, flags=0x%08X, err=%d\n", id, flags, errorCode );
						
				record->callback( obj, record, flags, errorCode, record->context );
			}
		}
	}
	if( err != kNoErr )
	{
		check( record );
		if( record )
		{
			record->callback( obj, NULL, 0, err, NULL );
		}
	}
}

//===========================================================================================================================
//	DNSServiceQueryRecord_client
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceQueryRecord_client(
		DNSServiceRef *				outRef,
		const char *				inServer, 
		DNSServiceFlags				inFlags,
		uint32_t					inInterfaceIndex,
		const char *				inName,     
		uint16_t					inRRType,
		uint16_t					inRRClass,
		DNSServiceQueryRecordReply	inCallBack,
		void *						inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	
	obj = NULL;
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "QueryRecord flags=0x%08X, ifi=%d, name=\"%s\", rrType=%d, rrClass=%d\n", 
		inFlags, inInterfaceIndex, inName, inRRType, inRRClass );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadFlags );
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->context			= inContext;
	obj->u.query.callback	= inCallBack;
	
	// Open a session to the server and send the request.
	
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, RMxClientMessageCallBack, obj, 
		&obj->session, kRMxOpCodeQueryRecord, "wwshh", inFlags, inInterfaceIndex, inName, inRRType, inRRClass );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj		= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_client( obj );
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceQueryRecordReply_client
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceQueryRecordReply_client( RMxMessage *inMessage )
{
	OSStatus				err;
	DNSServiceRef			obj;
	DNSServiceFlags			flags;
	uint32_t				interfaceIndex;
	DNSServiceErrorType		errorCode;
	const char *			name;
	uint16_t				rrType;
	uint16_t				rrClass;
	uint8_t *				rData;
	size_t					rDataSize;
	uint32_t				ttl;
	
	check( inMessage );
	obj = (DNSServiceRef) inMessage->context;
	check( obj );
	
	err = inMessage->status;
	if( err == kNoErr )
	{
		err = RMxUnpack( inMessage->recvData, inMessage->recvSize, "wwwshhnw", 
			&flags, &interfaceIndex, &errorCode, &name, NULL, &rrType, &rrClass, &rData, &rDataSize, &ttl );
		check_noerr( err );
		if( err == kNoErr )
		{
			dlog( kDebugLevelTrace, DEBUG_NAME
				"QueryRecord reply flags=0x%08X, ifi=%d, err=%d, name=\"%s\", rrType=%d, rrClass=%d, rDataSize=%d, ttl=%d\n", 
				flags, interfaceIndex, errorCode, name, rrType, rrClass, rDataSize, ttl );
			
			obj->u.query.callback( obj, flags, interfaceIndex, errorCode, name, rrType, rrClass, (uint16_t) rDataSize, rData, 
				ttl, obj->context );
		}
	}
	if( err != kNoErr )
	{
		obj->u.query.callback( obj, 0, 0, err, "", 0, 0, 0, NULL, 0, obj->context );
	}
}

//===========================================================================================================================
//	DNSServiceReconfirmRecord_client
//===========================================================================================================================

void
	DNSServiceReconfirmRecord_client(
		const char *	inServer, 
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,   
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData )
{
	DNSServiceErrorType		err;
	
	dlog( kDebugLevelTrace, "\n" DEBUG_NAME "ReconfirmRecord flags=0x%08X, ifi=%d, name=\"%s\", rrType=%d, rrClass=%d\n", 
		inFlags, inInterfaceIndex, inName, inRRType, inRRClass );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
		
	err = RMxSessionOpen( inServer, kRMxSessionFlagsNoClose, kInvalidSocketRef, NULL, NULL, NULL, 
		kRMxOpCodeReconfirmRecord, "wwshhn", inFlags, inInterfaceIndex, inName, inRRType, inRRClass, inRDataSize, inRData );
	require_noerr( err, exit );
	
exit:
	return;
}

#ifdef	__cplusplus
	}
#endif
