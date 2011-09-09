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

$Log: DNSSD.h,v $
Revision 1.7  2004/05/11 03:08:53  bradley
Updated TXT Record API based on latest proposal. This still includes dynamic TXT record building for
a final CVS snapshot for private libraries before this functionality is removed from the public API.

Revision 1.6  2004/05/06 18:42:58  ksekar
General dns_sd.h API cleanup, including the following radars:
<rdar://problem/3592068>: Remove flags with zero value
<rdar://problem/3479569>: Passing in NULL causes a crash.

Revision 1.5  2004/05/03 10:34:24  bradley
Implemented preliminary version of the TXTRecord API.

Revision 1.4  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.3  2004/04/09 21:03:14  bradley
Changed port numbers to use network byte order for consistency with other platforms.

Revision 1.2  2004/04/08 09:43:42  bradley
Changed callback calling conventions to __stdcall so they can be used with C# delegates.

Revision 1.1  2004/01/30 02:45:21  bradley
High-level implementation of the DNS-SD API. Supports both "direct" (compiled-in mDNSCore) and "client"
(IPC<->service) usage. Conditionals can exclude either "direct" or "client" to reduce code size.

*/

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		DNSSD.h
	
	@abstract	DNS Service Discovery provides registration, domain and service discovery, and name resolving support.
	
	@discussion	
	
	High-level implementation of the DNS-SD API. This supports both "direct" (compiled-in mDNSCore) and "client" (IPC to 
	service) usage. Conditionals can exclude either "direct" or "client" to reduce code size.
*/

#ifndef _DNS_SD_H
#define _DNS_SD_H

#include	"CommonServices.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Configuration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	DNS_SD_DIRECT_ENABLED

	@abstract	Enables or disables DNS-SD direct.
*/

#if( !defined( DNS_SD_DIRECT_ENABLED ) )
	#define	DNS_SD_DIRECT_ENABLED		1
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	DNS_SD_CLIENT_ENABLED

	@abstract	Enables or disables DNS-SD client.
*/

#if( !defined( DNS_SD_CLIENT_ENABLED ) )
	#define	DNS_SD_CLIENT_ENABLED		1
#endif

#if 0
#pragma mark == Types ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	DNSServiceRef

	@abstract	Reference to a DNSService object.
	
	@discussion
	
	Note: client is responsible for serializing access to these structures if they are shared between concurrent threads.
*/

typedef struct _DNSServiceRef_t *		DNSServiceRef;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	DNSRecordRef

	@abstract	Reference to a DNSRecord object.
	
	@discussion
	
	Note: client is responsible for serializing access to these structures if they are shared between concurrent threads.
*/

typedef struct _DNSRecordRef_t *		DNSRecordRef;

#if 0
#pragma mark == Flags ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	DNSServiceFlags

	@abstract	General flags used in DNS-SD functions.
	
	@constant	kDNSServiceFlagsNone
					No flags (makes it clearer to use this symbolic constant rather than using a 0).
					
	@constant	kDNSServiceFlagsMoreComing
					MoreComing indicates to a callback that at least one more result is queued and will be delivered 
					following immediately after this one. Applications should not update their UI to display browse 
					results when the MoreComing flag is set, because this would result in a great deal of ugly 
					flickering on the screen. Applications should instead wait until until MoreComing is not set 
					(i.e. "Finished", for now), and then update their UI. When MoreComing is not set (i.e. 
					"Finished") that doesn't mean there will be no more answers EVER, just that there are no more 
					answers immediately available right now at this instant. If more answers become available in 
					the future they will be delivered as usual.

	@constant	kDNSServiceFlagsAdd
					Service or domain has been added during browsing/querying or domain enumeration.

	@constant	kDNSServiceFlagsDefault
					Default domain has been added during domain enumeration.
	
	@constant	kDNSServiceFlagsNoAutoRename
					Auto renaming should not be performed. Only valid if a name is explicitly specified when 
					registering a service (i.e. the default name is not used).

	@constant	kDNSServiceFlagsShared
					Shared flag for registering individual records on a connected DNSServiceRef. Indicates that there 
					may be multiple records with this name on the network (e.g. PTR records).

	@constant	kDNSServiceFlagsUnique
					Shared flag for registering individual records on a connected DNSServiceRef. Indicates that the 
					record's name is to be unique on the network (e.g. SRV records).

	@constant	kDNSServiceFlagsBrowseDomains
					Enumerates domains recommended for browsing.

	@constant	kDNSServiceFlagsRegistrationDomains
					Enumerates domains recommended for registration.
*/

typedef uint32_t		DNSServiceFlags;
enum
{
	kDNSServiceFlagsNone				= 0, 
	
	kDNSServiceFlagsMoreComing			= ( 1 << 0 ),
	
	kDNSServiceFlagsAdd					= ( 1 << 1 ),
	kDNSServiceFlagsDefault				= ( 1 << 2 ),

	kDNSServiceFlagsNoAutoRename		= ( 1 << 3 ),

	kDNSServiceFlagsShared				= ( 1 << 4 ),
	kDNSServiceFlagsUnique				= ( 1 << 5 ),

	kDNSServiceFlagsBrowseDomains		= ( 1 << 6 ),
	kDNSServiceFlagsRegistrationDomains	= ( 1 << 7 )
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@enum		DNSServiceInitializeFlags

	@abstract	Flags used with DNSServiceInitialize.
	
	@constant	kDNSServiceInitializeFlagsAdvertise
					Indicates that interfaces should be advertised on the network. Software that only performs searches 
					do not need to set this flag.

	@constant	kDNSServiceInitializeFlagsNoServerCheck
					No check for a valid server should be performed. Only applicable if client support is enabled.
*/

typedef uint32_t		DNSServiceInitializeFlags;

#define	kDNSServiceInitializeFlagsNone				0
#define	kDNSServiceInitializeFlagsAdvertise			( 1 << 0 )
#define	kDNSServiceInitializeFlagsNoServerCheck		( 1 << 1 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kDNSServiceMaxDomainName

	@abstract	Maximum length, in bytes, of a domain name represented as an escaped C-String
*/

#define kDNSServiceMaxDomainName				1005

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kDNSServiceInterfaceIndexAny

	@abstract	Interface index to indicate any interface.
*/

#define	kDNSServiceInterfaceIndexAny			0

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kDNSServiceInterfaceIndexAny

	@abstract	Use the local interface only.
*/

#define	kDNSServiceInterfaceIndexLocalOnly		( (uint32_t) ~0 )

#if 0
#pragma mark == Errors ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	DNSServiceErrorType

	@abstract	Possible error code values.
*/

typedef int32_t		DNSServiceErrorType;
enum
{
	// Error codes are in the range FFFE FF00 (-65792) to FFFE FFFF (-65537)

	kDNSServiceErr_NoError					= 0,
	kDNSServiceErr_Unknown					= -65537,	// 0xFFFE FFFF (first error code)
	kDNSServiceErr_NoSuchName				= -65538,
	kDNSServiceErr_NoMemory					= -65539,
	kDNSServiceErr_BadParam					= -65540,
	kDNSServiceErr_BadReference				= -65541,
	kDNSServiceErr_BadState					= -65542,
	kDNSServiceErr_BadFlags					= -65543,
	kDNSServiceErr_Unsupported				= -65544,
	kDNSServiceErr_NotInitialized			= -65545,
	kDNSServiceErr_NoCache					= -65546,
	kDNSServiceErr_AlreadyRegistered		= -65547,
	kDNSServiceErr_NameConflict				= -65548,
	kDNSServiceErr_Invalid					= -65549,
	kDNSServiceErr_Incompatible				= -65551,
	kDNSServiceErr_BadInterfaceIndex		= -65552, 
	kDNSServiceErr_Refused					= -65553, 
	kDNSServiceErr_NoSuchRecord				= -65554,
	kDNSServiceErr_NoAuth					= -65555,
	kDNSServiceErr_NoSuchKey				= -65556, 
	kDNSServiceErr_NoValue					= -65557, 
	kDNSServiceErr_BufferTooSmall			= -65558, 
	
	// TCP Connection Status

	kDNSServiceErr_ConnectionPending		= -65570,
	kDNSServiceErr_ConnectionFailed			= -65571,
	kDNSServiceErr_ConnectionEstablished 	= -65572, 

	// Non-error values

	kDNSServiceErr_GrowCache				= -65790,
	kDNSServiceErr_ConfigChanged			= -65791,
	kDNSServiceErr_MemFree					= -65792	// 0xFFFE FF00 (last error code)
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@var		gDNSSDServer

	@abstract	Global controlling the server to communicate with. Set to NULL for local operation.
*/

extern const char *		gDNSSDServer;

#if 0
#pragma mark == DNS Classes ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@enum		DNSServiceDNSClass

	@abstract	DNS Class
*/

typedef enum											// From RFC 1035
{
	kDNSServiceDNSClass_IN					= 1,		// Internet
	kDNSServiceDNSClass_CS					= 2,		// CSNET
	kDNSServiceDNSClass_CH					= 3,		// CHAOS
	kDNSServiceDNSClass_HS					= 4,		// Hesiod
	kDNSServiceDNSClass_NONE				= 254,		// Used in DNS UPDATE [RFC 2136]

	kDNSServiceDNSClass_Mask				= 0x7FFF,	// Multicast DNS uses the bottom 15 bits to identify the record class...
	kDNSServiceDNSClass_UniqueRRSet			= 0x8000,	// ... and the top bit indicates that all other cached records are now invalid

	kDNSServiceDNSQClass_ANY				= 255,		// Not a DNS class, but a DNS query class, meaning "all classes"
	kDNSServiceDNSQClass_UnicastResponse	= 0x8000	// Top bit set in a question means "unicast response acceptable"
	
}	DNSServiceDNSClass;

#if 0
#pragma mark == DNS Types ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@enum		DNSServiceDNSType

	@abstract	DNS Type
*/

typedef enum							// From RFC 1035
{
	kDNSServiceDNSType_A		= 1,	// Address
	kDNSServiceDNSType_NS		= 2,	// Name Server
	kDNSServiceDNSType_MD		= 3,	// Mail Destination
	kDNSServiceDNSType_MF		= 4,	// Mail Forwarder
	kDNSServiceDNSType_CNAME	= 5,	// Canonical Name
	kDNSServiceDNSType_SOA		= 6,	// Start of Authority
	kDNSServiceDNSType_MB		= 7,	// Mailbox
	kDNSServiceDNSType_MG		= 8,	// Mail Group
	kDNSServiceDNSType_MR		= 9,	// Mail Rename
	kDNSServiceDNSType_NULL		= 10,	// NULL RR
	kDNSServiceDNSType_WKS		= 11,	// Well-known-service
	kDNSServiceDNSType_PTR		= 12,	// Domain name pointer
	kDNSServiceDNSType_HINFO	= 13,	// Host information
	kDNSServiceDNSType_MINFO	= 14,	// Mailbox information
	kDNSServiceDNSType_MX		= 15,	// Mail Exchanger
	kDNSServiceDNSType_TXT		= 16,	// Arbitrary text string
	kDNSServiceDNSType_AAAA 	= 28,	// IPv6 address
	kDNSServiceDNSType_SRV 		= 33,	// Service record
	kDNSServiceDNSTypeQType_ANY = 255	// Not a DNS type, but a DNS query type, meaning "all types"
	
}	DNSServiceDNSType;

#if 0
#pragma mark == General ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceInitialize
	
	@abstract	Initializes DNS-SD.
	
	@param		inFlags				Flags to control the initialization process (only applicable for DNS-SD direct).
	@param		inCacheEntryCount	Size of the cache (only applicable for DNS-SD direct).
*/

DNSServiceErrorType	DNSServiceInitialize( DNSServiceInitializeFlags inFlags, int inCacheEntryCount );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceFinalize
	
	@abstract	Finalizes DNS-SD.
*/

void	DNSServiceFinalize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceCheckVersion
	
	@abstract	Performs a version check. Mainly only needed when client-mode is enabled to verify the server is compatible.
*/

DNSServiceErrorType	DNSServiceCheckVersion( void );

#if 0
#pragma mark == Properties ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSPropertyCode
	
	@abstract	Identifies a property.
*/

typedef uint32_t	DNSPropertyCode;

#define	kDNSPropertyCodeVersion		0x76657273 // 'vers'
	// Field:	"version"
	// Format:	<w:serverCurrentVersion> <w:serverOldestClientVersion> <w:serverOldestServerVersion> 

//---------------------------------------------------------------------------------------------------------------------------
/*! @struct		DNSPropertyData
	
	@abstract	Data for a property.
*/

typedef struct	DNSPropertyData		DNSPropertyData;
struct	DNSPropertyData
{
	DNSPropertyCode		code;
	
	union
	{
		struct	// version
		{
			uint32_t		clientCurrentVersion;
			uint32_t		clientOldestServerVersion;
			uint32_t		serverCurrentVersion;
			uint32_t		serverOldestClientVersion;
			
		}	version;
	
	}	u;
};

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceCopyProperty
	
	@abstract	Copies a property into a newly allocated buffer.
	
	@param		inCode		Property to copy.
	@param		outData		Receives the copied property data. Must be release with DNSServiceReleaseProperty.
*/

DNSServiceErrorType	DNSServiceCopyProperty( DNSPropertyCode inCode, DNSPropertyData *outData );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceReleaseProperty
	
	@abstract	Releases a property copied with a successful call to DNSServiceCopyProperty.
*/

DNSServiceErrorType	DNSServiceReleaseProperty( DNSPropertyData *inData );

#if 0
#pragma mark == Unix Domain Socket Access ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceRefSockFD
	
	@param		inRef	A DNSServiceRef initialized by any of the DNSService calls.
	
	@result		The DNSServiceRef's underlying socket descriptor, or -1 on error.
	
	@discussion
	
	Access underlying Unix domain socket for an initialized DNSServiceRef. The DNS Service Discovery implmementation uses 
	this socket to communicate between the client and the mDNSResponder daemon. The application MUST NOT directly read 
	from or write to this socket. Access to the socket is provided so that it can be used as a run loop source, or in a 
	select() loop: when data is available for reading on the socket, DNSServiceProcessResult() should be called, which will 
	extract the daemon's reply from the socket, and pass it to the appropriate application callback. By using a run loop 
	or select(), results from the daemon can be processed asynchronously. Without using these constructs, 
	DNSServiceProcessResult() will block until the response from the daemon arrives. The client is responsible for ensuring 
	that the data on the socket is processed in a timely fashion - the daemon may terminate its connection with a client 
	that does not clear its socket buffer.
*/

int	DNSServiceRefSockFD( DNSServiceRef inRef );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceProcessResult
	
	@param		inRef	A DNSServiceRef initialized by any of the DNSService calls that take a callback parameter.
	
	@result		Returns kDNSServiceErr_NoError on success, otherwise returns an error code indicating the specific failure 
				that occurred.
	
	@discussion
	
	Read a reply from the daemon, calling the appropriate application callback. This call will block until the daemon's 
	response is received. Use DNSServiceRefSockFD() in conjunction with a run loop or select() to determine the presence 
	of a response from the server before calling this function to process the reply without blocking. Call this function 
	at any point if it is acceptable to block until the daemon's response arrives. Note that the client is responsible 
	for ensuring that DNSServiceProcessResult() is called whenever there is a reply from the daemon - the daemon may 
	terminate its connection with a client that does not process the daemon's responses.
*/

DNSServiceErrorType	DNSServiceProcessResult( DNSServiceRef inRef );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceRefDeallocate
	
	@param		inRef	A DNSServiceRef initialized by any of the DNSService calls.
		
	@discussion
	
	Terminate a connection with the daemon and free memory associated with the DNSServiceRef. Any services or records 
	registered with this DNSServiceRef will be deregistered. Any Browse, Resolve, or Query operations called with this 
	reference will be terminated.
	
	Note: If the reference's underlying socket is used in a run loop or select() call, it should be removed BEFORE 
	DNSServiceRefDeallocate() is called, as this function closes the reference's socket. 
	
	Note: If the reference was initialized with DNSServiceCreateConnection(), any DNSRecordRefs created via this reference 
	will be invalidated by this call - the resource records are deregistered, and their DNSRecordRefs may not be used in 
	subsequent functions. Similarly, if the reference was initialized with DNSServiceRegister, and an extra resource record 
	was added to the service via DNSServiceAddRecord(), the DNSRecordRef created by the Add() call is invalidated when this 
	function is called - the DNSRecordRef may not be used in subsequent functions.

	Note: This call is to be used only with the DNSServiceRef defined by this API. It is not compatible with 
	dns_service_discovery_ref objects defined in the legacy Mach-based DNSServiceDiscovery.h API.
*/

void	DNSServiceRefDeallocate( DNSServiceRef inRef );

#if 0
#pragma mark == Domain Enumeration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceDomainEnumReply
	
	@abstract	Callback function for DNSServiceEnumerateDomains.
	
	@param		inRef
					The DNSServiceRef initialized by DNSServiceEnumerateDomains().

	@param		inFlags
					Possible values are:
					1 (MoreComing)
					2 (Add/Remove)
					4 (Add Default)

	@param		inInterfaceIndex
					 Specifies the interface on which the domain exists. (The index for a given interface is determined 
					 via the if_nametoindex() family of calls). 
		
	@param		inErrorCode
					Will be kDNSServiceErr_NoError (0) on success, otherwise indicates the failure that occurred (other 
					parameters are undefined if errorCode is nonzero).
	
	@param		inDomain
					The name of the domain.
	
	@param		inContext
					 The context pointer passed to DNSServiceEnumerateDomains.
*/

typedef void
	( CALLBACK_COMPAT *DNSServiceDomainEnumReply )(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inDomain,	
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceEnumerateDomains
	
	@abstract	Asynchronously enumerate domains available for browsing and registration.
	
	@param		outRef
					A pointer to an uninitialized DNSServiceRef. May be passed to DNSServiceRefDeallocate() to cancel the 
					enumeration.

	@param		inFlags
					Possible values are:
					64 (BrowseDomains) to enumerate domains recommended for browsing.
					128 (RegistrationDomains) to enumerate domains recommended for registration.

	@param		inInterfaceIndex
					If non-zero, specifies the interface on which to look for domains (the index for a given interface is 
					determined via the if_nametoindex() family of calls). Most applications will pass 0 to enumerate 
					domains on all interfaces. 	

	@param		inCallBack
					The function to be called when a domain is found or the call asynchronously fails.

	@param		inContext
					An application context pointer which is passed to the callback function (may be NULL).

	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is not invoked 
				and the DNSServiceRef is not initialized).
	
	@discussion
	
	Currently, the only domain returned is "local.", but other domains will be returned in future.
	
	The enumeration MUST be cancelled via DNSServiceRefDeallocate() when no more domains are to be found.
*/

DNSServiceErrorType
	DNSServiceEnumerateDomains(
		DNSServiceRef *					outRef,
		const DNSServiceFlags			inFlags,
		const uint32_t					inInterfaceIndex,
		const DNSServiceDomainEnumReply	inCallBack,
		void *							inContext );	// may be NULL

#if 0
#pragma mark == Service Registration ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceRegisterReply
	
	@abstract	Callback function for DNSServiceRegister.
	
	@param		inRef
					The DNSServiceRef initialized by DNSServiceRegister().

	@param		inFlags
					Currently unused, reserved for future use.
		
	@param		inErrorCode
					Will be kDNSServiceErr_NoError on success, otherwise will indicate the failure that occurred 
					(including name conflicts, if the kDNSServiceFlagsNoAutoRenameOnConflict flag was passed to the 
					callout). Other parameters are undefined if errorCode is nonzero.
	
	@param		inName
					The service name registered (if the application did not specify a name in DNSServiceRegister(), this 
					indicates what name was automatically chosen).
	
	@param		inType
					The type of service registered, as it was passed to the callout.
	
	@param		inDomain
					The domain on which the service was registered (if the application did not specify a domain in 
					DNSServiceRegister(), this indicates the default domain on which the service was registered).

	@param		inContext
					 The context pointer that was passed to the callout.
*/

typedef void
	( CALLBACK_COMPAT *DNSServiceRegisterReply )(
		DNSServiceRef 			inRef,
		DNSServiceFlags 		inFlags,
		DNSServiceErrorType 	inErrorCode,
		const char *			inName,	 
		const char *			inType,	
		const char *			inDomain,	
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceRegister
	
	@abstract	Register a service that is discovered via Browse() and Resolve() calls.
	
	@param		outRef
					A pointer to an uninitialized DNSServiceRef. If this call succeeds, the reference may be passed to 
					DNSServiceRefDeallocate() to deregister the service.

	@param		inInterfaceIndex
					If non-zero, specifies the interface on which to register the service (the index for a given interface 
					is determined via the if_nametoindex() family of calls). Most applications will pass 0 to register on 
					all available interfaces. Pass -1 to register a service only on the local machine (service will not be 
					visible to remote hosts).

	@param		inFlags
					Indicates the renaming behavior on name conflict (most applications will pass 0). See flag definitions 
					above for details.

	@param		inName
					If non-NULL, specifies the service name to be registered. Most applications will not specify a name, in 
					which case the computer name is used (this name is communicated to the client via the callback).

	@param		inType
					The service type followed by the protocol, separated by a dot (e.g. "_ftp._tcp"). The transport protocol 
					must be "_tcp" or "_udp".

	@param		inDomain
					If non-NULL, specifies the domain on which to advertise the service. Most applications will not specify 
					a domain, instead automatically registering in the default domain(s).

	@param		inHost
					If non-NULL, specifies the SRV target host name. Most applications will not specify a host, instead 
					automatically using the machine's default host name(s). Note that specifying a non-NULL host does NOT 
					create an address record for that host - the application is responsible for ensuring that the appropriate 
					address record exists, or creating it via DNSServiceRegisterRecord().

	@param		inPort
					The port on which the service accepts connections in network byte order. Pass 0 for a "placeholder" 
					service (i.e. a service that will not be discovered by browsing, but will cause a name conflict if 
					another client tries to register that same name). Most clients will not use placeholder services.

	@param		inTXTSize
					The length of the txtRecord, in bytes. Must be zero if the txtRecord is NULL.

	@param		inTXT
					The txt record rdata. May be NULL. Note that a non-NULL txtRecord MUST be a properly formatted DNS TXT 
					record, i.e. <length byte> <data> <length byte> <data> ...

	@param		inCallBack
					The function to be called when the registration completes or asynchronously fails. The client MAY pass 
					NULL for the callback -	The client will NOT be notified of the default values picked on its behalf, and 
					the client will NOT be notified of any asynchronous errors (e.g. out of memory errors, etc). that may 
					prevent the registration of the service. The client may NOT pass the NoAutoRename flag if the callback 
					is NULL. The client may still deregister the service at any time via DNSServiceRefDeallocate().

	@param		inContext
					An application context pointer which is passed to the callback function (may be NULL).

	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is never invoked 
				and the DNSServiceRef is not initialized).
*/

DNSServiceErrorType
	DNSServiceRegister(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,			// may be NULL
		const char *			inType,
		const char *			inDomain,		// may be NULL
		const char *			inHost,			// may be NULL
		uint16_t				inPort,
		uint16_t				inTXTSize,
		const void *			inTXT,			// may be NULL
		DNSServiceRegisterReply	inCallBack,		// may be NULL
		void *					inContext );	// may be NULL

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceAddRecord
	
	@abstract	Add a record to a registered service.
	
	@param		inRef
					A DNSServiceRef initialized by DNSServiceRegister().
	
	@param		inRecordRef
					A pointer to an uninitialized DNSRecordRef. Upon succesfull completion of this call, this ref may be 
					passed to DNSServiceUpdateRecord() or DNSServiceRemoveRecord(). If the above DNSServiceRef is passed 
					to DNSServiceRefDeallocate(), RecordRef is also invalidated and may not be used further.

	@param		inFlags
					Currently ignored, reserved for future use.

	@param		inRRType
					The type of the record (e.g. TXT, SRV, etc), as defined in nameser.h.

	@param		inRDataSize
					The length, in bytes, of the rdata.

	@param		inRData
					The raw rdata to be contained in the added resource record.

	@param		inTTL
					The time to live of the resource record, in seconds.

	@result		Returns kDNSServiceErr_NoError on success, otherwise returns an error code indicating the error that 
				occurred (the RecordRef is not initialized).
	
	@discussion
	
	The name of the record will be the same as the registered service's name. The record can later be updated or 
	deregistered by passing the RecordRef initialized by this function to DNSServiceUpdateRecord() or 
	DNSServiceRemoveRecord().
*/

DNSServiceErrorType
	DNSServiceAddRecord(
		DNSServiceRef		inRef,
		DNSRecordRef *		inRecordRef,
		DNSServiceFlags 	inFlags,
		uint16_t 			inRRType,
		uint16_t 			inRDataSize,
		const void *		inRData,
		uint32_t 			inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceUpdateRecord
	
	@abstract	Update a registered resource record.
	
	@param		inRef
					A DNSServiceRef that was initialized by DNSServiceRegister() or DNSServiceCreateConnection().
	
	@param		inRecordRef
					A DNSRecordRef initialized by DNSServiceAddRecord, or NULL to update the service's primary txt record.

	@param		inFlags
					Currently ignored, reserved for future use.

	@param		inRDataSize
					The length, in bytes, of the new rdata.

	@param		inRData
					The new rdata to be contained in the updated resource record.

	@param		inTTL
					The time to live of the updated resource record, in seconds.

	@result		Returns kDNSServiceErr_NoError on success, otherwise returns an error code indicating the error that occurred.
	
	@discussion
	
	The record must either be:
		- The primary txt record of a service registered via DNSServiceRegister()
		- A record added to a registered service via DNSServiceAddRecord()
		- An individual record registered by DNSServiceRegisterRecord()
*/

DNSServiceErrorType
	DNSServiceUpdateRecord(
		DNSServiceRef		inRef,
		DNSRecordRef		inRecordRef,	// may be NULL
		DNSServiceFlags		inFlags,
		uint16_t 			inRDataSize,
		const void *		inRData,
		uint32_t 			inTTL );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceRemoveRecord
	
	@abstract	Remove a record previously added to a service record set via DNSServiceAddRecord(), or deregister 
				an record registered individually via DNSServiceRegisterRecord().
	
	@param		inRef
					A DNSServiceRef that was initialized by DNSServiceRegister() or DNSServiceCreateConnection().
	
	@param		inRecordRef
					A DNSRecordRef initialized by a successful call to DNSServiceAddRecord() or DNSServiceRegisterRecord().

	@param		inFlags
					Currently ignored, reserved for future use.

	@result		Returns kDNSServiceErr_NoError on success, otherwise returns an error code indicating the error that occurred.
*/

DNSServiceErrorType
	DNSServiceRemoveRecord(
		DNSServiceRef 	inRef,
		DNSRecordRef 	inRecordRef,
		DNSServiceFlags	inFlags );

#if 0
#pragma mark == Service Discovery ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceBrowseReply
	
	@abstract	Callback function for DNSServiceBrowse.
	
	@param		inRef
					The DNSServiceRef initialized by DNSServiceBrowse().

	@param		inFlags
					Possible values are MoreComing and Add/Remove. See flag definitions for details.

	@param		inInterfaceIndex
					The interface on which the service is advertised. This index should be passed to DNSServiceResolve() 
					when resolving the service. 
		
	@param		inErrorCode
					Will be kDNSServiceErr_NoError (0) on success, otherwise will indicate the failure that occurred.
					Other parameters are undefined if the errorCode is nonzero.
	
	@param		inName
					The service name discovered.
	
	@param		inType
					The service type, as passed in to DNSServiceBrowse().
	
	@param		inDomain
					The domain on which the service was discovered (if the application did not specify a domain in 
					DNSServicBrowse(), this indicates the domain on which the service was discovered).

	@param		inContext
					 The context pointer that was passed to the callout.
*/

typedef void
	( CALLBACK_COMPAT *DNSServiceBrowseReply )(
		DNSServiceRef 			inRef,
		DNSServiceFlags 		inFlags,
		uint32_t 				inInterfaceIndex,
		DNSServiceErrorType 	inErrorCode,
		const char *			inName,	
		const char *			inType,	
		const char *			inDomain,	
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceBrowse
	
	@abstract	Browse for instances of a service.
	
	@param		outRef
					A pointer to an uninitialized DNSServiceRef. May be passed to DNSServiceRefDeallocate() to terminate 
					the browse.

	@param		inFlags
					Currently ignored, reserved for future use.
		
	@param		inInterfaceIndex
					If non-zero, specifies the interface on which to browse for services (the index for a given interface 
					is determined via the if_nametoindex() family of calls). Most applications will pass 0 to browse on all 
					available interfaces. Pass -1 to only browse for services provided on the local host.
	
	@param		inType
					The service type being browsed for followed by the protocol, separated by a dot (e.g. "_ftp._tcp").
					The transport protocol must be "_tcp" or "_udp".
	
	@param		inDomain
					If non-NULL, specifies the domain on which to browse for services. Most applications will not specify a 
					domain, instead browsing on the default domain(s).
	
	@param		inCallBack
					The function to be called when an instance of the service being browsed for is found, or if the call 
					asynchronously fails.

	@param		inContext
					 An application context pointer which is passed to the callback function (may be NULL).
	
	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is not invoked 
				and the DNSServiceRef is not initialized).
*/

DNSServiceErrorType
	DNSServiceBrowse(
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inType,	
		const char *			inDomain,			// may be NULL
		DNSServiceBrowseReply	inCallBack,
		void *					inContext );		// may be NULL

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceResolveReply
	
	@abstract	Callback function for DNSServiceResolve.
	
	@param		inRef
					The DNSServiceRef initialized by DNSServiceResolve().

	@param		inFlags
					Currently unused, reserved for future use.

	@param		inInterfaceIndex
					The interface on which the service was resolved. 

	@param		inErrorCode
					Will be kDNSServiceErr_NoError (0) on success, otherwise will indicate the failure that occurred. 
					Other parameters are undefined if the errorCode is nonzero.

	@param		inFullName
					The full service domain name, in the form <servicename>.<protocol>.<domain>. (Any literal dots (".") 
					are escaped with a backslash ("\."), and literal backslashes are escaped with a second backslash ("\\"), 
					e.g. a web server named "Dr. Pepper" would have the fullname "Dr\.\032Pepper._http._tcp.local."). 
					This is the appropriate format to pass to standard system DNS APIs such as res_query(), or to the 
					special-purpose functions included in this API that take fullname parameters.

	@param		inHostName
					The target hostname of the machine providing the service. This name can be passed to functions like 
					gethostbyname() to identify the host's IP address.

	@param		inPort
					The port number on which connections are accepted for this service in network byte order.

	@param		inTXTSize
					The length of the txt record, in bytes.

	@param		inTXT
					The service's primary txt record, in standard txt record format.
	
	@param		inContext
					The context pointer that was passed to the callout.
*/

typedef void
	( CALLBACK_COMPAT *DNSServiceResolveReply )(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,	
		const char *			inHostName,
		uint16_t 				inPort,
		uint16_t 				inTXTSize,
		const char *			inTXT,
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceResolve
	
	@abstract	Resolve a service name discovered via DNSServiceBrowse() to a target host name, port number, and txt record.
	
	@param		outRef
					A pointer to an uninitialized DNSServiceRef. May be passed to DNSServiceRefDeallocate() to terminate 
					the resolve.

	@param		inFlags
					Currently unused, reserved for future use.

	@param		inInterfaceIndex
					The interface on which to resolve the service. The client should pass the interface on which the 
					servicename was discovered, i.e. the inInterfaceIndex passed to the DNSServiceBrowseReply callback, or 0 
					to resolve the named service on all available interfaces.

	@param		inName
					The service name to be resolved.

	@param		inType
					The service type being resolved followed by the protocol, separated by a dot (e.g. "_ftp._tcp"). The 
					transport protocol must be "_tcp" or "_udp". 

	@param		inDomain
					The domain on which the service is registered, i.e. the domain passed to the DNSServiceBrowseReply 
					callback.

	@param		inCallBack
					The function to be called when a result is found, or if the call asynchronously fails.

	@param		inContext
					An application context pointer which is passed to the callback function (may be NULL).

	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is never 
				invoked and the DNSServiceRef is not initialized). The context pointer that was passed to the callout.
	
	@discussion
	
	Resolve a service name discovered via DNSServiceBrowse() to a target host name, port number, and txt record.

	Note: Applications should NOT use DNSServiceResolve() solely for txt record monitoring - use DNSServiceQueryRecord() 
	instead, as it is more efficient for this task.

	Note: When the desired results have been returned, the client MUST terminate the resolve by calling 
	DNSServiceRefDeallocate().

	Note: DNSServiceResolve() behaves correctly for typical services that have a single SRV record and a single TXT record 
	(the TXT record may be empty). To resolve non-standard services with multiple SRV or TXT records, 
	DNSServiceQueryRecord() should be used.
*/

DNSServiceErrorType
	DNSServiceResolve( 
		DNSServiceRef *			outRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		const char *			inName,	 
		const char *			inType,	
		const char *			inDomain,	
		DNSServiceResolveReply	inCallBack,
		void *					inContext );	// may be NULL

#if 0
#pragma mark == Special Purpose ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceConstructFullName
	
	@abstract	Concatenate a three-part domain name (as returned by the above callbacks) into a properly-escaped full 
				domain name. Note that callbacks in the above functions ALREADY ESCAPE strings where necessary.
	
	@param		outFullName
					A pointer to a buffer that where the resulting full domain name is to be written. The buffer must be 
					kDNSServiceDiscoveryMaxDomainName (1005) bytes in length to accommodate the longest legal domain name 
					without buffer overrun.

	@param		inName
					The service name - any dots or slashes must NOT be escaped. May be NULL (to construct a PTR record 
					name, e.g. "_ftp._tcp.apple.com").

	@param		inType
					The service type followed by the protocol, separated by a dot (e.g. "_ftp._tcp"). 

	@param		inDomain
					The domain name, e.g. "apple.com". Any literal dots or backslashes must be escaped.

	@result		Returns 0 on success, -1 on error.
	
	@discussion
	
	DNS Naming Conventions:

	The following functions refer to resource records by their full domain name, unlike the above
	functions which divide the name into servicename/regtype/domain fields. In the above functions,
	a dot (".") is considered to be a literal dot in the servicename field (e.g. "Dr. Pepper") and 
	a label separator in the regtype ("_ftp._tcp") or domain ("apple.com") fields. Literal dots in 
	the domain field would be escaped with a backslash, and literal backslashes would be escaped with
	a second backslash (this is generally not an issue, as domain names on the Internet today almost 
	never use characters other than letters, digits, or hyphens, and the dots are label separators).
	Furthermore, this is transparent to the caller, so long as the fields are passed between functions 
	without manipulation. However, the following, special-purpose calls use a single, full domain name. 
	As such, all dots are considered to be label separators, unless escaped, and all backslashes are 
	considered to be escape characters, unless preceded by a second backslash. For example, the name 
	"Dr. Smith \ Dr. Johnson" could be passed literally as a service name parameter in the above calls, 
	but in the special purpose call, the dots and backslash would have to be escaped 
	(e.g. "Dr\. Smith \\ Dr\. Johnson._ftp._tcp.apple.com" for an ftp service on the apple.com domain).
*/

int
	DNSServiceConstructFullName(
		char *			outFullName,
		const char *	inName,		// may be NULL
		const char *	inType,
		const char *	inDomain );
	
//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceCreateConnection
	
	@abstract	Create a connection to the daemon allowing efficient registration of multiple individual records.
	
	@param		outRef
					A pointer to an uninitialized DNSServiceRef. Deallocating the reference (via DNSServiceRefDeallocate()) 
					severs the connection and deregisters all records registered on this connection.

	@result		Returns kDNSServiceErr_NoError on success, otherwise returns an error code indicating the specific failure 
				that occurred (in which case the DNSServiceRef is not initialized).

*/

DNSServiceErrorType	DNSServiceCreateConnection( DNSServiceRef *outRef );
 
 //---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceRegisterRecordReply
	
	@abstract	Callback function for DNSServiceRegisterRecord.
	
	@param		inRef
					The connected DNSServiceRef initialized by DNSServiceCreateConnection().

	@param		inRecordRef
					The DNSRecordRef initialized by DNSServiceRegisterRecord(). If the above DNSServiceRef is passed to 
					DNSServiceRefDeallocate(), this DNSRecordRef is invalidated, and may not be used further.

	@param		inFlags
					Currently unused, reserved for future use.

	@param		inErrorCode
					Will be kDNSServiceErr_NoError on success, otherwise will indicate the failure that occurred (including 
					name conflicts). Other parameters are undefined if errorCode is nonzero.

	@param		inContext
					The context pointer that was passed to the callout.
*/

 typedef void
 	( CALLBACK_COMPAT *DNSServiceRegisterRecordReply )(
		DNSServiceRef			inRef,
		DNSRecordRef			inRecordRef,
		DNSServiceFlags			inFlags,
		DNSServiceErrorType		inErrorCode,
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceRegisterRecord
	
	@abstract	Register an individual resource record on a connected DNSServiceRef. 
	
	@param		inRef
					A DNSServiceRef initialized by DNSServiceCreateConnection().

	@param		inRecordRef
					A pointer to an uninitialized DNSRecordRef. Upon succesfull completion of this call, this ref may be 
					passed to DNSServiceUpdateRecord() or DNSServiceRemoveRecord(). (To deregister ALL records registered 
					on a single connected DNSServiceRef and deallocate each of their corresponding DNSServiceRecordRefs, 
					call DNSServiceRefDealloocate()).

	@param		inFlags
					Possible values are Shared/Unique (see flag type definitions for details).

	@param		inInterfaceIndex
					If non-zero, specifies the interface on which to register the record (the index for a given interface 
					is determined via the if_nametoindex() family of calls). Passing 0 causes the record to be registered 
					on all interfaces. Passing -1 causes the record to only be visible on the local host.

	@param		inFullName
					The full domain name of the resource record.

	@param		inRRType
					The numerical type of the resource record (e.g. PTR, SRV, etc), as defined in nameser.h.

	@param		inRRClass
					The class of the resource record, as defined in nameser.h (usually 1 for the Internet class).

	@param		inRDataSize
					Length, in bytes, of the rdata.

	@param		inRData
					A pointer to the raw rdata, as it is to appear in the DNS record.

	@param		inTTL
					The time to live of the resource record, in seconds.

	@param		inCallBack
					The function to be called when a result is found, or if the call asynchronously fails (e.g. because 
					of a name conflict).

	@param		inContext
					An application context pointer which is passed to the callback function (may be NULL).

	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is never 
				invoked and the DNSRecordRef is not initialized).

	@discussion
	
	Note that name conflicts occurring for records registered via this call must be handled by the client in the callback.
*/

DNSServiceErrorType
	DNSServiceRegisterRecord(
		DNSServiceRef					inRef,
		DNSRecordRef *					inRecordRef,
		DNSServiceFlags					inFlags,
		uint32_t						inInterfaceIndex,
		const char *					inFullName,	
		uint16_t						inRRType,
		uint16_t						inRRClass,
		uint16_t						inRDataSize,
		const void *					inRData,
		uint32_t						inTTL,
		DNSServiceRegisterRecordReply	inCallBack,
		void *	inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @typedef	DNSServiceQueryRecordReply
	
	@abstract	Callback function for DNSServiceQueryRecord.
	
	@param		inRef
					The DNSServiceRef initialized by DNSServiceQueryRecord().

	@param		inFlags
					Possible values are Finished/MoreComing and Add/Remove. The Remove flag is set for PTR records with 
					a TTL of 0.

	@param		inInterfaceIndex
					The interface on which the query was resolved (the index for a given interface is determined via the 
					if_nametoindex() family of calls).

	@param		inErrorCode
					Will be kDNSServiceErr_NoError on success, otherwise will indicate the failure that occurred. Other 
					parameters are undefined if errorCode is nonzero.

	@param		inFullName
					The resource record's full domain name.

	@param		inRRType
					The resource record's type (e.g. PTR, SRV, etc) as defined in nameser.h.

	@param		inRRClass
					The class of the resource record, as defined in nameser.h (usually 1).

	@param		inRDataSize
					The length, in bytes, of the resource record rdata.

	@param		inRData
					The raw rdata of the resource record.

	@param		inTTL
					The resource record's time to live, in seconds.

	@param		inContext
					The context pointer that was passed to the callout.
 */

typedef void
	( CALLBACK_COMPAT *DNSServiceQueryRecordReply )(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,	
		uint16_t				inRRType,
		uint16_t				inRRClass,
		uint16_t				inRDataSize,
		const void *			inRData,
		uint32_t				inTTL,
		void *					inContext );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceQueryRecord
	
	@abstract	Query for an arbitrary DNS record.

	@param		outRef
					A pointer to an uninitialized DNSServiceRef.

	@param		inFlags
					Currently unused, reserved for future use.

	@param		inInterfaceIndex
					If non-zero, specifies the interface on which to issue the query (the index for a given interface is 
					determined via the if_nametoindex() family of calls). Passing 0 causes the name to be queried for on all 
					interfaces. Passing -1 causes the name to be queried for only on the local host.

	@param		inName
					The full domain name of the resource record to be queried for.

	@param		inRRType
					The numerical type of the resource record to be queried for (e.g. PTR, SRV, etc) as defined in nameser.h.

	@param		inRRClass
					The class of the resource record, as defined in nameser.h (usually 1 for the Internet class).

	@param		inCallBack
					The function to be called when a result is found, or if the call asynchronously fails.

	@param		inContext
					An application context pointer which is passed to the callback function (may be NULL).

	@result		Returns kDNSServiceErr_NoError on success (any subsequent, asynchronous errors are delivered to the 
				callback), otherwise returns an error code indicating the error that occurred (the callback is never 
				invoked and the DNSServiceRef is not initialized).

	@discussion
	
	Note that name conflicts occurring for records registered via this call must be handled by the client in the callback.
*/
 
DNSServiceErrorType
	DNSServiceQueryRecord(
		DNSServiceRef *				outRef,
		DNSServiceFlags				inFlags,
		uint32_t					inInterfaceIndex,
		const char *				inName, 
		uint16_t					inRRType,
		uint16_t					inRRClass,
		DNSServiceQueryRecordReply	inCallBack,
		void *						inContext );	// may be NULL

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	DNSServiceReconfirmRecord
	
	@abstract	Instruct the daemon to verify the validity of a resource record that appears to be out of date (e.g. 
				because tcp connection to a service's target failed). Causes the record to be flushed from the daemon's 
				cache (as well as all other daemons' caches on the network) if the record is determined to be invalid.

	@param		inFlags
					Currently unused, reserved for future use.

	@param		inName
					The resource record's full domain name.

	@param		inRRType
					The resource record's type (e.g. PTR, SRV, etc) as defined in nameser.h.

	@param		inRRClass
					The class of the resource record, as defined in nameser.h (usually 1).

	@param		inRDataSize
					The length, in bytes, of the resource record rdata.

	@param		inRData
					The raw rdata of the resource record.
*/
 
void
	DNSServiceReconfirmRecord(
		DNSServiceFlags	inFlags,
		uint32_t		inInterfaceIndex,
		const char *	inName,	
		uint16_t		inRRType,
		uint16_t		inRRClass,
		uint16_t		inRDataSize,
		const void *	inRData );

#if 0
#pragma mark == TXT Record Building ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	TXTRecordRef

	@abstract	Reference to a TXTRecord object representing a DNS-SD TXT record.
	
	@discussion
	
	Note: client is responsible for serializing access to these structures if they are shared between concurrent threads.
*/

typedef struct _TXTRecordRef_t *		TXTRecordRef;

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordCreate
	
	@abstract	Creates an empty TXTRecordRef.
	
	@param		outRef
					A pointer to an uninitialized TXTRecordRef. Filled in on success.
	
	@result		Returns kDNSServiceErr_NoError on success.
				Returns kDNSServiceErr_BadParam if the reference pointer is NULL.
				Returns kDNSServiceErr_NoMemory if there is not enough memory to create the TXTRecord.
	
	@discussion
	
	Once you've created a TXTRecordRef, you can pass it to TXTRecordSetValue and other functions to add "key=value" 
	pairs to it. Finally, you can extract the raw bytes again to pass to DNSServiceRegister() or DNSServiceUpdateRecord().
	
	A typical calling sequence for TXT record construction is something like:
	
	TXTRecordCreate();
	TXTRecordSetValue();
	TXTRecordSetValue();
	TXTRecordSetValue();
	...
	DNSServiceRegister( ... TXTRecordGetLength(), TXTRecordGetBytesPtr() ... );
	TXTRecordDeallocate();
*/

DNSServiceErrorType	TXTRecordCreate( TXTRecordRef *outRef );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordDeallocate
	
	@abstract	Releases the memory associated with a TXTRecordRef.
	
	@param		inRef
					A TXTRecordRef initialized by calling TXTRecordCreate.
*/

void	TXTRecordDeallocate( TXTRecordRef inRef );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordSetValue
	
	@abstract	Adds a key (optionally with a value) to a TXTRecordRef.
	
	@param		inRef
					A TXTRecordRef initialized by TXTRecordCreate.
					
	@param		inKey
					Null-terminated key of the value to add. Must be at least 1 character and consist of only printable 
					US-ASCII characters (0x20-0x7E), excluding '=' (0x3D). Should be 14 characters or less (not counting 
					the terminating null). Keys are case insensitive (i.e. key "test" replaces key "TEST").
					
	@param		inValue
					Pointer to value to add. For values that represent textual data, UTF-8 is STRONGLY recommended.
					If NULL, then the key will be added with no value.
					If non-NULL but valueSize is zero, then "key=" will be added with an empty value.
	
	@param		inValueSize
					Number of bytes in the value. Must be 0 if inValue is NULL.
	
	@result		Returns kDNSServiceErr_NoError on success.
				Returns kDNSServiceErr_BadParam if the parameters are illegal or not supported.
				Returns kDNSServiceErr_Invalid if the key string contains illegal characters.
				Returns kDNSServiceErr_NoMemory if there is not enough memory to set the value.
	
	@discussion
	
	If the key is already present in the TXTRecordRef, then the current value will be replaced with the new value.
	Keys may be in four states with respect to a given TXT record:
	
	- Absent (key does not appear at all).
	- Present with no value ("key" appears alone).
	- Present with empty value ("key=" appears in the TXT record).
	- Present with non-empty value ("key=value" appears in the TXT record).
	
	For more details refer to "Data Syntax for DNS-SD TXT Records" in
	<http://files.dns-sd.org/draft-cheshire-dnsext-dns-sd.txt>
*/

DNSServiceErrorType
	TXTRecordSetValue( 
		TXTRecordRef 	inRef, 
		const char *	inKey, 
		uint8_t 		inValueSize,	// may be zero
		const void *	inValue );		// may be NULL

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordRemoveValue

	@abstract	Removes a key from a TXTRecordRef.
	
	@param		inRef
					A TXTRecordRef initialized by TXTRecordCreate.
					
	@param		inKey
					Null-terminated key to remove. Note: keys are case insensitive.

	@result		Returns kDNSServiceErr_NoError on success.
				Returns kDNSServiceErr_NoSuchKey if the key is not present in the TXTRecordRef.
				Returns kDNSServiceErr_BadParam if the parameters are illegal or not supported.
*/

DNSServiceErrorType	TXTRecordRemoveValue( TXTRecordRef inRef, const char *inKey );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordGetLength
	
	@abstract	Returns the number of raw bytes inside a TXTRecordRef.
	
	@param		inRef
					A TXTRecordRef initialized by TXTRecordCreate.
	
	@result		Returns the number of raw bytes inside a TXTRecordRef which you can pass directly to DNSServiceRegister()
				or to DNSServiceUpdateRecord(). Returns 0 if the TXTRecordRef is empty.

	@discussion
	
	The length may become invalid if you subsequently make changes to the TXTRecordRef by calling TXTRecordSetValue() 
	or TXTRecordRemoveValue().
*/

uint16_t	TXTRecordGetLength( TXTRecordRef inRef );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordGetBytesPtr
	
	@abstract	Returns a pointer to the raw bytes inside the TXTRecordRef.
	
	@param		inRef
					A TXTRecordRef initialized by TXTRecordCreate.

	@result		Returns a pointer to the raw bytes inside the TXTRecordRef which you can pass directly to 
				DNSServiceRegister() or to DNSServiceUpdateRecord(). Returns NULL if the TXTRecordRef is empty.

	@discussion
	
	The pointer may become invalid if you subsequently make changes to the TXTRecordRef by calling TXTRecordSetValue() 
	or TXTRecordRemoveValue().
*/

const void *	TXTRecordGetBytesPtr( TXTRecordRef inRef );

#if 0
#pragma mark == TXT Record Parsing ==
#endif

/*---------------------------------------------------------------------------------------------------------------------------
	A typical calling sequence for TXT record parsing is something like:
	Receive TXT record data in the DNSServiceResolve() callback then:
	
	cosnt void *	value1Ptr;
	uint8_t			value1Size;
	
	err = TXTRecordGetValuePtr( txtSize, txtRecord, "key1", &value1Ptr, &value1Size );
	if( err == kDNSServiceErr_NoError )
	{
		// "key1" found. Do work with "value1Ptr" data if needed.
	}
	...
	return;
	
	If you wish to retain the values after returning from the DNSServiceResolve() callback, then you need to copy the data 
	to your own storage using memcpy() or something similar so it does not go out of scope until you're done with it.

	If for some reason you need to parse a TXT record you built yourself using the TXT record construction functions above, 
	then you can do that using TXTRecordGetLength and TXTRecordGetBytesPtr functions:
	
	TXTRecordGetValue( TXTRecordGetLength( x ), TXTRecordGetBytesPtr( x ), "key1", &value1Ptr, &value1Size );

	Most applications only fetch keys they know about from a TXT record and ignore the rest.
	However, some debugging tools wish to fetch and display all keys. To do that, use the TXTRecordGetCount() and 
	TXTRecordGetItemAtIndex() functions.
*/

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordGetValuePtr
	
	@abstract	Allows you to retrieve the value for a given key from a TXT Record.
	
	@param		inTXTSize
					Number of bytes in the TXT record.
	
	@param		inTXTBytes
					Pointer to the raw TXT record bytes.
	
	@param		inKey
					A null-terminated key to search for. Note: keys are case insensitive.

	@param		outValue
					Pointer to be filled in with a pointer to the value within the TXT record bytes.
					Resulting pointer will be NULL if the key is present, but has no value.
					Resulting pointer will be non-NULL and size zero if the key is present, but has an empty value.
					Resulting pointer will be non-NULL and size non-zero if key is present and has a non-empty value.
					May be NULL if only interested in the value size or if the key is present.
	
	@param		outValueSize
					Pointer to receive the size of the value.
					Size will be 0 if there is no value or the value is empty.
					May be NULL if not interested in getting the size.

	@result		Returns kDNSServiceErr_NoError if a key with the specified name is found.
				Returns kDNSServiceErr_NoSuchKey if the key does not exist in the TXTRecordRef.
				Returns kDNSServiceErr_BadParam if the parameters are illegal or not supported.
				Returns kDNSServiceErr_Invalid if the TXT record is malformed.
	
	@discussion
	
	The pointer may become invalid if you subsequently make changes to the TXT record by calling TXTRecordSetValue() 
	or TXTRecordRemoveValue().
*/

DNSServiceErrorType
	TXTRecordGetValuePtr( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		const char *	inKey, 
		const void **	outValue, 			// may be NULL
		uint8_t *		outValueSize );		// may be NULL

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordGetCount
	
	@abstract	Returns the total number of keys in the TXT Record.
	
	@param		inTXTSize
					Number of bytes in the TXT record.
	
	@param		inTXTBytes
					Pointer to the raw TXT record bytes.

	@result		Returns the total number of keys in the TXT Record.
	
	@discussion
	
	The count can be used with TXTRecordGetItemAtIndex() to iterate through the keys.
	The count may become invalid if you subsequently make changes to the TXT record by calling TXTRecordSetValue() 
	or TXTRecordRemoveValue().	
*/

uint16_t	TXTRecordGetCount( uint16_t inTXTSize, const void *inTXTBytes );

//---------------------------------------------------------------------------------------------------------------------------
/*! @function	TXTRecordGetItemAtIndex
	
	@abstract	Allows you to retrieve a key name, given an index into a TXT Record.
	
	@param		inTXTSize
					Number of bytes in the TXT record.
	
	@param		inTXTBytes
					Pointer to the raw TXT record bytes.
					
	@param		inIndex
					Index of item to get. The index is 0-based (0 is the first item).
					Legal index values range from 0 to TXTRecordGetCount() - 1.
					
	@param		inKeyBuffer
					A string buffer used to store the null-terminated key name. The buffer must be at least 256 bytes
					in order to hold the maximum possible key name.
					May be NULL if not interested in the key name.

	@param		outValue
					Pointer to be filled in with a pointer to the value within the TXT record bytes.
					Resulting pointer will be NULL if the key is present, but has no value.
					Resulting pointer will be non-NULL and size zero if the key is present, but has an empty value.
					Resulting pointer will be non-NULL and size non-zero if key is present and has a non-empty value.
					May be NULL if only interested in the value size or if the key is present.
	
	@param		outValueSize
					Pointer to receive the size of the value.
					Size will be 0 if there is no value or the value is empty.
					May be NULL if not interested in getting the size.

	@result		Returns kDNSServiceErr_NoError if a key with the specified name is found.
				Returns kDNSServiceErr_Invalid if the index is greater than TXTRecordGetCount() - 1 or the TXT record is malformed.
				Returns kDNSServiceErr_BadParam if the parameters are illegal or not supported.
	
	@discussion
	
	It also possible to iterate through keys in a TXT record by simply calling TXTRecordGetItemAtIndex() repeatedly, 
	beginning with index zero and increasing until TXTRecordGetItemAtIndex() returns an non-zero error code.
 
	The pointer may become invalid if you subsequently make changes to the TXTRecordRef by calling TXTRecordSetValue() 
	or TXTRecordRemoveValue().
*/

DNSServiceErrorType
	TXTRecordGetItemAtIndex( 
		uint16_t 		inTXTSize, 
		const void *	inTXTBytes, 
		uint16_t 		inIndex, 
		char *			inKeyBuffer, 		// may be NULL
		const void **	outValue, 			// may be NULL
		uint8_t *		outValueSize );		// may be NULL

#ifdef	__cplusplus
	}
#endif

#endif	// _DNS_SD_H
