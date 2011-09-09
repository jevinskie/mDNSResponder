/*
 * Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

$Log: DNSCommon.h,v $
Revision 1.14  2004/05/28 23:42:36  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.13  2004/05/18 23:51:25  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.12  2004/04/22 04:03:59  cheshire
Headers should use "extern" declarations, not "mDNSexport"

Revision 1.11  2004/04/14 23:09:28  ksekar
Support for TSIG signed dynamic updates.

Revision 1.10  2004/03/13 01:57:33  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.9  2004/02/21 08:56:58  bradley
Wrap prototypes with extern "C" for C++ builds.

Revision 1.8  2004/02/06 23:04:18  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.7  2004/02/03 19:47:36  ksekar
Added an asyncronous state machine mechanism to uDNS.c, including
calls to find the parent zone for a domain name.  Changes include code
in repository previously dissabled via "#if 0 //incomplete".  Codepath
is currently unused, and will be called to create update records, etc.

Revision 1.6  2004/01/27 20:15:22  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.5  2004/01/24 03:40:56  cheshire
Move mDNSAddrIsDNSMulticast() from DNSCommon.h to mDNSClientAPI.h so clients can use it

Revision 1.4  2004/01/24 03:38:27  cheshire
Fix minor syntactic error: Headers should use "extern" declarations, not "mDNSexport"

Revision 1.3  2004/01/23 23:23:14  ksekar
Added TCP support for truncated unicast messages.

Revision 1.2  2004/01/21 21:12:23  cheshire
Add missing newline at end of file to make Unix tools happier

Revision 1.1  2003/12/13 03:05:27  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

 
 */

#ifndef __DNSCOMMON_H_
#define __DNSCOMMON_H_

#include "mDNSClientAPI.h"

#ifdef	__cplusplus
	extern "C" {
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - DNS Protocol Constants
#endif

typedef enum
	{
	kDNSFlag0_QR_Mask     = 0x80,		// Query or response?
	kDNSFlag0_QR_Query    = 0x00,
	kDNSFlag0_QR_Response = 0x80,
	
	kDNSFlag0_OP_Mask     = 0x78,		// Operation type
	kDNSFlag0_OP_StdQuery = 0x00,
	kDNSFlag0_OP_Iquery   = 0x08,
	kDNSFlag0_OP_Status   = 0x10,
	kDNSFlag0_OP_Unused3  = 0x18,
	kDNSFlag0_OP_Notify   = 0x20,
	kDNSFlag0_OP_Update   = 0x28,
	
	kDNSFlag0_QROP_Mask   = kDNSFlag0_QR_Mask | kDNSFlag0_OP_Mask,
	
	kDNSFlag0_AA          = 0x04,		// Authoritative Answer?
	kDNSFlag0_TC          = 0x02,		// Truncated?
	kDNSFlag0_RD          = 0x01,		// Recursion Desired?
	kDNSFlag1_RA          = 0x80,		// Recursion Available?
	
	kDNSFlag1_Zero        = 0x40,		// Reserved; must be zero
	kDNSFlag1_AD          = 0x20,		// Authentic Data [RFC 2535]
	kDNSFlag1_CD          = 0x10,		// Checking Disabled [RFC 2535]

	kDNSFlag1_RC          = 0x0F,		// Response code
	kDNSFlag1_RC_NoErr    = 0x00,
	kDNSFlag1_RC_FmtErr   = 0x01,
	kDNSFlag1_RC_SrvErr   = 0x02,
	kDNSFlag1_RC_NXDomain = 0x03,
	kDNSFlag1_RC_NotImpl  = 0x04,
	kDNSFlag1_RC_Refused  = 0x05,
	kDNSFlag1_RC_YXDomain = 0x06,
	kDNSFlag1_RC_YXRRSet  = 0x07,
	kDNSFlag1_RC_NXRRSet  = 0x08,
	kDNSFlag1_RC_NotAuth  = 0x09,
	kDNSFlag1_RC_NotZone  = 0x0A
	} DNS_Flags;

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

extern const NetworkInterfaceInfo *GetFirstActiveInterface(const NetworkInterfaceInfo *intf);
extern mDNSInterfaceID GetNextActiveInterfaceID(const NetworkInterfaceInfo *intf);

extern mDNSu32 mDNSRandom(mDNSu32 max);

#define mDNSSameIPv4Address(A,B) ((A).NotAnInteger == (B).NotAnInteger)
#define mDNSSameIPv6Address(A,B) ((A).l[0] == (B).l[0] && (A).l[1] == (B).l[1] && (A).l[2] == (B).l[2] && (A).l[3] == (B).l[3])

#define mDNSIPv4AddressIsZero(A) mDNSSameIPv4Address((A), zeroIPAddr)
#define mDNSIPv6AddressIsZero(A) mDNSSameIPv6Address((A), zerov6Addr)

#define mDNSIPv4AddressIsOnes(A) mDNSSameIPv4Address((A), onesIPv4Addr)
#define mDNSIPv6AddressIsOnes(A) mDNSSameIPv6Address((A), onesIPv6Addr)

#define mDNSAddressIsZero(X) (                                              \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsZero((X)->ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsZero((X)->ip.v6))    )

#define mDNSAddressIsOnes(X) (                                              \
	((X)->type == mDNSAddrType_IPv4 && mDNSIPv4AddressIsOnes((X)->ip.v4)) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSIPv6AddressIsOnes((X)->ip.v6))    )

#define mDNSAddressIsValid(X) (                                                                                             \
	((X)->type == mDNSAddrType_IPv4) ? !(mDNSIPv4AddressIsZero((X)->ip.v4) || mDNSIPv4AddressIsOnes((X)->ip.v4)) :          \
	((X)->type == mDNSAddrType_IPv6) ? !(mDNSIPv6AddressIsZero((X)->ip.v6) || mDNSIPv6AddressIsOnes((X)->ip.v6)) : mDNSfalse)



// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Domain Name Utility Functions
#endif

#define mdnsIsDigit(X)     ((X) >= '0' && (X) <= '9')
#define mDNSIsUpperCase(X) ((X) >= 'A' && (X) <= 'Z')
#define mDNSIsLowerCase(X) ((X) >= 'a' && (X) <= 'z')
#define mdnsIsLetter(X)    (mDNSIsUpperCase(X) || mDNSIsLowerCase(X))


#define mdnsValidHostChar(X, notfirst, notlast) (mdnsIsLetter(X) || mdnsIsDigit(X) || ((notfirst) && (notlast) && (X) == '-') )

extern mDNSu16 CompressedDomainNameLength(const domainname *const name, const domainname *parent);

extern mDNSBool LabelContainsSuffix(const domainlabel *const name, const mDNSBool RichText);
extern mDNSu32 RemoveLabelSuffix(domainlabel *name, mDNSBool RichText);
extern void AppendLabelSuffix(domainlabel *name, mDNSu32 val, mDNSBool RichText);
extern void mDNS_HostNameCallback(mDNS *const m, AuthRecord *const rr, mStatus result);
#define ValidateDomainName(N) (DomainNameLength(N) <= MAX_DOMAIN_NAME)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

extern mDNSu32 RDataHashValue(mDNSu16 const rdlength, const RDataBody *const rdb);

extern mDNSBool SameRData(const ResourceRecord *const r1, const ResourceRecord *const r2);

extern mDNSBool ResourceRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q);


extern mDNSu16 GetRDLength(const ResourceRecord *const rr, mDNSBool estimate);

#define GetRRDomainNameTarget(RR) (                                                                          \
	((RR)->rrtype == kDNSType_CNAME || (RR)->rrtype == kDNSType_PTR || (RR)->rrtype == kDNSType_NS)          \
	                                                                 ? &(RR)->rdata->u.name       :          \
	((RR)->rrtype == kDNSType_SRV                                  ) ? &(RR)->rdata->u.srv.target : mDNSNULL )

extern mDNSBool ValidateRData(const mDNSu16 rrtype, const mDNSu16 rdlength, const RData *const rd);


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - DNS Message Creation Functions
#endif

extern void InitializeDNSMessage(DNSMessageHeader *h, mDNSOpaque16 id, mDNSOpaque16 flags);
extern const mDNSu8 *FindCompressionPointer(const mDNSu8 *const base, const mDNSu8 *const end, const mDNSu8 *const domname);

extern mDNSu8 *putDomainNameAsLabels(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name);
	
extern mDNSu8 *putRData(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, ResourceRecord *rr);

extern mDNSu8 *PutResourceRecordTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 ttl);

extern mDNSu8 *PutResourceRecordCappedTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 maxttl);

extern mDNSu8 *putEmptyResourceRecord(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, mDNSu16 *count, const AuthRecord *rr);

extern mDNSu8 *putQuestion(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name, mDNSu16 rrtype, mDNSu16 rrclass);
	
#define PutResourceRecord(MSG, P, C, RR) PutResourceRecordTTL((MSG), (P), (C), (RR), (RR)->rroriginalttl)


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNS Message Parsing Functions
#endif

extern mDNSu32 DomainNameHashValue(const domainname *const name);

extern void SetNewRData(ResourceRecord *const rr, RData *NewRData, mDNSu16 rdlength);


extern const mDNSu8 *skipDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end);

extern const mDNSu8 *getDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end,
	domainname *const name);
	
extern const mDNSu8 *skipResourceRecord(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

extern const mDNSu8 *GetResourceRecord(mDNS *const m, const DNSMessage * const msg, const mDNSu8 *ptr, 
    const mDNSu8 * const end, const mDNSInterfaceID InterfaceID, mDNSu8 RecordType, CacheRecord *rr, RData *RDataStorage);

extern const mDNSu8 *skipQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end);

extern const mDNSu8 *getQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end, const mDNSInterfaceID InterfaceID,
	DNSQuestion *question);
	
extern const mDNSu8 *LocateAnswers(const DNSMessage *const msg, const mDNSu8 *const end);

extern const mDNSu8 *LocateAuthorities(const DNSMessage *const msg, const mDNSu8 *const end);

extern const mDNSu8 *LocateAdditionals(const DNSMessage *const msg, const mDNSu8 *const end);

#define GetLargeResourceRecord(m, msg, p, e, i, t, L) \
	(((L)->r.rdatastorage.MaxRDLength = MaximumRDSize), GetResourceRecord((m), (msg), (p), (e), (i), (t), &(L)->r, (RData*)&(L)->r.rdatastorage))

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

extern mStatus mDNSSendDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end, mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport);
extern mStatus mDNSSendDNSMessage_tcp(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end, int sd);
extern mStatus mDNSSendSignedDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 *end,
    mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport, uDNS_AuthInfo *authInfo);
extern mStatus mDNSSendSignedDNSMessage_tcp(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end, int sd, uDNS_AuthInfo *authInfo);
	
#ifdef	__cplusplus
	}
#endif

#endif // __DNSCOMMON_H_
