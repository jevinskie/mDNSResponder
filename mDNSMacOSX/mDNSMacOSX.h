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

$Log: mDNSMacOSX.h,v $
Revision 1.37  2004/06/04 08:58:30  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.36  2004/05/26 17:06:33  cheshire
<rdar://problem/3668515>: Don't rely on CFSocketInvalidate() to remove RunLoopSource

Revision 1.35  2004/05/18 23:51:26  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.34  2004/05/12 22:03:09  ksekar
Made GetSearchDomainList a true platform-layer call (declaration moved
from mDNSMacOSX.h to mDNSClientAPI.h), impelemted to return "local"
only on non-OSX platforms.  Changed call to return a copy of the list
to avoid shared memory issues.  Added a routine to free the list.

Revision 1.33  2004/05/12 02:03:25  ksekar
Non-local domains will only be browsed by default, and show up in
_browse domain enumeration, if they contain an _browse._dns-sd ptr record.

Revision 1.32  2004/04/21 02:20:47  cheshire
Rename interface field 'CurrentlyActive' to more descriptive 'Exists'

Revision 1.31  2004/04/09 17:40:26  cheshire
Remove unnecessary "Multicast" field -- it duplicates the semantics of the existing TxAndRx field

Revision 1.30  2004/01/28 02:30:08  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.29  2004/01/27 22:57:48  cheshire
<rdar://problem/3534352>: Need separate socket for issuing unicast queries

Revision 1.28  2004/01/27 20:15:23  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.27  2004/01/24 08:46:26  bradley
Added InterfaceID<->Index platform interfaces since they are now used by all platforms for the DNS-SD APIs.

Revision 1.26  2003/12/08 21:00:46  rpantos
Changes to support mDNSResponder on Linux.

Revision 1.25  2003/11/08 22:18:29  cheshire
<rdar://problem/3477870>: Don't need to show process ID in *every* mDNSResponder syslog message

Revision 1.24  2003/11/08 22:13:00  cheshire
Move extern declarations inside '#ifdef __cplusplus extern "C" {' section

Revision 1.23  2003/09/23 16:38:25  cheshire
When LogAllOperations is false, treat LogOperation() like debugf()
(i.e. show in debug builds), rather than unconditionally ignoring

Revision 1.22  2003/09/23 02:12:43  cheshire
Also include port number in list of services registered via new UDS API

Revision 1.21  2003/08/19 22:20:00  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured
More minor refinements

Revision 1.20  2003/08/19 05:39:43  cheshire
<rdar://problem/3380097> SIGINFO dump should include resolves started by DNSServiceQueryRecord

Revision 1.19  2003/08/19 05:36:45  cheshire
Add missing "extern" directives

Revision 1.18  2003/08/19 03:04:43  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured

Revision 1.17  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

Revision 1.16  2003/08/08 18:36:04  cheshire
<rdar://problem/3344154> Only need to revalidate on interface removal on platforms that have the PhantomInterfaces bug

Revision 1.15  2003/08/05 00:32:28  cheshire
<rdar://problem/3326712> Time to turn off MACOSX_MDNS_MALLOC_DEBUGGING

Revision 1.14  2003/07/20 03:38:51  ksekar
<rdar://problem/3320722> Completed support for Unix-domain socket based API.

Revision 1.13  2003/07/18 00:30:00  cheshire
<rdar://problem/3268878> Remove mDNSResponder version from packet header and use HINFO record instead

Revision 1.12  2003/07/12 03:15:20  cheshire
<rdar://problem/3324848> After SCDynamicStore notification, mDNSResponder updates
m->hostlabel even if user hasn't actually actually changed their dot-local hostname

Revision 1.11  2003/07/02 21:19:51  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.10  2003/06/25 23:42:19  ksekar
<rdar://problem/3249292>: Feature: New Rendezvous APIs (#7875)
Reviewed by: Stuart Cheshire
Added files necessary to implement Unix domain sockets based enhanced
Rendezvous APIs, and integrated with existing Mach-port based daemon.

Revision 1.9  2003/06/10 01:14:11  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call

Revision 1.8  2003/05/14 07:08:37  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.7  2003/04/26 02:39:24  cheshire
Remove extern void LogMsg(const char *format, ...);

Revision 1.6  2003/03/05 21:59:56  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder

Revision 1.5  2003/03/05 01:50:38  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder

Revision 1.4  2003/02/21 01:54:10  cheshire
<rdar://problem/3099194> mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.3  2002/09/21 20:44:51  zarzycki
Added APSL info

Revision 1.2  2002/09/19 04:20:44  cheshire
Remove high-ascii characters that confuse some systems

Revision 1.1  2002/09/17 01:04:09  cheshire
Defines mDNS_PlatformSupport_struct for OS X

*/

#ifndef __mDNSOSX_h
#define __mDNSOSX_h

#ifdef  __cplusplus
    extern "C" {
#endif

#include <SystemConfiguration/SystemConfiguration.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "mDNSClientAPI.h"  // for domain name structure
	

typedef struct NetworkInterfaceInfoOSX_struct NetworkInterfaceInfoOSX;

typedef struct
	{
	mDNS                    *m;
	NetworkInterfaceInfoOSX *info;
	int                      sktv4;
	CFSocketRef              cfsv4;
	CFRunLoopSourceRef       rlsv4;
	int                      sktv6;
	CFSocketRef	             cfsv6;
	CFRunLoopSourceRef       rlsv6;
	} CFSocketSet;

struct NetworkInterfaceInfoOSX_struct
	{
	NetworkInterfaceInfo     ifinfo;			// MUST be the first element in this structure
	NetworkInterfaceInfoOSX *next;
	mDNSu32                  Exists;			// 1 = currently exists in getifaddrs list; 0 = doesn't
												// 2 = exists, but McastTxRx state changed
	char                    *ifa_name;			// Memory for this is allocated using malloc
	mDNSu32                  scope_id;			// interface index / IPv6 scope ID
	u_short                  sa_family;
	mDNSBool                 Multicast;
	CFSocketSet              ss;
	};

struct mDNS_PlatformSupport_struct
    {
    NetworkInterfaceInfoOSX *InterfaceList;
    CFSocketSet              unicastsockets;
    domainlabel              userhostlabel;
    SCDynamicStoreRef        Store;
    CFRunLoopSourceRef       StoreRLS;
    io_connect_t             PowerConnection;
    io_object_t              PowerNotifier;
    CFRunLoopSourceRef       PowerRLS;
    };

extern mDNSBool mDNSMacOSXSystemBuildNumber(char *HINFO_SWstring);

extern const char mDNSResponderVersionString[];

#ifdef  __cplusplus
    }
#endif

#endif
