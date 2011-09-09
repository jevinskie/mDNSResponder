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

$Log: DNSSDDirect.h,v $
Revision 1.1  2004/01/30 02:46:15  bradley
Portable implementation of the DNS-SD API. This interacts with mDNSCore to perform all the real work
of the DNS-SD API. This code does not rely on any platform-specifics so it should run on any platform
with an mDNS platform plugin available. Software that cannot or does not want to use the IPC mechanism
(e.g. Windows CE, VxWorks, etc.) can use this code directly without any of the IPC pieces.

*/

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		DNSSDDirect.h
	
	@abstract	Direct (compiled-in) implementation of DNS-SD APIs.
	
	@discussion	
	
	Portable implementation of the DNS-SD API. This interacts with mDNSCore to perform all the real work of the DNS-SD API.
	This code does not rely on any platform-specifics so it should run on any platform with an mDNS platform plugin 
	available. Software that cannot or does not want to use the IPC mechanism (e.g. Windows CE, VxWorks, etc.) can use this 
	code directly without any of the IPC pieces.
*/

#ifndef __DNS_SD_DIRECT__
#define __DNS_SD_DIRECT__

#include	"CommonServices.h"

#include	"DNSSD.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == General ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kDNSServiceCacheEntryCountDefault

	@abstract	Default number of mDNS cache entries.
*/

#define	kDNSServiceCacheEntryCountDefault		512

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceInitialize_direct

	@abstract	Initializes the DNSService API. No DNSService API's should be called before this call returns successfully.
*/

DNSServiceErrorType	DNSServiceInitialize_direct( DNSServiceInitializeFlags inFlags, int inCacheEntryCount );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceFinalize_direct

	@abstract	Finalizes the DNSService API. No DNSService API's should be called after this call is made.
*/

void	DNSServiceFinalize_direct( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRefDeallocate_direct

	@abstract	Direct version of DNSServiceRefDeallocate.
*/

void	DNSServiceRefDeallocate_direct( DNSServiceRef inRef );

#if 0
#pragma mark == Domain Enumeration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceEnumerateDomains_client

	@abstract	Direct version of DNSServiceEnumerateDomains.
*/

DNSServiceErrorType
	DNSServiceEnumerateDomains_direct(
		DNSServiceRef *					outRef,
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext );

#if 0
#pragma mark == Service Registration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRegister_direct

	@abstract	Direct version of DNSServiceRegister.
*/

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
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceAddRecord_direct

	@abstract	Direct version of DNSServiceAddRecord.
*/

DNSServiceErrorType
	DNSServiceAddRecord_direct(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceUpdateRecord_direct

	@abstract	Direct version of DNSServiceUpdateRecord.
*/

DNSServiceErrorType
	DNSServiceUpdateRecord_direct(
    DNSServiceRef	inRef,
    DNSRecordRef	inRecordRef,
    DNSServiceFlags	inFlags,
    uint16_t 		inRDataSize,
    const void *	inRData,
    uint32_t		inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRemoveRecord_direct

	@abstract	Direct version of DNSServiceRemoveRecord.
*/

DNSServiceErrorType DNSServiceRemoveRecord_direct( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags );

#if 0
#pragma mark == Service Discovery ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceBrowse_direct

	@abstract	Direct version of DNSServiceBrowse.
*/

DNSServiceErrorType
	DNSServiceBrowse_direct(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceResolve_direct

	@abstract	Direct version of DNSServiceResolve.
*/

DNSServiceErrorType
	DNSServiceResolve_direct( 
		DNSServiceRef *			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,     
		const char *			inType,  
		const char *			inDomain,   
		DNSServiceResolveReply	inCallBack,
		void *					inContext );

#if 0
#pragma mark == Special Purpose ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceCreateConnection_direct

	@abstract	Direct version of DNSServiceCreateConnection.
*/

DNSServiceErrorType	DNSServiceCreateConnection_direct( DNSServiceRef *outRef );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRegisterRecord_direct

	@abstract	Direct version of DNSServiceRegisterRecord.
*/

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
		void *							inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceQueryRecord_direct

	@abstract	Direct version of DNSServiceQueryRecord.
*/

DNSServiceErrorType
	DNSServiceQueryRecord_direct(
		DNSServiceRef *				outRef,
		DNSServiceFlags				inFlags,
		uint32_t					inInterfaceIndex,
		const char *				inName,     
		uint16_t					inRRType,
		uint16_t					inRRClass,
		DNSServiceQueryRecordReply	inCallBack,
		void *						inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceReconfirmRecord_direct

	@abstract	Direct version of DNSServiceReconfirmRecord.
*/

void
	DNSServiceReconfirmRecord_direct(
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,   
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData );

#ifdef	__cplusplus
	}
#endif

#endif	// __DNS_SD_DIRECT__
