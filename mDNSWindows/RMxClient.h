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

$Log: RMxClient.h,v $
Revision 1.1  2004/01/30 02:35:13  bradley
Rendezvous Message Exchange implementation for DNS-SD IPC on Windows.

*/

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		RMxClient.h
	
	@abstract	Client-side implementation of the DNS-SD IPC API.
	
	@discussion	
	
	This handles sending and receiving messages from the service to perform DNS-SD operations and get DNS-SD responses.
*/

#ifndef __RMx_CLIENT__
#define __RMx_CLIENT__

#include	"DNSSD.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == RMx ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxClientInitialize

	@abstract	Initializes RMx for client usage. This must be called before any RMx functions are called.
*/

OSStatus	RMxClientInitialize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxClientFinalize

	@abstract	Finalizes client usage of RMx. No RMx calls should be made after this call is made.
*/

void		RMxClientFinalize( void );

#if 0
#pragma mark == DNS-SD General ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRefDeallocate_client

	@abstract	Client-side version of DNSServiceRefDeallocate.
*/

void	DNSServiceRefDeallocate_client( DNSServiceRef inRef );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceCheckVersion_client

	@abstract	Client-side version of DNSServiceCheckVersion.
*/

DNSServiceErrorType	DNSServiceCheckVersion_client( const char *inServer );

#if 0
#pragma mark == DNS-SD Properties ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceCopyProperty_client

	@abstract	Client-side version of DNSServiceCopyProperty.
*/

DNSServiceErrorType	DNSServiceCopyProperty_client( const char *inServer, DNSPropertyCode inCode, DNSPropertyData *outData );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceReleaseProperty_client

	@abstract	Client-side version of DNSServiceReleaseProperty.
*/

DNSServiceErrorType	DNSServiceReleaseProperty_client( DNSPropertyData *inData );

#if 0
#pragma mark == DNS-SD Domain Enumeration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceEnumerateDomains_client

	@abstract	Client-side version of DNSServiceEnumerateDomains.
*/

DNSServiceErrorType
	DNSServiceEnumerateDomains_client(
		DNSServiceRef *					outRef,
		const char *					inServer, 
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext );

#if 0
#pragma mark == DNS-SD Service Registration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRegister_client

	@abstract	Client-side version of DNSServiceRegister.
*/

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
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceAddRecord_client

	@abstract	Client-side version of DNSServiceAddRecord.
*/

DNSServiceErrorType
	DNSServiceAddRecord_client(
		DNSServiceRef 	inRef,
		DNSRecordRef *	outRecordRef,
		DNSServiceFlags	inFlags,
		uint16_t		inRRType,
		uint16_t		inRDataSize,
		const void *	inRData,
		uint32_t		inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceUpdateRecord_client

	@abstract	Client-side version of DNSServiceUpdateRecord.
*/

DNSServiceErrorType
	DNSServiceUpdateRecord_client(
	    DNSServiceRef	inRef,
	    DNSRecordRef	inRecordRef,
	    DNSServiceFlags	inFlags,
	    uint16_t 		inRDataSize,
	    const void *	inRData,
	    uint32_t		inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRemoveRecord_client

	@abstract	Client-side version of DNSServiceRemoveRecord.
*/

DNSServiceErrorType	DNSServiceRemoveRecord_client( DNSServiceRef inRef, DNSRecordRef inRecordRef, DNSServiceFlags inFlags );

#if 0
#pragma mark == DNS-SD Service Discovery ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceBrowse_client

	@abstract	Client-side version of DNSServiceBrowse.
*/

DNSServiceErrorType
	DNSServiceBrowse_client(
		DNSServiceRef *			outRef,
		const char *			inServer, 
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,   
		const char *			inDomain,
		DNSServiceBrowseReply	inCallBack,
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceResolve_client

	@abstract	Client-side version of DNSServiceResolve.
*/

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
		void *					inContext );

#if 0
#pragma mark == DNS-SD Special Purpose ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceCreateConnection_client

	@abstract	Client-side version of DNSServiceCreateConnection.
*/

DNSServiceErrorType	DNSServiceCreateConnection_client( DNSServiceRef *outRef, const char *inServer );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceRegisterRecord_client

	@abstract	Client-side version of DNSServiceRegisterRecord.
*/

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
		void *							inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceQueryRecord_client

	@abstract	Client-side version of DNSServiceQueryRecord.
*/

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
		void *						inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	DNSServiceReconfirmRecord_client

	@abstract	Client-side version of DNSServiceReconfirmRecord.
*/

void
	DNSServiceReconfirmRecord_client(
		const char *	inServer, 
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

#endif  // __RMx_CLIENT__
