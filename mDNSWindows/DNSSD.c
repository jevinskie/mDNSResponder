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
    
$Log: DNSSD.c,v $
Revision 1.3  2004/05/11 03:08:53  bradley
Updated TXT Record API based on latest proposal. This still includes dynamic TXT record building for
a final CVS snapshot for private libraries before this functionality is removed from the public API.

Revision 1.2  2004/05/03 10:34:24  bradley
Implemented preliminary version of the TXTRecord API.

Revision 1.1  2004/01/30 02:45:21  bradley
High-level implementation of the DNS-SD API. Supports both "direct" (compiled-in mDNSCore) and "client"
(IPC<->service) usage. Conditionals can exclude either "direct" or "client" to reduce code size.

*/

#include	<ctype.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSDDirect.h"
#include	"RMxClient.h"

#include	"DNSSD.h"

#ifdef	__cplusplus
	extern "C" {
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[DNS-SD] "

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

DEBUG_LOCAL int DomainEndsInDot( const char *dom );

//===========================================================================================================================
//	Globals
//===========================================================================================================================

#if( DNS_SD_CLIENT_ENABLED )
	const char *			gDNSSDServer			= NULL;
	DEBUG_LOCAL	bool		gDNSSDServerCompatible 	= false;
#endif

#if 0
#pragma mark == General ==
#endif

//===========================================================================================================================
//	DNSServiceInitialize
//===========================================================================================================================

DNSServiceErrorType	DNSServiceInitialize( DNSServiceInitializeFlags inFlags, int inCacheEntryCount )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		err = RMxClientInitialize();
		if( err == kNoErr )
		{
			// Perform a version check to see if the server is compatible.
			
			if( !( inFlags & kDNSServiceInitializeFlagsNoServerCheck ) )
			{
				err = DNSServiceCheckVersion();
				if( err == kNoErr )
				{
					goto exit;
				}
			}
			else
			{
				// Version check disabled so just assume the server is compatible.
				
				gDNSSDServerCompatible = true;
				goto exit;
			}
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		#if( DNS_SD_CLIENT_ENABLED )
			dlog( kDebugLevelNotice, DEBUG_NAME "server missing or incompatible...falling back to direct implementation\n" );
		#endif
		err = DNSServiceInitialize_direct( inFlags, inCacheEntryCount );
		goto exit;
	#else
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inCacheEntryCount );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceFinalize
//===========================================================================================================================

void	DNSServiceFinalize( void )
{
	#if( DNS_SD_CLIENT_ENABLED )
		RMxClientFinalize();
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceFinalize_direct();
	#endif
}

//===========================================================================================================================
//	DNSServiceCheckVersion
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCheckVersion( void )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		err = DNSServiceCheckVersion_client( gDNSSDServer );
		if( err == kNoErr )
		{
			gDNSSDServerCompatible = true;
		}
	#else
		err = kUnsupportedErr;
	#endif
	
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Properties ==
#endif

//===========================================================================================================================
//	DNSServiceCopyProperty
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCopyProperty( DNSPropertyCode inCode, DNSPropertyData *outData )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceCopyProperty_client( gDNSSDServer, inCode, outData );
			goto exit;
		}
	#else
		DEBUG_UNUSED( inCode );
		DEBUG_UNUSED( outData );
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = kUnsupportedErr;
		goto exit;
	#else
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceReleaseProperty
//===========================================================================================================================

DNSServiceErrorType	DNSServiceReleaseProperty( DNSPropertyData *inData )
{	
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceReleaseProperty_client( inData );
			goto exit;
		}
	#else
		DEBUG_UNUSED( inData );
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = kUnsupportedErr;
		goto exit;
	#else
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Unix Domain Socket Access ==
#endif

//===========================================================================================================================
//	DNSServiceRefSockFD
//===========================================================================================================================

int	DNSServiceRefSockFD( DNSServiceRef inRef )
{
	DEBUG_UNUSED( inRef );
	
	dlog( kDebugLevelError, "DNSServiceRefSockFD is not supported\n" );
	return( -1 );
}

//===========================================================================================================================
//	DNSServiceProcessResult
//===========================================================================================================================

DNSServiceErrorType	DNSServiceProcessResult( DNSServiceRef inRef )
{
	DEBUG_UNUSED( inRef );
	
	dlog( kDebugLevelError, "DNSServiceProcessResult is not supported\n" );
	return( kDNSServiceErr_Unsupported );
}

//===========================================================================================================================
//	DNSServiceRefDeallocate
//===========================================================================================================================

void	DNSServiceRefDeallocate( DNSServiceRef inRef )
{
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			DNSServiceRefDeallocate_client( inRef );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceRefDeallocate_direct( inRef );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );

		goto exit;
	#endif
	
exit:
	return;
}

#if 0
#pragma mark -
#pragma mark == Domain Enumeration ==
#endif

//===========================================================================================================================
//	DNSServiceEnumerateDomains
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceEnumerateDomains(
		DNSServiceRef *					outRef,
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceEnumerateDomains_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceEnumerateDomains_direct( outRef, inFlags, inInterfaceIndex, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Service Registration ==
#endif

//===========================================================================================================================
//	DNSServiceRegister
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegister(
		DNSServiceRef *			outRef,
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
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRegister_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inType, inDomain, 
				inHost, inPort, inTXTSize, inTXT, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRegister_direct( outRef, inFlags, inInterfaceIndex, inName, inType, inDomain, inHost, inPort, 
			inTXTSize, inTXT, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inHost );
		DEBUG_UNUSED( inPort );
		DEBUG_UNUSED( inTXTSize );
		DEBUG_UNUSED( inTXT );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceAddRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceAddRecord(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceAddRecord_client( inRef, outRecordRef, inFlags, inRRType, inRDataSize, inRData, inTTL );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceAddRecord_direct( inRef, outRecordRef, inFlags, inRRType, inRDataSize, inRData, inTTL );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( outRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceUpdateRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceUpdateRecord(
	    DNSServiceRef	inRef,
	    DNSRecordRef	inRecordRef,
	    DNSServiceFlags	inFlags,
	    uint16_t 		inRDataSize,
	    const void *	inRData,
	    uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceUpdateRecord_client( inRef, inRecordRef, inFlags, inRDataSize, inRData, inTTL );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceUpdateRecord_direct( inRef, inRecordRef, inFlags, inRDataSize, inRData, inTTL );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( inRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceRemoveRecord
//===========================================================================================================================

DNSServiceErrorType DNSServiceRemoveRecord( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRemoveRecord_client( inRef, inRecordRef, inFlags );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRemoveRecord_direct( inRef, inRecordRef, inFlags );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( inRecordRef );
		DEBUG_UNUSED( inFlags );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Service Discovery ==
#endif

//===========================================================================================================================
//	DNSServiceBrowse
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceBrowse(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceBrowse_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inType, inDomain, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceBrowse_direct( outRef, inFlags, inInterfaceIndex, inType, inDomain, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceResolve
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceResolve( 
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,     
		const char *			inType,  
		const char *			inDomain,   
		DNSServiceResolveReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceResolve_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inType, inDomain, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceResolve_direct( outRef, inFlags, inInterfaceIndex, inName, inType, inDomain, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inType );
		DEBUG_UNUSED( inDomain );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Special Purpose ==
#endif

//===========================================================================================================================
//	DNSServiceConstructFullName
//
//	Copied from dnssd_clientstub.c with minimal changes to support NULL/empty name, type, and domain.
//===========================================================================================================================

int	DNSServiceConstructFullName( char *fullName, const char *service, const char *regtype, const char *domain )
{
	size_t len;
	unsigned char c;
	char *fn = fullName;
	const char *s = service;
	const char *r = regtype;
	const char *d = domain;
	
	if (service && *service)
	{
		while(*s)
		{
			c = (unsigned char) *s++;
			if (c == '.' || (c == '\\')) *fn++ = '\\';		// escape dot and backslash literals
			else if (c <= ' ')					// escape non-printable characters
			{
				*fn++ = '\\';
				*fn++ = (char) ('0' + (c / 100));
				*fn++ = (char) ('0' + (c / 10) % 10);
				c = (unsigned char)('0' + (c % 10));
			}
   			*fn++ = (char) c;
		}
		*fn++ = '.';
	}
	
	if (regtype && *regtype)
	{
		len = strlen(regtype);
		if (DomainEndsInDot(regtype)) len--;
		if (len < 4) return -1;					// regtype must end in _udp or _tcp
		if (strncmp((regtype + len - 4), "_tcp", 4) && strncmp((regtype + len - 4), "_udp", 4)) return -1;
		while(*r)
			*fn++ = *r++;
		if (!DomainEndsInDot(regtype)) *fn++ = '.';
	}
	
	if (!domain || !(*domain))
	{
		domain = "local.";
		d = domain;
	}
	len = strlen(domain);
	if (!len) return -1;
	while(*d) 
		*fn++ = *d++;						
	if (!DomainEndsInDot(domain)) *fn++ = '.';
	*fn = '\0';
	return 0;
}

//===========================================================================================================================
//	DNSServiceCreateConnection
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCreateConnection( DNSServiceRef *outRef )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceCreateConnection_client( outRef, gDNSSDServer );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceCreateConnection_direct( outRef );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );

		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegisterRecord(
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
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceRegisterRecord_client( inRef, outRecordRef, inFlags, inInterfaceIndex, inName, 
				inRRType, inRRClass, inRDataSize, inRData, inTTL, inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceRegisterRecord_direct( inRef, outRecordRef, inFlags, inInterfaceIndex, inName, 
			inRRType, inRRClass, inRDataSize, inRData, inTTL, inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( inRef );
		DEBUG_UNUSED( outRecordRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );
		DEBUG_UNUSED( inTTL );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceQueryRecord
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceQueryRecord(
		DNSServiceRef *				outRef,
		DNSServiceFlags				inFlags,
		uint32_t					inInterfaceIndex,
		const char *				inName,     
		uint16_t					inRRType,
		uint16_t					inRRClass,
		DNSServiceQueryRecordReply	inCallBack,
		void *						inContext )
{
	DNSServiceErrorType		err;
	
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			err = DNSServiceQueryRecord_client( outRef, gDNSSDServer, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
				inCallBack, inContext );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		err = DNSServiceQueryRecord_direct( outRef, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
			inCallBack, inContext );
		goto exit;
	#else
		DEBUG_UNUSED( outRef );
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inCallBack );
		DEBUG_UNUSED( inContext );
		
		err = kUnsupportedErr;
		goto exit;
	#endif
	
exit:
	return( err );
}

//===========================================================================================================================
//	DNSServiceReconfirmRecord
//===========================================================================================================================

void
	DNSServiceReconfirmRecord(
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,   
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData )
{
	#if( DNS_SD_CLIENT_ENABLED )
		if( gDNSSDServerCompatible )
		{
			DNSServiceReconfirmRecord_client( gDNSSDServer, inFlags, inInterfaceIndex, inName, inRRType, inRRClass, 
				inRDataSize, inRData );
			goto exit;
		}
	#endif
	
	#if( DNS_SD_DIRECT_ENABLED )
		DNSServiceReconfirmRecord_direct( inFlags, inInterfaceIndex, inName, inRRType, inRRClass, inRDataSize, inRData );
		goto exit;
	#else
		DEBUG_UNUSED( inFlags );
		DEBUG_UNUSED( inInterfaceIndex );
		DEBUG_UNUSED( inName );
		DEBUG_UNUSED( inRRType );
		DEBUG_UNUSED( inRRClass );
		DEBUG_UNUSED( inRDataSize );
		DEBUG_UNUSED( inRData );

		goto exit;
	#endif
	
exit:
	return;
}

#if 0
#pragma mark -
#pragma mark == Utilities ==
#endif

//===========================================================================================================================
//	DomainEndsInDot
//
//	Copied from dnssd_clientstub.c.
//===========================================================================================================================

DEBUG_LOCAL int DomainEndsInDot( const char *dom )
{
	check( dom );
	
	while(*dom && *(dom + 1))
	{
		if (*dom == '\\')	// advance past escaped byte sequence
		{		
			if (*(dom + 1) >= '0' && *(dom + 1) <= '9') dom += 4;
			else dom += 2;
		}
		else dom++;		// else read one character
	}
	return (*dom == '.');
}

#if 0
#pragma mark -
#pragma mark == TXT Record Building ==
#endif

//===========================================================================================================================
//	Structures
//===========================================================================================================================

typedef struct	_TXTRecordRef_t		_TXTRecordRef_t;
struct	_TXTRecordRef_t
{
	uint8_t *		txt;
	uint16_t		size;
};

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

DEBUG_LOCAL DNSServiceErrorType
	TXTRecordGetValuePtrInternal( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		const char *	inKey, 
		uint8_t **		outItem, 
		uint16_t *		outItemSize, 
		const void **	outValue, 
		uint8_t *		outValueSize );

DEBUG_LOCAL DNSServiceErrorType
	TXTRecordGetItemAtIndexInternal( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		uint16_t 		inIndex, 
		uint8_t **		outItem, 
		uint16_t *		outItemSize, 
		char *			inKeyBuffer, 
		const void **	outValue, 
		uint8_t *		outValueSize );

DEBUG_LOCAL int	TXTRecordMemEqv( const void *inLeft, size_t inLeftSize, const void *inRight, size_t inRightSize );

//===========================================================================================================================
//	TXTRecordCreate
//===========================================================================================================================

DNSServiceErrorType	TXTRecordCreate( TXTRecordRef *outRef )
{
	DNSServiceErrorType		err;
	TXTRecordRef			obj;
	
	require_action_expect( outRef, exit, err = kDNSServiceErr_BadParam );
		
	obj = (TXTRecordRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	*outRef	= obj;
	err = kDNSServiceErr_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	TXTRecordDeallocate
//===========================================================================================================================

void	TXTRecordDeallocate( TXTRecordRef inRef )
{
	check( inRef );
	if( inRef )
	{
		if( inRef->txt ) free( inRef->txt );
		free( inRef );
	}
}

//===========================================================================================================================
//	TXTRecordSetValue
//===========================================================================================================================

DNSServiceErrorType	TXTRecordSetValue( TXTRecordRef inRef, const char *inKey, uint8_t inValueSize, const void *inValue )
{
	DNSServiceErrorType		err;
	const char *			p;
	size_t					keySize;
	uint8_t *				item;
	uint8_t *				itemEnd;
	uintptr_t				itemOffset;
	uint16_t				oldItemSize;
	uint16_t				newItemSize;
	uint16_t				delta;
	uint8_t *				txt;
	
	require_action_expect( inRef, exit, err = kDNSServiceErr_BadParam );
	require_action_expect( inKey, exit, err = kDNSServiceErr_BadParam );
	require_action_expect( inValue || ( inValueSize == 0 ), exit, err = kDNSServiceErr_BadParam );
	
	// Make sure the key is printable US-ASCII values (0x20-0x7E), excluding '=' (0x3D).
	
	for( p = inKey; *p != '\0'; ++p )
	{
		if( ( *p < 0x20 ) || ( *p > 0x7E ) || ( *p == 0x3D ) )
		{
			break;
		}
	}
	keySize = (size_t)( p - inKey );
	require_action( ( keySize > 0 ) && ( keySize <= 255 ) && ( *p == '\0' ), exit, err = kDNSServiceErr_Invalid );
	
	// Make sure the total item size does not exceed 255 bytes.
	
	newItemSize = (uint16_t) keySize;
	if( inValue ) newItemSize += 1;		// Add '=' (only if there is a non-NULL value)
	newItemSize = (uint16_t)( newItemSize + inValueSize );
	require_action( newItemSize <= 255, exit, err = kDNSServiceErr_Invalid );
	
	// Search for an existing item with the same key. If found, replace its value. Otherwise, append a new item.
	
	err = TXTRecordGetValuePtrInternal( inRef->size, inRef->txt, inKey, &item, &oldItemSize, NULL, NULL );
	if( err == kDNSServiceErr_NoError )
	{
		if( newItemSize > oldItemSize )
		{
			// Expand the buffer then shift subsequent item(s) forward to make room for the larger value.
			// Note: this saves off and restores the item pointer since the pointer is invalidated by realloc.
			
			itemOffset = (uintptr_t)( item - inRef->txt );
			delta = (uint16_t)( newItemSize - oldItemSize );
			txt = (uint8_t *) realloc( inRef->txt, (size_t)( inRef->size + delta ) );
			require_action( txt, exit, err = kDNSServiceErr_NoMemory );
			
			item	= txt + itemOffset;
			itemEnd = item + ( 1 + oldItemSize );
			memmove( item + ( 1 + newItemSize ), itemEnd, (size_t)( ( inRef->txt + inRef->size ) - itemEnd ) );
			inRef->txt  = txt;
			inRef->size = (uint16_t)( inRef->size + delta );
		}
		else if( newItemSize < oldItemSize )
		{
			// Shift subsequent item(s) backward to take up the slack then shrink the buffer to fit.
			// Note: this saves off and restores the item pointer since the pointer is invalidated by realloc.
			
			itemEnd = item + ( 1 + oldItemSize );
			memmove( item + ( 1 + newItemSize ), itemEnd, (size_t)( ( inRef->txt + inRef->size ) - itemEnd ) );
			
			itemOffset = (uintptr_t)( item - inRef->txt );
			inRef->size -= ( oldItemSize - newItemSize );
			txt = (uint8_t *) realloc( inRef->txt, inRef->size );
			require_action( txt, exit, err = kDNSServiceErr_NoMemory );
			
			item		= txt + itemOffset;
			inRef->txt	= txt;
		}
	}
	else
	{
		// Resize the buffer to hold the new item.
		
		delta = (uint16_t)( 1 + newItemSize );
		txt = (uint8_t *) realloc( inRef->txt, (size_t)( inRef->size + delta ) );
		require_action( txt, exit, err = kDNSServiceErr_NoMemory );
		
		item		= txt + inRef->size;
		inRef->txt  = txt;
		inRef->size	= (uint16_t)( inRef->size + delta );
	}
	
	// Write the new key/value pair to the TXT record in the form key[=<value>].
	// Note: This always writes the entire item to handle case changes in the key.
	
	*item++ = (uint8_t) newItemSize;
	memcpy( item, inKey, keySize );
	item += keySize;
	if( inValue ) *item++ = '=';	// Add '=' (only if there is a non-NULL value)
	memcpy( item, inValue, inValueSize );
	err = kDNSServiceErr_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	TXTRecordRemoveValue
//===========================================================================================================================

DNSServiceErrorType	TXTRecordRemoveValue( TXTRecordRef inRef, const char *inKey )
{
	DNSServiceErrorType		err;
	uint8_t *				item;
	uint8_t *				itemEnd;
	uint16_t				size;
	uint8_t *				txt;
	uint8_t *				txtEnd;
	
	err = TXTRecordGetValuePtrInternal( inRef->size, inRef->txt, inKey, &item, &size, NULL, NULL );
	require_noerr_quiet( err, exit );
	
	// Shift subsequent item(s) back to take up the slack of removing the item.
	
	itemEnd = item + ( 1 + size );
	txtEnd  = inRef->txt + inRef->size;
	if( itemEnd < txtEnd )
	{
		memmove( item, itemEnd, (size_t)( txtEnd - itemEnd ) );
	}
	
	// Shrink the buffer to fit. If size goes to 0, use free because realloc with size 0 is not consistent across platforms.
	
	inRef->size = (uint16_t)( inRef->size - ( (uint16_t)( itemEnd - item ) ) );
	if( inRef->size > 0 )
	{
		txt = (uint8_t *) realloc( inRef->txt, inRef->size );
		require_action_quiet( txt, exit, err = kDNSServiceErr_Unknown );
	}
	else
	{
		free( inRef->txt );
		txt = NULL;
	}
	inRef->txt = txt;
	
exit:
	return( err );
}

//===========================================================================================================================
//	TXTRecordGetLength
//===========================================================================================================================

uint16_t	TXTRecordGetLength( TXTRecordRef inRef )
{
	check( inRef );
	
	if( inRef && inRef->txt )
	{
		return( inRef->size );
	}
	return( 0 );
}

//===========================================================================================================================
//	TXTRecordGetBytesPtr
//===========================================================================================================================

const void *	TXTRecordGetBytesPtr( TXTRecordRef inRef )
{
	check( inRef );
	
	if( inRef && inRef->txt )
	{
		return( inRef->txt );
	}
	return( NULL );
}

#if 0
#pragma mark -
#pragma mark == TXT Record Parsing ==
#endif

//===========================================================================================================================
//	TXTRecordGetValuePtr
//===========================================================================================================================

DNSServiceErrorType
	TXTRecordGetValuePtr( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		const char *	inKey, 
		const void **	outValue, 
		uint8_t *		outValueSize )
{
	return( TXTRecordGetValuePtrInternal( inTXTSize, inTXTBytes, inKey, NULL, NULL, outValue, outValueSize ) );
}

//===========================================================================================================================
//	TXTRecordGetCount
//===========================================================================================================================

uint16_t	TXTRecordGetCount( uint16_t inTXTSize, const void *inTXTBytes )
{
	uint16_t			n;
	const uint8_t *		p;
	const uint8_t *		q;
	
	require_action( inTXTBytes, exit, n = 0 );
	
	n = 0;
	p = (const uint8_t *) inTXTBytes;
	q = p + inTXTSize;
	while( p < q )
	{
		++n;
		p += ( 1 + *p );
		require_action( p <= q, exit, n = 0 );
	}
	
exit:
	return( n );
}

//===========================================================================================================================
//	TXTRecordGetItemAtIndex
//===========================================================================================================================

DNSServiceErrorType
	TXTRecordGetItemAtIndex( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		uint16_t 		inIndex, 
		char *			inKeyBuffer, 
		const void **	outValue, 
		uint8_t *		outValueSize )
{
	return( TXTRecordGetItemAtIndexInternal( inTXTSize, inTXTBytes, inIndex, NULL, NULL, inKeyBuffer, outValue, outValueSize ) );
}

#if 0
#pragma mark -
#pragma mark == TXT Record Internal ==
#endif

//===========================================================================================================================
//	TXTRecordGetValuePtrInternal
//===========================================================================================================================

DEBUG_LOCAL DNSServiceErrorType
	TXTRecordGetValuePtrInternal( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		const char *	inKey, 
		uint8_t **		outItem, 
		uint16_t *		outItemSize, 
		const void **	outValue, 
		uint8_t *		outValueSize )
{
	DNSServiceErrorType		err;
	size_t					keySize;
	const uint8_t *			p;
	const uint8_t *			q;
	const uint8_t *			r;
	const uint8_t *			key;
	const uint8_t *			keyEnd;
	const uint8_t *			value;
	const uint8_t *			valueEnd;
	
	require_action_quiet( inTXTBytes, exit, err = kDNSServiceErr_NoSuchKey );
	require_action_expect( inKey, exit, err = kDNSServiceErr_BadParam );
	keySize = strlen( inKey );
	
	// The following initializations are not necessary, but some compilers warn because they cannot detect it.
	
	keyEnd	 = NULL;
	value	 = NULL;
	valueEnd = NULL;
	
	// Find the item with the specified key.
	
	p = (const uint8_t *) inTXTBytes;
	q = p + inTXTSize;
	while( p < q )
	{
		// Parse the key/value tokens. No '=' means the key takes up the entire item.
		
		r = p + ( 1 + *p );
		require_action( r <= q, exit, err = kDNSServiceErr_Invalid );
		
		key		= p + 1;
		keyEnd	= (const uint8_t *) memchr( key, '=', *p );
		if( keyEnd )
		{
			value = keyEnd + 1;
		}
		else
		{
			keyEnd	= r;
			value	= r;
		}
		valueEnd = r;
		if( TXTRecordMemEqv( key, (size_t)( keyEnd - key ), inKey, keySize ) == 0 )
		{
			break;
		}
		
		p = r;
		check( p <= q );
	}
	require_action_quiet( p < q, exit, err = kDNSServiceErr_NoSuchKey );	
	
	// Fill in the results the caller wants.
	
	if( outItem )		*outItem		= (uint8_t *) p;
	if( outItemSize )	*outItemSize	= *p;
	if( outValue )		*outValue		= ( value == keyEnd ) ? NULL : value;	// NULL value ptr means no value.
	if( outValueSize )	*outValueSize	= (uint8_t)( valueEnd - value );
	err = kDNSServiceErr_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	TXTRecordGetItemAtIndexInternal
//===========================================================================================================================

DEBUG_LOCAL DNSServiceErrorType
	TXTRecordGetItemAtIndexInternal( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		uint16_t 		inIndex, 
		uint8_t **		outItem, 
		uint16_t *		outItemSize, 
		char *			inKeyBuffer, 
		const void **	outValue, 
		uint8_t *		outValueSize )
{
	DNSServiceErrorType		err;
	uint16_t				n;
	const uint8_t *			p;
	const uint8_t *			q;
	const uint8_t *			r;
	const uint8_t *			key;
	const uint8_t *			keyEnd;
	const uint8_t *			value;
	const uint8_t *			valueEnd;
	
	require_action_quiet( inTXTBytes, exit, err = kDNSServiceErr_Invalid );
	
	// Find the Nth item in the TXT record.
	
	n = 0;
	p = (const uint8_t *) inTXTBytes;
	q = p + inTXTSize;
	while( p < q )
	{
		if( n == inIndex ) break;
		++n;
		
		p += ( 1 + *p );
		require_action( p <= q, exit, err = kDNSServiceErr_Invalid );
	}
	require_action_quiet( p < q, exit, err = kDNSServiceErr_Invalid );
	
	// Item found. Parse the key/value tokens. No '=' means the key takes up the entire item.
	
	r = p + ( 1 + *p );
	require_action( r <= q, exit, err = kDNSServiceErr_Invalid );
	
	key		= p + 1;
	keyEnd	= (const uint8_t *) memchr( key, '=', *p );
	if( keyEnd )
	{
		value = keyEnd + 1;
	}
	else
	{
		keyEnd	= r;
		value	= r;
	}
	valueEnd = r;
	
	// Fill in the results the caller wants.
	
	if( outItem )		*outItem 		= (uint8_t *) p;
	if( outItemSize )	*outItemSize	= *p;
	if( inKeyBuffer )
	{
		n = (uint16_t)( keyEnd - key );
		memcpy( inKeyBuffer, key, n );
		inKeyBuffer[ n ] = '\0';
	}
	if( outValue )		*outValue		= ( value == keyEnd ) ? NULL : value;	// NULL value ptr means no value.
	if( outValueSize )	*outValueSize	= (uint8_t)( valueEnd - value );
	err = kDNSServiceErr_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	TXTRecordMemEqv
//===========================================================================================================================

DEBUG_LOCAL int	TXTRecordMemEqv( const void *inLeft, size_t inLeftSize, const void *inRight, size_t inRightSize )
{
	const uint8_t *		p1;
	const uint8_t *		p2;
	const uint8_t *		end;
	int					c1;
	int					c2;
	
	if( inLeftSize < inRightSize ) return( -1 );
	if( inLeftSize > inRightSize ) return(  1 );
	
	p1  = (const uint8_t *) inLeft;
	p2  = (const uint8_t *) inRight;
	end = p1 + inLeftSize;
	while( p1 < end )
	{
		c1 = tolower( *p1 );
		c2 = tolower( *p2 );
		if( c1 < c2 ) return( -1 );
		if( c1 > c2 ) return(  1 );
		
		++p1;
		++p2;
	}
	return( 0 );
}

#if( DEBUG )
//===========================================================================================================================
//	TXTRecordTest
//===========================================================================================================================

DNSServiceErrorType	TXTRecordTest( void );

DNSServiceErrorType	TXTRecordTest( void )
{
	DNSServiceErrorType		err;
	const char *			s;
	TXTRecordRef			ref;
	const void *			txt;
	uint16_t				size;
	uint16_t				n;
	char					key[ 256 ];
	const void *			value;
	uint8_t					valueSize;
	
	// Create Existing Test
	
	s = "\014Anon Allowed"
		"\010Options="
		"\025InstalledPlugins=JPEG";
	size = (uint16_t) strlen( s );
	
	n = TXTRecordGetCount( size, s );
	require_action( n == 3, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "test", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "Anon", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "Allowed", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "Anon Allowed", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value == NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "Options", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value != NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, s, "InstalledPlugins", &value, &valueSize );
	require_noerr( err, exit );
	require_action( valueSize == 4, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( value, "JPEG", 4 ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	key[ 0 ] = '\0';
	err = TXTRecordGetItemAtIndex( size, s, 0, key, &value, &valueSize );
	require_noerr( err, exit );
	require_action( strcmp( key, "Anon Allowed" ) == 0, exit, err = kDNSServiceErr_Unknown );
	require_action( value == NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	key[ 0 ] = '\0';
	err = TXTRecordGetItemAtIndex( size, s, 1, key, &value, &valueSize );
	require_noerr( err, exit );
	require_action( strcmp( key, "Options" ) == 0, exit, err = kDNSServiceErr_Unknown );
	require_action( value != NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	key[ 0 ] = '\0';
	err = TXTRecordGetItemAtIndex( size, s, 2, key, &value, &valueSize );
	require_noerr( err, exit );
	require_action( strcmp( key, "InstalledPlugins" ) == 0, exit, err = kDNSServiceErr_Unknown );
	require_action( value != NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 4, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( value, "JPEG", valueSize ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	key[ 0 ] = '\0';
	err = TXTRecordGetItemAtIndex( size, s, 3, key, &value, &valueSize );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
		
	// Empty Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt == NULL, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 0, exit, err = kDNSServiceErr_Unknown );
			
	TXTRecordDeallocate( ref );
	
	// Create Single Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 13, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, "\014Anon Allowed", size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	n = TXTRecordGetCount( size, txt );
	require_action( n == 1, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "test", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Anon", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Allowed", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Anon Allowed", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value == NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "aNoN aLlOwEd", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value == NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	TXTRecordDeallocate( ref );
	
	// Create Multiple Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Options", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "InstalledPlugins", 4, "JPEG" );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 44, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, 
		"\014Anon Allowed"
		"\010Options="
		"\025InstalledPlugins=JPEG", 
		size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	n = TXTRecordGetCount( size, txt );
	require_action( n == 3, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "test", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Anon", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Allowed", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "", NULL, NULL );
	require_action( err != kDNSServiceErr_NoError, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Anon Allowed", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value == NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "Options", &value, &valueSize );
	require_noerr( err, exit );
	require_action( value != NULL, exit, err = kDNSServiceErr_Unknown );
	require_action( valueSize == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordGetValuePtr( size, txt, "InstalledPlugins", &value, &valueSize );
	require_noerr( err, exit );
	require_action( valueSize == 4, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( value, "JPEG", 4 ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	TXTRecordDeallocate( ref );
	
	// Remove Single Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordRemoveValue( ref, "Anon Allowed" );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt == NULL, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 0, exit, err = kDNSServiceErr_Unknown );
	
	TXTRecordDeallocate( ref );
	
	// Remove Multiple Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Options", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "InstalledPlugins", 4, "JPEG" );
	require_noerr( err, exit );
	
	err = TXTRecordRemoveValue( ref, "Options" );
	require_noerr( err, exit );

	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 35, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, 
		"\014Anon Allowed"
		"\025InstalledPlugins=JPEG", 
		size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	n = TXTRecordGetCount( size, txt );
	require_action( n == 2, exit, err = kDNSServiceErr_Unknown );

	err = TXTRecordRemoveValue( ref, "Anon Allowed" );
	require_noerr( err, exit );

	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 22, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, 
		"\025InstalledPlugins=JPEG", 
		size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	n = TXTRecordGetCount( size, txt );
	require_action( n == 1, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordRemoveValue( ref, "InstalledPlugins" );
	require_noerr( err, exit );

	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt == NULL, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 0, exit, err = kDNSServiceErr_Unknown );
		
	TXTRecordDeallocate( ref );
	
	// Replace Test
	
	err = TXTRecordCreate( &ref );
	require_noerr( err, exit );

	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Options", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Options", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "InstalledPlugins", 4, "JPEG" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "InstalledPlugins", 4, "JPEG" );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 44, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, 
		"\014Anon Allowed"
		"\010Options="
		"\025InstalledPlugins=JPEG", 
		size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 4, "test" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, NULL );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 0, "" );
	require_noerr( err, exit );
	
	err = TXTRecordSetValue( ref, "Anon Allowed", 4, "test" );
	require_noerr( err, exit );
	
	txt = TXTRecordGetBytesPtr( ref );
	require_action( txt, exit, err = kDNSServiceErr_Unknown );
	size = TXTRecordGetLength( ref );
	require_action( size == 49, exit, err = kDNSServiceErr_Unknown );
	require_action( memcmp( txt, 
		"\021Anon Allowed=test"
		"\010Options="
		"\025InstalledPlugins=JPEG", 
		size ) == 0, exit, err = kDNSServiceErr_Unknown );
	
	TXTRecordDeallocate( ref );
	
exit:	
	return( err );
}
#endif	// DEBUG

#ifdef	__cplusplus
	}
#endif
