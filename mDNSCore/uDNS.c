/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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

$Log: uDNS.c,v $
Revision 1.54  2004/06/22 02:10:53  ksekar
<rdar://problem/3705433>: Lighthouse failure causes packet flood to DNS

Revision 1.53  2004/06/17 20:49:09  ksekar
<rdar://problem/3690436>: Tiger8A148: repeated crash of mDNSResponder while location cycling

Revision 1.52  2004/06/17 01:13:11  ksekar
<rdar://problem/3696616>: polling interval too short

Revision 1.51  2004/06/10 04:36:44  cheshire
Fix compiler warning

Revision 1.50  2004/06/10 00:55:13  ksekar
<rdar://problem/3686213>: crash on network reconnect

Revision 1.49  2004/06/10 00:10:50  ksekar
<rdar://problem/3686174>: Infinite Loop in uDNS_Execute()

Revision 1.48  2004/06/09 20:03:37  ksekar
<rdar://problem/3686163>: Incorrect copying of resource record in deregistration

Revision 1.47  2004/06/09 03:48:28  ksekar
<rdar://problem/3685226>: nameserver address fails with prod. Lighthouse server

Revision 1.46  2004/06/09 01:44:30  ksekar
<rdar://problem/3681378> reworked Cache Record copy code

Revision 1.45  2004/06/08 18:54:47  ksekar
<rdar://problem/3681378>: mDNSResponder leaks after exploring in Printer Setup Utility

Revision 1.44  2004/06/05 00:33:51  cheshire
<rdar://problem/3681029>: Check for incorrect time comparisons

Revision 1.43  2004/06/05 00:14:44  cheshire
Fix signed/unsigned and other compiler warnings

Revision 1.42  2004/06/04 22:36:16  ksekar
Properly set u->nextevent in uDNS_Execute

Revision 1.41  2004/06/04 08:58:29  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.40  2004/06/03 03:09:58  ksekar
<rdar://problem/3668626>: Garbage Collection for Dynamic Updates

Revision 1.39  2004/06/01 23:46:50  ksekar
<rdar://problem/3675149>: DynDNS: dynamically look up LLQ/Update ports

Revision 1.38  2004/05/31 22:19:44  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on
record changes (#7805) - revert to polling mode on setup errors

Revision 1.37  2004/05/28 23:42:37  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.36  2004/05/18 23:51:25  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.35  2004/05/07 23:01:04  ksekar
Cleaned up list traversal in deriveGoodbyes - removed unnecessary
conditional assignment.

Revision 1.34  2004/05/05 18:26:12  ksekar
Periodically re-transmit questions if the send() fails.  Include
internal questions in retransmission.

Revision 1.33  2004/05/05 17:40:06  ksekar
Removed prerequisite from deregistration update - it does not work for
shared records, and is unnecessary until we have more sophisticated
name conflict management.

Revision 1.32  2004/05/05 17:32:18  ksekar
Prevent registration of loopback interface caused by removal of
Multicast flag in interface structure.

Revision 1.31  2004/05/05 17:05:02  ksekar
Use LargeCacheRecord structs when pulling records off packets

Revision 1.30  2004/04/16 21:33:27  ksekar
Fixed bug in processing GetZoneData responses that do not use BIND formatting.

Revision 1.29  2004/04/15 20:03:13  ksekar
Clarified log message when pulling bad resource records off packet.

Revision 1.28  2004/04/15 00:51:28  bradley
Minor tweaks for Windows and C++ builds. Added casts for signed/unsigned integers and 64-bit pointers.
Prefix some functions with mDNS to avoid conflicts. Disable benign warnings on Microsoft compilers.

Revision 1.27  2004/04/14 23:09:28  ksekar
Support for TSIG signed dynamic updates.

Revision 1.26  2004/04/14 19:36:05  ksekar
Fixed memory corruption error in deriveGoodbyes.

Revision 1.25  2004/04/14 04:07:11  ksekar
Fixed crash in IsActiveUnicastQuery().  Removed redundant checks in routine.

Revision 1.24  2004/04/08 09:41:40  bradley
Added const to AuthRecord in deadvertiseIfCallback to match callback typedef.

Revision 1.23  2004/03/24 00:29:45  ksekar
Make it safe to call StopQuery in a unicast question callback

Revision 1.22  2004/03/19 10:11:09  bradley
Added AuthRecord * cast from umalloc for C++ builds.

Revision 1.21  2004/03/15 02:03:45  bradley
Added const to params where needed to match prototypes. Changed SetNewRData calls to use 0 instead
of -1 for unused size to fix warning. Disable assignment within conditional warnings with Visual C++.

Revision 1.20  2004/03/13 02:07:26  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.19  2004/03/13 01:57:33  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.18  2004/02/21 08:34:15  bradley
Added casts from void * to specific type for C++ builds. Changed void * l-value cast
r-value cast to fix problems with VC++ builds. Removed empty switch to fix VC++ error.

Revision 1.17  2004/02/21 02:06:24  cheshire
Can't use anonymous unions -- they're non-standard and don't work on all compilers

Revision 1.16  2004/02/12 01:51:45  cheshire
Don't try to send uDNS queries unless we have at least one uDNS server available

Revision 1.15  2004/02/10 03:02:46  cheshire
Fix compiler warning

Revision 1.14  2004/02/06 23:04:19  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.13  2004/02/03 22:15:01  ksekar
Fixed nameToAddr error check: don't abort state machine on nxdomain error.

Revision 1.12  2004/02/03 19:47:36  ksekar
Added an asyncronous state machine mechanism to uDNS.c, including
calls to find the parent zone for a domain name.  Changes include code
in repository previously dissabled via "#if 0 //incomplete".  Codepath
is currently unused, and will be called to create update records, etc.

Revision 1.11  2004/01/30 02:12:30  ksekar
Changed uDNS_ReceiveMsg() to correctly return void.

Revision 1.10  2004/01/29 02:59:17  ksekar
Unicast DNS: Changed from a resource record oriented question/response
matching to packet based matching.  New callback architecture allows
collections of records in a response to be processed differently
depending on the nature of the request, and allows the same structure
to be used for internal and client-driven queries with different processing needs.

Revision 1.9  2004/01/28 20:20:45  ksekar
Unified ActiveQueries and ActiveInternalQueries lists, using a flag to
demux them.  Check-in includes work-in-progress code, #ifdef'd out.

Revision 1.8  2004/01/28 02:30:07  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.7  2004/01/27 20:15:22  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.6  2004/01/24 23:47:17  cheshire
Use mDNSOpaque16fromIntVal() instead of shifting and masking

Revision 1.5  2004/01/24 04:59:15  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.4  2004/01/24 04:19:26  cheshire
Restore overwritten checkin 1.2

Revision 1.3  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.2  2004/01/22 03:48:41  cheshire
Make sure uDNS client doesn't accidentally use query ID zero

Revision 1.1  2003/12/13 03:05:27  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

 */

#include "uDNS.h"

#if(defined(_MSC_VER))
	// Disable "assignment within conditional expression".
	// Other compilers understand the convention that if you place the assignment expression within an extra pair
	// of parentheses, this signals to the compiler that you really intended an assignment and no warning is necessary.
	// The Microsoft compiler doesn't understand this convention, so in the absense of any other way to signal
	// to the compiler that the assignment is intentional, we have to just turn this warning off completely.
	#pragma warning(disable:4706)
#endif

#ifndef NULL
#define NULL mDNSNULL
#endif // NULL


#define ustrcpy(d,s)       mDNSPlatformStrCopy(s,d)       // use strcpy(2) param ordering
#define ustrlen(s)         mDNSPlatformStrLen(s)
#define umalloc(x)         mDNSPlatformMemAllocate(x)       // short hands for common routines
#define ufree(x)           mDNSPlatformMemFree(x)
#define ubzero(x,y)        mDNSPlatformMemZero(x,y)
#define umemcpy(x, y, l)   mDNSPlatformMemCopy(y, x, l)  // uses memcpy(2) arg ordering


// Asyncronous operation types

typedef enum
	{
	zoneDataResult
	// other async. operation names go here
	} AsyncOpResultType;

typedef struct
	{
    domainname zoneName; 
    mDNSAddr primaryAddr;
    mDNSu16 zoneClass;
    mDNSIPPort llqPort;
    mDNSIPPort updatePort;
	} zoneData_t;

// other async. result struct defs go here

typedef struct
	{
    AsyncOpResultType type;
    zoneData_t zoneData;
    // other async result structs go here
	} AsyncOpResult;

typedef void AsyncOpCallback(mStatus err, mDNS *const m, void *info, const AsyncOpResult *result); 


// Private Function Prototypes
// Note:  In general, functions are ordered such that they do not require forward declarations.
// However, prototypes are used where cyclic call graphs exist (e.g. foo calls bar, and bar calls
// foo), or when they aid in the grouping or readability of code (e.g. state machine code that is easier
// read top-to-bottom.)

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question,  const mDNSAddr *src, mDNS *m);
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, mDNSBool findUpdatePort, mDNSBool findLLQPort,
								   AsyncOpCallback callback, void *callbackInfo);
mDNSlocal mDNSBool recvLLQResponse(mDNS *m, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, const mDNSInterfaceID InterfaceID);
mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr);
mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs);

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - General Utility Functions
#endif

mDNSlocal mDNSOpaque16 newMessageID(uDNS_GlobalInfo *u)
	{
	// if NextMessageID is 0 (ininitialized) or 0xffff (reserved for TCP packets) reset to 1
	if (!u->NextMessageID || u->NextMessageID == (mDNSu16)~0) u->NextMessageID = 1;
	return mDNSOpaque16fromIntVal(u->NextMessageID++);
	}

// unlink an AuthRecord from a linked list
mDNSlocal mStatus unlinkAR(AuthRecord **list, AuthRecord *const rr)
	{
	AuthRecord *rptr, *prev = NULL;
	
	for (rptr = *list; rptr; rptr = rptr->next)
		{
		if (rptr == rr)
			{
			if (prev) prev->next = rptr->next;
			else *list  = rptr->next;
			rptr->next = NULL;
			return mStatus_NoError;
			}
		prev = rptr;
		}
	LogMsg("ERROR: unlinkAR - no such active record");
	return mStatus_UnknownErr;
	}

mDNSlocal void LinkActiveQuestion(uDNS_GlobalInfo *u, DNSQuestion *q)
	{
	if (IsActiveUnicastQuery(q, u))
		{ LogMsg("LinkActiveQuestion - %s (%d) already in list!", q->qname.c, q->qtype); return; }
	
	q->next = u->ActiveQueries;
	u->ActiveQueries = q;
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Name Server List Management
#endif

mDNSexport void mDNS_RegisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    //!!!KRS do this dynamically!
    uDNS_GlobalInfo *u = &m->uDNS_info;
    int i;

	if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to register DNS with IP address 0");
        return;
        }

    for (i = 0; i < 32; i++)
        {
        if (!u->Servers[i].ip.v4.NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = dnsAddr->NotAnInteger;
			u->Servers[i].type = mDNSAddrType_IPv4;
            return;
            }
        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
            {
            LogMsg("ERROR: mDNS_RegisterDNS - DNS already registered");
            return;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_RegisterDNS - too many registered servers");  }

    }

mDNSexport void mDNS_DeregisterDNS(mDNS *const m, mDNSv4Addr *const dnsAddr)
    {
    uDNS_GlobalInfo *u = &m->uDNS_info;
    int i;

    if (!dnsAddr->NotAnInteger)
        {
        LogMsg("ERROR: attempt to deregister DNS with IP address 0");
        return;
        }

    for (i = 0; i < 32; i++)
        {

        if (u->Servers[i].ip.v4.NotAnInteger == dnsAddr->NotAnInteger)
            {
            u->Servers[i].ip.v4.NotAnInteger = 0;
            return;
            }
        }
    if (i == 32) {  LogMsg("ERROR: mDNS_DeregisterDNS - no such DNS registered");  }
    }

mDNSexport void mDNS_DeregisterDNSList(mDNS *const m)
    {
    ubzero(m->uDNS_info.Servers, 32 * sizeof(mDNSAddr));
    }

mDNSexport mDNSBool mDNS_DNSRegistered(mDNS *const m)
	{
	int i;

	for (i = 0; i < 32; i++) if (m->uDNS_info.Servers[i].ip.v4.NotAnInteger) return mDNStrue;
	return mDNSfalse;
	}
	   

 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - authorization management
#endif


mDNSexport mStatus mDNS_UpdateDomainRequiresAuthentication(mDNS *m, domainname *zone, domainname *key,
    mDNSu8 *sharedSecret, mDNSu32 ssLen, mDNSBool base64)
	{
	uDNS_AuthInfo *info;
	mDNSu8 keybuf[1024];
	mDNSs32 keylen;
	
	info = (uDNS_AuthInfo*)umalloc(sizeof(uDNS_AuthInfo) + ssLen);
	if (!info) { LogMsg("ERROR: umalloc"); return mStatus_NoMemoryErr; }
   	ubzero(info, sizeof(uDNS_AuthInfo));
	ustrcpy(info->zone.c, zone->c);
	ustrcpy(info->keyname.c, key->c);

	if (base64)
		{
		keylen = DNSDigest_Base64ToBin((const char*)sharedSecret, keybuf, 1024);
		if (keylen < 0)
			{
			LogMsg("ERROR: mDNS_UpdateDomainRequiresAuthentication - could not convert shared secret from base64");
			ufree(info);
			return mStatus_UnknownErr;
			}		
		DNSDigest_ConstructHMACKey(info, keybuf, (mDNSu32)keylen);		
		}
	else DNSDigest_ConstructHMACKey(info, sharedSecret, ssLen);

    // link into list
	// !!!KRS this should be a hashtable since we must check if updates are required on each registration
	info->next = m->uDNS_info.AuthInfoList;
	m->uDNS_info.AuthInfoList = info;
	return mStatus_NoError;
	}

mDNSexport void mDNS_ClearAuthenticationList(mDNS *m)
	{
	uDNS_AuthInfo *fptr, *ptr = m->uDNS_info.AuthInfoList;
	
	while (ptr)
		{
		fptr = ptr;
		ptr = ptr->next;
		ufree(fptr);
		}
	m->uDNS_info.AuthInfoList = NULL;
	}

mDNSlocal uDNS_AuthInfo *GetAuthInfoForZone(const uDNS_GlobalInfo *u, const domainname *zone)
	{
	uDNS_AuthInfo *ptr;
	domainname *z;
	mDNSu32 zoneLen, ptrZoneLen;

	zoneLen = ustrlen(zone->c);
	for (ptr = u->AuthInfoList; ptr; ptr = ptr->next)
		{
		z = &ptr->zone;
		ptrZoneLen = ustrlen(z->c);
		if (zoneLen < ptrZoneLen) continue;
		// return info if zone ends in info->zone
		if (mDNSPlatformMemSame(z->c, zone->c + (zoneLen - ptrZoneLen), ptrZoneLen)) return ptr;
		}	   	  
	return NULL;
	}	
	



 // ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - host name and interface management
#endif


mDNSlocal void hostnameCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	// note that the rr is already unlinked if result is non-zero

	if (result == mStatus_MemFree) return;
	if (result == mStatus_NameConflict && rr->resrec.RecordType == kDNSRecordTypeUnique)
		{
		// if we get a name conflict, make sure our name/addr isn't already registered by re-registering
		rr->resrec.RecordType = kDNSRecordTypeKnownUnique;
		uDNS_RegisterRecord(m, rr);
		return;
		}

	if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique)
		// we've already tried to re-register.  reset RecordType before returning RR to client
		{
		if (result == mStatus_NoSuchRecord)  // name is advertised for some other address
			result = mStatus_NameConflict;
		rr->resrec.RecordType = kDNSRecordTypeUnique;
		}
		
	if (!result) rr->resrec.RecordType = kDNSRecordTypeVerified;
	if (result)
		((NetworkInterfaceInfo *)(rr->RecordContext))->uDNS_info.registered = mDNSfalse;
	mDNS_HostNameCallback(m, rr, result);	
	}


mDNSlocal void deadvertiseIfCallback(mDNS *const m, AuthRecord *const rr, mStatus err)
	{
	(void)m; // unused
	
	if (err == mStatus_MemFree) ufree(rr);
	else LogMsg("deadvertiseIfCallback - error %s for record %s", err, rr->resrec.name.c);
	}
 
 mDNSexport void uDNS_DeadvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set)
	{
	AuthRecord *copy;
	AuthRecord *rr = &set->uDNS_info.RR_A;
	
	// NOTE: for compatibility w/ mDNS architecture, we make a copy of the address record before sending a
	// goodbye, since mDNS does not send goodbyes for address records and expects the memory to be immediately
	// freed
	
   	if (set->uDNS_info.registered)
		{
		// copy resource record
		copy = (AuthRecord*)umalloc(sizeof(AuthRecord));  // allocate storage
		if (!copy) { LogMsg("ERROR: Malloc"); return; }        
		umemcpy(copy, rr, sizeof(AuthRecord));            // copy all fields
        copy->resrec.rdata = &copy->rdatastorage;         // set rdata pointer         
        if (rr->resrec.rdata != &rr->rdatastorage) 
            { LogMsg("ERROR: uDNS_DeadvertiseInterface - expected local rdata storage.  Aborting deregistration"); return; }
        
		// link copy into list
		copy->next = m->uDNS_info.RecordRegistrations;
		m->uDNS_info.RecordRegistrations = copy;
		copy->RecordCallback = deadvertiseIfCallback;
		
		// unlink the original
		unlinkAR(&m->uDNS_info.RecordRegistrations, rr);
		rr->uDNS_info.state = regState_Unregistered;
		set->uDNS_info.registered = mDNSfalse;
		uDNS_DeregisterRecord(m, copy);
		}
	else debugf("uDNS_DeadvertiseInterface - interface not registered");
	return;	
	}

mDNSexport void uDNS_AdvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set) 
	{
	mDNSu8 *ip = set->ip.ip.v4.b;	
	AuthRecord *a   = &set->uDNS_info.RR_A;
	a->RecordContext = set;
	if (set->ip.type != mDNSAddrType_IPv4                                 // non-v4
		|| (ip[0] == 169 && ip[1] == 254)                             // link-local
		|| (ip[0] == 127 && ip[1] == 0 && ip[2] == 0 && ip[3] == 1))  // loopback
		return;		
	
	if (set->uDNS_info.registered && SameDomainName(&m->uDNS_info.hostname, &set->uDNS_info.regname))		
	  return; // already registered

	if (!m->uDNS_info.hostname.c[0])
		{
		// no hostname available
		set->uDNS_info.registered = mDNSfalse;
		return;
		}
		
	set->uDNS_info.registered = mDNStrue;
	ustrcpy(set->uDNS_info.regname.c, m->uDNS_info.hostname.c);
        //!!!KRS temp ttl 1
	mDNS_SetupResourceRecord(a, mDNSNULL, 0, kDNSType_A,  1, kDNSRecordTypeShared /*Unique*/, hostnameCallback, set); //!!!KRS

	ustrcpy(a->resrec.name.c, m->uDNS_info.hostname.c);
	a->resrec.rdata->u.ip = set->ip.ip.v4;
	LogMsg("uDNS_AdvertiseInterface: advertising %s", m->uDNS_info.hostname.c);
		
	uDNS_RegisterRecord(m, a); 
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Incoming Message Processing
#endif

mDNSlocal mDNSBool sameResourceRecord(ResourceRecord *r1, ResourceRecord *r2)
	{
	return (r1->namehash == r2->namehash &&
			r1->rrtype == r2->rrtype && 
			SameDomainName(&r1->name, &r2->name) &&
			SameRData(r1, r2));
	}

mDNSlocal mDNSBool kaListContainsAnswer(DNSQuestion *question, CacheRecord *rr)
	{
	CacheRecord *ptr;

	for (ptr = question->uDNS_info.knownAnswers; ptr; ptr = ptr->next)
		if (sameResourceRecord(&ptr->resrec, &rr->resrec)) return mDNStrue;

	return mDNSfalse;
	}


mDNSlocal void removeKnownAnswer(DNSQuestion *question, CacheRecord *rr)
	{
	CacheRecord *ptr, *prev = NULL;

	for (ptr = question->uDNS_info.knownAnswers; ptr; ptr = ptr->next)
		{
		if (sameResourceRecord(&ptr->resrec, &rr->resrec))
			{
			if (prev) prev->next = ptr->next;
			else question->uDNS_info.knownAnswers = ptr->next;
			ufree(ptr);
			return;
			}
		prev = ptr;
		}
	LogMsg("removeKnownAnswer() called for record not in KA list");
	}


mDNSlocal void addKnownAnswer(DNSQuestion *question, const CacheRecord *rr)
	{
	CacheRecord *newCR = NULL;
	mDNSu32 size;	

	size = sizeof(CacheRecord) + rr->resrec.rdlength - InlineCacheRDSize;
	newCR = (CacheRecord *)umalloc(size);
	if (!newCR) { LogMsg("ERROR: addKnownAnswer - malloc"); return; }
	umemcpy(newCR, rr, size);
	newCR->resrec.rdata = (RData*)&newCR->rdatastorage;
	newCR->resrec.rdata->MaxRDLength = rr->resrec.rdlength;
	newCR->next = question->uDNS_info.knownAnswers;
	question->uDNS_info.knownAnswers = newCR;
	}

mDNSlocal void deriveGoodbyes(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question)
	{
	const mDNSu8 *ptr;
	int i;
	CacheRecord *fptr, *ka, *cr, *answers = NULL, *prev = NULL;
	LargeCacheRecord *lcr;
	
	if (question != m->uDNS_info.CurrentQuery) { LogMsg("ERROR: deriveGoodbyes called without CurrentQuery set!"); return; }

	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;

	if (!msg->h.numAnswers)
		{
		// delete the whole KA list
		ka = question->uDNS_info.knownAnswers;
		while (ka)
			{
			debugf("deriving goodbye for %s", ka->resrec.name.c);
			question->QuestionCallback(m, question, &ka->resrec, mDNSfalse);
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("deriveGoodbyes - question removed via callback.  returning.");
				return;
				}
			fptr = ka;
			ka = ka->next;
			ufree(fptr);
			}
		question->uDNS_info.knownAnswers = NULL;
		return;
		}
	
	// make a list of all the new answers
	for (i = 0; i < msg->h.numAnswers; i++)
		{
		lcr = (LargeCacheRecord *)umalloc(sizeof(LargeCacheRecord));
		if (!lcr) goto malloc_error;
		ubzero(lcr, sizeof(LargeCacheRecord));
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, lcr);
		if (!ptr) goto pkt_error;
		cr = &lcr->r;
		if (ResourceRecordAnswersQuestion(&cr->resrec, question)) 
			{
			cr->next = answers;
			answers = cr;
			}
		else ufree(cr);
		}
	
	// make sure every known answer is in the answer list
	ka = question->uDNS_info.knownAnswers;
	while (ka)
		{
		for (cr = answers; cr; cr = cr->next)
			{ if (sameResourceRecord(&ka->resrec, &cr->resrec)) break; }
		if (!cr)
			{
			// record is in KA list but not answer list - remove from KA list
			if (prev) prev->next = ka->next;
			else question->uDNS_info.knownAnswers = ka->next;
			debugf("deriving goodbye for %s", ka->resrec.name.c);
			question->QuestionCallback(m, question, &ka->resrec, mDNSfalse);
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("deriveGoodbyes - question removed via callback.  returning.");
				return;
				}
			fptr = ka;
			ka = ka->next;
			ufree(fptr);
			}
		else
			{
			prev = ka;
			ka = ka->next;
			}
		}

	// free temp answers list
	cr = answers;
	while (cr) { fptr = cr; cr = cr->next; ufree(fptr); }

	return;
	
	pkt_error:
	LogMsg("ERROR: deriveGoodbyes - received malformed response to query for %s (%d)",
		   question->qname.c, question->qtype);
	return;

	malloc_error:
	LogMsg("ERROR: Malloc");	
	}

mDNSlocal void pktResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, mDNSBool llq)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	CacheRecord *cr = &lcr.r;
	mDNSBool goodbye, inKAList;
	LLQ_Info *llqInfo = question->uDNS_info.llq;
	
	if (question != m->uDNS_info.CurrentQuery)
		{ LogMsg("ERROR: pktResponseHdnlr called without CurrentQuery ptr set!");  return; }
		
	ptr = LocateAnswers(msg, end);
	if (!ptr) goto pkt_error;

	for (i = 0; i < msg->h.numAnswers; i++)
		{
		ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
		if (!ptr) goto pkt_error;
		if (ResourceRecordAnswersQuestion(&cr->resrec, question))
			{
			goodbye = llq ? ((mDNSs32)cr->resrec.rroriginalttl == -1) : mDNSfalse;
			inKAList = kaListContainsAnswer(question, cr);

			if ((goodbye && !inKAList) || (!goodbye && inKAList)) continue;  // list up to date
			if (!inKAList) addKnownAnswer(question, cr);
			if (goodbye) removeKnownAnswer(question, cr);
			question->QuestionCallback(m, question, &cr->resrec, !goodbye);
			if (question != m->uDNS_info.CurrentQuery)
				{
				debugf("pktResponseHndlr - CurrentQuery changed by QuestionCallback - returning");
				return;
				}
			}			
		else
			{
			LogMsg("unexpected answer: %s", cr->resrec.name.c);
			}
		}
	if (llq && (llqInfo->state == LLQ_Poll || llqInfo->deriveRemovesOnResume))	
		{ deriveGoodbyes(m, msg, end,question);  llqInfo->deriveRemovesOnResume = mDNSfalse; }
	//!!!KRS should we derive goodbyes for non-LLQs?

	return;

	pkt_error:
	LogMsg("ERROR: pktResponseHndlr - received malformed response to query for %s (%d)",
		   question->qname.c, question->qtype);
	return;
	}

mDNSlocal void simpleResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, void *context)
	{
	(void)context; // unused
	pktResponseHndlr(m, msg, end, question, mDNSfalse);
	}

mDNSlocal void llqResponseHndlr(mDNS * const m, DNSMessage *msg, const  mDNSu8 *end, DNSQuestion *question, void *context)
	{
	(void)context; // unused
	pktResponseHndlr(m, msg, end, question, mDNStrue);
	}



mDNSlocal void unlinkSRS(uDNS_GlobalInfo *u, ServiceRecordSet *srs)
	{
	ServiceRecordSet *ptr, *prev = NULL;
	
	for (ptr = u->ServiceRegistrations; ptr; ptr = ptr->next)
		{
		if (ptr == srs)
			{
			if (prev) prev->next = ptr->next;
			else u->ServiceRegistrations = ptr->next;
			ptr->next = NULL;
			return;
			}
		prev = ptr;
		}
	LogMsg("ERROR: unlinkSRS - SRS not found in ServiceRegistrations list");
	}


mDNSlocal mStatus checkUpdateResult(domainname *name, mDNSu8 rcode, const DNSMessage *msg)
	{
	(void)msg;  // currently unused, needed for TSIG errors
	if (!rcode) return mStatus_NoError;
	else if (rcode == kDNSFlag1_RC_YXDomain)
		{
		LogMsg("Name in use: %s", name->c);
		return mStatus_NameConflict;
		}
	else if (rcode == kDNSFlag1_RC_Refused)
		{
		LogMsg("Update %s refused", name->c);
		return mStatus_Refused;
		}
	else if (rcode == kDNSFlag1_RC_NXRRSet)
		{
		LogMsg("Reregister refusted (NXRRSET): %s", name->c);
		return mStatus_NoSuchRecord;
		}
	else if (rcode == kDNSFlag1_RC_NotAuth)
		{
		LogMsg("Permission denied (NOAUTH): %s", name->c);
		return mStatus_NoAuth;
		}
	else if (rcode == kDNSFlag1_RC_FmtErr)
		{
		LogMsg("Format Error: %s", name->c);
		return mStatus_UnknownErr;
		//!!!KRS need to parse message for TSIG errors
		}
	else
		{
		LogMsg("Update %s failed with rcode %d", name->c, rcode);
		return mStatus_UnknownErr;
		}
	}

mDNSlocal void hndlServiceUpdateReply(mDNS * const m, ServiceRecordSet *srs,  mStatus err)
	{	
    //!!!KRS make sure we're doing the right thing w/ MemFree
	
	switch (srs->uDNS_info.state)
		{
		case regState_Pending:
		case regState_Refresh:
			if (err)
				{
				if (srs->uDNS_info.lease && err == mStatus_UnknownErr)
					{
					LogMsg("Re-trying update of service %s without lease option", srs->RR_SRV.resrec.name.c);
					srs->uDNS_info.lease = mDNSfalse;
					srs->uDNS_info.expire = -1;
					SendServiceRegistration(m, srs);
					return;
					}
				else
					{
					LogMsg("hndlServiceUpdateReply: Error %d returned for registration of %s",
						   err, srs->RR_SRV.resrec.name.c);
					srs->uDNS_info.state = regState_Unregistered;
					break;
					}
				}
			else
				{
				if (srs->uDNS_info.state == regState_Refresh)
					{
					srs->uDNS_info.state = regState_Registered;				
					return;
					}
				srs->uDNS_info.state = regState_Registered;
				break;
				}				
		case regState_DeregPending:
			if (err) LogMsg("hndlServiceUpdateReply: Error %d returned for dereg of %s",
							err, srs->RR_SRV.resrec.name.c);
			else err = mStatus_MemFree;
			break;
		case regState_DeregDeferred:
			if (err) LogMsg("hndlServiceUpdateReply: Error %d received prior to deferred derigstration of %s",
							err, srs->RR_SRV.resrec.name.c);
			LogMsg("Performing deferred deregistration of %s", srs->RR_SRV.resrec.name.c);
			uDNS_DeregisterService(m, srs);
			return;			
		case regState_TargetChange:
			if (err)
				{
				LogMsg("hdnlServiceUpdateReply: Error %d returned for host target update of %s",
					   err, srs->RR_SRV.resrec.name.c);
				srs->uDNS_info.state = regState_Unregistered;
				// !!!KRS we are leaving the ptr/txt records registered
				}
			else srs->uDNS_info.state = regState_Registered;
			break;			
		default:
			LogMsg("hndlServiceUpdateReply called for service %s in unexpected state %d with error %d.  Unlinking.",
				   srs->RR_SRV.resrec.name.c, srs->uDNS_info.state, err);
			err = mStatus_UnknownErr;
		}

	if (err)
		{
		unlinkSRS(&m->uDNS_info, srs);		// name conflicts, force dereg, and errors
		srs->uDNS_info.state = regState_Unregistered;
		}
	
	srs->ServiceCallback(m, srs, err);
	// NOTE: do not touch structures after calling ServiceCallback
	}

mDNSlocal void hndlRecordUpdateReply(mDNS *m, AuthRecord *rr, mStatus err)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	if (rr->uDNS_info.state == regState_DeregPending)
		{					
		debugf("Received reply for deregister record %s type %d", rr->resrec.name.c, rr->resrec.rrtype);
		if (err) LogMsg("ERROR: Deregistration of record %s type %s failed with error %d",
						rr->resrec.name.c, rr->resrec.rrtype, err);
		else err = mStatus_MemFree;
		if (unlinkAR(&m->uDNS_info.RecordRegistrations, rr))
			LogMsg("ERROR: Could not unlink resource record following deregistration");
		rr->uDNS_info.state = regState_Unregistered;
		rr->RecordCallback(m, rr, err);
		return;
		}

	if (rr->uDNS_info.state == regState_DeregDeferred)
		{
		if (err)
			{
			LogMsg("Cancelling deferred deregistration record %s type %d due to registration error %d",
				   rr->resrec.name.c, rr->resrec.rrtype, err);
			unlinkAR(&m->uDNS_info.RecordRegistrations, rr);
			rr->uDNS_info.state = regState_Unregistered;
			return;
			}
		LogMsg("Calling deferred deregistration of record %s type %d",
			   rr->resrec.name.c, rr->resrec.rrtype);
		rr->uDNS_info.state = regState_Registered;
		uDNS_DeregisterRecord(m, rr);					
		return;
		}

	if (rr->uDNS_info.state == regState_Pending || rr->uDNS_info.state == regState_Refresh)
		{
		if (err)
			{
			if (rr->uDNS_info.lease && err == mStatus_UnknownErr)
				{
				LogMsg("Re-trying update of record %s without lease option", rr->resrec.name.c);
				rr->uDNS_info.lease = mDNSfalse;
				rr->uDNS_info.expire = -1;
				sendRecordRegistration(m, rr);
				return;
				}
			
			LogMsg("Registration of record %s type %d failed with error %d",
				   rr->resrec.name.c, rr->resrec.rrtype, err);
			unlinkAR(&u->RecordRegistrations, rr); 
			rr->uDNS_info.state = regState_Unregistered;
			}
		else
			{
			if (rr->uDNS_info.state == regState_Refresh)
				rr->uDNS_info.state = regState_Registered;			
			else
				{
				rr->uDNS_info.state = regState_Registered;
				rr->RecordCallback(m, rr, err);
				}
			return;
			}
		}
	
	LogMsg("Received unexpected response for record %s type %d, in state %d, with response error %d",
		   rr->resrec.name.c, rr->resrec.rrtype, rr->uDNS_info.state, err);
	}


mDNSlocal void SetUpdateExpiration(mDNS *m, DNSMessage *msg, const mDNSu8 *end, uDNS_RegInfo *info)
	{
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	int i;
	mDNSu32 lease = 0;

	ptr = LocateAdditionals(msg, end);

	if (info->lease && (ptr = LocateAdditionals(msg, end)))
		{
		for (i = 0; i < msg->h.numAdditionals; i++)
			{
			ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr);
			if (!ptr) break;
			if (lcr.r.resrec.rrtype == kDNSType_OPT)
				{
				if (lcr.r.resrec.rdlength < LEASE_OPT_SIZE) continue;
				if (lcr.r.resrec.rdata->u.opt.opt != kDNSOpt_Lease) continue;
				lease = lcr.r.resrec.rdata->u.opt.OptData.lease;
				break;
				}
			}
		}
	
	if (lease > 0)
		info->expire = (mDNSPlatformTimeNow() + (((mDNSs32)lease * mDNSPlatformOneSecond)) * 3/4);
	else info->expire = -1;
	}

mDNSexport void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr,
	const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl)
	{
	DNSQuestion *qptr;
	AuthRecord *rptr;
	ServiceRecordSet *sptr;
	mStatus err = mStatus_NoError;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	mDNSu8 StdR    = kDNSFlag0_QR_Response | kDNSFlag0_OP_StdQuery;
	mDNSu8 UpdateR = kDNSFlag0_OP_Update   | kDNSFlag0_QR_Response;
	mDNSu8 QR_OP   = (mDNSu8)(msg->h.flags.b[0] & kDNSFlag0_QROP_Mask);
	mDNSu8 rcode   = (mDNSu8)(msg->h.flags.b[1] & kDNSFlag1_RC);
   	
    // unused
	(void)srcaddr;
	(void)srcport;
	(void)dstaddr;
	(void)dstport;
	(void)ttl;
	(void)InterfaceID;
	
	if (QR_OP == StdR)
		{		
		// !!!KRS we should to a table lookup here to see if it answers an LLQ or a 1-shot
		if (recvLLQResponse(m, msg, end, srcaddr, srcport, InterfaceID)) return;
	
		for (qptr = u->ActiveQueries; qptr; qptr = qptr->next)
			{
			//!!!KRS we should have a hashtable, hashed on message id
			if (qptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				if (msg->h.flags.b[0] & kDNSFlag0_TC)
					{ hndlTruncatedAnswer(qptr, srcaddr, m); return; }
				else
					{
					u->CurrentQuery = qptr;
					qptr->uDNS_info.responseCallback(m, msg, end, qptr, qptr->uDNS_info.context);
					u->CurrentQuery = NULL;
					// Note: responseCallback can invalidate qptr
					return;
					}
				}
			}
		}
	if (QR_OP == UpdateR)
		{
		for (sptr = u->ServiceRegistrations; sptr; sptr = sptr->next)
			{
			if (sptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(&sptr->RR_SRV.resrec.name, rcode, msg);
				if (!err) SetUpdateExpiration(m, msg, end, &sptr->uDNS_info);
				hndlServiceUpdateReply(m, sptr, err);
				return;
				}
			}
		for (rptr = u->RecordRegistrations; rptr; rptr = rptr->next)
			{
			if (rptr->uDNS_info.id.NotAnInteger == msg->h.id.NotAnInteger)
				{
				err = checkUpdateResult(&rptr->resrec.name, rcode, msg);
				if (!err) SetUpdateExpiration(m, msg, end, &rptr->uDNS_info);				
				hndlRecordUpdateReply(m, rptr, err);
				return;
				}
			}
		}
	debugf("Received unexpected response: ID %d matches no active records", mDNSVal16(msg->h.id));
	}

		
mDNSlocal void receiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
	const mDNSInterfaceID InterfaceID)
	{
	mDNSAddr *sa = NULL, *da = NULL;
	mDNSIPPort sp, dp;
	mDNSu8 ttl = 0;

	sp.NotAnInteger = 0;
	dp.NotAnInteger = 0;
	uDNS_ReceiveMsg(m, msg, end, sa, sp, da, dp, InterfaceID, ttl);
	}

//!!!KRS this should go away (don't just pick one randomly!)
mDNSlocal const mDNSAddr *getInitializedDNS(uDNS_GlobalInfo *u)
    {
    int i;
    for (i = 0; i < 32; i++)
		if (u->Servers[i].ip.v4.NotAnInteger) return &u->Servers[i];

	return NULL;
    }

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Query Routines
#endif

#define sameID(x,y) mDNSPlatformMemSame(x,y,8)

mDNSlocal void initializeQuery(DNSMessage *msg, DNSQuestion *question)
	{
	mDNSOpaque16 flags = QueryFlags;
	
	ubzero(msg, sizeof(msg));
	flags.b[0] |= kDNSFlag0_RD;  // recursion desired
    InitializeDNSMessage(&msg->h, question->uDNS_info.id, flags);
	}

mDNSlocal mStatus constructQueryMsg(DNSMessage *msg, mDNSu8 **endPtr, DNSQuestion *const question)
	{
	initializeQuery(msg, question);

	*endPtr = putQuestion(msg, msg->data, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
    if (!*endPtr)
        {
        LogMsg("ERROR: Unicast query out of space in packet");
        return mStatus_UnknownErr;
        }
	return mStatus_NoError;
	}

mDNSlocal mDNSu8 *putLLQ(DNSMessage *const msg, mDNSu8 *ptr, DNSQuestion *question, LLQOptData *data, mDNSBool includeQuestion)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec; 
	rdataOpt *optRD;
	
	//!!!KRS when we implement multiple llqs per message, we'll need to memmove anything past the question section
	if (includeQuestion)
		{
		ptr = putQuestion(msg, ptr, msg->data + AbsoluteMaxDNSMessageData, &question->qname, question->qtype, question->qclass);
		if (!ptr) { LogMsg("ERROR: putLLQ - putQuestion"); return NULL; }
		}
	// locate OptRR if it exists, set pointer to end 
	// !!!KRS implement me

	
	// format opt rr (fields not specified are zero-valued)
	ubzero(&rr, sizeof(AuthRecord));
	opt->rdata = &rr.rdatastorage;
	
	opt->RecordType = kDNSRecordTypeKnownUnique;  // to avoid warnings in other layers
	opt->rrtype = kDNSType_OPT;
	opt->rdlength = LLQ_OPT_SIZE;
	opt->rdestimate = LLQ_OPT_SIZE;

	optRD = &rr.resrec.rdata->u.opt;
	optRD->opt = kDNSOpt_LLQ;
	optRD->optlen = sizeof(LLQOptData);
	umemcpy(&optRD->OptData.llq, data, sizeof(LLQOptData));
	ptr = PutResourceRecordTTL(msg, ptr, &msg->h.numAdditionals, opt, 0);
	if (!ptr) { LogMsg("ERROR: putLLQ - PutResourceRecordTTL"); return NULL; }

	return ptr;
	}

			  
mDNSlocal mDNSBool getLLQAtIndex(mDNS *m, DNSMessage *msg, const mDNSu8 *end, LLQOptData *llq, int index)
	{
	LargeCacheRecord lcr;
	int i;
	const mDNSu8 *ptr;
	
	ptr = LocateAdditionals(msg, end);
	if (!ptr) return mDNSfalse;

	// find the last additional
	for (i = 0; i < msg->h.numAdditionals; i++)
//		{ ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr); if (!ptr) return mDNSfalse; }
//!!!KRS workaround for LH server bug, which puts OPT as first additional
		{ ptr = GetLargeResourceRecord(m, msg, ptr, end, 0, kDNSRecordTypePacketAdd, &lcr); if (!ptr) return mDNSfalse; if (lcr.r.resrec.rrtype == kDNSType_OPT) break; }		
	if (lcr.r.resrec.rrtype != kDNSType_OPT) return mDNSfalse;
	if (lcr.r.resrec.rdlength < (index + 1) * LLQ_OPT_SIZE) return mDNSfalse;  // rdata too small 
	umemcpy(llq, (mDNSu8 *)&lcr.r.resrec.rdata->u.opt.OptData.llq + (index * sizeof(LLQOptData)), sizeof(LLQOptData));
	return mDNStrue;
	}

mDNSlocal void recvRefreshReply(mDNS *m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *q)
	{
	LLQ_Info *qInfo;
	LLQOptData pktData;

	qInfo = q->uDNS_info.llq;
	if (!getLLQAtIndex(m, msg, end, &pktData, 0)) { LogMsg("ERROR recvRefreshReply - getLLQAtIndex"); return; }
	if (pktData.llqOp != kLLQ_Refresh) return;
	if (!sameID(pktData.id, qInfo->id)) { LogMsg("recvRefreshReply - ID mismatch.  Discarding");  return; }
	if (pktData.err != LLQErr_NoError) { LogMsg("recvRefreshReply: received error %d from server", pktData.err); return; }

	qInfo->expire = mDNSPlatformTimeNow() + ((mDNSs32)pktData.lease * mDNSPlatformOneSecond);
	qInfo->retry = qInfo->expire + ((mDNSs32)pktData.lease * mDNSPlatformOneSecond * 3/4);
 
	qInfo->origLease = pktData.lease;
	qInfo->state = LLQ_Established;	
	}

mDNSlocal void sendLLQRefresh(mDNS *m, DNSQuestion *q, mDNSu32 lease)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llq;
	LLQ_Info *info = q->uDNS_info.llq;
	mStatus err;

	if (info->state == kLLQ_Refresh && info->ntries >= kLLQ_MAX_TRIES)
		{
		LogMsg("sendLLQRefresh - %d failed attempts for llq %s", info->ntries, q->qname.c);
		info->state = LLQ_Retry;
		info->retry = mDNSPlatformTimeNow() + kLLQ_DEF_RETRY * mDNSPlatformOneSecond;
		info->deriveRemovesOnResume = mDNStrue;
		return;
		//!!!KRS handle this - periodically try to re-establish
		}
		
	llq.vers = kLLQ_Vers;
	llq.llqOp = kLLQ_Refresh;
	llq.err = LLQErr_NoError;
	umemcpy(llq.id, info->id, 8);
	llq.lease = lease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llq, mDNStrue);
	if (!end) { LogMsg("ERROR: sendLLQRefresh - putLLQ"); return; }
	
	err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: sendLLQRefresh - mDNSSendDNSMessage returned %d", err);

	if (info->state == LLQ_Established) info->ntries = 1;
	else info->ntries++;
	info->state = LLQ_Refresh;
	q->LastQTime = mDNSPlatformTimeNow();
	info->retry = (info->expire - q->LastQTime) / 2;	
	}

mDNSlocal void recvLLQEvent(mDNS *m, DNSQuestion *q, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, mDNSInterfaceID InterfaceID)
	{
	DNSMessage ack;
	mDNSu8 *ackEnd = ack.data;
	mStatus err;

    // invoke response handler
	m->uDNS_info.CurrentQuery = q;
	q->uDNS_info.responseCallback(m, msg, end, q, q->uDNS_info.context);
	if (m->uDNS_info.CurrentQuery != q) return;
	
    //  format and send ack
	InitializeDNSMessage(&ack.h, msg->h.id, ResponseFlags);
	ackEnd = putQuestion(&ack, ack.data, ack.data + AbsoluteMaxDNSMessageData, &q->qname, q->qtype, q->qclass);
	if (!ackEnd) { LogMsg("ERROR: recvLLQEvent - putQuestion");  return; }
	err = mDNSSendDNSMessage(m, &ack, ackEnd, InterfaceID, srcaddr, srcport); 
	if (err) LogMsg("ERROR: recvLLQEvent - mDNSSendDNSMessage returned %d", err);
    }



mDNSlocal void hndlChallengeResponseAck(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	
	if (llq->err) { LogMsg("hndlChallengeResponseAck - received error %d from server", llq->err); goto error; }
	if (!sameID(info->id, llq->id)) { LogMsg("hndlChallengeResponseAck - ID changed.  discarding"); return; } // this can happen rarely (on packet loss + reordering)
	info->expire = mDNSPlatformTimeNow() + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
	info->retry = info->expire + ((mDNSs32)llq->lease * mDNSPlatformOneSecond * 3/4);
 
	info->origLease = llq->lease;
	info->state = LLQ_Established;	
	q->uDNS_info.responseCallback = llqResponseHndlr;
	llqResponseHndlr(m, pktMsg, end, q, NULL);
	return;

	error:
	info->state = LLQ_Error;
	}

mDNSlocal void sendChallengeResponse(mDNS *m, DNSQuestion *q, LLQOptData *llq)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	DNSMessage response;
	mDNSu8 *responsePtr = response.data;
	mStatus err;
	LLQOptData llqBuf;
	mDNSs32 timenow = mDNSPlatformTimeNow();
	
	if (info->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("sendChallengeResponse: %d failed attempts for LLQ %s. Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		info->state = LLQ_Retry;
		info->retry = timenow + (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}
		

	if (!llq)
		{
		llq = &llqBuf;
		llq->vers    = kLLQ_Vers;
		llq->llqOp   = kLLQ_Setup;
		llq->err     = LLQErr_NoError;
		umemcpy(llq->id, info->id, 8);
		llq->lease    = info->origLease;
		}

	q->LastQTime = timenow;
	info->retry = timenow  + (kLLQ_INIT_RESEND * info->ntries * mDNSPlatformOneSecond);
	
	if (constructQueryMsg(&response, &responsePtr, q)) goto error;
	responsePtr = putLLQ(&response, responsePtr, q, llq, mDNSfalse);
	if (!responsePtr) { LogMsg("ERROR: sendChallengeResponse - putLLQ"); goto error; }
	
	err = mDNSSendDNSMessage(m, &response, responsePtr, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: sendChallengeResponse - mDNSSendDNSMessage returned %d", err);
	// on error, we procede as normal and retry after the appropriate interval

	return;

	error:
	info->state = LLQ_Error;
	}



mDNSlocal void hndlRequestChallenge(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, LLQOptData *llq, DNSQuestion *q)
	{
	LLQ_Info *info = q->uDNS_info.llq;
	mDNSs32 timenow = mDNSPlatformTimeNow();
	switch(llq->err)
		{
		case LLQErr_NoError: break;
		case LLQErr_ServFull:
			LogMsg("hndlRequestChallenge - received ServFull from server for LLQ %s.  Retry in %d sec", q->qname.c, llq->lease);
			info->retry = timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);
			info->state = LLQ_Retry;
			simpleResponseHndlr(m, pktMsg, end, q, NULL);  // get available answers
			info->deriveRemovesOnResume = mDNStrue;
		case LLQErr_Static:
			info->state = LLQ_Static;
			LogMsg("LLQ %s: static", q->qname.c);
			simpleResponseHndlr(m, pktMsg, end, q, NULL);
			return;
		case LLQErr_FormErr:
			LogMsg("ERROR: hndlRequestChallenge - received FormErr from server for LLQ %s", q->qname.c);
			goto error;
		case LLQErr_BadVers:
			LogMsg("ERROR: hndlRequestChallenge - received BadVers from server");
			goto error;
		case LLQErr_UnknownErr:
			LogMsg("ERROR: hndlRequestChallenge - received UnknownErr from server for LLQ %s", q->qname.c);
			goto error;
		default:
			LogMsg("ERROR: hndlRequestChallenge - received invalid error %d for LLQ %s", llq->err, q->qname.c);
			goto error;
		}

	if (info->origLease != llq->lease)
		LogMsg("hndlRequestChallenge: requested lease %d, granted lease %d", info->origLease, llq->lease);

	// cache expiration in case we go to sleep before finishing setup
	info->origLease = llq->lease;
	info->expire = timenow + ((mDNSs32)llq->lease * mDNSPlatformOneSecond);

	// update state and timestamp
	info->state = LLQ_SecondaryRequest;
	umemcpy(info->id, llq->id, 8);
	info->ntries = 0; // first attempt to send response

	sendChallengeResponse(m, q, llq);
	return;


	error:
	info->state = LLQ_Error;	
	}


// response handler for initial and secondary setup responses
mDNSlocal void recvSetupResponse(mDNS *m, DNSMessage *pktMsg, const mDNSu8 *end, DNSQuestion *q, void *clientContext)
	{
	DNSQuestion pktQuestion;
	LLQOptData llq;
	const mDNSu8 *ptr = pktMsg->data;
	LLQ_Info *info = q->uDNS_info.llq;
	mDNSu8 rcode = (mDNSu8)(pktMsg->h.flags.b[1] & kDNSFlag1_RC);

	(void)clientContext;  // unused
	
	if (rcode && rcode != kDNSFlag1_RC_NXDomain)
		{
		LogMsg("LLQ Setup for %s failed with rcode %d.  Reverting to polling mode", q->qname.c, rcode);
		info->state = LLQ_Poll;
		q->uDNS_info.responseCallback = simpleResponseHndlr;
		q->LastQTime = mDNSPlatformTimeNow();
		q->ThisQInterval = 1;
		return;
		}
	
	ptr = getQuestion(pktMsg, ptr, end, 0, &pktQuestion);
	if (!ptr) { LogMsg("ERROR: recvSetupResponse - getQuestion"); goto error; }
	if (!SameDomainName(&q->qname, &pktQuestion.qname))
		{ LogMsg("ERROR: recvSetupResponse - mismatched question in response for llq setup %s", q->qname.c);   goto error; }

	if (!getLLQAtIndex(m, pktMsg, end, &llq, 0)) { LogMsg("ERROR: recvSetupResponse - GetLLQAtIndex"); goto error; }
	if (llq.llqOp != kLLQ_Setup) { LogMsg("ERROR: recvSetupResponse - bad op %d", llq.llqOp); goto error; } 
	if (llq.vers != kLLQ_Vers) { LogMsg("ERROR: recvSetupResponse - bad vers %d", llq.vers);  goto error; } 

	if (info->state == LLQ_InitialRequest) { hndlRequestChallenge(m, pktMsg, end, &llq, q); return; }
	if (info->state == LLQ_SecondaryRequest) { hndlChallengeResponseAck(m, pktMsg, end, &llq, q); return; }
	LogMsg("recvSetupResponse - bad state %d", info->state);


	error:
	info->state = LLQ_Error;
	}



mDNSlocal void startLLQHandshake(mDNS *m, LLQ_Info *info)
	{
	DNSMessage msg;
	mDNSu8 *end;
	LLQOptData llqData;
	DNSQuestion *q = info->question; 
	mStatus err;
	mDNSs32 timenow = mDNSPlatformTimeNow();
	
	if (info->ntries++ == kLLQ_MAX_TRIES)
		{
		LogMsg("startLLQHandshake: %d failed attempts for LLQ %s. Will re-try in %d minutes",
			   kLLQ_MAX_TRIES, q->qname.c, kLLQ_DEF_RETRY / 60);
		info->state = LLQ_Retry;
		info->retry = timenow + (kLLQ_DEF_RETRY * mDNSPlatformOneSecond);
		// !!!KRS give a callback error in these cases?
		return;
		}	
	
    // set llq rdata
	llqData.vers    = kLLQ_Vers;
	llqData.llqOp   = kLLQ_Setup;
	llqData.err     = LLQErr_NoError;
	ubzero(llqData.id, 8);
	llqData.lease    = kLLQ_DefLease;

	initializeQuery(&msg, q);
	end = putLLQ(&msg, msg.data, q, &llqData, mDNStrue);
	if (!end)
		{
		LogMsg("ERROR: startLLQHandshake - putLLQ");
		info->state = LLQ_Error;		
		return;
		}

	err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, &info->servAddr, info->servPort);
	if (err) LogMsg("ERROR: startLLQHandshake - mDNSSendDNSMessage returned %d", err);
	// on error, we procede as normal and retry after the appropriate interval

	// update question/info state
	info->state = LLQ_InitialRequest;
	info->origLease = kLLQ_DefLease;
    info->retry = timenow + (kLLQ_INIT_RESEND * mDNSPlatformOneSecond);
	q->LastQTime = timenow;
	q->uDNS_info.responseCallback = recvSetupResponse;
	q->uDNS_info.internal = mDNStrue;
	}

// wrapper for startLLQHandshake, invoked by async op callback
mDNSlocal void startLLQHandshakeCallback(mStatus err, mDNS *const m, void *llqInfo, const AsyncOpResult *result)
	{
	LLQ_Info *info = (LLQ_Info *)llqInfo;
	const zoneData_t *zoneInfo = &result->zoneData;

	if (err)
		{
		LogMsg("ERROR: startLLQHandshakeCallback invoked with error code %d", err);
		info->state = LLQ_Poll;
		info->question->LastQTime = 0;  // trigger immediate poll
		info->question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
		return;
		}

	if (info->state == LLQ_Cancelled)
		{
		// StopQuery was called while we were getting the zone info
		LogMsg("startLLQHandshake - LLQ Cancelled.");
		info->question = NULL;  // question may be deallocated
		ufree(info);
		return;
		}

	if (info->state != LLQ_GetZoneInfo)
		{
		LogMsg("ERROR: startLLQHandshake - bad state %d", info->state);
		return;
		}
	
    // cache necessary zone data
	info->servAddr.type = zoneInfo->primaryAddr.type;
	info->servAddr.ip.v4.NotAnInteger = zoneInfo->primaryAddr.ip.v4.NotAnInteger;
	info->servPort.NotAnInteger = zoneInfo->llqPort.NotAnInteger;
    info->ntries = 0;
	startLLQHandshake(m, info);
	}

mDNSlocal mStatus startLLQ(mDNS *m, DNSQuestion *question)
    {
	LLQ_Info *info;
	mStatus err = mStatus_NoError;
	
	// allocate / init info struct
    info = umalloc(sizeof(LLQ_Info));
    if (!info) { LogMsg("ERROR: startLLQ - malloc"); return mStatus_NoMemoryErr; }
	ubzero(info, sizeof(LLQ_Info));
    info->state = LLQ_GetZoneInfo;
	
	// link info/question
	info->question = question;
	question->uDNS_info.llq = info;

	question->uDNS_info.responseCallback = llqResponseHndlr;
	
	err = startGetZoneData(&question->qname, m, mDNSfalse, mDNStrue, startLLQHandshakeCallback, info);
    if (err)
		{
		LogMsg("ERROR: startLLQ - startGetZoneData returned %d", err);
		info->question = NULL;
		ufree(info);
		question->uDNS_info.llq = NULL;
		return err;
		}

	LinkActiveQuestion(&m->uDNS_info, question);
	return err;
	}

mDNSlocal mDNSBool recvLLQResponse(mDNS *m, DNSMessage *msg, const mDNSu8 *end, const mDNSAddr *srcaddr, mDNSIPPort srcport, const mDNSInterfaceID InterfaceID)
	{
	DNSQuestion pktQ, *q;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	const mDNSu8 *ptr = msg->data;
	LLQ_Info *llqInfo;
	
	if (!msg->h.numQuestions) return mDNSfalse;

	ptr = getQuestion(msg, ptr, end, 0, &pktQ);
	if (!ptr) return mDNSfalse;
	pktQ.uDNS_info.id = msg->h.id;
	
	// !!!KRS we should do a table lookup to quickly determine if this packet is for an LLQ
   
	q = u->ActiveQueries;
	while (q)
		{
		llqInfo = q->uDNS_info.llq;
		if (q->LongLived &&                          
			llqInfo &&
			q->qnamehash == pktQ.qnamehash &&        
			q->qtype == pktQ.qtype &&
			SameDomainName(&q->qname, &pktQ.qname))
			{
			u->CurrentQuery = q;
			if (llqInfo->state == LLQ_Established || (llqInfo->state == LLQ_Refresh && msg->h.numAnswers))			
				{ recvLLQEvent(m, q, msg, end, srcaddr, srcport, InterfaceID); return mDNStrue; }
			else if (msg->h.id.NotAnInteger != q->uDNS_info.id.NotAnInteger)
				{ q = q->next; continue; }
			else if (llqInfo->state == LLQ_Refresh && msg->h.numAdditionals && !msg->h.numAnswers)
				{ recvRefreshReply(m, msg, end, q); return mDNStrue; }
			else if (llqInfo->state < LLQ_Static)
				{ q->uDNS_info.responseCallback(m, msg, end, q, q->uDNS_info.context); return mDNStrue; }			
			}
		q = q->next;
		}
	return mDNSfalse;
	}

mDNSexport mDNSBool IsActiveUnicastQuery(DNSQuestion *const question, uDNS_GlobalInfo *u)
    {
	DNSQuestion *q;

	for (q = u->ActiveQueries; q; q = q->next)
		{
		if (q == question)
			{
			if (!question->uDNS_info.id.NotAnInteger || question->InterfaceID || IsLocalDomain(&question->qname))
				LogMsg("Warning: Question %s in Active Unicast Query list with id %d, interfaceID %x",
					   question->qname.c, question->uDNS_info.id.NotAnInteger, question->InterfaceID);
			return mDNStrue;
			}
		}
	return mDNSfalse;
	}

// stopLLQ happens IN ADDITION to stopQuery
mDNSlocal void stopLLQ(mDNS *m, DNSQuestion *question)
	{	
	LLQ_Info *info = question->uDNS_info.llq;
	(void)m;  // unused

	if (!question->LongLived) { LogMsg("ERROR: stopLLQ - LongLived flag not set"); return; }
	if (!info)                { LogMsg("ERROR: stopLLQ - llq info is NULL");       return; }

	switch (info->state)
		{
		case LLQ_UnInit:
			LogMsg("ERROR: stopLLQ - state LLQ_UnInit");
			//!!!KRS should we unlink info<->question here?
			return;
		case LLQ_GetZoneInfo:
			info->question = NULL; // remove ref to question, as it may be freed when we get called back from async op
			info->state = LLQ_Cancelled;
			return;			
		case LLQ_Established:
		case LLQ_Refresh:
			// refresh w/ lease 0
			sendLLQRefresh(m, question, 0);
			goto free_info;	
		default:			
			debugf("stopLLQ - silently discarding LLQ in state %d", info->state);				
			goto free_info;
		}
	
	free_info:
	info->question = NULL;
	ufree(info);
	question->uDNS_info.llq = NULL;
	
	}

mDNSexport mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSQuestion *qptr, *prev = NULL;
	CacheRecord *ka;
	
	qptr = u->ActiveQueries;
	while (qptr)
        {
        if (qptr == question)
            {
			if (question->LongLived && question->uDNS_info.llq)
				stopLLQ(m, question);
			if (m->uDNS_info.CurrentQuery == question)
				m->uDNS_info.CurrentQuery = m->uDNS_info.CurrentQuery->next;
			while (question->uDNS_info.knownAnswers)
				{
				ka = question->uDNS_info.knownAnswers;
				question->uDNS_info.knownAnswers = question->uDNS_info.knownAnswers->next;
				ufree(ka);
				}
			if (prev) prev->next = question->next;
            else u->ActiveQueries = question->next;
			return mStatus_NoError;
            }
        prev = qptr;
		qptr = qptr->next;
        }
    LogMsg("uDNS_StopQuery: no such active query (%s)", question->qname.c);
    return mStatus_UnknownErr;
    }

mDNSexport void uDNS_SuspendLLQs(mDNS *m)
	{
	DNSQuestion *q;
	LLQ_Info *llq;
	for (q = m->uDNS_info.ActiveQueries; q; q = q->next)
		{
		llq = q->uDNS_info.llq;
		if (q->LongLived && llq && llq->state < LLQ_Suspended)
			{
			if (llq->state == LLQ_Established || llq->state == LLQ_Refresh)
				sendLLQRefresh(m, q, 0);
			// note that we suspend LLQs in setup states too			
			if (llq->state != LLQ_Retry) llq->state = LLQ_Suspended;
			}
		}
	}

extern void uDNS_RestartLLQs(mDNS *m)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSQuestion *q;
	LLQ_Info *llqInfo;
	
	u->CurrentQuery = u->ActiveQueries;
	while (u->CurrentQuery)
		{
		q = u->CurrentQuery;
		u->CurrentQuery = u->CurrentQuery->next;
		llqInfo = q->uDNS_info.llq;
		if (q->LongLived && llqInfo && llqInfo->state == LLQ_Suspended)		
			{ llqInfo->ntries = 0; llqInfo->deriveRemovesOnResume = mDNStrue; startLLQHandshake(m, llqInfo); }
		}
	}


mDNSlocal mStatus startQuery(mDNS *const m, DNSQuestion *const question, mDNSBool internal)
    {
    uDNS_GlobalInfo *u = &m->uDNS_info;
    DNSMessage msg;
    mDNSu8 *endPtr;
    mStatus err = mStatus_NoError;
	const mDNSAddr *server;
	
    //!!!KRS we should check if the question is already in our acivequestion list
	if (!ValidateDomainName(&question->qname))
		{
		LogMsg("Attempt to start query with invalid qname %##s %s", question->qname.c, DNSTypeName(question->qtype));
		return mStatus_Invalid;
		}
		
	question->next = NULL;
	question->qnamehash = DomainNameHashValue(&question->qname);    // to do quick domain name comparisons
    question->uDNS_info.id = newMessageID(u);
	
	// break here if its and LLQ
	if (question->LongLived) return startLLQ(m, question);

	err = constructQueryMsg(&msg, &endPtr, question);
	if (err) return err;

	// else send the query to our server

	question->LastQTime = mDNSPlatformTimeNow();
	question->ThisQInterval = INIT_UCAST_POLL_INTERVAL;
    // store the question/id in active question list
    question->uDNS_info.timestamp = question->LastQTime;
	question->uDNS_info.internal = internal;
	LinkActiveQuestion(u, question);
	question->uDNS_info.knownAnswers = NULL;

	server = getInitializedDNS(u);
	if (!server) { LogMsg("startQuery - no initialized DNS"); err =  mStatus_NotInitializedErr; }
	else err = mDNSSendDNSMessage(m, &msg, endPtr, question->InterfaceID, server, UnicastDNSPort);	
	if (err) { LogMsg("ERROR: startQuery - %d (keeping question in list for retransmission", err); }

	return err;
	}
	
mDNSexport mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question)
    {
	ubzero(&question->uDNS_info, sizeof(uDNS_QuestionInfo));
	question->uDNS_info.responseCallback = simpleResponseHndlr;
	question->uDNS_info.context = NULL;
	return startQuery(m, question, 0);
    }

// explicitly set response handler
mDNSlocal mStatus startInternalQuery(DNSQuestion *q, mDNS *m, InternalResponseHndlr callback, void *hndlrContext)
    {
	ubzero(&q->uDNS_info, sizeof(uDNS_QuestionInfo));
    q->QuestionContext = hndlrContext;	
    q->uDNS_info.responseCallback = callback;
	q->uDNS_info.context = hndlrContext;
    return startQuery(m, q, 1);
    }



// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Domain -> Name Server Conversion
#endif


/* startGetZoneData
 *
 * asyncronously find the address of the nameserver for the enclosing zone for a given domain name,
 * i.e. the server to which update and LLQ requests will be sent for a given name.  Once the address is
 * derived, it will be passed to the callback, along with a context pointer.  If the zone cannot
 * be determined or if an error occurs, an all-zeros address will be passed and a message will be
 * written to the syslog.
 *
 * If the FindUpdatePort arg is set, the port on which the server accepts dynamic updates is determined
 * by querying for the _update._dns-sd._udp.<zone>. SRV record.  Likewise, if the FindLLQPort arg is set,
 * the port on which the server accepts long lived queries is determined by querying for _llq._dns-sd.
 * _udp.<zone>. record.  If either of these queries fail, or flags are not specified, the llqPort and
 * updatePort fields in the result structure are set to zero.
 *
 *  Steps for deriving the zone name are as follows:
 *
 * Query for an SOA record for the required domain.  If we don't get an answer (or an SOA in the Authority
 * section), we strip the leading label from the name and repeat, until we get an answer.
 *
 * The name of the SOA record is our enclosing zone.  The mname field in the SOA rdata is the domain
 * name of the primary NS.
 *
 * We verify that there is an NS record with this zone for a name and the mname for its rdata.
 * (!!!KRS this seems redundant, but BIND does this, and it should normally be zero-overhead since
 * the NS query will get us address records in the additionals section, which we'd otherwise have to
 * explicitly query for.)
 *
 * We then query for the address record for this nameserver (if it is not in the addionals section of
 * the NS record response.)
 */
 

// state machine types and structs
//

// state machine states
typedef enum
    {
    init,
    lookupSOA,
	foundZone,
	lookupNS,
	foundNS,
	lookupA,
	foundA,
	lookupPort,
	foundPort,
	complete
    } ntaState;

// state machine actions
typedef enum
    {
    smContinue,  // continue immediately to next state 
    smBreak,     // break until next packet/timeout 
	smError      // terminal error - cleanup and abort
    } smAction;
 
typedef struct
    {
    domainname 	origName;            // name we originally try to convert
    domainname 	*curSOA;             // name we have an outstanding SOA query for
    ntaState  	state;               // determines what we do upon receiving a packet
    mDNS	    *m;
    domainname  zone;                // left-hand-side of SOA record
    mDNSu16     zoneClass; 
    domainname  ns;                  // mname in SOA rdata, verified in confirmNS state
    mDNSv4Addr  addr;                // address of nameserver
    DNSQuestion question;            // storage for any active question
    DNSQuestion extraQuestion;       // additional storage
    mDNSBool    questionActive;      // if true, StopQuery() can be called on the question field
    mDNSBool    findUpdatePort;      
    mDNSBool    findLLQPort;
    mDNSIPPort  updatePort;
    mDNSIPPort  llqPort;
    AsyncOpCallback *callback;       // caller specified function to be called upon completion
    void        *callbackInfo;
    } ntaContext;


// function prototypes (for routines that must be used as fn pointers prior to their definitions,
// and allows states to be read top-to-bottom in logical order)
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr);
mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr);
mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);
mDNSlocal smAction hndlLookupPorts(DNSMessage *msg, const mDNSu8 *end, ntaContext *context);

// initialization
mDNSlocal mStatus startGetZoneData(domainname *name, mDNS *m, mDNSBool findUpdatePort, mDNSBool findLLQPort,
								   AsyncOpCallback callback, void *callbackInfo)
    {
    ntaContext *context = (ntaContext*)umalloc(sizeof(ntaContext));
    if (!context) { LogMsg("ERROR: startGetZoneData - umalloc failed");  return mStatus_NoMemoryErr; }
	ubzero(context, sizeof(ntaContext));
    ustrcpy(context->origName.c, name->c);
    context->state = init;
    context->m = m;
	context->callback = callback;
	context->callbackInfo = callbackInfo;
	context->findUpdatePort = findUpdatePort;
	context->findLLQPort = findLLQPort;
    getZoneData(m, NULL, NULL, NULL, context);
    return mStatus_NoError;
    }

// state machine entry routine
mDNSlocal void getZoneData(mDNS *const m, DNSMessage *msg, const mDNSu8 *end, DNSQuestion *question, void *contextPtr)
    {
	AsyncOpResult result;
	ntaContext *context = (ntaContext*)contextPtr;
	smAction action;

    // unused
	(void)m;
	(void)question;
	
	// stop any active question
	if (context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}

	if (msg && msg->h.flags.b[2] >> 4 && msg->h.flags.b[2] >> 4 != kDNSFlag1_RC_NXDomain)
		{
		// rcode non-zero, non-nxdomain
		LogMsg("ERROR: getZoneData - received response w/ rcode %d", msg->h.flags.b[2] >> 4);
		goto error;
		}
 	
	switch (context->state)
        {
        case init:
        case lookupSOA:
            action = hndlLookupSOA(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundZone:
		case lookupNS:
			action = confirmNS(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundNS:
		case lookupA:
			action = lookupNSAddr(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
		case foundA:
			if (!context->findUpdatePort && !context->findLLQPort)
				{
				context->state = complete;
				break;
				}
		case lookupPort:
			action = hndlLookupPorts(msg, end, context);
			if (action == smError) goto error;
			if (action == smBreak) return;
			if (action == smContinue) context->state = complete;
		case foundPort:
		case complete: break;			
		}
					  
	if (context->state != complete)
		{
		LogMsg("ERROR: getZoneData - exited state machine with state %d", context->state);
		goto error;
		}
	
	result.type = zoneDataResult;
	result.zoneData.primaryAddr.ip.v4.NotAnInteger = context->addr.NotAnInteger;
	result.zoneData.primaryAddr.type = mDNSAddrType_IPv4;
	ustrcpy(result.zoneData.zoneName.c, context->zone.c);
	result.zoneData.zoneClass = context->zoneClass;
	result.zoneData.llqPort    = context->findLLQPort    ? context->llqPort    : zeroIPPort;
	result.zoneData.updatePort = context->findUpdatePort ? context->updatePort : zeroIPPort;
	context->callback(mStatus_NoError, context->m, context->callbackInfo, &result);
	goto cleanup;	
			
error:
	if (context && context->callback)
		context->callback(mStatus_UnknownErr, context->m, context->callbackInfo, NULL);	
cleanup:
	if (context && context->questionActive)
		{
		uDNS_StopQuery(context->m, &context->question);
		context->questionActive = mDNSfalse;
		}		
    if (context) ufree(context);
	}

mDNSlocal smAction hndlLookupSOA(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
    {
    mStatus err;
    LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	DNSQuestion *query = &context->question;
	const mDNSu8 *ptr;
	
    if (msg)
        {
        // if msg contains SOA record in answer or authority sections, update context/state and return 		
		int i;
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Answers - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_SOA && SameDomainName(context->curSOA, &rr->name))
				{
				processSOA(context, rr);
				return smContinue;
				}
			}		
		ptr = LocateAuthorities(msg, end);
		// SOA not in answers, check in authority
		for (i = 0; i < msg->h.numAuthorities; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr); ///!!!KRS using type PacketAns for auth
			if (!ptr) { LogMsg("ERROR: hndlLookupSOA, Authority - GetLargeResourceRecord returned NULL");  return smError; }		 		
			if (rr->rrtype == kDNSType_SOA)
				{
				processSOA(context, rr);
				return smContinue;
				}
			}
		}    

    if (context->state != init && !context->curSOA->c[0])
        {
        // we've gone down to the root and have not found an SOA  
        LogMsg("ERROR: hndlLookupSOA - recursed to root label of %s without finding SOA", 
                context->origName.c);
		return smError;
        }

    ubzero(query, sizeof(DNSQuestion));
    // chop off leading label unless this is our first try
    if (context->state == init)  context->curSOA = &context->origName;
    else                         context->curSOA = (domainname *)(context->curSOA->c + context->curSOA->c[0]+1);
    
    context->state = lookupSOA;
    ustrcpy(query->qname.c, context->curSOA->c);
    query->qtype = kDNSType_SOA;
    query->qclass = kDNSClass_IN;
    err = startInternalQuery(query, context->m, getZoneData, context);
	context->questionActive = mDNStrue;
	if (err) LogMsg("hndlLookupSOA: startInternalQuery returned error %d (breaking until next periodic retransmission)", err);

    return smBreak;     // break from state machine until we receive another packet	
    }

mDNSlocal void processSOA(ntaContext *context, ResourceRecord *rr)
	{
	ustrcpy(context->zone.c, rr->name.c);
	context->zoneClass = rr->rrclass;
	ustrcpy(context->ns.c, rr->rdata->u.soa.mname.c);
	context->state = foundZone;
	}


mDNSlocal smAction confirmNS(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	DNSQuestion *query = &context->question;
	mStatus err;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	const mDNSu8 *ptr;
	int i;
		
	if (context->state == foundZone)
		{
		// we've just learned the zone.  confirm that an NS record exists
		ustrcpy(query->qname.c, context->zone.c);
		query->qtype = kDNSType_NS;
		query->qclass = kDNSClass_IN;
		err = startInternalQuery(query, context->m, getZoneData, context);
		context->questionActive = mDNStrue;	   
		if (err) LogMsg("confirmNS: startInternalQuery returned error %d (breaking until next periodic retransmission", err);
		context->state = lookupNS;
		return smBreak;  // break from SM until we receive another packet
		}
	else if (context->state == lookupNS)
		{
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: confirmNS, Answers - GetLargeResourceRecord returned NULL");  return smError; }
			if (rr->rrtype == kDNSType_NS &&
				SameDomainName(&context->zone, &rr->name) && SameDomainName(&context->ns, &rr->rdata->u.name))
				{
				context->state = foundNS;
				return smContinue;  // next routine will examine additionals section of A record				
				}				
			}
		LogMsg("ERROR: could not confirm existance of NS record %s", context->zone.c);
		return smError;
		}
	else { LogMsg("ERROR: confirmNS - bad state %d", context->state); return smError; }
	}

mDNSlocal smAction queryNSAddr(ntaContext *context)
	{
	mStatus err;
	DNSQuestion *query = &context->question;
	
	ustrcpy(query->qname.c, context->ns.c);
	query->qtype = kDNSType_A;
	query->qclass = kDNSClass_IN;
	err = startInternalQuery(query, context->m, getZoneData, context);
	context->questionActive = mDNStrue;	
	if (err) LogMsg("confirmNS: startInternalQuery returned error %d (breaking until next periodic retransmission)", err);
	context->state = lookupA;
	return smBreak;
	}

mDNSlocal smAction lookupNSAddr(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	const mDNSu8 *ptr;
	int i;
	LargeCacheRecord lcr;
	ResourceRecord *rr = &lcr.r.resrec;
	
	if (context->state == foundNS)
		{
		// we just found the NS record - look for the corresponding A record in the Additionals section
		if (!msg->h.numAdditionals) return queryNSAddr(context);
		ptr = LocateAdditionals(msg, end);
		if (!ptr)
			{
			LogMsg("ERROR: lookupNSAddr - LocateAdditionals returned NULL, expected %d additionals", msg->h.numAdditionals);
			return queryNSAddr(context);
			}
		else
			{
			for (i = 0; i < msg->h.numAdditionals; i++)
				{
				ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
				if (!ptr)
					{
					LogMsg("ERROR: lookupNSAddr, Additionals - GetLargeResourceRecord returned NULL");
					return queryNSAddr(context);
					}
				if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
					{
					context->addr.NotAnInteger = rr->rdata->u.ip.NotAnInteger;
					context->state = foundA;
					return smContinue;
					}
				}
			}
		// no A record in Additionals - query the server
		return queryNSAddr(context);
		}
	else if (context->state == lookupA)
		{
		ptr = LocateAnswers(msg, end);
		if (!ptr) { LogMsg("ERROR: lookupNSAddr: LocateAnswers returned NULL");  return smError; }
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: lookupNSAddr, Answers - GetLargeResourceRecord returned NULL"); break; }			
			if (rr->rrtype == kDNSType_A && SameDomainName(&context->ns, &rr->name))
				{
				context->addr.NotAnInteger = rr->rdata->u.ip.NotAnInteger;
				context->state = foundA;
				return smContinue;
				}
			}		
		LogMsg("ERROR: lookupNSAddr: Address record not found in answer section");
		return smError;
		}
	else { LogMsg("ERROR: lookupNSAddr - bad state %d", context->state); return smError; }
	}
	
mDNSlocal smAction lookupDNSPort(DNSMessage *msg, const mDNSu8 *end, ntaContext *context, char *portName, mDNSIPPort *port)
	{
	int i;
	LargeCacheRecord lcr;
	const mDNSu8 *ptr;
	DNSQuestion *q;
	mStatus err;
	
	if (context->state == lookupPort)  // we've already issued the query
		{
		if (!msg) { LogMsg("ERROR: hndlLookupUpdatePort - NULL message"); return smError; }
		ptr = LocateAnswers(msg, end);
		for (i = 0; i < msg->h.numAnswers; i++)
			{
			ptr = GetLargeResourceRecord(context->m, msg, ptr, end, 0, kDNSRecordTypePacketAns, &lcr);
			if (!ptr) { LogMsg("ERROR: hndlLookupUpdatePort - GetLargeResourceRecord returned NULL");  return smError; }
			if (ResourceRecordAnswersQuestion(&lcr.r.resrec, &context->question))
				{
				port->NotAnInteger = lcr.r.resrec.rdata->u.srv.port.NotAnInteger;
				context->state = foundPort;
				return smContinue;
				}
			}
		LogMsg("hndlLookupUpdatePort %s - answer not contained in reply.  Guessing port %d", portName, UnicastDNSPort);
		*port = UnicastDNSPort;
		context->state = foundPort;
		return smContinue;
		}

	// query the server for the update port for the zone
	context->state = lookupPort;
	q = &context->question;
	MakeDomainNameFromDNSNameString(&q->qname, portName);
	ustrcpy((q->qname.c + ustrlen(q->qname.c)), context->zone.c);
    q->qtype = kDNSType_SRV;
    q->qclass = kDNSClass_IN;
    err = startInternalQuery(q, context->m, getZoneData, context);
	context->questionActive = mDNStrue;
    if (err) LogMsg("hndlLookupSOA: startInternalQuery returned error %d (breaking until next periodic retransmission)", err);
    return smBreak;     // break from state machine until we receive another packet	
	}

mDNSlocal smAction hndlLookupPorts(DNSMessage *msg, const mDNSu8 *end, ntaContext *context)
	{
	smAction action;
	
	if (context->findUpdatePort && !context->updatePort.NotAnInteger)
		{
		action = lookupDNSPort(msg, end, context, UPDATE_PORT_NAME, &context->updatePort);
		if (action != smContinue) return action;
		}
	if (context->findLLQPort && !context->llqPort.NotAnInteger)
		return lookupDNSPort(msg, end, context, LLQ_PORT_NAME, &context->llqPort);

	return smContinue;
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Truncation Handling
#endif

typedef struct
	{
    DNSQuestion  *question;
    DNSMessage reply;
    mDNSu16  replylen;
    int nread;
    mDNS *m;
	} tcpInfo_t;

// issue queries over a conected socket
mDNSlocal void conQueryCallback(int sd, void *context, mDNSBool ConnectionEstablished)
	{
	mStatus err = 0;
	char msgbuf[356];  // 96 (hdr) + 256 (domain) + 4 (class/type)
	DNSMessage *msg;
	mDNSu8 *end;
	tcpInfo_t *info = (tcpInfo_t *)context;
	DNSQuestion *question = info->question;
	int n;

	question->uDNS_info.id.NotAnInteger = (mDNSu16)~0;

	if (ConnectionEstablished)
		{
		// connection is established - send the message
		msg = (DNSMessage *)&msgbuf;
		err = constructQueryMsg(msg, &end, question);
		if (err) { LogMsg("ERROR: conQueryCallback: constructQueryMsg - %d", err);  goto error; }
		err = mDNSSendDNSMessage_tcp(info->m, msg, end, sd);
		if (err) { LogMsg("ERROR: conQueryCallback: mDNSSendDNSMessage_tcp - %d", err);  goto error; }
		return;
		}
	else
		{
		if (!info->nread)
			{
			// read msg len
			n = mDNSPlatformReadTCP(sd, &info->replylen, 2);
			if (n != 2)
				{
				LogMsg("ERROR:conQueryCallback - attempt to read message length failed (read returned %d)", n);
				goto error;
				}
			}
		n = mDNSPlatformReadTCP(sd, ((char *)&info->reply) + info->nread, info->replylen - info->nread);
		if (n < 0) { LogMsg("ERROR: conQueryCallback - read returned %d", n); goto error; }
		info->nread += n;
		if (info->nread == info->replylen)
			{
			// finished reading message
			receiveMsg(info->m, &info->reply, ((mDNSu8 *)&info->reply) + info->replylen, question->InterfaceID);
			mDNSPlatformTCPCloseConnection(sd);
			ufree(info);
			return;
			}
		else return;
		}
	return;

	error:
	mDNSPlatformTCPCloseConnection(sd);
	ufree(info);
	}

mDNSlocal void hndlTruncatedAnswer(DNSQuestion *question, const  mDNSAddr *src, mDNS *m)
	{
	mStatus connectionStatus;
	uDNS_QuestionInfo *info = &question->uDNS_info;
	int sd;
	tcpInfo_t *context;

	context = (tcpInfo_t *)umalloc(sizeof(tcpInfo_t));
	if (!context) { LogMsg("ERROR: hndlTruncatedAnswer - memallocate failed"); return; }
	ubzero(context, sizeof(tcpInfo_t));
	context->question = question;
	context->m = m;

	info->id.NotAnInteger = (mDNSu16)~0;             // all 1's indicates TCP queries
	info->timestamp = mDNSPlatformTimeNow();         // reset timestamp

	connectionStatus = mDNSPlatformTCPConnect(src, UnicastDNSPort, question->InterfaceID, conQueryCallback, context, &sd);
	if (connectionStatus == mStatus_ConnectionEstablished)  // manually invoke callback if connection completes
		{
		conQueryCallback(sd, context, mDNStrue);
		return;
		}
	if (connectionStatus == mStatus_ConnectionPending) return; // callback will be automatically invoked when connection completes
	LogMsg("hndlTruncatedAnswer: connection failed");
	uDNS_StopQuery(m, question);  //!!!KRS can we really call this here?
	}


// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark - Dynamic Updates
#endif


mDNSlocal mDNSu8 *putZone(DNSMessage *const msg, mDNSu8 *ptr, mDNSu8 *limit, const domainname *zone, mDNSOpaque16 zoneClass)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, zone);
	if (!ptr || ptr + 4 > limit) return NULL;		// If we're out-of-space, return NULL
	((mDNSOpaque16 *)ptr)->NotAnInteger = kDNSType_SOA;
	ptr += 2;
	((mDNSOpaque16 *)ptr)->NotAnInteger = zoneClass.NotAnInteger;
	ptr += 2;
	msg->h.mDNS_numZones++;
	return ptr;
	}



mDNSlocal mDNSu8 *putPrereqNameNotInUse(domainname *name, DNSMessage *msg, mDNSu8 *ptr, mDNSu8 *end)
	{
	AuthRecord prereq;

	ubzero(&prereq, sizeof(AuthRecord));
	ustrcpy(prereq.resrec.name.c, name->c);
	prereq.resrec.rrtype = kDNSQType_ANY;
	prereq.resrec.rrclass = kDNSClass_NONE;
	ptr = putEmptyResourceRecord(msg, ptr, end, &msg->h.mDNS_numPrereqs, &prereq);
	return ptr;
	}

mDNSlocal mDNSu8 *putDeletionRecord(DNSMessage *msg, mDNSu8 *ptr, ResourceRecord *rr)
	{
	mDNSu16 origclass;
	// deletion: specify record w/ TTL 0, class NONE

	origclass = rr->rrclass;
	rr->rrclass = kDNSClass_NONE;
	ptr = PutResourceRecordTTL(msg, ptr, &msg->h.mDNS_numUpdates, rr, 0);
	rr->rrclass = origclass;
	return ptr;
	}

mDNSlocal mDNSu8 *putUpdateLease(DNSMessage *msg, mDNSu8 *end)
	{
	AuthRecord rr;
	ResourceRecord *opt = &rr.resrec; 
	rdataOpt *optRD;

	ubzero(&rr, sizeof(AuthRecord));
	opt->rdata = &rr.rdatastorage;
	
	opt->RecordType = kDNSRecordTypeKnownUnique;  // to avoid warnings in other layers
	opt->rrtype = kDNSType_OPT;
	opt->rdlength = LEASE_OPT_SIZE;
	opt->rdestimate = LEASE_OPT_SIZE;

	optRD = &rr.resrec.rdata->u.opt;
	optRD->opt = kDNSOpt_Lease;
	optRD->optlen = sizeof(mDNSs32);
	optRD->OptData.lease = kUpdate_DefLease;
	end = PutResourceRecordTTL(msg, end, &msg->h.numAdditionals, opt, 0);
	if (!end) { LogMsg("ERROR: putUpdateLease - PutResourceRecordTTL"); return NULL; }

	return end;

	}

mDNSlocal void sendRecordRegistration(mDNS *const m, AuthRecord *rr)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSOpaque16 id;
	uDNS_AuthInfo *authInfo;
	uDNS_RegInfo *regInfo = &rr->uDNS_info;
	mStatus err = mStatus_UnknownErr;

	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	rr->uDNS_info.id.NotAnInteger = id.NotAnInteger;
	
    // set zone
	ptr = putZone(&msg, ptr, end, &regInfo->zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto error;
	
	if (rr->resrec.RecordType == kDNSRecordTypeKnownUnique || rr->uDNS_info.state == regState_Refresh)
		{
		// KnownUnique means the record must ALREADY exist, as does refresh
		// prereq: record must exist (put record in prereq section w/ TTL 0)
		ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &rr->resrec, 0);
		if (!ptr) goto error;
		}
	else if (rr->resrec.RecordType != kDNSRecordTypeShared)
		{
		ptr = putPrereqNameNotInUse(&rr->resrec.name, &msg, ptr, end);
		if (!ptr) goto error;
		}

	ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec);
	if (!ptr) goto error;

	if (rr->uDNS_info.lease)
		ptr = putUpdateLease(&msg, ptr);
	   
	rr->uDNS_info.expire = -1;
	
	authInfo = GetAuthInfoForZone(u, &regInfo->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &regInfo->ns, regInfo->port, authInfo);
		if (err) { LogMsg("ERROR: sendRecordRegistration - mDNSSendSignedDNSMessage - %d", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &regInfo->ns, regInfo->port);
		if (err) { LogMsg("ERROR: sendRecordRegistration - mDNSSendDNSMessage - %d", err); goto error; }
		}

	if (regInfo->state != regState_Refresh) regInfo->state = regState_Pending;
	return;

error:
	if (rr->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, rr);
		rr->uDNS_info.state = regState_Unregistered;
		}
	rr->RecordCallback(m, rr, err);
	// NOTE: not safe to touch any client structures here	
	}

mDNSlocal void RecordRegistrationCallback(mStatus err, mDNS *const m, void *authPtr, const AsyncOpResult *result)	
	{
	AuthRecord *newRR = (AuthRecord*)authPtr;
	const zoneData_t *zoneData = &result->zoneData;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	AuthRecord *ptr;

	for (ptr = u->RecordRegistrations; ptr; ptr = ptr->next)
		if (ptr == newRR) break;
	if (!ptr) { LogMsg("RecordRegistrationCallback - RR no longer in list.  Discarding."); return; }		
	
	if (err) { LogMsg("RecordRegistrationCallback: error %d", err); goto error; }
	if (newRR->uDNS_info.state == regState_Cancelled)
		{
		//!!!KRS we should send a memfree callback here!
		LogMsg("Registration of %s type %d cancelled prior to update",
			   newRR->resrec.name.c, newRR->resrec.rrtype);
		newRR->uDNS_info.state = regState_Unregistered;
		unlinkAR(&u->RecordRegistrations, newRR);
		return;
		}
	
	if (result->type != zoneDataResult)
		{
		LogMsg("ERROR: buildUpdatePacket passed incorrect result type %d", result->type);
		goto error;
		}

	if (newRR->resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("ERROR: New resource record's class (%d) does not match zone class (%d)",
			   newRR->resrec.rrclass, zoneData->zoneClass);
		goto error;
		}	
	
	// cache zone data
	ustrcpy(newRR->uDNS_info.zone.c, zoneData->zoneName.c);
    newRR->uDNS_info.ns.type = mDNSAddrType_IPv4;
	newRR->uDNS_info.ns.ip.v4.NotAnInteger = zoneData->primaryAddr.ip.v4.NotAnInteger;
	newRR->uDNS_info.port.NotAnInteger = zoneData->updatePort.NotAnInteger;

	sendRecordRegistration(m, newRR);
	return;
		
error:
	if (newRR->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, newRR);
		newRR->uDNS_info.state = regState_Unregistered;
		}
	newRR->RecordCallback(m, newRR, err);
	// NOTE: not safe to touch any client structures here
	}


mDNSlocal mDNSBool setHostTarget(AuthRecord *rr, mDNS *m)
	{
	domainname *target;

	if (!rr->HostTarget)
	{
	debugf("Service %s - not updating host target", rr->resrec.name.c);
	return mDNSfalse;
	}

    // set SRV target
	target = GetRRDomainNameTarget(&rr->resrec);
	if (!target)
		{
		LogMsg("ERROR: setHostTarget: Can't set target of rrtype %d", rr->resrec.rrtype);
		return mDNSfalse;
		}

	if (SameDomainName(target, &m->uDNS_info.hostname))
		{
		debugf("Host target for %s unchanged", rr->resrec.name.c);
		return mDNSfalse;
		}
	AssignDomainName(*target, m->uDNS_info.hostname);
	SetNewRData(&rr->resrec, NULL, 0);
	return mDNStrue;
	}

mDNSlocal void SendServiceRegistration(mDNS *m, ServiceRecordSet *srs)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	mDNSOpaque16 id;
	uDNS_AuthInfo *authInfo;
	uDNS_RegInfo *rInfo = &srs->uDNS_info;
	mStatus err = mStatus_UnknownErr;
	
	id = newMessageID(u);
	InitializeDNSMessage(&msg.h, id, UpdateReqFlags);
	rInfo->id.NotAnInteger = id.NotAnInteger;

	// setup resource records
	if (setHostTarget(&srs->RR_SRV, m))
		SetNewRData(&srs->RR_SRV.resrec, NULL, 0);  // set rdlen/estimate/hash
	
	//SetNewRData(&srs->RR_ADV.resrec, NULL, 0); //!!!KRS
	SetNewRData(&srs->RR_PTR.resrec, NULL, 0);	
	SetNewRData(&srs->RR_TXT.resrec, NULL, 0);
	
	// construct update packet
    // set zone
	ptr = putZone(&msg, ptr, end, &rInfo->zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) goto error;

	if (srs->uDNS_info.state == regState_Refresh)
		{
		// prereq: record must exist (put record in prereq section w/ TTL 0)
		ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0);
		if (!ptr) goto error;
		}
	else
		{
		// use SRV for prereq
		ptr = putPrereqNameNotInUse(&srs->RR_SRV.resrec.name, &msg, ptr, end);
		if (!ptr) goto error;
		}

	//!!!KRS  Need to do bounds checking and use TCP if it won't fit!!!
	//if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_ADV.resrec))) goto error;
	if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_PTR.resrec))) goto error;
	if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_SRV.resrec))) goto error;
	if (!(ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &srs->RR_TXT.resrec))) goto error;
    // !!!KRS do subtypes/extras etc.

	if (srs->uDNS_info.lease)
		ptr = putUpdateLease(&msg, ptr);
	   
	srs->uDNS_info.expire = -1;

	authInfo = GetAuthInfoForZone(u, &rInfo->zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &rInfo->ns, rInfo->port, authInfo);
		if (err) { LogMsg("ERROR: SendServiceRegistration - mDNSSendSignedDNSMessage - %d", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &rInfo->ns, rInfo->port);
		if (err) { LogMsg("ERROR: SendServiceRegistration - mDNSSendDNSMessage - %d", err); goto error; }
		}
	if (rInfo->state != regState_Refresh)
		rInfo->state = regState_Pending;
	return;

error:
	unlinkSRS(u, srs);
	rInfo->state = regState_Unregistered;
	srs->ServiceCallback(m, srs, err);
	//!!!KRS will mem still be free'd on error?
	// NOTE: not safe to touch any client structures here	
	}

mDNSlocal void serviceRegistrationCallback(mStatus err, mDNS *const m, void *srsPtr, const AsyncOpResult *result)
	{
	ServiceRecordSet *srs = (ServiceRecordSet *)srsPtr;
	const zoneData_t *zoneData = &result->zoneData;
	uDNS_GlobalInfo *u = &m->uDNS_info;
	
	if (err) goto error;
	if (result->type != zoneDataResult)
		{
		LogMsg("ERROR: buildUpdatePacket passed incorrect result type %d", result->type);
		goto error;
		}

	if (srs->uDNS_info.state == regState_Cancelled)
		{
		// client cancelled registration while fetching zone data
		srs->uDNS_info.state = regState_Unregistered;
		unlinkSRS(u, srs);
		srs->ServiceCallback(m, srs, mStatus_MemFree);
		return;
		}

	if (srs->RR_SRV.resrec.rrclass != zoneData->zoneClass)
		{
		LogMsg("Service %s - class does not match zone", srs->RR_SRV.resrec.name.c);
		goto error;
		}
	// cache zone data
	ustrcpy(srs->uDNS_info.zone.c, zoneData->zoneName.c);
    srs->uDNS_info.ns.type = mDNSAddrType_IPv4;
	srs->uDNS_info.ns.ip.v4.NotAnInteger = zoneData->primaryAddr.ip.v4.NotAnInteger;
	srs->uDNS_info.port.NotAnInteger = zoneData->updatePort.NotAnInteger;

	SendServiceRegistration(m, srs);
	return;
		
error:
	unlinkSRS(u, srs);
	srs->uDNS_info.state = regState_Unregistered;
	srs->ServiceCallback(m, srs, err);
	//!!!KRS will mem still be free'd on error?
	// NOTE: not safe to touch any client structures here
	}

mDNSexport void uDNS_UpdateServiceTargets(mDNS *const m)
	{
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	uDNS_GlobalInfo *u = &m->uDNS_info;
	ServiceRecordSet *srs;
	AuthRecord *rr;
	mStatus err = mStatus_NoError;
	
	if (!m->uDNS_info.hostname.c[0])
		{
		LogMsg("ERROR: uDNS_UpdateServiceTargets called before registration of hostname");
		return;
		//!!!KRS need to handle this case properly!
		}
	
	for (srs = u->ServiceRegistrations; srs; srs = srs->next)
		{
		if (err) srs = u->ServiceRegistrations;
   		    // start again from beginning of list, since it may have changed
		    // (setHostTarget() will skip records already updated)
		rr = &srs->RR_SRV;
		if (srs->uDNS_info.state != regState_Registered)
			{
			LogMsg("ERROR: uDNS_UpdateServiceTargets - service %s not registered", rr->resrec.name.c);
			continue;
			//!!!KRS need to handle this
			}
		InitializeDNSMessage(&msg.h, srs->uDNS_info.id, UpdateReqFlags);
		
		// construct update packet
		ptr = putZone(&msg, ptr, end, &srs->uDNS_info.zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
		if (ptr) ptr = putDeletionRecord(&msg, ptr, &rr->resrec);  // delete the old target
		// update the target
		if (!setHostTarget(rr, m)) continue;
		if (ptr) ptr = PutResourceRecord(&msg, ptr, &msg.h.mDNS_numUpdates, &rr->resrec);  // put the new target
		// !!!KRS do subtypes/extras etc.
		if (!ptr) err = mStatus_UnknownErr;
		else err = mDNSSendDNSMessage(m, &msg, ptr, 0, &srs->uDNS_info.ns, srs->uDNS_info.port);
		if (err) 			
			{
			LogMsg("ERROR: uDNS_UpdateServiceTargets - %s", ptr ? "mDNSSendDNSMessage" : "message formatting error");
			unlinkSRS(u, srs);
			srs->uDNS_info.state = regState_Unregistered;
			srs->ServiceCallback(m, srs, err);
			//!!!KRS will mem still be free'd on error?
			// NOTE: not safe to touch any client structures here		
			}
		else srs->uDNS_info.state = regState_TargetChange;
	   }
}			


mDNSexport mStatus uDNS_RegisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	domainname *target = GetRRDomainNameTarget(&rr->resrec);
	
	if (rr->uDNS_info.state == regState_FetchingZoneData ||
		rr->uDNS_info.state == regState_Pending ||
		rr->uDNS_info.state ==  regState_Registered)
		{
		LogMsg("Requested double-registration of physical record %s type %s",
			   rr->resrec.name.c, rr->resrec.rrtype);
		return mStatus_AlreadyRegistered;
		}
	
	rr->resrec.rdlength   = GetRDLength(&rr->resrec, mDNSfalse);
	rr->resrec.rdestimate = GetRDLength(&rr->resrec, mDNStrue);

	if (!ValidateDomainName(&rr->resrec.name))
		{
		LogMsg("Attempt to register record with invalid name: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	// Don't do this until *after* we've set rr->resrec.rdlength
	if (!ValidateRData(rr->resrec.rrtype, rr->resrec.rdlength, rr->resrec.rdata))
		{ LogMsg("Attempt to register record with invalid rdata: %s", GetRRDisplayString(m, rr));
		return mStatus_Invalid;
		}

	rr->resrec.namehash   = DomainNameHashValue(&rr->resrec.name);
	rr->resrec.rdatahash  = RDataHashValue(rr->resrec.rdlength, &rr->resrec.rdata->u);
	rr->resrec.rdnamehash = target ? DomainNameHashValue(target) : 0;

	rr->uDNS_info.state = regState_FetchingZoneData;
	rr->next = m->uDNS_info.RecordRegistrations;
	m->uDNS_info.RecordRegistrations = rr;

	rr->uDNS_info.lease = mDNStrue;
	return startGetZoneData(&rr->resrec.name, m, mDNStrue, mDNSfalse, RecordRegistrationCallback, rr);
	}



mDNSexport mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mStatus err;
	uDNS_AuthInfo *authInfo;
	switch (rr->uDNS_info.state)
		{
		case regState_FetchingZoneData:
			rr->uDNS_info.state = regState_Cancelled;
			return mStatus_NoError;
		case regState_Pending:
			rr->uDNS_info.state = regState_DeregDeferred;
			debugf("Deferring deregistration of record %s until registration completes", rr->resrec.name.c);
			return mStatus_NoError;
		case regState_Registered:			
			break;
		case regState_DeregPending:
		case regState_Cancelled:
			LogMsg("Double deregistration of record %s type %d",
				   rr->resrec.name.c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		case regState_Unregistered:
			LogMsg("Requested deregistration of unregistered record %s type %d",
				   rr->resrec.name.c, rr->resrec.rrtype);
			return mStatus_UnknownErr;
		default:
			LogMsg("ERROR: uDNS_DeregisterRecord called for record %s type %d with unknown state %d", 
				   rr->resrec.name.c, rr->resrec.rrtype, rr->uDNS_info.state);
			return mStatus_UnknownErr;
		}
		
	InitializeDNSMessage(&msg.h, rr->uDNS_info.id, UpdateReqFlags);

	// put zone
	ptr = putZone(&msg, ptr, end, &rr->uDNS_info.zone, mDNSOpaque16fromIntVal(rr->resrec.rrclass));
	if (!ptr) goto error;

	if (!(ptr = putDeletionRecord(&msg, ptr, &rr->resrec))) goto error;

	authInfo = GetAuthInfoForZone(u, &rr->uDNS_info.zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &rr->uDNS_info.ns, rr->uDNS_info.port, authInfo);
		if (err) { LogMsg("ERROR: uDNS_DeregiserRecord - mDNSSendSignedDNSMessage - %d", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &rr->uDNS_info.ns, rr->uDNS_info.port);
		if (err) { LogMsg("ERROR: uDNS_DeregisterRecord - mDNSSendDNSMessage - %d", err); goto error; }
		}
	
	return mStatus_NoError;

	error:
	if (rr->uDNS_info.state != regState_Unregistered)
		{
		unlinkAR(&u->RecordRegistrations, rr);
		rr->uDNS_info.state = regState_Unregistered;
		}
	return mStatus_UnknownErr;
	}


mDNSexport mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	if (!*m->uDNS_info.NameRegDomain)
		{
		LogMsg("ERROR: uDNS_RegisterService - cannot register unicast service "
			   "without setting the NameRegDomain via mDNSResponder.conf");
		srs->uDNS_info.state = regState_Unregistered;
		return mStatus_UnknownErr;
		}
	
	srs->RR_SRV.resrec.rroriginalttl = 3;
	srs->RR_TXT.resrec.rroriginalttl = 3;
	srs->RR_PTR.resrec.rroriginalttl = 3;
	
	// set state and link into list
	srs->uDNS_info.state = regState_FetchingZoneData;
	srs->next = m->uDNS_info.ServiceRegistrations;
	m->uDNS_info.ServiceRegistrations = srs;
	srs->uDNS_info.lease = mDNStrue;
	
	return startGetZoneData(&srs->RR_SRV.resrec.name, m, mDNStrue, mDNSfalse, serviceRegistrationCallback, srs);
	}

mDNSexport mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs)
	{
	uDNS_GlobalInfo *u = &m->uDNS_info;
	DNSMessage msg;
	mDNSu8 *ptr = msg.data;
	mDNSu8 *end = (mDNSu8 *)&msg + sizeof(DNSMessage);
	mStatus err = mStatus_UnknownErr;
	uDNS_AuthInfo *authInfo;
	
	//!!!KRS make sure we're doing the right thing w/ memfree
	
	switch (srs->uDNS_info.state)
		{
		case regState_Unregistered:
			LogMsg("ERROR: uDNS_DeregisterService - service not registerd");
			return mStatus_UnknownErr;
		case regState_FetchingZoneData:
		case regState_Pending:
			// let the async op complete, then terminate
			srs->uDNS_info.state = regState_Cancelled;
			return mStatus_NoError;  // deliver memfree upon completion of async op
		case regState_DeregPending:
		case regState_DeregDeferred:
		case regState_Cancelled:
			LogMsg("uDNS_DeregisterService - deregistration in process");
			return mStatus_UnknownErr;
		}

	srs->uDNS_info.state = regState_DeregPending;
	InitializeDNSMessage(&msg.h, srs->uDNS_info.id, UpdateReqFlags);

    // put zone
	ptr = putZone(&msg, ptr, end, &srs->uDNS_info.zone, mDNSOpaque16fromIntVal(srs->RR_SRV.resrec.rrclass));
	if (!ptr) { LogMsg("ERROR: uDNS_DeregisterService - putZone"); goto error; }
	
    // prereq: record must exist (put record in prereq section w/ TTL 0)
	ptr = PutResourceRecordTTL(&msg, ptr, &msg.h.mDNS_numPrereqs, &srs->RR_SRV.resrec, 0);
	if (!ptr) { LogMsg("ERROR: uDNS_DeregisterService - PutResourceRecordTTL"); goto error; }

	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_SRV.resrec))) goto error;
	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_TXT.resrec))) goto error;
	if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_PTR.resrec))) goto error;
	//if (!(ptr = putDeletionRecord(&msg, ptr, &srs->RR_ADV.resrec))) goto error;
	//!!!KRS  need to handle extras/subtypes etc


	authInfo = GetAuthInfoForZone(u, &srs->uDNS_info.zone);
	if (authInfo)
		{
		err = mDNSSendSignedDNSMessage(m, &msg, ptr, 0, &srs->uDNS_info.ns, srs->uDNS_info.port, authInfo);
		if (err) { LogMsg("ERROR: uDNS_DeregiserService - mDNSSendSignedDNSMessage - %d", err); goto error; }
		}
	else
		{
		err = mDNSSendDNSMessage(m, &msg, ptr, 0, &srs->uDNS_info.ns, srs->uDNS_info.port);
		if (err) { LogMsg("ERROR: uDNS_DeregisterService - mDNSSendDNSMessage - %d", err); goto error; }
		}
	
	return mStatus_NoError;

	error:
	unlinkSRS(u, srs);
	srs->uDNS_info.state = regState_Unregistered;
	return err;
	}

mDNSexport void uDNS_Execute(mDNS *const m)
	{
	DNSQuestion *q;
	DNSMessage msg;
	mStatus err;
	mDNSu8 *end;
	mDNSs32 sendtime;
	LLQ_Info *llq;
	AuthRecord *rr;
	ServiceRecordSet *srs;
	uDNS_RegInfo *rInfo;	
	uDNS_GlobalInfo *u = &m->uDNS_info;
	const mDNSAddr *server = getInitializedDNS(&m->uDNS_info);
	mDNSs32 timenow = mDNSPlatformTimeNow();
	
	u->nextevent = timenow + 0x78000000;
	if (!server) { debugf("uDNS_Execute - no DNS server"); return; }	
	
	for (q = u->ActiveQueries; q; q = q->next)
		{
		llq = q->uDNS_info.llq;
		if (q->LongLived && llq->state != LLQ_Poll)
			{
			if (llq->state >= LLQ_InitialRequest && llq->state <= LLQ_Suspended && llq->retry <= timenow)				
				{

				// sanity check to avoid packet flood bugs
				if (!llq->retry) 
					LogMsg("ERROR: retry timer not set for LLQ %s in state %d", q->qname.c, llq->state);
				else if (llq->state == LLQ_Established || llq->state == LLQ_Refresh)
					sendLLQRefresh(m, q, llq->origLease);
				else if (llq->state == LLQ_InitialRequest) 
					startLLQHandshake(m, llq);
				else if (llq->state == LLQ_SecondaryRequest)
					sendChallengeResponse(m, q, NULL);
				else if (llq->state == LLQ_Retry) 
					{ llq->ntries = 0; startLLQHandshake(m, llq); }			
				}
			}
		else
			{
			sendtime = q->LastQTime + q->ThisQInterval;
			if (sendtime <= timenow)
				{
				err = constructQueryMsg(&msg, &end, q);
				if (err)
					{
					LogMsg("Error: uDNS_Idle - constructQueryMsg.  Skipping question %s",
						   q->qname.c);
					continue;
					}
				err = mDNSSendDNSMessage(m, &msg, end, q->InterfaceID, server, UnicastDNSPort);
				if (err) { debugf("ERROR: uDNS_idle - mDNSSendDNSMessage - %d", err); } // surpress syslog messages if we have no network
				q->LastQTime = timenow;
				if (q->ThisQInterval < MAX_UCAST_POLL_INTERVAL) q->ThisQInterval = q->ThisQInterval * 2;
				}
			else if (u->nextevent - sendtime > 0) u->nextevent = sendtime;
			}
		}

	//!!!KRS list should be pre-sorted by expiration
	for (rr = u->RecordRegistrations; rr; rr = rr->next)
		{
		rInfo = &rr->uDNS_info;		
		if (rInfo->lease && rInfo->state == regState_Registered && rInfo->expire > 0)
		    {		    
		    if (rInfo->expire < timenow)
		        {
		        debugf("refreshing record %s", rr->resrec.name.c);
		        rInfo->state = regState_Refresh;
		        sendRecordRegistration(m, rr);
		        }
		    else if (u->nextevent - rInfo->expire > 0) u->nextevent = rInfo->expire;
		    }
		}
	//!!!KRS list should be pre-sorted by expiration
	for (srs = u->ServiceRegistrations; srs; srs = srs->next)
		{
		rInfo = &srs->uDNS_info;	
		if (rInfo->lease && rInfo->state == regState_Registered && rInfo->expire > 0)
		    {		    
		    if (rInfo->expire < timenow)
		        {	
			    debugf("refreshing service %s", srs->RR_SRV.resrec.name.c);
			    rInfo->state = regState_Refresh;
			    SendServiceRegistration(m, srs);
		        }
		    else if (u->nextevent - rInfo->expire > 0) u->nextevent = rInfo->expire;
		    }	
		}
	}

mDNSexport void uDNS_Init(mDNS *const m)
	{
	mDNSPlatformMemZero(&m->uDNS_info, sizeof(uDNS_GlobalInfo));
	m->uDNS_info.nextevent = mDNSPlatformTimeNow() + 0x78000000;
	}
