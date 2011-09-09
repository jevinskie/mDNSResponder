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

$Log: uDNS.h,v $
Revision 1.11  2004/06/17 01:13:11  ksekar
<rdar://problem/3696616>: polling interval too short

Revision 1.10  2004/06/11 05:45:03  ksekar
<rdar://problem/3682397>: Change SRV names for LLQ/Update port lookups

Revision 1.9  2004/06/01 23:46:50  ksekar
<rdar://problem/3675149>: DynDNS: dynamically look up LLQ/Update ports

Revision 1.8  2004/05/28 23:42:37  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.7  2004/05/18 23:51:25  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.6  2004/03/13 01:57:33  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.5  2004/02/21 08:56:58  bradley
Wrap prototypes with extern "C" for C++ builds.

Revision 1.4  2004/02/06 23:04:19  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.3  2004/01/24 03:38:27  cheshire
Fix minor syntactic error: Headers should use "extern" declarations, not "mDNSexport"

Revision 1.2  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.1  2003/12/13 03:05:27  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

 
 */

#ifndef __UDNS_H_
#define __UDNS_H_

#include "mDNSClientAPI.h"
#include "DNSCommon.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#define INIT_UCAST_POLL_INTERVAL (15 * mDNSPlatformOneSecond)
#define MAX_UCAST_POLL_INTERVAL (15 * 60 * mDNSPlatformOneSecond)
#define NO_GOODBYE                       // will we receive goodbye packets from the server?
#define UPDATE_PORT_NAME "_dns-update._udp."
#define LLQ_PORT_NAME "_dns-llq._udp"
	
// Entry points into unicast-specific routines

extern void uDNS_AdvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set);
extern void uDNS_DeadvertiseInterface(mDNS *const m, NetworkInterfaceInfo *set);

extern mStatus uDNS_StartQuery(mDNS *const m, DNSQuestion *const question);
extern mDNSBool IsActiveUnicastQuery(DNSQuestion *const question, uDNS_GlobalInfo *u);  // returns true if OK to call StopQuery
extern mStatus uDNS_StopQuery(mDNS *const m, DNSQuestion *const question);

// SuspendLLQs stops all LLQs, preserving known answers.  RestartLLQs re-starts these suspended LLQs, generating appropriate add/removes
// Call SuspendLLQs prior to sleep, and on shutdown.  Call RestartLLQs on wake from sleep.
extern void uDNS_SuspendLLQs(mDNS *m);
extern void uDNS_RestartLLQs(mDNS *m);
	
extern mStatus uDNS_RegisterRecord(mDNS *const m, AuthRecord *const rr);
extern mStatus uDNS_DeregisterRecord(mDNS *const m, AuthRecord *const rr);

extern mStatus uDNS_RegisterService(mDNS *const m, ServiceRecordSet *srs);
extern mStatus uDNS_DeregisterService(mDNS *const m, ServiceRecordSet *srs);
extern void uDNS_UpdateServiceTargets(mDNS *const m);  // call following namechange

// integer fields of msg header must be in HOST byte order before calling this routine
extern void uDNS_ReceiveMsg(mDNS *const m, DNSMessage *const msg, const mDNSu8 *const end,
const mDNSAddr *const srcaddr, const mDNSIPPort srcport, const mDNSAddr *const dstaddr, 
const mDNSIPPort dstport, const mDNSInterfaceID InterfaceID, mDNSu8 ttl);

// returns time of next scheduled event
extern void uDNS_Execute(mDNS *const m);

extern void uDNS_Init(mDNS *const m);
	
#ifdef	__cplusplus
	}
#endif

#endif // __UDNS_H_
