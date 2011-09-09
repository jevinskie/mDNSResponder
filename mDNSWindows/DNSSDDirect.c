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
    
$Log: DNSSDDirect.c,v $
Revision 1.7  2004/06/05 00:04:27  cheshire
<rdar://problem/3668639>: wide-area domains should be returned in reg. domain enumeration

Revision 1.6  2004/05/08 12:25:50  bradley
Changed to use symbolic enums to prevent some compilers from treating it as a sign conversion.

Revision 1.5  2004/05/06 18:42:58  ksekar
General dns_sd.h API cleanup, including the following radars:
<rdar://problem/3592068>: Remove flags with zero value
<rdar://problem/3479569>: Passing in NULL causes a crash.

Revision 1.4  2004/04/15 06:55:50  bradley
Changed resolving to manually query for SRV and TXT records instead of using mDNS_StartResolveService.
This avoids extra A/AAAA record queries and allows local-only resolves to work with normal services.

Revision 1.3  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.2  2004/04/09 21:03:14  bradley
Changed port numbers to use network byte order for consistency with other platforms.

Revision 1.1  2004/01/30 02:46:15  bradley
Portable implementation of the DNS-SD API. This interacts with mDNSCore to perform all the real work
of the DNS-SD API. This code does not rely on any platform-specifics so it should run on any platform
with an mDNS platform plugin available. Software that cannot or does not want to use the IPC mechanism
(e.g. Windows CE, VxWorks, etc.) can use this code directly without any of the IPC pieces.

*/

#include	<stdlib.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	"DNSSD.h"

#if( DNS_SD_DIRECT_ENABLED )

#include	"mDNSClientAPI.h"

#include	"DNSSDDirect.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME		"[DNS-SD Direct] "

typedef uint32_t		DNSServiceRegisterFlags;

#define	kDNSServiceRegisterFlagsAutoName				( 1 << 0 )
#define	kDNSServiceRegisterFlagsAutoNameOnFree			( 1 << 1 )
#define	kDNSServiceRegisterFlagsRenameOnConflict		( 1 << 2 )

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
	DNSServiceRef									next;
	DNSServiceRefReleaseCallBack					releaseCallBack;
	void *											context;
	
	union
	{
		struct	// EnumerateDomains
		{
			DNSQuestion								question;
			mDNSBool								questionActive;
			DNSQuestion								defaultQuestion;
			mDNSBool								defaultQuestionActive;
			DNSServiceDomainEnumReply				callback;
		
		}	domain;

		struct	// Register
		{
			ServiceRecordSet *						set;
			domainlabel								name;
			DNSServiceRegisterFlags					flags;
			DNSServiceRegisterReply					callback;
			
		}	reg;

		struct	// Browse
		{
			DNSQuestion								question;
			mDNSBool								questionActive;
			DNSServiceBrowseReply					callback;
			
		}	browse;
		
		struct	// Resolve
		{
			DNSServiceFlags							flags;
			DNSQuestion								srvQuestion;
			mDNSBool								srvQuestionActive;
			const ResourceRecord *					srvAnswer;
			DNSQuestion								txtQuestion;
			mDNSBool								txtQuestionActive;
			const ResourceRecord *					txtAnswer;
			DNSServiceResolveReply					callback;
			
		}	resolve;
				
		struct	// CreateConnection
		{
			DNSRecordRef							records;
			
		}	connection;
		
		struct	// Query
		{
			DNSQuestion								question;
			mDNSBool								questionActive;
			DNSServiceQueryRecordReply				callback;
			
		}	query;
		
	}	u;
};

// DNSRecordRef

typedef struct _DNSRecordRef_t	_DNSRecordRef_t;
struct	_DNSRecordRef_t
{
	union
	{
		struct	// Service-based records (i.e. DNSServiceRegister-based DNSServiceRef's)
		{
			ExtraResourceRecord					extra;
			
		}	service;
		
		struct	// Connection-based records (i.e. DNSServiceCreateConnection-based DNSServiceRef's)
		{
			DNSRecordRef						next;
			DNSServiceRef						owner;
			DNSServiceRegisterRecordReply		callback;
			void *								context;
			AuthRecord							rr;
			
		}	connection;
	
	}	u;
	
	// WARNING: Do not add fields after the resource record. That is where oversized RData space is allocated.
};

#define	kDNSRecordServiceFixedSize			sizeof_field( _DNSRecordRef_t, u.service )
#define	kDNSRecordConnectionFixedSize		sizeof_field( _DNSRecordRef_t, u.connection )

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

// General

#define	DNSServiceLock()	mDNSPlatformLock( gMDNSPtr )
#define	DNSServiceUnlock()	mDNSPlatformUnlock( gMDNSPtr )

DEBUG_LOCAL void	DNSServiceMDNSCallBack( mDNS * const inMDNS, mStatus inStatus );

// Domain Enumeration

DEBUG_LOCAL void	DNSServiceEnumerateDomainsRelease_direct( DNSServiceRef inRef );
DEBUG_LOCAL void
	DNSServiceEnumerateDomainsCallBack_direct( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord );

// Service Discovery

DEBUG_LOCAL void	DNSServiceBrowseRelease_direct( DNSServiceRef inRef );
DEBUG_LOCAL void
	DNSServiceBrowseCallBack_direct( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord );

DEBUG_LOCAL void	DNSServiceResolveRelease_direct( DNSServiceRef inRef );
DEBUG_LOCAL void
	DNSServiceResolveCallBack_direct(
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const	inAnswer, 
		mDNSBool 						inAddRecord );

// Service Registration

DEBUG_LOCAL void	DNSServiceRegisterRelease_direct( DNSServiceRef inRef );
DEBUG_LOCAL void	DNSServiceRegisterCallBack_direct( mDNS * const inMDNS, ServiceRecordSet * const inSet, mStatus inResult );
DEBUG_LOCAL void	DNSServiceRegisterFree_direct( DNSServiceRef inRef );

DEBUG_LOCAL void	DNSServiceUpdateRecordCallBack_direct( mDNS * const inMDNS, AuthRecord * const inRR, RData *inOldRData );

// Special Purpose

DEBUG_LOCAL void			DNSServiceCreateConnectionRelease_direct( DNSServiceRef inRef );
DEBUG_LOCAL DNSRecordRef	DNSServiceConnectionRecordRemove_direct( DNSServiceRef inRef, DNSRecordRef inRecordRef );

DEBUG_LOCAL void			DNSServiceRegisterRecordCallBack_direct( mDNS *const inMDNS, AuthRecord *const inRR, mStatus inResult );

DEBUG_LOCAL void			DNSServiceQueryRecordRelease_direct( DNSServiceRef inRef );
DEBUG_STATIC void
	DNSServiceQueryRecordCallBack_direct(
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const	inAnswer, 
		mDNSBool 						inAddRecord );

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

mDNS							gMDNS;
DEBUG_LOCAL mDNS *				gMDNSPtr				= NULL;
DEBUG_LOCAL CacheRecord *		gMDNSCache 				= NULL;
DEBUG_LOCAL DNSServiceRef		gDNSServiceRefList		= NULL;
DEBUG_LOCAL DNSServiceRef		gDNSCurrentServiceRef	= NULL;
DEBUG_LOCAL DNSRecordRef		gDNSCurrentRecord		= NULL;

#if 0
#pragma mark -
#pragma mark == General ==
#endif

//===========================================================================================================================
//	DNSServiceInitialize_direct
//===========================================================================================================================

DNSServiceErrorType	DNSServiceInitialize_direct( DNSServiceInitializeFlags inFlags, int inCacheEntryCount )
{
	DNSServiceErrorType		err;
	mDNSBool				advertise;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "initializing (flags=0x%08X, cache=%d/%d)\n", (int) inFlags, 
		inCacheEntryCount, ( inCacheEntryCount == 0 ) ? kDNSServiceCacheEntryCountDefault : inCacheEntryCount );
	
	// Allocate the record cache.
	
	if( inCacheEntryCount == 0 )
	{
		inCacheEntryCount = kDNSServiceCacheEntryCountDefault;
	}
	gMDNSCache = (CacheRecord *) malloc( inCacheEntryCount * sizeof( *gMDNSCache ) );
	require_action( gMDNSCache, exit, err = kDNSServiceErr_NoMemory );
	
	// Initialize mDNS.
	
	if( inFlags & kDNSServiceInitializeFlagsAdvertise )
	{
		advertise = mDNS_Init_AdvertiseLocalAddresses;
	}
	else
	{
		advertise = mDNS_Init_DontAdvertiseLocalAddresses;
	}
	err = mDNS_Init( &gMDNS, NULL, gMDNSCache, (mDNSu32) inCacheEntryCount, advertise, DNSServiceMDNSCallBack, NULL );
	require_noerr( err, exit );
	err = gMDNS.mDNSPlatformStatus;
	require_noerr( err, exit );
	
	gMDNSPtr = &gMDNS;
	
exit:
	dlog( kDebugLevelTrace, DEBUG_NAME "initializing done (err=%d %m)\n", err, err );
	if( err )
	{
		DNSServiceFinalize_direct();
	}
	return( err );
}

//===========================================================================================================================
//	DNSServiceFinalize_direct
//===========================================================================================================================

void	DNSServiceFinalize_direct( void )
{
	dlog( kDebugLevelTrace, DEBUG_NAME "finalizing\n" );
	
	if( gMDNSPtr )
	{
		mDNS_Close( &gMDNS );
		gMDNSPtr = NULL;
	}
	if( gMDNSCache )
	{
		free( gMDNSCache );
		gMDNSCache = mDNSNULL;
	}
	
	dlog( kDebugLevelTrace, DEBUG_NAME "finalizing done\n" );
}

//===========================================================================================================================
//	DNSServiceMDNSCallBack
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceMDNSCallBack( mDNS * const inMDNS, mStatus inStatus )
{
	DEBUG_USE_ONLY( inMDNS );
	check( inMDNS );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "MDNS callback (status=%d)\n", inStatus );
	
	if( inStatus == mStatus_ConfigChanged )
	{
		// Notify all callbacks that the configuration has changed so they can do any additional processing.
		//
		// Warning: This is likely to call a user callback, which may change the object lists. Any code walking 
		// Warning: or changing these lists must use the "current" ptr mechanism to protect against this.
		
		DNSServiceLock();
		dlog( kDebugLevelTrace, DEBUG_NAME "handling ConfigChanged\n" );
		
		check_string( !gDNSCurrentServiceRef, "somebody is already using gDNSCurrentServiceRef!" );
		gDNSCurrentServiceRef = gDNSServiceRefList;
		while( gDNSCurrentServiceRef )
		{
			DNSServiceRef		obj;
			
			obj = gDNSCurrentServiceRef;
			gDNSCurrentServiceRef = obj->next;
			
			// Call the callback with the ConfigChanged error code. Use the releaseCallBack to determine the type.
			
			if( obj->releaseCallBack == DNSServiceEnumerateDomainsRelease_direct )
			{
				obj->u.domain.callback( obj, 0, 0, kDNSServiceErr_ConfigChanged, "", obj->context );
			}
			else if( obj->releaseCallBack == DNSServiceRegisterRelease_direct )
			{
				// If auto-renaming and the system name has changed then trigger a re-register with the new name.
				
				if( obj->u.reg.flags & kDNSServiceRegisterFlagsAutoName )
				{
					if( !SameDomainLabel( obj->u.reg.name.c, gMDNSPtr->nicelabel.c ) )
					{
						mStatus		err;
					
						obj->u.reg.flags |= kDNSServiceRegisterFlagsAutoNameOnFree;
						err = mDNS_DeregisterService( gMDNSPtr, obj->u.reg.set );
						check_noerr( err );
					}
				}
				else
				{
					check_string( obj->u.reg.callback, "not auto-naming, but no callback?" );
					
					obj->u.reg.callback( obj, 0, kDNSServiceErr_ConfigChanged, "", "", "", obj->context );
				}
			}
			else if( obj->releaseCallBack == DNSServiceBrowseRelease_direct )
			{
				obj->u.browse.callback( obj, 0, 0, kDNSServiceErr_ConfigChanged, "", "", "", obj->context );
			}
			else if( obj->releaseCallBack == DNSServiceResolveRelease_direct )
			{
				obj->u.resolve.callback( obj, 0, 0, kDNSServiceErr_ConfigChanged, "", "", 0, 0, NULL, obj->context );
			}
			else if( obj->releaseCallBack == DNSServiceCreateConnectionRelease_direct )
			{
				check_string( !gDNSCurrentRecord, "somebody is already using gDNSCurrentRecord!" );
				gDNSCurrentRecord = obj->u.connection.records;
				while( gDNSCurrentRecord )
				{
					DNSRecordRef		record;
					
					record = gDNSCurrentRecord;
					gDNSCurrentRecord = record->u.connection.next;
					
					record->u.connection.callback( record->u.connection.owner, record, 0, kDNSServiceErr_ConfigChanged, 
						record->u.connection.context );
				}
			}
			else if( obj->releaseCallBack == DNSServiceQueryRecordRelease_direct )
			{
				obj->u.query.callback( obj, 0, 0, kDNSServiceErr_ConfigChanged, "", 0, 0, 0,NULL, 0, obj->context );
			}
		}
		DNSServiceUnlock();
	}
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	DNSServiceRefDeallocate_direct
//===========================================================================================================================

void	DNSServiceRefDeallocate_direct( DNSServiceRef inRef )
{
	check( inRef );
	
	dlog( kDebugLevelNotice, DEBUG_NAME "%s: %#p\n", __ROUTINE__, inRef );
	
	DNSServiceLock();
	if( inRef )
	{
		DNSServiceRef *		p;
		
		// Remove the object from the list.
		
		for( p = &gDNSServiceRefList; *p; p = &( *p )->next )
		{
			if( *p == inRef )
			{
				break;
			}
		}
		check( *p );
		
		// Release the object if it was found.
		
		if( *p )
		{
			*p = inRef->next;
			
			// If somebody will be looking at this object next, move the current ptr to the next object.
			
			if( inRef == gDNSCurrentServiceRef )
			{
				dlog( kDebugLevelInfo, DEBUG_NAME "deleting gDNSCurrentServiceRef (%#p)\n", inRef );
				gDNSCurrentServiceRef = inRef->next;
			}
			
			check( inRef->releaseCallBack );
			if( inRef->releaseCallBack )
			{
				inRef->releaseCallBack( inRef );
			}
		}
	}
	DNSServiceUnlock();
}

#if 0
#pragma mark -
#pragma mark == Domain Enumeration ==
#endif

//===========================================================================================================================
//	DNSServiceEnumerateDomains_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceEnumerateDomains_direct(
		DNSServiceRef *					outRef,
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	mDNS_DomainType			type;
	mDNS_DomainType			defaultType;
	DNSServiceFlags			flags;
	mDNSInterfaceID			interfaceID;
	
	obj = NULL;
	DNSServiceLock();
	require_action( outRef, exit, err = kDNSServiceErr_BadParam );
	require_action( ( inFlags == kDNSServiceFlagsBrowseDomains ) || 
					( inFlags == kDNSServiceFlagsRegistrationDomains ), 
					exit, err = kDNSServiceErr_BadFlags );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->releaseCallBack	= DNSServiceEnumerateDomainsRelease_direct;
	obj->context 			= inContext;
	obj->u.domain.callback 	= inCallBack;
	
	obj->next 				= gDNSServiceRefList;
	gDNSServiceRefList		= obj;
				
	// Start the browse operations.

	if( inFlags & kDNSServiceFlagsRegistrationDomains )
	{
		type 		= mDNS_DomainTypeRegistration;
		defaultType	= mDNS_DomainTypeRegistrationDefault;
	}
	else
	{
		type 		= mDNS_DomainTypeBrowse;
		defaultType	= mDNS_DomainTypeBrowseDefault;
	}
	interfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	err = mDNS_GetDomains( gMDNSPtr, &obj->u.domain.question, type, NULL, interfaceID, 
		DNSServiceEnumerateDomainsCallBack_direct, obj );
	require_noerr( err, exit );
	obj->u.domain.questionActive = mDNStrue;
	
	err = mDNS_GetDomains( gMDNSPtr, &obj->u.domain.defaultQuestion, defaultType, NULL, interfaceID, 
		DNSServiceEnumerateDomainsCallBack_direct, obj );
	require_noerr( err, exit );
	obj->u.domain.defaultQuestionActive = mDNStrue;
	
	// Call back immediately with "local." since that is always available for all types of browsing.
	
	flags = kDNSServiceFlagsDefault | kDNSServiceFlagsAdd;
	inCallBack( obj, flags, inInterfaceIndex, kDNSServiceErr_NoError, "local.", inContext );
	
	// Success!
	
	*outRef	= obj;
	obj 	= NULL;
	
exit:
	if( obj )
	{	
		DNSServiceRefDeallocate_direct( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceEnumerateDomainsRelease_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceEnumerateDomainsRelease_direct( DNSServiceRef inRef )
{
	OSStatus		err;
	
	check( inRef );
	
	if( inRef->u.domain.questionActive )
	{
		err = mDNS_StopGetDomains( gMDNSPtr, &inRef->u.domain.question );
		check_noerr( err );
	}
	if( inRef->u.domain.defaultQuestionActive )
	{
		err = mDNS_StopGetDomains( gMDNSPtr, &inRef->u.domain.defaultQuestion );
		check_noerr( err );
	}
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceEnumerateDomainsCallBack_direct
//
//	Warning: Assumes the mDNS platform lock is held (held by mDNS before invoking this callback).
//===========================================================================================================================

DEBUG_LOCAL void
	DNSServiceEnumerateDomainsCallBack_direct( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord )
{
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	char				domain[ MAX_ESCAPED_DOMAIN_NAME ];
	
	DEBUG_UNUSED( inMDNS );
	
	check( inQuestion );
	obj = (DNSServiceRef) inQuestion->QuestionContext;
	check( obj );
	
	flags = inAddRecord ? kDNSServiceFlagsAdd : kDNSServiceFlagsNone;
	if( inAddRecord )
	{
		if( inQuestion == &obj->u.domain.defaultQuestion )
		{
			flags |= kDNSServiceFlagsDefault;
		}
	}
	interfaceIndex = mDNSPlatformInterfaceIndexfromInterfaceID( &gMDNS, inAnswer->InterfaceID );
	ConvertDomainNameToCString( &inAnswer->rdata->u.name, domain );
	
	obj->u.domain.callback( obj, flags, interfaceIndex, kDNSServiceErr_NoError, domain, obj->context );
}

#if 0
#pragma mark -
#pragma mark == Service Registration ==
#endif

//===========================================================================================================================
//	DNSServiceRegister_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegister_direct(
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
	DNSServiceRef			obj;
	mDNSBool				autoName;
	domainname *			host;
	domainname				tempHost;
	mDNSBool				ok;
	size_t					size;
	domainlabel				name;
	domainname				type;
	domainname				domain;
	mDNSIPPort				port;
	mDNSInterfaceID			interfaceID;
	
	obj = NULL;
	DNSServiceLock();
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( ( inFlags == 0 ) || ( inFlags == kDNSServiceFlagsNoAutoRename ), exit, err = kDNSServiceErr_BadFlags );
	autoName = !inName || ( *inName == '\0' );
	require_action( !autoName || !( inFlags & kDNSServiceFlagsNoAutoRename ), exit, err = kDNSServiceErr_BadParam );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	require_action( inTXT || ( inTXTSize == 0 ), exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack || autoName, exit, err = kDNSServiceErr_BadParam );
	
	// Convert all the input strings and make sure they are valid. Use the system name if auto-naming.
	
	if( autoName )
	{
		name = gMDNSPtr->nicelabel;
	}
	else
	{
		ok = MakeDomainLabelFromLiteralString( &name, inName );
		require_action( ok, exit, err = kDNSServiceErr_BadParam );
	}
	
	ok = MakeDomainNameFromDNSNameString( &type, inType ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	if( !inDomain || ( *inDomain == '\0' ) )
	{
		inDomain = "local.";
	}
	ok = MakeDomainNameFromDNSNameString( &domain, inDomain ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	// Set up the host name (if not using the default).
	
	host = NULL;
	if( inHost && ( *inHost != '\0' ) )
	{
		host = &tempHost;
		ok = MakeDomainNameFromDNSNameString( host, inHost ) != NULL;
		require_action( ok, exit, err = kDNSServiceErr_BadParam );
		
		AppendDomainName( host, &domain );
	}
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );

	obj->releaseCallBack	= DNSServiceRegisterRelease_direct;
	obj->context 			= inContext;
	obj->u.reg.flags		= 0;
	if( autoName )
	{
		obj->u.reg.flags |= kDNSServiceRegisterFlagsAutoName;
	}
	if( !( inFlags & kDNSServiceFlagsNoAutoRename ) )
	{
		obj->u.reg.flags |= kDNSServiceRegisterFlagsRenameOnConflict;
	}
	obj->u.reg.callback 	= inCallBack;
	
	// Allocate space for the records, including any extra space to handle an oversized TXT record.
	
	size = sizeof( ServiceRecordSet );
	if( inTXTSize > sizeof( RDataBody ) )
	{
		size += ( inTXTSize - sizeof( RDataBody ) );
	}
	obj->u.reg.set = (ServiceRecordSet *) calloc( 1, size );
	require_action( obj->u.reg.set, exit, err = kDNSServiceErr_NoMemory );

	obj->next 				= gDNSServiceRefList;
	gDNSServiceRefList		= obj;
	
	// Register the service with mDNS.
	
	port.NotAnInteger = inPort;
	interfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	err = mDNS_RegisterService( gMDNSPtr, obj->u.reg.set, &name, &type, &domain, host, port, 
		(const mDNSu8 *) inTXT, inTXTSize, NULL, 0, interfaceID, DNSServiceRegisterCallBack_direct, obj );
	require_noerr( err, exit );
	
	// Success!
	
	*outRef	= obj;
	obj 	= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRegisterFree_direct( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterRelease_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterRelease_direct( DNSServiceRef inRef )
{
	mStatus		err;
	
	check( inRef );
	
	// Deregister the service. If an error occurs (which should never happen), we have to assume that mDNS does not
	// know about the registration and will not call us back with mStatus_MemFree so we free the memory here.
	// Otherwise, mDNS will call us back with a mStatus_MemFree error code when it is safe for us to free the memory.
	
	err = mDNS_DeregisterService( gMDNSPtr, inRef->u.reg.set );
	check_noerr( err );
	if( err != mStatus_NoError )
	{
		DNSServiceRegisterFree_direct( inRef );
	}
}

//===========================================================================================================================
//	DNSServiceRegisterCallBack_direct
//
//	Warning: Assumes the mDNS platform lock is held (held by mDNS before invoking this callback).
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterCallBack_direct( mDNS * const inMDNS, ServiceRecordSet * const inSet, mStatus inResult )
{
	DNSServiceRef		obj;
	mDNSBool			ok;
	domainlabel			name;
	domainname			type;
	domainname			domain;
	char				nameString[ MAX_DOMAIN_LABEL + 1 ];
	char				typeString[ MAX_ESCAPED_DOMAIN_NAME ];
	char				domainString[ MAX_ESCAPED_DOMAIN_NAME ];
	
	DEBUG_UNUSED( inMDNS );
	DEBUG_UNUSED( inSet );
	
	obj = (DNSServiceRef) inSet->ServiceContext;
	check( obj );
	
	if( inResult == mStatus_NoError )
	{
		// Successful Registration.
		
		if( obj->u.reg.callback )
		{
			ok = DeconstructServiceName( &inSet->RR_SRV.resrec.name, &name, &type, &domain );
			check( ok );
			
	        ConvertDomainLabelToCString_unescaped( &name, nameString );
		    ConvertDomainNameToCString( &type, typeString );
		    ConvertDomainNameToCString( &domain, domainString );
			
			obj->u.reg.callback( obj, 0, kDNSServiceErr_NoError, nameString, typeString, domainString, obj->context );
		}
	}
	else if( inResult == mStatus_MemFree )
	{
		// If the AutoNameOnFree flag is set, it means we should re-register with the system name instead of freeing.
		// Otherwise, mDNS is done with the memory so we can safely free it.
		
		if( obj->u.reg.flags & kDNSServiceRegisterFlagsAutoNameOnFree )
		{
			obj->u.reg.flags &= ~kDNSServiceRegisterFlagsAutoNameOnFree;
			obj->u.reg.name = gMDNSPtr->nicelabel;
			inResult = mDNS_RenameAndReregisterService( gMDNSPtr, obj->u.reg.set, &obj->u.reg.name );
			check_noerr( inResult );
		}
		if( inResult != mStatus_NoError )
		{
			DNSServiceRegisterFree_direct( obj );
		}
	}
	else if( inResult == mStatus_NameConflict )
	{
		// Name conflict. If the auto renaming flags are enabled silently rename and re-register.
		// Otherwise, mDNS will not send call us again with mStatus_MemFree so free the memory here.
		
		if( obj->u.reg.flags & ( kDNSServiceRegisterFlagsAutoName | kDNSServiceRegisterFlagsRenameOnConflict ) )
		{
			inResult = mDNS_RenameAndReregisterService( gMDNSPtr, obj->u.reg.set, mDNSNULL );
		}
		if( inResult != mStatus_NoError )
		{
			if( obj->u.reg.callback )
			{
				obj->u.reg.callback( obj, 0, kDNSServiceErr_NameConflict, "", "", "", obj->context );
			}
			DNSServiceRegisterFree_direct( obj );
		}
	}
	else
	{
		dlog( kDebugLevelNotice, "unknown register result (%d)\n", inResult );
		if( obj->u.reg.callback )
		{
			obj->u.reg.callback( obj, 0, inResult, "", "", "", obj->context );
		}
	}
}

//===========================================================================================================================
//	DNSServiceRegisterFree_direct
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterFree_direct( DNSServiceRef inRef )
{
	check( inRef );
	
	if( inRef->u.reg.set )
	{
		// Note: Each "Extras" record is a "DNSServiceRef", which is just a syntactic wrapper for ExtraResourceRecord.
		// This avoids the need for casting and simplifies access, but still allows the following loop to work correctly.
		
		while( inRef->u.reg.set->Extras )
		{
			ExtraResourceRecord *		extra;
			
			extra = inRef->u.reg.set->Extras;
			inRef->u.reg.set->Extras = extra->next;
			
			free( extra );
		}    
		free( inRef->u.reg.set );
	}
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceAddRecord_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceAddRecord_direct(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL )
{
	DNSServiceErrorType			err;
	size_t						size;
	DNSRecordRef				obj;
	ExtraResourceRecord *		extra;
	
	obj = NULL;
	DNSServiceLock();
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inRef->releaseCallBack == DNSServiceRegisterRelease_direct, exit, err = kDNSServiceErr_BadParam );
	require_action( inRef->u.reg.set, exit, err = kDNSServiceErr_NotInitialized );
	require_action( outRecordRef, exit, err = kDNSServiceErr_BadParam );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the record. Allocate oversized record space at the end of the record.
	
	size = ( inRDataSize > sizeof( RDataBody ) ) ? inRDataSize : sizeof( RDataBody );
	obj = (DNSRecordRef) calloc( 1, ( kDNSRecordServiceFixedSize - sizeof( RDataBody ) ) + size );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	extra 								= &obj->u.service.extra;
	extra->r.resrec.rrtype				= inRRType;
	extra->r.resrec.rdlength			= inRDataSize;
	extra->r.rdatastorage.MaxRDLength	= (mDNSu16) size;
	memcpy( extra->r.rdatastorage.u.data, inRData, inRDataSize );
	
	// Register the record with mDNS.
	
	err = mDNS_AddRecordToService( gMDNSPtr, inRef->u.reg.set, extra, &extra->r.rdatastorage, inTTL );
	require_noerr( err, exit );
	
	// Success!
	
	*outRecordRef	= (DNSRecordRef) obj;
	obj 			= NULL;
	
exit:
	if( obj )
	{
		free( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceUpdateRecord_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceUpdateRecord_direct(
	    DNSServiceRef	inRef,
	    DNSRecordRef	inRecordRef,
	    DNSServiceFlags	inFlags,
	    uint16_t 		inRDataSize,
	    const void *	inRData,
	    uint32_t		inTTL )
{
	DNSServiceErrorType		err;
	AuthRecord *			rr;
	size_t					size;
	RData *					rd;	
	
	rd = NULL;
	DNSServiceLock();
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( ( inRef->releaseCallBack == DNSServiceRegisterRelease_direct ) || 
					( inRef->releaseCallBack == DNSServiceCreateConnectionRelease_direct ), 
					exit, err = kDNSServiceErr_BadParam );
	require_action( inRef->u.reg.set, exit, err = kDNSServiceErr_NotInitialized );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inRData, exit, err = kDNSServiceErr_BadParam );
	
	// Get the embedded AuthRecord from the DNSRecordRef. Determine the type of DNSServiceRef from the releaseCallBack.
	
	if( inRef->releaseCallBack == DNSServiceRegisterRelease_direct )
	{
		rr = inRecordRef ? &inRecordRef->u.service.extra.r : &inRef->u.reg.set->RR_TXT;
	}
	else if( inRef->releaseCallBack == DNSServiceCreateConnectionRelease_direct )
	{
		require_action( inRecordRef, exit, err = kDNSServiceErr_BadReference );
		rr = &inRecordRef->u.connection.rr;
	}
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "trying to remove a DNSRecordRef with an unsupported DNSServiceRef\n" );
		err = kDNSServiceErr_Unsupported;
		goto exit;
	}
	
	// Allocate and initialize the data. Allocate oversized data at the end of the record.
	
	size = ( inRDataSize > sizeof( RDataBody ) ) ? inRDataSize : sizeof( RDataBody );
	rd = (RData *) calloc( 1, ( sizeof( *rd ) - sizeof( RDataBody ) ) + size );
	require_action( rd, exit, err = kDNSServiceErr_NoMemory );
	
	rd->MaxRDLength	= (mDNSu16) size;
	memcpy( rd->u.data, inRData, inRDataSize );
	
	// Update the record. A NULL record means to update the primary TXT record.
	
	err = mDNS_Update( gMDNSPtr, rr, inTTL, inRDataSize, rd, DNSServiceUpdateRecordCallBack_direct );
	require_noerr( err, exit );
	
	// Success!
	
	rd = NULL;
	
exit:
	if( rd )
	{
		free( rd );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceUpdateRecord_direct
//
//	Warning: It is not safe to make any mDNS calls here.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceUpdateRecordCallBack_direct( mDNS * const inMDNS, AuthRecord * const inRR, RData *inOldRData )
{
	DEBUG_UNUSED( inMDNS );
	
	check( inOldRData );
	
	if( inOldRData != &inRR->rdatastorage )
	{
		free( inOldRData );
	}
}

//===========================================================================================================================
//	DNSServiceRemoveRecord_direct
//===========================================================================================================================

DNSServiceErrorType DNSServiceRemoveRecord_direct( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags )
{
	DNSServiceErrorType		err;
	
	DNSServiceLock();
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( ( inRef->releaseCallBack == DNSServiceRegisterRelease_direct ) || 
					( inRef->releaseCallBack == DNSServiceCreateConnectionRelease_direct ), 
					exit, err = kDNSServiceErr_BadParam );
	require_action( inRef->u.reg.set, exit, err = kDNSServiceErr_NotInitialized );
	require_action( inRecordRef, exit, err = kDNSServiceErr_BadParam );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	
	// Get the embedded AuthRecord from the DNSRecordRef. Determine the type of DNSServiceRef from the releaseCallBack.
	
	if( inRef->releaseCallBack == DNSServiceRegisterRelease_direct )
	{
		err = mDNS_RemoveRecordFromService( gMDNSPtr, inRef->u.reg.set, &inRecordRef->u.service.extra );
		free( inRecordRef );
		require_noerr( err, exit );
	}
	else if( inRef->releaseCallBack == DNSServiceCreateConnectionRelease_direct )
	{
		mDNSBool		freeRR;
		
		inRecordRef = DNSServiceConnectionRecordRemove_direct( inRef, inRecordRef );
		require_action( inRecordRef, exit, err = kDNSServiceErr_BadParam );
		
		freeRR = ( inRecordRef->u.connection.rr.resrec.RecordType != kDNSRecordTypeShared );
		err = mDNS_Deregister( gMDNSPtr, &inRecordRef->u.connection.rr );
		check_noerr( err );
		if( freeRR || ( err != mStatus_NoError ) )
		{
			free( inRecordRef );
		}
	}
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "trying to remove a DNSRecordRef with an unsupported DNSServiceRef\n" );
		err = kDNSServiceErr_Unsupported;
		goto exit;
	}
	
exit:
	DNSServiceUnlock();
	return( err );
}

#if 0
#pragma mark -
#pragma mark == Service Discovery ==
#endif

//===========================================================================================================================
//	DNSServiceBrowse_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceBrowse_direct(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext )
{
	DNSServiceErrorType		err;
	DNSServiceRef			obj;
	mDNSBool				ok;
	domainname				type;
	domainname				domain;
	mDNSInterfaceID			interfaceID;
	
	obj = NULL;
	DNSServiceLock();
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	// Convert the input strings and make sure they are valid.
	
	ok = MakeDomainNameFromDNSNameString( &type, inType ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	if( !inDomain || ( *inDomain == '\0' ) )
	{
		inDomain = "local.";
	}
	ok = MakeDomainNameFromDNSNameString( &domain, inDomain ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );

	obj->releaseCallBack	= DNSServiceBrowseRelease_direct;
	obj->context 			= inContext;
	obj->u.browse.callback 	= inCallBack;

	obj->next 				= gDNSServiceRefList;
	gDNSServiceRefList		= obj;
		
	// Start browsing.
	
	interfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	err = mDNS_StartBrowse( gMDNSPtr, &obj->u.browse.question, &type, &domain, interfaceID, 
		DNSServiceBrowseCallBack_direct, obj );
	require_noerr( err, exit );
	obj->u.browse.questionActive = mDNStrue;
	
	// Success!
	
	*outRef	= obj;
	obj 	= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_direct( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceBrowseRelease_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceBrowseRelease_direct( DNSServiceRef inRef )
{
	OSStatus		err;
	
	check( inRef );
	
	if( inRef->u.browse.questionActive )
	{
		err = mDNS_StopBrowse( gMDNSPtr, &inRef->u.browse.question );
		check_noerr( err );
	}
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceBrowseCallBack_direct
//
//	Warning: Assumes the mDNS platform lock is held (held by mDNS before invoking this callback).
//===========================================================================================================================

DEBUG_LOCAL void
	DNSServiceBrowseCallBack_direct( 
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const 	inAnswer, 
		mDNSBool						inAddRecord )
{
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	mDNSBool			ok;
	domainlabel			name;
	domainname			type;
	domainname			domain;
	char				nameString[ MAX_DOMAIN_LABEL + 1 ];
	char				typeString[ MAX_ESCAPED_DOMAIN_NAME ];
	char				domainString[ MAX_ESCAPED_DOMAIN_NAME ];
	
	DEBUG_UNUSED( inMDNS );
	check( inQuestion );
	obj = (DNSServiceRef) inQuestion->QuestionContext;
	check( obj );

	flags = inAddRecord ? kDNSServiceFlagsAdd : kDNSServiceFlagsNone;
	interfaceIndex = mDNSPlatformInterfaceIndexfromInterfaceID( &gMDNS, inAnswer->InterfaceID );
	
	ok = DeconstructServiceName( &inAnswer->rdata->u.name, &name, &type, &domain );
	check( ok );
	
	ConvertDomainLabelToCString_unescaped( &name, nameString );
	ConvertDomainNameToCString( &type, typeString );
	ConvertDomainNameToCString( &domain, domainString );
	
	obj->u.browse.callback( obj, flags, interfaceIndex, kDNSServiceErr_NoError, nameString, typeString, domainString, 
		obj->context );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	DNSServiceResolve_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceResolve_direct( 
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
	DNSServiceRef			obj;
	mDNSBool				ok;
	domainlabel				name;
	domainname				type;
	domainname				domain;
	domainname				fqdn;
	mDNSInterfaceID			interfaceID;
	DNSQuestion *			q;
	
	obj = NULL;
	DNSServiceLock();
	dlog( kDebugLevelTrace, DEBUG_NAME "%s\n", __ROUTINE__ );
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inType, exit, err = kDNSServiceErr_BadParam );
	if( !inDomain || ( *inDomain == '\0' ) ) inDomain = "local.";
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	// Convert all the input strings and make sure they are valid.
	
	ok = MakeDomainLabelFromLiteralString( &name, inName );
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	ok = MakeDomainNameFromDNSNameString( &type, inType ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	ok = MakeDomainNameFromDNSNameString( &domain, inDomain ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	ok = ConstructServiceName( &fqdn, &name, &type, &domain ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	interfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->releaseCallBack	= DNSServiceResolveRelease_direct;
	obj->context 			= inContext;
	obj->u.resolve.flags	= inFlags;
	obj->u.resolve.callback	= inCallBack;
	
	obj->next 			= gDNSServiceRefList;
	gDNSServiceRefList	= obj;
	
	// Start the SRV query.
	
	q					= &obj->u.resolve.srvQuestion;
	q->InterfaceID 		= interfaceID;
	AssignDomainName( q->qname, fqdn );
	q->qtype 			= kDNSType_SRV;
	q->qclass 			= kDNSClass_IN;
	q->QuestionCallback = DNSServiceResolveCallBack_direct;
	q->QuestionContext 	= obj;
	
	err = mDNS_StartQuery( gMDNSPtr, q );
	require_noerr( err, exit );
	obj->u.resolve.srvQuestionActive = mDNStrue;
	
	// Start the TXT query.
	
	q					= &obj->u.resolve.txtQuestion;
	q->InterfaceID 		= interfaceID;
	AssignDomainName( q->qname, fqdn );
	q->qtype 			= kDNSType_TXT;
	q->qclass 			= kDNSClass_IN;
	q->QuestionCallback = DNSServiceResolveCallBack_direct;
	q->QuestionContext 	= obj;
	
	err = mDNS_StartQuery( gMDNSPtr, q );
	require_noerr( err, exit );
	obj->u.resolve.txtQuestionActive = mDNStrue;
	
	// Success!
	
	*outRef	= obj;
	obj 	= NULL;
	
exit:
	if( obj )
	{
		DNSServiceRefDeallocate_direct( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceResolveRelease_direct
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceResolveRelease_direct( DNSServiceRef inRef )
{
	OSStatus		err;
	
	check( inRef );
		
	if( inRef->u.resolve.srvQuestionActive )
	{
		inRef->u.resolve.srvQuestionActive = mDNSfalse;
		err = mDNS_StopQuery( gMDNSPtr, &inRef->u.resolve.srvQuestion );
		check_noerr( err );
	}
	if( inRef->u.resolve.txtQuestionActive )
	{
		inRef->u.resolve.txtQuestionActive = mDNSfalse;
		err = mDNS_StopQuery( gMDNSPtr, &inRef->u.resolve.txtQuestion );
		check_noerr( err );
	}	
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceResolveCallBack_direct
//===========================================================================================================================

DEBUG_LOCAL void
	DNSServiceResolveCallBack_direct(
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const	inAnswer, 
		mDNSBool 						inAddRecord )
{
	DNSServiceRef				obj;
	const ResourceRecord **		answer;
	uint32_t					ifi;
	char						fullName[ MAX_ESCAPED_DOMAIN_NAME ];
	char						hostName[ MAX_ESCAPED_DOMAIN_NAME ];
	uint16_t					port;
	const char *				txt;
	uint16_t					txtSize;
	
	DEBUG_UNUSED( inMDNS );
	
	check( inQuestion );
	obj = (DNSServiceRef) inQuestion->QuestionContext;
	check( obj );
	check( inAnswer );
	
	// Select the answer based on the type.
	
	if(      inAnswer->rrtype == kDNSType_SRV ) answer = &obj->u.resolve.srvAnswer;
	else if( inAnswer->rrtype == kDNSType_TXT ) answer = &obj->u.resolve.txtAnswer;
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "%s: unexpected rrtype (%d)\n", __ROUTINE__, inAnswer->rrtype );
		goto exit;
	}
	
	// If the record is being removed, invalidate the previous answer. Otherwise, update with the new answer.
	
	if( !inAddRecord )
	{
		if( *answer == inAnswer ) *answer = NULL;
		goto exit;
	}
	*answer = inAnswer;
	
	// Only deliver the result if we have both answers.
	
	if( !obj->u.resolve.srvAnswer || !obj->u.resolve.txtAnswer )
	{
		goto exit;
	}
	
	// Convert the results to the appropriate format and call the callback.
	
	ifi		= mDNSPlatformInterfaceIndexfromInterfaceID( &gMDNS, obj->u.resolve.srvAnswer->InterfaceID );
	ConvertDomainNameToCString( &obj->u.resolve.srvAnswer->name, fullName );
    ConvertDomainNameToCString( &obj->u.resolve.srvAnswer->rdata->u.srv.target, hostName );
    port	= obj->u.resolve.srvAnswer->rdata->u.srv.port.NotAnInteger;
    txt		= (const char *) obj->u.resolve.txtAnswer->rdata->u.txt.c;
    txtSize	= obj->u.resolve.txtAnswer->rdlength;
    
	obj->u.resolve.callback( obj, 0, ifi, kDNSServiceErr_NoError, fullName, hostName, port, txtSize, txt, obj->context );
	
exit:
	return;
}

#if 0
#pragma mark -
#pragma mark == Special Purpose ==
#endif

//===========================================================================================================================
//	DNSServiceCreateConnection_direct
//===========================================================================================================================

DNSServiceErrorType	DNSServiceCreateConnection_direct( DNSServiceRef *outRef )
{
	OSStatus			err;
	DNSServiceRef		obj;
	
	DNSServiceLock();
	require_action( outRef, exit, err = kDNSServiceErr_BadReference );
		
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kNoMemoryErr );

	obj->releaseCallBack 	= DNSServiceCreateConnectionRelease_direct;

	obj->next 				= gDNSServiceRefList;
	gDNSServiceRefList		= obj;
	
	// Success!
	
	*outRef	= obj;
	err 	= kNoErr;
	
exit:
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceCreateConnectionRelease_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceCreateConnectionRelease_direct( DNSServiceRef inRef )
{
	check( inRef );
		
	while( inRef->u.connection.records )
	{
		DNSRecordRef		record;
		mStatus				err;
		mDNSBool			freeRR;
		
		record = inRef->u.connection.records;
		inRef->u.connection.records = record->u.connection.next;
		
		// If somebody will be looking at this object next, move the current ptr to the next object.
		
		if( record == gDNSCurrentRecord )
		{
			dlog( kDebugLevelInfo, DEBUG_NAME "deleting gDNSCurrentRecord (%#p)\n", record );
			gDNSCurrentRecord = record->u.connection.next;
		}
		
		freeRR = ( record->u.connection.rr.resrec.RecordType != kDNSRecordTypeShared );
		err = mDNS_Deregister( gMDNSPtr, &record->u.connection.rr );
		check_noerr( err );
		if( freeRR || ( err != mStatus_NoError ) )
		{
			free( record );
		}
	}
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceConnectionRecordRemove_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL DNSRecordRef	DNSServiceConnectionRecordRemove_direct( DNSServiceRef inRef, DNSRecordRef inRecordRef )
{
	DNSRecordRef *		p;
	
	for( p = &inRef->u.connection.records; *p; p = &( *p )->u.connection.next )
	{
		if( *p == inRecordRef )
		{
			break;
		}
	}
	inRecordRef = *p;
	if( inRecordRef )
	{
		*p = inRecordRef->u.connection.next;
		
		// If somebody will be looking at this object next, move the current ptr to the next object.
		
		if( inRecordRef == gDNSCurrentRecord )
		{
			gDNSCurrentRecord = inRecordRef->u.connection.next;
		}
	}
	return( inRecordRef );
}

//===========================================================================================================================
//	DNSServiceRegisterRecord_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceRegisterRecord_direct(
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
	size_t					size;
	DNSRecordRef			obj;
	AuthRecord *			rr;
	mDNSBool				ok;
		
	obj = NULL;
	DNSServiceLock();
	require_action( inRef, exit, err = kDNSServiceErr_BadReference );
	require_action( outRecordRef, exit, err = kDNSServiceErr_BadParam );
	require_action( ( inFlags == kDNSServiceFlagsShared ) || 
					( inFlags == kDNSServiceFlagsUnique ), 
					exit, err = kDNSServiceErr_BadFlags );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the record. Allocate oversized record space at the end of the record.
	
	size = ( inRDataSize > sizeof( RDataBody ) ) ? inRDataSize : sizeof( RDataBody );
	obj = (DNSRecordRef) calloc( 1, ( kDNSRecordConnectionFixedSize - sizeof( RDataBody ) ) + size );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->u.connection.owner			= inRef;
	obj->u.connection.callback		= inCallBack;
	obj->u.connection.context		= inContext;
	
	obj->u.connection.next 			= inRef->u.connection.records;
	inRef->u.connection.records 	= obj;
	
	rr 								= &obj->u.connection.rr;
	rr->resrec.RecordType			= (mDNSu8)( ( inFlags == kDNSServiceFlagsShared ) ? kDNSRecordTypeShared : kDNSRecordTypeUnique );
	rr->resrec.InterfaceID			= mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	ok = MakeDomainNameFromDNSNameString( &rr->resrec.name, inName ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	rr->resrec.rrtype 				= inRRType;
	rr->resrec.rrclass				= inRRClass;
	rr->resrec.rroriginalttl		= inTTL;
	rr->resrec.rdlength 			= inRDataSize;
	rr->resrec.rdata 				= &rr->rdatastorage;
	rr->resrec.rdata->MaxRDLength 	= inRDataSize;
	memcpy( rr->resrec.rdata->u.data, inRData, inRDataSize );
	rr->RecordContext 				= obj;
	rr->RecordCallback 				= DNSServiceRegisterRecordCallBack_direct;
	
	// Register the record with mDNS.
	
	err = mDNS_Register( gMDNSPtr, rr );
	require_noerr( err, exit );
	
	// Success!
	
	*outRecordRef	= obj;
	obj 			= NULL;
	
exit:
	if( obj )
	{
		DNSServiceConnectionRecordRemove_direct( inRef, obj );
		free( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceRegisterRecord_direct
//
//	Warning: Assumes the mDNS platform lock is held (held by mDNS before invoking this callback).
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceRegisterRecordCallBack_direct( mDNS * const inMDNS, AuthRecord * const inRR, mStatus inResult )
{
	DNSRecordRef		obj;
	
	DEBUG_UNUSED( inMDNS );
	
	check( inRR );
	obj = (DNSRecordRef) inRR->RecordContext;
	check( obj );
	
	if( inResult == mStatus_MemFree )
	{
		DNSServiceConnectionRecordRemove_direct( obj->u.connection.owner, obj );
		free( inRR );
	}
	else
	{
		obj->u.connection.callback( obj->u.connection.owner, obj, 0, inResult, obj->u.connection.context );
	}
}

//===========================================================================================================================
//	DNSServiceQueryRecord_direct
//===========================================================================================================================

DNSServiceErrorType
	DNSServiceQueryRecord_direct(
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
	DNSServiceRef			obj;
	DNSQuestion *			q;
	mDNSBool				ok;
	
	obj = NULL;
	DNSServiceLock();
	require_action( outRef, exit, err = kDNSServiceErr_BadParam );
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inCallBack, exit, err = kDNSServiceErr_BadParam );
	
	// Allocate and initialize the object.
	
	obj = (DNSServiceRef) calloc( 1, sizeof( *obj ) );
	require_action( obj, exit, err = kDNSServiceErr_NoMemory );
	
	obj->releaseCallBack 	= DNSServiceQueryRecordRelease_direct;
	obj->context			= inContext;
	obj->u.query.callback	= inCallBack;
	
	q						= &obj->u.query.question;
	q->InterfaceID 			= mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	ok = MakeDomainNameFromDNSNameString( &q->qname, inName ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	q->qtype 				= inRRType;
	q->qclass 				= inRRClass;
	q->QuestionCallback 	= DNSServiceQueryRecordCallBack_direct;
	q->QuestionContext 		= obj;

	obj->next			 	= gDNSServiceRefList;
	gDNSServiceRefList		= obj;
	
	// Start the query with mDNS.
	
	err = mDNS_StartQuery( gMDNSPtr, q );
	require_noerr( err, exit );
	obj->u.query.questionActive = mDNStrue;
	
	// Success!
	
	*outRef = obj;
	obj 	= NULL;
	
exit:
	if( obj )
	{	
		DNSServiceRefDeallocate_direct( obj );
	}
	DNSServiceUnlock();
	return( err );
}

//===========================================================================================================================
//	DNSServiceQueryRecordRelease_direct
//
//	Warning: Assumes the mDNS platform lock is held.
//===========================================================================================================================

DEBUG_LOCAL void	DNSServiceQueryRecordRelease_direct( DNSServiceRef inRef )
{
	OSStatus		err;
	
	check( inRef );
	
	if( inRef->u.query.questionActive )
	{
		err = mDNS_StopQuery( gMDNSPtr, &inRef->u.query.question );
		check_noerr( err );
	}
	free( inRef );
}

//===========================================================================================================================
//	DNSServiceQueryRecordCallBack_direct
//
//	Warning: Assumes the mDNS platform lock is held (held by mDNS before invoking this callback).
//===========================================================================================================================

DEBUG_LOCAL void
	DNSServiceQueryRecordCallBack_direct(
		mDNS * const 					inMDNS, 
		DNSQuestion *					inQuestion, 
		const ResourceRecord * const	inAnswer, 
		mDNSBool 						inAddRecord )
{
	DNSServiceRef		obj;
	DNSServiceFlags		flags;
	uint32_t			interfaceIndex;
	char				name[ MAX_ESCAPED_DOMAIN_NAME ];
	
	DEBUG_UNUSED( inMDNS );
	
	check( inQuestion );
	obj = (DNSServiceRef) inQuestion->QuestionContext;
	check( obj );
	check( inAnswer );
		
	flags = inAddRecord ? kDNSServiceFlagsAdd : kDNSServiceFlagsNone;
	interfaceIndex = mDNSPlatformInterfaceIndexfromInterfaceID( &gMDNS, inAnswer->InterfaceID );
	ConvertDomainNameToCString( &inAnswer->name, name );
	obj->u.query.callback( obj, flags, interfaceIndex, kDNSServiceErr_NoError, name, inAnswer->rrtype, inAnswer->rrclass, 
		inAnswer->rdlength, &inAnswer->rdata->u, inAddRecord ? inAnswer->rroriginalttl : 0, obj->context );
}

//===========================================================================================================================
//	DNSServiceReconfirmRecord_direct
//===========================================================================================================================

void
	DNSServiceReconfirmRecord_direct(
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,   
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData )
{
	DNSServiceErrorType		err;
	size_t					size;
	AuthRecord *			rr;
	mDNSBool				ok;
		
	rr = NULL;
	DNSServiceLock();
	require_action( inFlags == 0, exit, err = kDNSServiceErr_BadFlags );
	require_action( inName, exit, err = kDNSServiceErr_BadParam );
	require_action( inRData && ( inRDataSize > 0 ), exit, err = kDNSServiceErr_BadParam );
		
	size = ( inRDataSize > sizeof( RDataBody ) ) ? inRDataSize : sizeof( RDataBody );
	rr = (AuthRecord *) calloc( 1, ( sizeof( *rr ) - sizeof( RDataBody ) ) + size );
	require_action( rr, exit, err = kDNSServiceErr_NoMemory );
	
	rr->resrec.RecordType			= (mDNSu8)( ( inFlags == kDNSServiceFlagsShared ) ? kDNSRecordTypeShared : kDNSRecordTypeUnique );
	rr->resrec.InterfaceID			= mDNSPlatformInterfaceIDfromInterfaceIndex( gMDNSPtr, inInterfaceIndex );
	
	ok = MakeDomainNameFromDNSNameString( &rr->resrec.name, inName ) != NULL;
	require_action( ok, exit, err = kDNSServiceErr_BadParam );
	
	rr->resrec.rrtype 				= inRRType;
	rr->resrec.rrclass				= inRRClass;
	rr->resrec.rdlength 			= inRDataSize;
	rr->resrec.rdata 				= &rr->rdatastorage;
	rr->resrec.rdata->MaxRDLength 	= inRDataSize;
	memcpy( rr->resrec.rdata->u.data, inRData, inRDataSize );

	err = mDNS_ReconfirmByValue( gMDNSPtr, &rr->resrec );
	check( ( err == mStatus_BadReferenceErr ) || ( err == mStatus_NoError ) );

exit:
	if( rr )
	{
		free( rr );
	}
	DNSServiceUnlock();
}

#ifdef	__cplusplus
	}
#endif

#endif	// DNS_SD_DIRECT_ENABLED
