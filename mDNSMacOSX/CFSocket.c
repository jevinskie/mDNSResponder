/*
 * Copright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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

$Log: CFSocket.c,v $
Revision 1.157  2004/06/08 18:54:48  ksekar
<rdar://problem/3681378>: mDNSResponder leaks after exploring in Printer Setup Utility

Revision 1.156  2004/06/05 00:04:26  cheshire
<rdar://problem/3668639>: wide-area domains should be returned in reg. domain enumeration

Revision 1.155  2004/06/04 08:58:30  ksekar
<rdar://problem/3668624>: Keychain integration for secure dynamic update

Revision 1.154  2004/05/31 22:22:28  ksekar
<rdar://problem/3668639>: wide-area domains should be returned in
reg. domain enumeration

Revision 1.153  2004/05/26 17:06:33  cheshire
<rdar://problem/3668515>: Don't rely on CFSocketInvalidate() to remove RunLoopSource

Revision 1.152  2004/05/18 23:51:26  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.151  2004/05/17 21:46:34  cheshire
<rdar://problem/3616426>: When interface is turned off, browse "remove" events are delivered with interface index zero
Take care to correctly update InterfaceIDs when a dormant interface comes back to life

Revision 1.150  2004/05/13 04:54:20  ksekar
Unified list copy/free code.  Added symetric list for

Revision 1.149  2004/05/13 03:55:14  ksekar
Fixed list traversal bug in FoundDefSearchDomain.

Revision 1.148  2004/05/12 22:03:08  ksekar
Made GetSearchDomainList a true platform-layer call (declaration moved
from mDNSMacOSX.h to mDNSClientAPI.h), impelemted to return "local"
only on non-OSX platforms.  Changed call to return a copy of the list
to avoid shared memory issues.  Added a routine to free the list.

Revision 1.147  2004/05/12 02:03:25  ksekar
Non-local domains will only be browsed by default, and show up in
_browse domain enumeration, if they contain an _browse._dns-sd ptr record.

Revision 1.146  2004/04/27 02:49:15  cheshire
<rdar://problem/3634655>: mDNSResponder leaks sockets on bind() error

Revision 1.145  2004/04/21 03:08:03  cheshire
Rename 'alias' to more descriptive name 'primary'

Revision 1.144  2004/04/21 03:04:35  cheshire
Minor cleanup for clarity

Revision 1.143  2004/04/21 03:03:30  cheshire
Preparation work: AddInterfaceToList() should return pointer to structure it creates

Revision 1.142  2004/04/21 02:49:11  cheshire
To reduce future confusion, renamed 'TxAndRx' to 'McastTxRx'

Revision 1.141  2004/04/21 02:20:47  cheshire
Rename interface field 'CurrentlyActive' to more descriptive 'Exists'

Revision 1.140  2004/04/14 23:09:29  ksekar
Support for TSIG signed dynamic updates.

Revision 1.139  2004/04/09 17:40:26  cheshire
Remove unnecessary "Multicast" field -- it duplicates the semantics of the existing McastTxRx field

Revision 1.138  2004/04/09 16:37:16  cheshire
Suggestion from Bob Bradley:
Move NumCacheRecordsForInterfaceID() to DNSCommon.c so it's available to all platform layers

Revision 1.137  2004/04/08 00:59:55  cheshire
<rdar://problem/3609972> When interface turned off, browse "remove" events delivered with interface index zero
Unify use of the InterfaceID field, and make code that walks the list respect the 'Exists' flag

Revision 1.136  2004/04/07 01:08:57  cheshire
<rdar://problem/3609972> When interface turned off, browse "remove" events delivered with interface index zero

Revision 1.135  2004/03/19 01:01:03  ksekar
Fixed config file parsing to chop newline

Revision 1.134  2004/03/13 01:57:34  ksekar
<rdar://problem/3192546>: DynDNS: Dynamic update of service records

Revision 1.133  2004/02/02 22:46:56  cheshire
Move "CFRelease(dict);" inside the "if (dict)" check

Revision 1.132  2004/01/28 02:30:08  ksekar
Added default Search Domains to unicast browsing, controlled via
Networking sharing prefs pane.  Stopped sending unicast messages on
every interface.  Fixed unicast resolving via mach-port API.

Revision 1.131  2004/01/27 22:57:48  cheshire
<rdar://problem/3534352>: Need separate socket for issuing unicast queries

Revision 1.130  2004/01/27 22:28:40  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53
Additional lingering port 53 code deleted

Revision 1.129  2004/01/27 20:15:23  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.128  2004/01/24 23:58:17  cheshire
Change to use mDNSVal16() instead of shifting and ORing

Revision 1.127  2004/01/24 04:59:16  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.126  2004/01/23 23:23:15  ksekar
Added TCP support for truncated unicast messages.

Revision 1.125  2004/01/22 03:43:09  cheshire
Export constants like mDNSInterface_LocalOnly so that the client layers can use them

Revision 1.124  2004/01/21 21:53:19  cheshire
<rdar://problem/3448144>: Don't try to receive unicast responses if we're not the first to bind to the UDP port

Revision 1.123  2004/01/20 03:18:25  cheshire
Removed "LogMsg("Hey There!");" that evidently got checked in my mistake

Revision 1.122  2003/12/17 20:43:59  cheshire
<rdar://problem/3496728>: Syslog messages saying "sendto failed"

Revision 1.121  2003/12/13 03:05:28  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

Revision 1.120  2003/12/08 21:00:46  rpantos
Changes to support mDNSResponder on Linux.

Revision 1.119  2003/12/03 02:35:15  cheshire
Also report value of m->timenow when logging sendto() failure

Revision 1.118  2003/11/14 20:59:09  cheshire
Clients can't use AssignDomainName macro because mDNSPlatformMemCopy is defined in mDNSPlatformFunctions.h.
Best solution is just to combine mDNSClientAPI.h and mDNSPlatformFunctions.h into a single file.

Revision 1.117  2003/11/08 22:18:29  cheshire
<rdar://problem/3477870>: Don't need to show process ID in *every* mDNSResponder syslog message

Revision 1.116  2003/09/23 16:39:49  cheshire
When LogAllOperations is set, also report registration and deregistration of interfaces

Revision 1.115  2003/09/10 00:45:55  cheshire
<rdar://problem/3412328> Don't log "sendto failed" errors during the first two minutes of startup

Revision 1.114  2003/08/27 02:55:13  cheshire
<rdar://problem/3387910>: Bug: Don't report mDNSPlatformSendUDP sendto errno 64 (Host is down)

Revision 1.113  2003/08/19 22:20:00  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured
More minor refinements

Revision 1.112  2003/08/19 03:04:43  cheshire
<rdar://problem/3376721> Don't use IPv6 on interfaces that have a routable IPv4 address configured

Revision 1.111  2003/08/18 22:53:37  cheshire
<rdar://problem/3382647> mDNSResponder divide by zero in mDNSPlatformTimeNow()

Revision 1.110  2003/08/16 03:39:00  cheshire
<rdar://problem/3338440> InterfaceID -1 indicates "local only"

Revision 1.109  2003/08/15 02:19:49  cheshire
<rdar://problem/3375225> syslog messages: myCFSocketCallBack recvfrom skt 6 error -1 errno 35
Also limit number of messages to at most 100

Revision 1.108  2003/08/12 22:24:52  cheshire
<rdar://problem/3375225> syslog messages: myCFSocketCallBack recvfrom skt 6 error -1 errno 35
This message indicates a kernel bug, but still we don't want to flood syslog.
Do a sleep(1) after writing this log message, to limit the rate.

Revision 1.107  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

Revision 1.106  2003/08/12 13:48:32  cheshire
Add comment explaining clockdivisor calculation

Revision 1.105  2003/08/12 13:44:14  cheshire
<rdar://problem/3370229> mDNSResponder *VERY* unhappy if time goes backwards
Use mach_absolute_time() (which is guaranteed to always go forwards, resetting only on reboot)
instead of gettimeofday() (which can jump back if the user manually changes their time/date)

Revision 1.104  2003/08/12 13:12:07  cheshire
Textual search/replace: Indicate local functions using "mDNSlocal" instead of "static"

Revision 1.103  2003/08/08 18:36:04  cheshire
<rdar://problem/3344154> Only need to revalidate on interface removal on platforms that have the PhantomInterfaces bug

Revision 1.102  2003/08/06 00:14:52  cheshire
<rdar://problem/3330324> Need to check IP TTL on responses
Also add corresponding checks in the IPv6 code path

Revision 1.101  2003/08/05 22:20:16  cheshire
<rdar://problem/3330324> Need to check IP TTL on responses

Revision 1.100  2003/08/05 21:18:50  cheshire
<rdar://problem/3363185> mDNSResponder should ignore 6to4
Only use interfaces that are marked as multicast-capable (IFF_MULTICAST)

Revision 1.99  2003/08/05 20:13:52  cheshire
<rdar://problem/3294080> mDNSResponder using IPv6 interfaces before they are ready
Ignore interfaces with the IN6_IFF_NOTREADY flag set

Revision 1.98  2003/07/20 03:38:51  ksekar
<rdar://problem/3320722>
Completed support for Unix-domain socket based API.

Revision 1.97  2003/07/19 03:15:16  cheshire
Add generic MemAllocate/MemFree prototypes to mDNSPlatformFunctions.h,
and add the obvious trivial implementations to each platform support layer

Revision 1.96  2003/07/18 00:30:00  cheshire
<rdar://problem/3268878> Remove mDNSResponder version from packet header and use HINFO record instead

Revision 1.95  2003/07/12 03:15:20  cheshire
<rdar://problem/3324848> After SCDynamicStore notification, mDNSResponder updates
m->hostlabel even if user hasn't actually actually changed their dot-local hostname

Revision 1.94  2003/07/03 00:51:54  cheshire
<rdar://problem/3287213> When select() and recvmgs() disagree, get more info from kernel about the socket state

Revision 1.93  2003/07/03 00:09:14  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call
Additional refinement suggested by Josh: Use info->scope_id instead of if_nametoindex(info->ifa_name);

Revision 1.92  2003/07/02 21:19:51  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.91  2003/06/24 01:53:51  cheshire
Minor update to comments

Revision 1.90  2003/06/24 01:51:47  cheshire
<rdar://problem/3303118> Oops: Double-dispose of sockets
Don't need to close sockets: CFSocketInvalidate() does that for us

Revision 1.89  2003/06/21 18:12:47  cheshire
<rdar://problem/3296061> Rendezvous cannot handle interfaces whose total name is >3 chars
One-line change: should say "IF_NAMESIZE", not sizeof(ifname)

Revision 1.88  2003/06/12 23:38:37  cheshire
<rdar://problem/3291162> mDNSResponder doesn't detect some configuration changes
Also check that scope_id matches before concluding that two interfaces are the same

Revision 1.87  2003/06/10 01:14:11  cheshire
<rdar://problem/3286004> New APIs require a mDNSPlatformInterfaceIDfromInterfaceIndex() call

Revision 1.86  2003/05/28 02:41:52  cheshire
<rdar://problem/3034346> Time to remove Mac OS 9 UDP Port 53 legacy support

Revision 1.85  2003/05/28 02:39:47  cheshire
Minor change to debugging messages

Revision 1.84  2003/05/27 22:29:40  cheshire
Remove out-dated comment

Revision 1.83  2003/05/26 03:21:29  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.82  2003/05/26 03:01:27  cheshire
<rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead

Revision 1.81  2003/05/24 02:06:42  cheshire
<rdar://problem/3268480> IPv6 Multicast Loopback doesn't work
Tried setting IPV6_MULTICAST_LOOP; it doesn't help.
However, it is probably wise to have the code explicitly set this socket
option anyway, in case the default changes in later versions of Unix.

Revision 1.80  2003/05/24 02:02:24  cheshire
<rdar://problem/3221880> if_indextoname consumes a lot of CPU
Fix error in myIfIndexToName; was returning prematurely

Revision 1.79  2003/05/23 23:07:44  cheshire
<rdar://problem/3268199> Must not write to stderr when running as daemon

Revision 1.78  2003/05/23 01:19:04  cheshire
<rdar://problem/3267085> mDNSResponder needs to signal type of service to AirPort
Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate

Revision 1.77  2003/05/23 01:12:05  cheshire
Minor code tidying

Revision 1.76  2003/05/22 01:26:01  cheshire
Tidy up log messages

Revision 1.75  2003/05/22 00:07:09  cheshire
<rdar://problem/3264366> myCFSocketCallBack recvfrom(5) error 1, errno 35
Extra logging to determine whether there is a bug in CFSocket

Revision 1.74  2003/05/21 20:20:12  cheshire
Fix warnings (mainly printf format string warnings, like using "%d" where
it should say "%lu", etc.) and improve error logging (use strerror()
to include textual error message as well as numeric error in log messages).

Revision 1.73  2003/05/21 17:56:29  ksekar
<rdar://problem/3191277>: mDNSResponder doesn't watch for IPv6 address changes

Revision 1.72  2003/05/14 18:48:41  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
More minor refinements:
CFSocket.c needs to do *all* its mDNS_DeregisterInterface calls before freeing memory
mDNS_DeregisterInterface revalidates cache record when *any* representative of an interface goes away

Revision 1.71  2003/05/14 07:08:37  cheshire
<rdar://problem/3159272> mDNSResponder should be smarter about reconfigurations
Previously, when there was any network configuration change, mDNSResponder
would tear down the entire list of active interfaces and start again.
That was very disruptive, and caused the entire cache to be flushed,
and caused lots of extra network traffic. Now it only removes interfaces
that have really gone, and only adds new ones that weren't there before.

Revision 1.70  2003/05/07 18:30:24  cheshire
Fix signed/unsigned comparison warning

Revision 1.69  2003/05/06 20:14:44  cheshire
Change "tp" to "tv"

Revision 1.68  2003/05/06 00:00:49  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.67  2003/04/29 00:43:44  cheshire
Fix compiler warnings

Revision 1.66  2003/04/26 02:41:58  cheshire
<rdar://problem/3241281> Change timenow from a local variable to a structure member

Revision 1.65  2003/04/26 02:34:01  cheshire
Add missing mDNSexport

Revision 1.64  2003/04/15 16:48:06  jgraessl
<rdar://problem/3228833>
Modified code in CFSocket notifier function to read all packets on the socket
instead of reading only one packet every time the notifier was called.

Revision 1.63  2003/04/15 16:33:50  jgraessl
<rdar://problem/3221880>
Switched to our own copy of if_indextoname to improve performance.

Revision 1.62  2003/03/28 01:55:44  cheshire
Minor improvements to debugging messages

Revision 1.61  2003/03/27 03:30:56  cheshire
<rdar://problem/3210018> Name conflicts not handled properly, resulting in memory corruption, and eventual crash
Problem was that HostNameCallback() was calling mDNS_DeregisterInterface(), which is not safe in a callback
Fixes:
1. Make mDNS_DeregisterInterface() safe to call from a callback
2. Make HostNameCallback() use mDNS_DeadvertiseInterface() instead
   (it never really needed to deregister the interface at all)

Revision 1.60  2003/03/15 04:40:38  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.59  2003/03/11 01:23:26  cheshire
<rdar://problem/3194246> mDNSResponder socket problems

Revision 1.58  2003/03/06 01:43:04  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder
Improve "LIST_ALL_INTERFACES" output

Revision 1.57  2003/03/05 22:36:27  cheshire
<rdar://problem/3186338> Loopback doesn't work with mDNSResponder-27
Temporary workaround: Skip loopback interface *only* if we found at least one v4 interface to use

Revision 1.56  2003/03/05 01:50:38  cheshire
<rdar://problem/3189097> Additional debugging code in mDNSResponder

Revision 1.55  2003/02/21 01:54:09  cheshire
<rdar://problem/3099194> mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.54  2003/02/20 06:48:35  cheshire
<rdar://problem/3169535> Xserve RAID needs to do interface-specific registrations
Reviewed by: Josh Graessley, Bob Bradley

Revision 1.53  2003/01/29 02:21:23  cheshire
Return mStatus_Invalid if can't send packet because socket not available

Revision 1.52  2003/01/28 19:39:43  jgraessl
Enabling AAAA over IPv4 support.

Revision 1.51  2003/01/28 05:11:23  cheshire
Fixed backwards comparison in SearchForInterfaceByName

Revision 1.50  2003/01/13 23:49:44  jgraessl
Merged changes for the following fixes in to top of tree:
<rdar://problem/3086540>  computer name changes not handled properly
<rdar://problem/3124348>  service name changes are not properly handled
<rdar://problem/3124352>  announcements sent in pairs, failing chattiness test

Revision 1.49  2002/12/23 22:13:30  jgraessl
Reviewed by: Stuart Cheshire
Initial IPv6 support for mDNSResponder.

Revision 1.48  2002/11/22 01:37:52  cheshire
<rdar://problem/3108426> mDNSResponder is monitoring ServiceEntities instead of InterfaceEntities

Revision 1.47  2002/09/21 20:44:51  zarzycki
Added APSL info

Revision 1.46  2002/09/19 21:25:35  cheshire
mDNS_snprintf() doesn't need to be in a separate file

Revision 1.45  2002/09/17 01:45:13  cheshire
Add LIST_ALL_INTERFACES symbol for debugging

Revision 1.44  2002/09/17 01:36:23  cheshire
Move Puma support to CFSocketPuma.c

Revision 1.43  2002/09/17 01:05:28  cheshire
Change mDNS_AdvertiseLocalAddresses to be an Init parameter instead of a global

Revision 1.42  2002/09/16 23:13:50  cheshire
Minor code tidying

 */

// ***************************************************************************
// mDNS-CFSocket.c:
// Supporting routines to run mDNS on a CFRunLoop platform
// ***************************************************************************

// For debugging, set LIST_ALL_INTERFACES to 1 to display all found interfaces,
// including ones that mDNSResponder chooses not to use.
#define LIST_ALL_INTERFACES 0

// For enabling AAAA records over IPv4. Setting this to 0 sends only
// A records over IPv4 and AAAA over IPv6. Setting this to 1 sends both
// AAAA and A records over both IPv4 and IPv6.
#define AAAA_OVER_V4 1

#include "mDNSClientAPI.h"          // Defines the interface provided to the client layer above
#include "mDNSMacOSX.h"             // Defines the specific types needed to run mDNS on this platform

#include <stdio.h>
#include <unistd.h>                 // For select() and close()
#include <stdarg.h>                 // For va_list support
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>                   // platform support for UTC time
#include <arpa/inet.h>              // for inet_aton

#include <netinet/in.h>             // For IP_RECVTTL
#ifndef IP_RECVTTL
#define IP_RECVTTL 24               // bool; receive reception TTL w/dgram
#endif

#include <netinet/in_systm.h>       // For n_long, required by <netinet/ip.h> below
#include <netinet/ip.h>             // For IPTOS_LOWDELAY etc.
#include <netinet6/in6_var.h>       // For IN6_IFF_NOTREADY etc.

#include <Security/Security.h>

// Code contributed by Dave Heller:
// Define RUN_ON_PUMA_WITHOUT_IFADDRS to compile code that will
// work on Mac OS X 10.1, which does not have the getifaddrs call.
#define RUN_ON_PUMA_WITHOUT_IFADDRS 0
#if RUN_ON_PUMA_WITHOUT_IFADDRS
#include "CFSocketPuma.c"
#else
#include <ifaddrs.h>
#endif

#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <mach/mach_time.h>

typedef struct AuthRecordListElem
	{
    struct AuthRecordListElem *next;
    AuthRecord ar;
	} AuthRecordListElem;

typedef struct SearchListElem
	{
    struct SearchListElem *next;
    domainname domain;
    int flag;  
    DNSQuestion browseQ;
    DNSQuestion registerQ;
    AuthRecordListElem *AuthRecs;
	} SearchListElem;


// ***************************************************************************
// Globals

static mDNSu32 clockdivisor = 0;
static mDNSBool DNSConfigInitialized = mDNSfalse;
#define MAX_SEARCH_DOMAINS 32

// for domain enumeration and default browsing
static SearchListElem *SearchList = NULL;    // where we search for _browse domains
static DNSQuestion DefBrowseDomainQ;         // our local enumeration query for _browse domains
static DNameListElem *DefBrowseList = NULL;  // cache of answers to above query (where we search for empty string browses)

#define CONFIG_FILE "/etc/mDNSResponder.conf"
#define LH_KEYCHAIN_DESC "Lighthouse Shared Secret"
#define LH_KEYCHAIN_SERVICE "Lighthouse"
#define SYS_KEYCHAIN_PATH "/Library/Keychains/System.keychain"
#define LH_SUFFIX "members.mac.com."

// ***************************************************************************
// Macros

#define mDNSSameIPv4Address(A,B) ((A).NotAnInteger == (B).NotAnInteger)
#define mDNSSameIPv6Address(A,B) ((A).l[0] == (B).l[0] && (A).l[1] == (B).l[1] && (A).l[2] == (B).l[2] && (A).l[3] == (B).l[3])

#define mDNSAddressIsAllDNSLinkGroup(X) (                                                     \
	((X)->type == mDNSAddrType_IPv4 && mDNSSameIPv4Address((X)->ip.v4, AllDNSLinkGroup  )) || \
	((X)->type == mDNSAddrType_IPv6 && mDNSSameIPv6Address((X)->ip.v6, AllDNSLinkGroupv6))    )

// ***************************************************************************
// Functions

mDNSlocal struct ifaddrs* myGetIfAddrs(int refresh)
	{
	static struct ifaddrs *ifa = NULL;
	
	if (refresh && ifa)
		{
		freeifaddrs(ifa);
		ifa = NULL;
		}
	
	if (ifa == NULL) getifaddrs(&ifa);
	
	return ifa;
	}

mDNSlocal int myIfIndexToName(u_short index, char* name)
	{
	struct ifaddrs *ifa;
	for (ifa = myGetIfAddrs(0); ifa; ifa = ifa->ifa_next)
		if (ifa->ifa_addr->sa_family == AF_LINK)
			if (((struct sockaddr_dl*)ifa->ifa_addr)->sdl_index == index)
				{ strncpy(name, ifa->ifa_name, IF_NAMESIZE); return 0; }
	return -1;
	}

mDNSexport mDNSInterfaceID mDNSPlatformInterfaceIDfromInterfaceIndex(const mDNS *const m, mDNSu32 index)
	{
	NetworkInterfaceInfoOSX *i;
	if (index == (uint32_t)~0) return(mDNSInterface_LocalOnly);
	if (index)
		for (i = m->p->InterfaceList; i; i = i->next)
			// Don't get tricked by inactive interfaces with no InterfaceID set
			if (i->ifinfo.InterfaceID && i->scope_id == index) return(i->ifinfo.InterfaceID);
	return(mDNSNULL);
	}
	
mDNSexport mDNSu32 mDNSPlatformInterfaceIndexfromInterfaceID(const mDNS *const m, mDNSInterfaceID id)
	{
	NetworkInterfaceInfoOSX *i;
	if (id == mDNSInterface_LocalOnly) return((mDNSu32)~0);
	if (id)
		for (i = m->p->InterfaceList; i; i = i->next)
			// Don't use i->ifinfo.InterfaceID here, because we DO want to find inactive interfaces, which have no InterfaceID set
			if ((mDNSInterfaceID)i == id) return(i->scope_id);
	return 0;
	}

// NOTE: If InterfaceID is NULL, it means, "send this packet through our anonymous unicast socket"
// NOTE: If InterfaceID is non-NULL it means, "send this packet through our port 5353 socket on the specified interface"
mDNSexport mStatus mDNSPlatformSendUDP(const mDNS *const m, const DNSMessage *const msg, const mDNSu8 *const end,
	mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstPort)
	{
	#pragma unused(m)
	
	// Note: For this platform we've adopted the convention that InterfaceIDs are secretly pointers
	// to the NetworkInterfaceInfoOSX structure that holds the active sockets. The mDNSCore code
	// doesn't know that and doesn't need to know that -- it just treats InterfaceIDs as opaque identifiers.
	NetworkInterfaceInfoOSX *info = (NetworkInterfaceInfoOSX *)InterfaceID;
	char *ifa_name = info ? info->ifa_name : "unicast";
	struct sockaddr_storage to;
	int s = -1, err;

	if (dst->type == mDNSAddrType_IPv4)
		{
		struct sockaddr_in *sin_to = (struct sockaddr_in*)&to;
		sin_to->sin_len            = sizeof(*sin_to);
		sin_to->sin_family         = AF_INET;
		sin_to->sin_port           = dstPort.NotAnInteger;
		sin_to->sin_addr.s_addr    = dst->ip.v4.NotAnInteger;
		s = info ? info->ss.sktv4 : m->p->unicastsockets.sktv4;
		}
	else if (dst->type == mDNSAddrType_IPv6)
		{
		struct sockaddr_in6 *sin6_to = (struct sockaddr_in6*)&to;
		sin6_to->sin6_len            = sizeof(*sin6_to);
		sin6_to->sin6_family         = AF_INET6;
		sin6_to->sin6_port           = dstPort.NotAnInteger;
		sin6_to->sin6_flowinfo       = 0;
		sin6_to->sin6_addr           = *(struct in6_addr*)&dst->ip.v6;
		sin6_to->sin6_scope_id       = info ? info->scope_id : 0;
		s = info ? info->ss.sktv6 : m->p->unicastsockets.sktv6;
		}
	else
		{
		LogMsg("mDNSPlatformSendUDP: dst is not an IPv4 or IPv6 address!\n");
		return mStatus_BadParamErr;
		}

	if (s >= 0)
		verbosedebugf("mDNSPlatformSendUDP: sending on InterfaceID %X %s/%d to %#a:%d skt %d",
			InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort), s);
	else
		verbosedebugf("mDNSPlatformSendUDP: NOT sending on InterfaceID %X %s/%d (socket of this type not available)",
			InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort));

	// Note: When sending, mDNSCore may often ask us to send both a v4 multicast packet and then a v6 multicast packet
	// If we don't have the corresponding type of socket available, then return mStatus_Invalid
	if (s < 0) return(mStatus_Invalid);

	err = sendto(s, msg, (UInt8*)end - (UInt8*)msg, 0, (struct sockaddr *)&to, to.ss_len);
	if (err < 0)
		{
        // Don't report EHOSTDOWN (i.e. ARP failure) to unicast destinations
		if (errno == EHOSTDOWN && !mDNSAddressIsAllDNSLinkGroup(dst)) return(err);
		// Don't report EHOSTUNREACH in the first three minutes after boot
		// This is because mDNSResponder intentionally starts up early in the boot process (See <rdar://problem/3409090>)
		// but this means that sometimes it starts before configd has finished setting up the multicast routing entries.
		if (errno == EHOSTUNREACH && (mDNSu32)(m->timenow) < (mDNSu32)(mDNSPlatformOneSecond * 180)) return(err);
		LogMsg("mDNSPlatformSendUDP sendto failed to send packet on InterfaceID %p %s/%ld to %#a:%d skt %d error %d errno %d (%s) %lu",
			InterfaceID, ifa_name, dst->type, dst, mDNSVal16(dstPort), s, err, errno, strerror(errno), (mDNSu32)(m->timenow));
		return(err);
		}
	
	return(mStatus_NoError);
	}

mDNSlocal ssize_t myrecvfrom(const int s, void *const buffer, const size_t max,
	struct sockaddr *const from, size_t *const fromlen, mDNSAddr *dstaddr, char ifname[IF_NAMESIZE], mDNSu8 *ttl)
	{
	static unsigned int numLogMessages = 0;
	struct iovec databuffers = { (char *)buffer, max };
	struct msghdr   msg;
	ssize_t         n;
	struct cmsghdr *cmPtr;
	char            ancillary[1024];

	*ttl = 255;  // If kernel fails to provide TTL data (e.g. Jaguar doesn't) then assume the TTL was 255 as it should be

	// Set up the message
	msg.msg_name       = (caddr_t)from;
	msg.msg_namelen    = *fromlen;
	msg.msg_iov        = &databuffers;
	msg.msg_iovlen     = 1;
	msg.msg_control    = (caddr_t)&ancillary;
	msg.msg_controllen = sizeof(ancillary);
	msg.msg_flags      = 0;
	
	// Receive the data
	n = recvmsg(s, &msg, 0);
	if (n<0)
		{
		if (errno != EWOULDBLOCK && numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) returned error %d errno %d", s, n, errno);
		return(-1);
		}
	if (msg.msg_controllen < (int)sizeof(struct cmsghdr))
		{
		if (numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) msg.msg_controllen %d < sizeof(struct cmsghdr) %lu",
			s, msg.msg_controllen, sizeof(struct cmsghdr));
		return(-1);
		}
	if (msg.msg_flags & MSG_CTRUNC)
		{
		if (numLogMessages++ < 100) LogMsg("CFSocket.c: recvmsg(%d) msg.msg_flags & MSG_CTRUNC", s);
		return(-1);
		}
	
	*fromlen = msg.msg_namelen;
	
	// Parse each option out of the ancillary data.
	for (cmPtr = CMSG_FIRSTHDR(&msg); cmPtr; cmPtr = CMSG_NXTHDR(&msg, cmPtr))
		{
		// debugf("myrecvfrom cmsg_level %d cmsg_type %d", cmPtr->cmsg_level, cmPtr->cmsg_type);
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVDSTADDR)
			{
			dstaddr->type = mDNSAddrType_IPv4;
			dstaddr->ip.v4.NotAnInteger = *(u_int32_t*)CMSG_DATA(cmPtr);
			}
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVIF)
			{
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)CMSG_DATA(cmPtr);
			if (sdl->sdl_nlen < IF_NAMESIZE)
				{
				mDNSPlatformMemCopy(sdl->sdl_data, ifname, sdl->sdl_nlen);
				ifname[sdl->sdl_nlen] = 0;
				// debugf("IP_RECVIF sdl_index %d, sdl_data %s len %d", sdl->sdl_index, ifname, sdl->sdl_nlen);
				}
			}
		if (cmPtr->cmsg_level == IPPROTO_IP && cmPtr->cmsg_type == IP_RECVTTL)
			{
			*ttl = *(u_char*)CMSG_DATA(cmPtr);
			}
		if (cmPtr->cmsg_level == IPPROTO_IPV6 && cmPtr->cmsg_type == IPV6_PKTINFO)
			{
			struct in6_pktinfo *ip6_info = (struct in6_pktinfo*)CMSG_DATA(cmPtr);
			dstaddr->type = mDNSAddrType_IPv6;
			dstaddr->ip.v6 = *(mDNSv6Addr*)&ip6_info->ipi6_addr;
			myIfIndexToName(ip6_info->ipi6_ifindex, ifname);
			}
		if (cmPtr->cmsg_level == IPPROTO_IPV6 && cmPtr->cmsg_type == IPV6_HOPLIMIT)
			{
			*ttl = *(int*)CMSG_DATA(cmPtr);
			}
		}

	return(n);
	}

// On entry, context points to our CFSocketSet
// If ss->info is NULL, we received this packet on our anonymous unicast socket
// If ss->info is non-NULL, we received this packet on port 5353 on the indicated interface
mDNSlocal void myCFSocketCallBack(CFSocketRef cfs, CFSocketCallBackType CallBackType, CFDataRef address, const void *data, void *context)
	{
	mDNSAddr senderAddr, destAddr;
	mDNSIPPort senderPort, destPort = MulticastDNSPort;
	const CFSocketSet *ss = (const CFSocketSet *)context;
	mDNS *const m = ss->m;
	const mDNSInterfaceID InterfaceID = ss->info ? ss->info->ifinfo.InterfaceID : mDNSNULL;
	DNSMessage packet;
	struct sockaddr_storage from;
	size_t fromlen = sizeof(from);
	char packetifname[IF_NAMESIZE] = "";
	int err, s1 = -1, skt = CFSocketGetNative(cfs);
	int count = 0;
	
	(void)address; // Parameter not used
	(void)data;    // Parameter not used
	
	if (CallBackType != kCFSocketReadCallBack)
		LogMsg("myCFSocketCallBack: Why is CallBackType %d not kCFSocketReadCallBack?", CallBackType);

	if      (cfs == ss->cfsv4) s1 = ss->sktv4;
	else if (cfs == ss->cfsv6) s1 = ss->sktv6;

	if (s1 < 0 || s1 != skt)
		{
		LogMsg("myCFSocketCallBack: s1 %d native socket %d, cfs %p", s1, skt, cfs);
		LogMsg("myCFSocketCallBack: cfsv4 %p, sktv4 %d", ss->cfsv4, ss->sktv4);
		LogMsg("myCFSocketCallBack: cfsv6 %p, sktv6 %d", ss->cfsv6, ss->sktv6);
		}

	mDNSu8 ttl;
	while ((err = myrecvfrom(s1, &packet, sizeof(packet), (struct sockaddr *)&from, &fromlen, &destAddr, packetifname, &ttl)) >= 0)
		{
		count++;
		if (from.ss_family == AF_INET)
			{
			struct sockaddr_in *sin = (struct sockaddr_in*)&from;
			senderAddr.type = mDNSAddrType_IPv4;
			senderAddr.ip.v4.NotAnInteger = sin->sin_addr.s_addr;
			senderPort.NotAnInteger = sin->sin_port;
			}
		else if (from.ss_family == AF_INET6)
			{
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&from;
			senderAddr.type = mDNSAddrType_IPv6;
			senderAddr.ip.v6 = *(mDNSv6Addr*)&sin6->sin6_addr;
			senderPort.NotAnInteger = sin6->sin6_port;
			}
		else
			{
			LogMsg("myCFSocketCallBack from is unknown address family %d", from.ss_family);
			return;
			}

		// Even though we indicated a specific interface in the IP_ADD_MEMBERSHIP call, a weirdness of the
		// sockets API means that even though this socket has only officially joined the multicast group
		// on one specific interface, the kernel will still deliver multicast packets to it no matter which
		// interface they arrive on. According to the official Unix Powers That Be, this is Not A Bug.
		// To work around this weirdness, we use the IP_RECVIF option to find the name of the interface
		// on which the packet arrived, and ignore the packet if it really arrived on some other interface.
		if (!ss->info)
			verbosedebugf("myCFSocketCallBack got a packet from %#a to %#a on unicast socket", &senderAddr, &destAddr);
		else if (!strcmp(ss->info->ifa_name, packetifname))
			verbosedebugf("myCFSocketCallBack got a packet from %#a to %#a on interface %#a/%s",
				&senderAddr, &destAddr, &ss->info->ifinfo.ip, ss->info->ifa_name);
		else
			{
			verbosedebugf("myCFSocketCallBack got a packet from %#a to %#a on interface %#a/%s (Ignored -- really arrived on interface %s)",
				&senderAddr, &destAddr, &ss->info->ifinfo.ip, ss->info->ifa_name, packetifname);
			return;
			}
		
		if (err < (int)sizeof(DNSMessageHeader)) { debugf("myCFSocketCallBack packet length (%d) too short", err); return; }
		
		mDNSCoreReceive(m, &packet, (unsigned char*)&packet + err, &senderAddr, senderPort, &destAddr, destPort, InterfaceID, ttl);
		}

	if (err < 0 && (errno != EWOULDBLOCK || count == 0))
		{
		// Something is busted here.
		// CFSocket says there is a packet, but myrecvfrom says there is not.
		// Try calling select() to get another opinion.
		// Find out about other socket parameter that can help understand why select() says the socket is ready for read
		// All of this is racy, as data may have arrived after the call to select()
		int save_errno = errno;
		int so_error = -1;
		int so_nread = -1;
		int fionread = -1;
		int solen = sizeof(int);
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(s1, &readfds);
		struct timeval timeout;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 0;
		int selectresult = select(s1+1, &readfds, NULL, NULL, &timeout);
		if (getsockopt(s1, SOL_SOCKET, SO_ERROR, &so_error, &solen) == -1)
			LogMsg("myCFSocketCallBack getsockopt(SO_ERROR) error %d", errno);
		if (getsockopt(s1, SOL_SOCKET, SO_NREAD, &so_nread, &solen) == -1)
			LogMsg("myCFSocketCallBack getsockopt(SO_NREAD) error %d", errno);
		if (ioctl(s1, FIONREAD, &fionread) == -1)
			LogMsg("myCFSocketCallBack ioctl(FIONREAD) error %d", errno);
		static unsigned int numLogMessages = 0;
		if (numLogMessages++ < 100)
			LogMsg("myCFSocketCallBack recvfrom skt %d error %d errno %d (%s) select %d (%spackets waiting) so_error %d so_nread %d fionread %d count %d",
				s1, err, save_errno, strerror(save_errno), selectresult, FD_ISSET(s1, &readfds) ? "" : "*NO* ", so_error, so_nread, fionread, count);
		sleep(1);		// After logging this error, rate limit so we don't flood syslog
		} 
	}

// TCP socket support for unicast DNS and Dynamic DNS Update

typedef struct
	{
    TCPConnectionCallback callback;
    void *context;
    int connected;
	} tcpInfo_t;

mDNSlocal void tcpCFSocketCallback(CFSocketRef cfs, CFSocketCallBackType CallbackType, CFDataRef address,
								   const void *data, void *context)
	{
	#pragma unused(CallbackType, address, data)
	mDNSBool connect = mDNSfalse;  
	
	tcpInfo_t *info = context;
	if (!info->connected)
		{
		connect = mDNStrue;
		info->connected = mDNStrue;  // prevent connected flag from being set in future callbacks
		}
	info->callback(CFSocketGetNative(cfs), info->context, connect);
	// NOTE: the callback may call CloseConnection here, which frees the context structure!  
	}

mDNSexport mStatus mDNSPlatformTCPConnect(const mDNSAddr *dst, mDNSOpaque16 dstport, mDNSInterfaceID InterfaceID,
										  TCPConnectionCallback callback, void *context, int *descriptor)
	{
	int sd, on = 1;  // "on" for setsockopt
	struct sockaddr_in saddr;
	CFSocketContext cfContext = { 0, NULL, 0, 0, 0 };  
	tcpInfo_t *info;
	CFSocketRef sr;
	CFRunLoopSourceRef rls;
	CFOptionFlags srFlags;
	
	(void)InterfaceID;	//!!!KRS use this if non-zero!!!

	*descriptor = 0;
	if (dst->type != mDNSAddrType_IPv4)
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - attempt to connect to an IPv6 address: opperation not supported");
		return mStatus_UnknownErr;
		}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0)
		{
		LogMsg("ERROR: socket; %s", strerror(errno));
		return mStatus_UnknownErr;
		}
	// set non-blocking
	if (fcntl(sd, F_SETFL, O_NONBLOCK) < 0)
		{
		LogMsg("ERROR: setsockopt O_NONBLOCK - %s", strerror(errno));
		return mStatus_UnknownErr;
		}
	
	// receive interface identifiers
	if (setsockopt(sd, IPPROTO_IP, IP_RECVIF, &on, sizeof(on)) < 0)
		{
		LogMsg("setsockopt IP_RECVIF - %s", strerror(errno));
		return mStatus_UnknownErr;
		}
	// set up CF wrapper, add to Run Loop
	info = mallocL("mDNSPlatformTCPConnect", sizeof(tcpInfo_t));
	info->callback = callback;
	info->context = context;
	cfContext.info = info;
	sr = CFSocketCreateWithNative(kCFAllocatorDefault, sd, kCFSocketReadCallBack | kCFSocketConnectCallBack,
								  tcpCFSocketCallback, &cfContext);
	if (!sr)
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - CFSocketRefCreateWithNative failed");
		freeL("mDNSPlatformTCPConnect", info);
		return mStatus_UnknownErr;
		}

	// prevent closing of native socket
	srFlags = CFSocketGetSocketFlags(sr);
	CFSocketSetSocketFlags(sr, srFlags & (~kCFSocketCloseOnInvalidate));
	
	rls = CFSocketCreateRunLoopSource(kCFAllocatorDefault, sr, 0);
	if (!rls) 
		{
		LogMsg("ERROR: mDNSPlatformTCPConnect - CFSocketCreateRunLoopSource failed");
		freeL("mDNSPlatformTCPConnect", info);
		return mStatus_UnknownErr;
		}
	
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
	CFRelease(rls);
	
	// initiate connection wth peer
	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = dstport.NotAnInteger;
	memcpy(&saddr.sin_addr, &dst->ip.v4.NotAnInteger, sizeof(saddr.sin_addr));
	if (connect(sd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		{
		if (errno == EINPROGRESS)
			{
			info->connected = 0;
			*descriptor= sd;
			return mStatus_ConnectionPending;
			}
		LogMsg("ERROR: mDNSPlatformTCPConnect - connect failed: %s", strerror(errno));
		freeL("mDNSPlatformTCPConnect", info);
		CFSocketInvalidate(sr);
		return mStatus_ConnectionFailed;
		}
	info->connected = 1;
	*descriptor = sd;
	return mStatus_ConnectionEstablished;
	}

mDNSexport void mDNSPlatformTCPCloseConnection(int sd)
	{
	CFSocketContext cfContext;
	tcpInfo_t *info;
	CFSocketRef sr;

    // get the CFSocket for the descriptor, if it exists
	sr = CFSocketCreateWithNative(kCFAllocatorDefault, sd, NULL, NULL, NULL);
	if (!sr)
		{
		LogMsg("ERROR: mDNSPlatformTCPCloseConnection - attempt to close a socket that was not properly created");
		return;
		}
	CFSocketGetContext(sr, &cfContext);
	if (!cfContext.info)
		{
		LogMsg("ERROR: mDNSPlatformTCPCloseConnection - could not retreive tcpInfo from socket context");
		CFRelease(sr);
		return;
		}
	CFRelease(sr);  // this only releases the copy we allocated with CreateWithNative above
	
	info = cfContext.info;
	CFSocketInvalidate(sr);
	CFRelease(sr);	
	close(sd);
	freeL("mDNSPlatformTCPCloseConnection", info);	
	}

mDNSexport int mDNSPlatformReadTCP(int sd, void *buf, int buflen)
	{
	int nread = recv(sd, buf, buflen, 0);
	if (nread < 0)
		{
		if (errno == EAGAIN) return 0;  // no data available (call would block)
		LogMsg("ERROR: mDNSPlatformReadTCP - recv: %s", strerror(errno));
		return -1;
		}
	return nread;
	}

mDNSexport int mDNSPlatformWriteTCP(int sd, const char *msg, int len)
	{
	int nsent = send(sd, msg, len, 0);

	if (nsent < 0)
		{
		if (errno == EAGAIN) return 0;  // blocked
		LogMsg("ERROR: mDNSPlatformWriteTCP - sendL %s", strerror(errno));
		return -1;
		}
	return nsent;
	}

// This gets the text of the field currently labelled "Computer Name" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedFriendlyComputerName(domainlabel *const namelabel)
	{
	CFStringEncoding encoding = kCFStringEncodingUTF8;
	CFStringRef cfs = SCDynamicStoreCopyComputerName(NULL, &encoding);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

// This gets the text of the field currently labelled "Rendezvous Name" in the Sharing Prefs Control Panel
mDNSlocal void GetUserSpecifiedRFC1034ComputerName(domainlabel *const namelabel)
	{
	CFStringRef cfs = SCDynamicStoreCopyLocalHostName(NULL);
	if (cfs)
		{
		CFStringGetPascalString(cfs, namelabel->c, sizeof(*namelabel), kCFStringEncodingUTF8);
		CFRelease(cfs);
		}
	}

// If mDNSIPPort port is non-zero, then it's a multicast socket on the specified interface
// If mDNSIPPort port is zero, then it's a randomly assigned port number, used for sending unicast queries
mDNSlocal mStatus SetupSocket(CFSocketSet *cp, mDNSIPPort port, const mDNSAddr *ifaddr, u_short sa_family)
	{
	int         *s        = (sa_family == AF_INET) ? &cp->sktv4 : &cp->sktv6;
	CFSocketRef *c        = (sa_family == AF_INET) ? &cp->cfsv4 : &cp->cfsv6;
	CFRunLoopSourceRef *r = (sa_family == AF_INET) ? &cp->rlsv4 : &cp->rlsv6;
	const int on = 1;
	const int twofivefive = 255;
	mStatus err = mStatus_NoError;
	char *errstr = mDNSNULL;

	if (*s >= 0) { LogMsg("SetupSocket ERROR: socket %d is already set", *s); return(-1); }
	if (*c) { LogMsg("SetupSocket ERROR: CFSocketRef %p is already set", *c); return(-1); }

	// Open the socket...
	int skt = socket(sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if (skt < 0) { LogMsg("socket error %d errno %d (%s)", skt, errno, strerror(errno)); return(skt); }

	// ... with a shared UDP port, if it's for multicast receiving
	if (port.NotAnInteger) err = setsockopt(skt, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
	if (err < 0) { errstr = "setsockopt - SO_REUSEPORT"; goto fail; }

	if (sa_family == AF_INET)
		{
		// We want to receive destination addresses
		err = setsockopt(skt, IPPROTO_IP, IP_RECVDSTADDR, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IP_RECVDSTADDR"; goto fail; }
		
		// We want to receive interface identifiers
		err = setsockopt(skt, IPPROTO_IP, IP_RECVIF, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IP_RECVIF"; goto fail; }
		
		// We want to receive packet TTL value so we can check it
		err = setsockopt(skt, IPPROTO_IP, IP_RECVTTL, &on, sizeof(on));
		// We ignore errors here -- we already know Jaguar doesn't support this, but we can get by without it
		
		// Add multicast group membership on this interface, if it's for multicast receiving
		if (port.NotAnInteger)
			{
			struct in_addr addr = { ifaddr->ip.v4.NotAnInteger };
			struct ip_mreq imr;
			imr.imr_multiaddr.s_addr = AllDNSLinkGroup.NotAnInteger;
			imr.imr_interface        = addr;
			err = setsockopt(skt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
			if (err < 0) { errstr = "setsockopt - IP_ADD_MEMBERSHIP"; goto fail; }
			
			// Specify outgoing interface too
			err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_IF, &addr, sizeof(addr));
			if (err < 0) { errstr = "setsockopt - IP_MULTICAST_IF"; goto fail; }
			}
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IP, IP_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IP_TTL"; goto fail; }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IP, IP_MULTICAST_TTL, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IP_MULTICAST_TTL"; goto fail; }

		// Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate
		const int ip_tosbits = IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
		err = setsockopt(skt, IPPROTO_IP, IP_TOS, &ip_tosbits, sizeof(ip_tosbits));
		if (err < 0) { errstr = "setsockopt - IP_TOS"; goto fail; }

		// And start listening for packets
		struct sockaddr_in listening_sockaddr;
		listening_sockaddr.sin_family      = AF_INET;
		listening_sockaddr.sin_port        = port.NotAnInteger;
		listening_sockaddr.sin_addr.s_addr = 0; // Want to receive multicasts AND unicasts on this socket
		err = bind(skt, (struct sockaddr *) &listening_sockaddr, sizeof(listening_sockaddr));
		if (err) { errstr = "bind"; goto fail; }
		}
	else if (sa_family == AF_INET6)
		{
		// We want to receive destination addresses and receive interface identifiers
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_PKTINFO"; goto fail; }
		
		// We want to receive packet hop count value so we can check it
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_HOPLIMIT"; goto fail; }
		
		// We want to receive only IPv6 packets, without this option, we may
		// get IPv4 addresses as mapped addresses.
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_V6ONLY"; goto fail; }
		
		if (port.NotAnInteger)
			{
			// Add multicast group membership on this interface, if it's for multicast receiving
			int interface_id = if_nametoindex(cp->info->ifa_name);
			struct ipv6_mreq i6mr;
			i6mr.ipv6mr_interface = interface_id;
			i6mr.ipv6mr_multiaddr = *(struct in6_addr*)&AllDNSLinkGroupv6;
			err = setsockopt(skt, IPPROTO_IPV6, IPV6_JOIN_GROUP, &i6mr, sizeof(i6mr));
			if (err < 0) { errstr = "setsockopt - IPV6_JOIN_GROUP"; goto fail; }
			
			// Specify outgoing interface too
			err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_IF, &interface_id, sizeof(interface_id));
			if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_IF"; goto fail; }
			}
		
		// Send unicast packets with TTL 255
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IPV6_UNICAST_HOPS"; goto fail; }
		
		// And multicast packets with TTL 255 too
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &twofivefive, sizeof(twofivefive));
		if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_HOPS"; goto fail; }
		
		// Note: IPV6_TCLASS appears not to be implemented on OS X right now (or indeed on ANY version of Unix?)
		#ifdef IPV6_TCLASS
		// Mark packets as high-throughput/low-delay (i.e. lowest reliability) to get maximum 802.11 multicast rate
		int tclass = IPTOS_LOWDELAY | IPTOS_THROUGHPUT; // This may not be right (since tclass is not implemented on OS X, I can't test it)
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_TCLASS, &tclass, sizeof(tclass));
		if (err < 0) { errstr = "setsockopt - IPV6_TCLASS"; goto fail; }
		#endif
		
		// Want to receive our own packets
		err = setsockopt(skt, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &on, sizeof(on));
		if (err < 0) { errstr = "setsockopt - IPV6_MULTICAST_LOOP"; goto fail; }
		
		// And start listening for packets
		struct sockaddr_in6 listening_sockaddr6;
		bzero(&listening_sockaddr6, sizeof(listening_sockaddr6));
		listening_sockaddr6.sin6_len         = sizeof(listening_sockaddr6);
		listening_sockaddr6.sin6_family      = AF_INET6;
		listening_sockaddr6.sin6_port        = port.NotAnInteger;
		listening_sockaddr6.sin6_flowinfo    = 0;
//		listening_sockaddr6.sin6_addr = IN6ADDR_ANY_INIT; // Want to receive multicasts AND unicasts on this socket
		listening_sockaddr6.sin6_scope_id    = 0;
		err = bind(skt, (struct sockaddr *) &listening_sockaddr6, sizeof(listening_sockaddr6));
		if (err) { errstr = "bind"; goto fail; }
		}
	
	fcntl(skt, F_SETFL, fcntl(skt, F_GETFL, 0) | O_NONBLOCK); // set non-blocking
	*s = skt;
	CFSocketContext myCFSocketContext = { 0, cp, NULL, NULL, NULL };
	*c = CFSocketCreateWithNative(kCFAllocatorDefault, *s, kCFSocketReadCallBack, myCFSocketCallBack, &myCFSocketContext);
	*r = CFSocketCreateRunLoopSource(kCFAllocatorDefault, *c, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), *r, kCFRunLoopDefaultMode);
	
	return(err);

fail:
	LogMsg("%s error %ld errno %d (%s)", errstr, err, errno, strerror(errno));
	close(skt);
	return(err);
	}

mDNSlocal mStatus SetupAddr(mDNSAddr *ip, const struct sockaddr *const sa)
	{
	if (sa->sa_family == AF_INET)
		{
		struct sockaddr_in *ifa_addr = (struct sockaddr_in *)sa;
		ip->type = mDNSAddrType_IPv4;
		ip->ip.v4.NotAnInteger = ifa_addr->sin_addr.s_addr;
		return(0);
		}
	else if (sa->sa_family == AF_INET6)
		{
		struct sockaddr_in6 *ifa_addr = (struct sockaddr_in6 *)sa;
		ip->type = mDNSAddrType_IPv6;
		if (IN6_IS_ADDR_LINKLOCAL(&ifa_addr->sin6_addr)) ifa_addr->sin6_addr.__u6_addr.__u6_addr16[1] = 0;
		ip->ip.v6 = *(mDNSv6Addr*)&ifa_addr->sin6_addr;
		return(0);
		}
	else
		{
		LogMsg("SetupAddr invalid sa_family %d", sa->sa_family);
		return(-1);
		}
	}

mDNSlocal NetworkInterfaceInfoOSX *AddInterfaceToList(mDNS *const m, struct ifaddrs *ifa)
	{
	mDNSu32 scope_id = if_nametoindex(ifa->ifa_name);
	mDNSAddr ip;
	SetupAddr(&ip, ifa->ifa_addr);
	NetworkInterfaceInfoOSX **p;
	for (p = &m->p->InterfaceList; *p; p = &(*p)->next)
		if (scope_id == (*p)->scope_id && mDNSSameAddress(&ip, &(*p)->ifinfo.ip))
			{
			debugf("AddInterfaceToList: Found existing interface %u with address %#a", scope_id, &ip);
			(*p)->Exists = mDNStrue;
			return(*p);
			}

	debugf("AddInterfaceToList: Making   new   interface %u with address %#a", scope_id, &ip);
	NetworkInterfaceInfoOSX *i = (NetworkInterfaceInfoOSX *)mallocL("NetworkInterfaceInfoOSX", sizeof(*i));
	if (!i) return(mDNSNULL);
	i->ifa_name        = (char *)mallocL("NetworkInterfaceInfoOSX name", strlen(ifa->ifa_name) + 1);
	if (!i->ifa_name) { freeL("NetworkInterfaceInfoOSX", i); return(mDNSNULL); }
	strcpy(i->ifa_name, ifa->ifa_name);

	bzero(&i->ifinfo.uDNS_info, sizeof(uDNS_NetworkInterfaceInfo));
	i->ifinfo.InterfaceID = mDNSNULL;
	i->ifinfo.ip          = ip;
	i->ifinfo.Advertise   = m->AdvertiseLocalAddresses;
	i->ifinfo.McastTxRx   = mDNSfalse; // For now; will be set up later at the end of UpdateInterfaceList
	
	i->next            = mDNSNULL;
	i->Exists          = mDNStrue;
	i->scope_id        = scope_id;
	i->sa_family       = ifa->ifa_addr->sa_family;
	i->Multicast       = (ifa->ifa_flags & IFF_MULTICAST) && !(ifa->ifa_flags & IFF_POINTOPOINT);

	i->ss.m     = m;
	i->ss.info  = i;
	i->ss.sktv4 = i->ss.sktv6 = -1;
	i->ss.cfsv4 = i->ss.cfsv6 = NULL;
	i->ss.rlsv4 = i->ss.rlsv6 = NULL;

	*p = i;
	return(i);
	}

mDNSlocal NetworkInterfaceInfoOSX *FindRoutableIPv4(mDNS *const m, mDNSu32 scope_id)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists && i->scope_id == scope_id && i->ifinfo.ip.type == mDNSAddrType_IPv4)
			if (!(i->ifinfo.ip.ip.v4.b[0] == 169 && i->ifinfo.ip.ip.v4.b[1] == 254))
				return(i);
	return(mDNSNULL);
	}

mDNSlocal mStatus UpdateInterfaceList(mDNS *const m)
	{
	mDNSBool foundav4           = mDNSfalse;
	struct ifaddrs *ifa         = myGetIfAddrs(1);
	struct ifaddrs *theLoopback = NULL;
	int err = (ifa != NULL) ? 0 : (errno != 0 ? errno : -1);
	int InfoSocket              = err ? -1 : socket(AF_INET6, SOCK_DGRAM, 0);
	if (err) return(err);

	// Set up the nice label
	m->nicelabel.c[0] = 0;
	GetUserSpecifiedFriendlyComputerName(&m->nicelabel);
	if (m->nicelabel.c[0] == 0) MakeDomainLabelFromLiteralString(&m->nicelabel, "Macintosh");

	// Set up the RFC 1034-compliant label
	domainlabel hostlabel;
	hostlabel.c[0] = 0;
	GetUserSpecifiedRFC1034ComputerName(&hostlabel);
	if (hostlabel.c[0] == 0) MakeDomainLabelFromLiteralString(&hostlabel, "Macintosh");
	// If the user has changed their dot-local host name since the last time we checked, then update our local copy.
	// If the user has not changed their dot-local host name, then leave ours alone (m->hostlabel may have gone through
	// repeated conflict resolution to get to its current value, and if we reset it, we'll have to go through all that again.)
	if (SameDomainLabel(m->p->userhostlabel.c, hostlabel.c))
		debugf("Userhostlabel (%#s) unchanged since last time; not changing m->hostlabel (%#s)", m->p->userhostlabel.c, m->hostlabel.c);
	else
		{
		debugf("Updating m->hostlabel to %#s", hostlabel.c);
		m->p->userhostlabel = m->hostlabel = hostlabel;
		mDNS_GenerateFQDN(m);
		if (mDNS_DNSRegistered(m)) mDNS_GenerateGlobalFQDN(m);
		}

	while (ifa)
		{
#if LIST_ALL_INTERFACES
		if (ifa->ifa_addr->sa_family == AF_APPLETALK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d is AF_APPLETALK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family == AF_LINK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d is AF_LINK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		else if (ifa->ifa_addr->sa_family != AF_INET && ifa->ifa_addr->sa_family != AF_INET6)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d not AF_INET (2) or AF_INET6 (30)",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_UP))
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface not IFF_UP",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (!(ifa->ifa_flags & IFF_MULTICAST))
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface not IFF_MULTICAST",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_POINTOPOINT)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface IFF_POINTOPOINT",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
		if (ifa->ifa_flags & IFF_LOOPBACK)
			debugf("UpdateInterfaceList: %4s(%d) Flags %04X Family %2d Interface IFF_LOOPBACK",
				ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_flags, ifa->ifa_addr->sa_family);
#endif
		if (ifa->ifa_flags & IFF_UP)
			if (ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6)
				{
				int ifru_flags6 = 0;
				if (ifa->ifa_addr->sa_family == AF_INET6 && InfoSocket >= 0)
					{
					struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ifa->ifa_addr;
					struct in6_ifreq ifr6;
					bzero((char *)&ifr6, sizeof(ifr6));
					strncpy(ifr6.ifr_name, ifa->ifa_name, sizeof(ifr6.ifr_name));
					ifr6.ifr_addr = *sin6;
					if (ioctl(InfoSocket, SIOCGIFAFLAG_IN6, &ifr6) != -1)
						ifru_flags6 = ifr6.ifr_ifru.ifru_flags6;
					verbosedebugf("%s %.16a %04X %04X", ifa->ifa_name, &sin6->sin6_addr, ifa->ifa_flags, ifru_flags6);
					}
				if (!(ifru_flags6 & (IN6_IFF_NOTREADY | IN6_IFF_DETACHED | IN6_IFF_DEPRECATED | IN6_IFF_TEMPORARY)))
					{
					if (ifa->ifa_flags & IFF_LOOPBACK)
						theLoopback = ifa;
					else
						{
						AddInterfaceToList(m, ifa);
						if (ifa->ifa_addr->sa_family == AF_INET)
							foundav4 = mDNStrue;
						}
					}
				}
		ifa = ifa->ifa_next;
		}

    //  Temporary workaround: Multicast loopback on IPv6 interfaces appears not to work.
    //  In the interim, we skip loopback interface only if we found at least one v4 interface to use
	if (!foundav4 && theLoopback)
		AddInterfaceToList(m, theLoopback);

	// Now the list is complete, set the McastTxRx setting for each interface.
	// We always send and receive using IPv4.
	// To reduce traffic, we send and receive using IPv6 only on interfaces that have no routable IPv4 address.
	// Having a routable IPv4 address assigned is a reasonable indicator of being on a large configured network,
	// which means there's a good chance that most or all the other devices on that network should also have v4.
	// By doing this we lose the ability to talk to true v6-only devices on that link, but we cut the packet rate in half.
	// At this time, reducing the packet rate is more important than v6-only devices on a large configured network,
	// so we are willing to make that sacrifice.
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists)
			{
			mDNSBool txrx = i->Multicast && ((i->ifinfo.ip.type == mDNSAddrType_IPv4) || !FindRoutableIPv4(m, i->scope_id));
			if (i->ifinfo.McastTxRx != txrx)
				{
				i->ifinfo.McastTxRx = txrx;
				i->Exists = 2; // State change; need to deregister and reregister this interface
				}
			}

	if (InfoSocket >= 0) close(InfoSocket);
	return(err);
	}

mDNSlocal NetworkInterfaceInfoOSX *SearchForInterfaceByName(mDNS *const m, char *ifname, int type)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists && !strcmp(i->ifa_name, ifname) &&
			((AAAA_OVER_V4                                              ) ||
			 (type == AF_INET  && i->ifinfo.ip.type == mDNSAddrType_IPv4) ||
			 (type == AF_INET6 && i->ifinfo.ip.type == mDNSAddrType_IPv6) )) return(i);
	return(NULL);
	}

mDNSlocal void SetupActiveInterfaces(mDNS *const m)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		if (i->Exists)
			{
			NetworkInterfaceInfo *n = &i->ifinfo;
			NetworkInterfaceInfoOSX *primary = SearchForInterfaceByName(m, i->ifa_name, i->sa_family);
			if (!primary) LogMsg("SetupActiveInterfaces ERROR! SearchForInterfaceByName didn't find %s", i->ifa_name);
	
			if (n->InterfaceID && n->InterfaceID != (mDNSInterfaceID)primary)	// Sanity check
				{
				LogMsg("SetupActiveInterfaces ERROR! n->InterfaceID %p != primary %p", n->InterfaceID, primary);
				n->InterfaceID = mDNSNULL;
				}
	
			if (!n->InterfaceID)
				{
				// NOTE: If n->InterfaceID is set, that means we've called mDNS_RegisterInterface() for this interface,
				// so we need to make sure we call mDNS_DeregisterInterface() before disposing it.
				// If n->InterfaceID is NOT set, then we haven't registered it and we should not try to deregister it
				n->InterfaceID = (mDNSInterfaceID)primary;
				mDNS_RegisterInterface(m, n);
				LogOperation("SetupActiveInterfaces: Registered  %s(%lu) InterfaceID %p %#a%s",
					i->ifa_name, i->scope_id, primary, &n->ip, n->InterfaceActive ? " (Primary)" : "");
				}
	
			if (!n->McastTxRx)
				debugf("SetupActiveInterfaces: No Tx/Rx on %s(%lu) InterfaceID %p %#a", i->ifa_name, i->scope_id, primary, &n->ip);
			else
				{
				if (i->sa_family == AF_INET && primary->ss.sktv4 == -1)
					{
					mStatus err = SetupSocket(&primary->ss, MulticastDNSPort, &i->ifinfo.ip, AF_INET);
					if (err == 0) debugf("SetupActiveInterfaces: v4 socket%2d %s(%lu) InterfaceID %p %#a", primary->ss.sktv4, i->ifa_name, i->scope_id, n->InterfaceID, &n->ip);
					else LogMsg("SetupActiveInterfaces: v4 socket%2d %s(%lu) InterfaceID %p %#a FAILED",   primary->ss.sktv4, i->ifa_name, i->scope_id, n->InterfaceID, &n->ip);
					}
			
				if (i->sa_family == AF_INET6 && primary->ss.sktv6 == -1)
					{
					mStatus err = SetupSocket(&primary->ss, MulticastDNSPort, &i->ifinfo.ip, AF_INET6);
					if (err == 0) debugf("SetupActiveInterfaces: v6 socket%2d %s(%lu) InterfaceID %p %#a", primary->ss.sktv6, i->ifa_name, i->scope_id, n->InterfaceID, &n->ip);
					else LogMsg("SetupActiveInterfaces: v6 socket%2d %s(%lu) InterfaceID %p %#a FAILED",   primary->ss.sktv6, i->ifa_name, i->scope_id, n->InterfaceID, &n->ip);
					}
				}
			}
	}

mDNSlocal void MarkAllInterfacesInactive(mDNS *const m)
	{
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		i->Exists = mDNSfalse;
	}

mDNSlocal void CloseSocketSet(CFSocketSet *ss)
	{
	// Note: MUST NOT close the underlying native BSD sockets.
	// CFSocketInvalidate() will do that for us, in its own good time, which may not necessarily be immediately,
	// because it first has to unhook the sockets from its select() call, before it can safely close them.
	if (ss->cfsv4) { CFRunLoopRemoveSource(CFRunLoopGetCurrent(), ss->rlsv4, kCFRunLoopDefaultMode); CFRelease(ss->rlsv4); CFSocketInvalidate(ss->cfsv4); CFRelease(ss->cfsv4); }
	if (ss->cfsv6) { CFRunLoopRemoveSource(CFRunLoopGetCurrent(), ss->rlsv6, kCFRunLoopDefaultMode); CFRelease(ss->rlsv6); CFSocketInvalidate(ss->cfsv6); CFRelease(ss->cfsv6); }
	ss->sktv4 = ss->sktv6 = -1;
	ss->cfsv4 = ss->cfsv6 = NULL;
	ss->rlsv4 = ss->rlsv6 = NULL;
	}

mDNSlocal void ClearInactiveInterfaces(mDNS *const m)
	{
	// First pass:
	// If an interface is going away, then deregister this from the mDNSCore.
	// We also have to deregister it if the primary interface that it's using for its InterfaceID is going away.
	// We have to do this because mDNSCore will use that InterfaceID when sending packets, and if the memory
	// it refers to has gone away we'll crash.
	// Don't actually close the sockets or free the memory yet: When the last representative of an interface goes away
	// mDNSCore may want to send goodbye packets on that interface. (Not yet implemented, but a good idea anyway.)
	NetworkInterfaceInfoOSX *i;
	for (i = m->p->InterfaceList; i; i = i->next)
		{
		// 1. If this interface is no longer active, or its InterfaceID is changing, deregister it
		NetworkInterfaceInfoOSX *primary = SearchForInterfaceByName(m, i->ifa_name, i->sa_family);
		if (i->ifinfo.InterfaceID)
			if (i->Exists == 0 || i->Exists == 2 || i->ifinfo.InterfaceID != (mDNSInterfaceID)primary)
				{
				LogOperation("ClearInactiveInterfaces: Deregistering %s(%lu) InterfaceID %p %#a%s",
					i->ifa_name, i->scope_id, i->ifinfo.InterfaceID, &i->ifinfo.ip, i->ifinfo.InterfaceActive ? " (Primary)" : "");
				mDNS_DeregisterInterface(m, &i->ifinfo);
				i->ifinfo.InterfaceID = mDNSNULL;
				// NOTE: If n->InterfaceID is set, that means we've called mDNS_RegisterInterface() for this interface,
				// so we need to make sure we call mDNS_DeregisterInterface() before disposing it.
				// If n->InterfaceID is NOT set, then it's not registered and we should not call mDNS_DeregisterInterface() on it.
				}
		}

	// Second pass:
	// Now that everything that's going to deregister has done so, we can close sockets and free the memory
	NetworkInterfaceInfoOSX **p = &m->p->InterfaceList;
	while (*p)
		{
		i = *p;
		// 2. Close all our CFSockets. We'll recreate them later as necessary.
		// (We may have previously had both v4 and v6, and we may not need both any more.)
		CloseSocketSet(&i->ss);
		// 3. If no longer active, delete interface from list and free memory
		if (!i->Exists && NumCacheRecordsForInterfaceID(m, (mDNSInterfaceID)i) == 0)
			{
			debugf("ClearInactiveInterfaces: Deleting      %#a", &i->ifinfo.ip);
			*p = i->next;
			if (i->ifa_name) freeL("NetworkInterfaceInfoOSX name", i->ifa_name);
			freeL("NetworkInterfaceInfoOSX", i);
			}
		else
			p = &i->next;
		}
	}


mDNSlocal mStatus RegisterNameServers(mDNS *const m, CFDictionaryRef dict)
	{
	int i, count;
	CFArrayRef values;
	char            buf[256];
	mDNSv4Addr      saddr;	
	CFStringRef s;


	mDNS_DeregisterDNSList(m); // deregister orig list
	values = CFDictionaryGetValue(dict, kSCPropNetDNSServerAddresses);
	if (!values) return mStatus_NoError;

	count = CFArrayGetCount(values);
	for (i = 0; i < count; i++)
		{
		s = CFArrayGetValueAtIndex(values, i);
		if (!s) { LogMsg("ERROR: RegisterNameServers - CFArrayGetValueAtIndex"); break; }
		if (!CFStringGetCString(s, buf, 256, kCFStringEncodingASCII))
			{
			LogMsg("ERROR: RegisterNameServers - CFStringGetCString");
			continue;
			}
		if (!inet_aton(buf, (struct in_addr *)saddr.b))
			{
			LogMsg("ERROR: RegisterNameServers - invalid address string %s", buf);
			continue;
			}
		mDNS_RegisterDNS(m, &saddr);
		}
	return mStatus_NoError;
	}

mDNSlocal void FreeARElemCallback(mDNS *const m, AuthRecord *const rr, mStatus result)
	{
	(void)m;  // unused
	AuthRecordListElem *elem = rr->RecordContext;
	if (result == mStatus_MemFree) freeL("FreeARElemCallback", elem);
	}

mDNSlocal void FoundDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)	
	{
	SearchListElem *slElem = question->QuestionContext;
	AuthRecordListElem *arElem, *ptr, *prev;
    AuthRecord *dereg;    
	char *name;
	mStatus err;
	
	if (AddRecord)
		{
		arElem = mallocL("FoundDomain - arElem", sizeof(AuthRecordListElem));
		if (!arElem) { LogMsg("ERROR: malloc");  return; }
		mDNS_SetupResourceRecord(&arElem->ar, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, FreeARElemCallback, arElem);
		if (question == &slElem->browseQ) name = "_browse._dns-sd._udp.local.";
		else                              name = "_register._dns-sd._udp.local.";
		MakeDomainNameFromDNSNameString(&arElem->ar.resrec.name, name);
		strcpy(arElem->ar.resrec.rdata->u.name.c, answer->rdata->u.name.c);
		err = mDNS_Register(m, &arElem->ar);
		if (err)
			{
			LogMsg("ERROR: FoundDomain - mDNS_Register returned %d", err);
			freeL("FoundDomain - arElem", arElem);				  
			return;
			}
		arElem->next = slElem->AuthRecs;
		slElem->AuthRecs = arElem;
		}
	else
		{
		ptr = slElem->AuthRecs;
		prev = NULL;
		while (ptr)
			{
			if (SameDomainName(&ptr->ar.resrec.name, &answer->name) && SameDomainName(&ptr->ar.resrec.rdata->u.name, &answer->rdata->u.name))
				{
				debugf("Deregistering PTR %s -> %s", ptr->ar.resrec.name.c, ptr->ar.resrec.rdata->u.name.c);
                dereg = &ptr->ar;
				if (prev) prev->next = ptr->next;
				else slElem->AuthRecs = ptr->next;
                ptr = ptr->next;
				err = mDNS_Deregister(m, dereg);				
				if (err) LogMsg("ERROR: FoundDomain - mDNS_Deregister returned %d", err);
				}
			else
				{
				prev = ptr;
				ptr = ptr->next;
				}
			}
		}
	}

mDNSlocal mStatus RegisterSearchDomains(mDNS *const m, CFDictionaryRef dict)
	{
	int i, count;
	CFArrayRef values;
	domainname domain;
	char            buf[MAX_ESCAPED_DOMAIN_NAME];
	CFStringRef s;
	SearchListElem *new, *ptr, *prev, *freeSLPtr;
	AuthRecordListElem *arList;
	mStatus err;
	
	// step 1: mark each elem for removal (-1)
	for (ptr = SearchList; ptr; ptr = ptr->next) ptr->flag = -1;
	
	values = CFDictionaryGetValue(dict, kSCPropNetDNSSearchDomains);
	if (values)
		{
		count = CFArrayGetCount(values);
		for (i = 0; i < count; i++)
			{
			s = CFArrayGetValueAtIndex(values, i);
			if (!s) { LogMsg("ERROR: RegisterNameServers - CFArrayGetValueAtIndex"); break; }
			if (!CFStringGetCString(s, buf, MAX_ESCAPED_DOMAIN_NAME, kCFStringEncodingASCII))
				{
				LogMsg("ERROR: RegisterNameServers - CFStringGetCString");
				continue;
				}		   			
			if (!MakeDomainNameFromDNSNameString(&domain, buf))
				{
				LogMsg("ERROR: RegisterNameServers - invalid search domain %s", buf);
				continue;
				}				
			// if domain is in list, mark as pre-existent (0)
			for (ptr = SearchList; ptr; ptr = ptr->next)
				if (SameDomainName(&ptr->domain, &domain)) { ptr->flag = 0; break; }

			// if domain not in list, add to list, mark as add (1)
			if (!ptr)
				{
				new = mallocL("RegisterSearchDomains - SearchListElem", sizeof(SearchListElem));
				if (!new) { LogMsg("ERROR: RegisterSearchDomains - malloc"); return mStatus_UnknownErr; }
				bzero(new, sizeof(SearchListElem));
				strcpy(new->domain.c, domain.c);
				new->flag = 1;  // add
				new->next = SearchList;
				SearchList = new;
				}
			}
		}
	// delete elems marked for removal, do queries for elems marked add
	prev = NULL;
	ptr = SearchList;
	while (ptr)
		{
		if (ptr->flag == -1)  // remove
			{				
			mDNS_StopQuery(m, &ptr->browseQ);
			mDNS_StopQuery(m, &ptr->registerQ);			
			// deregister records generated from answers to the query
			arList = ptr->AuthRecs;
			ptr->AuthRecs = NULL;
			while (arList)
				{
				AuthRecord *dereg = &arList->ar;
				arList = arList->next;
				debugf("Deregistering PTR %s -> %s", dereg->resrec.name.c, dereg->resrec.rdata->u.name.c);
				err = mDNS_Deregister(m, dereg);
				if (err) LogMsg("ERROR: RegisterSearchDomains mDNS_Deregister returned %d", err);
				}
			
			// remove elem from list, delete				
			if (prev) prev->next = ptr->next;
			else SearchList = ptr->next;
			freeSLPtr = ptr;
			ptr = ptr->next;
			freeL("RegisterNameServers - freeSLPtr", freeSLPtr);
			continue;
			}
		
		if (ptr->flag == 1)  // add
			{
			err = mDNS_GetDomains(m, &ptr->browseQ, mDNS_DomainTypeBrowse, &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			if (err) LogMsg("ERROR: RegisterNameServers - mDNS_DomainTypeBrowse, %d", err);

			err = mDNS_GetDomains(m, &ptr->registerQ, mDNS_DomainTypeRegistration, &ptr->domain, mDNSInterface_Any, FoundDomain, ptr);
			if (err) LogMsg("ERROR: RegisterNameServers - mDNS_DomainTypeRegistration, %d", err);
			ptr->flag = 0;
			}
		
		if (ptr->flag) { LogMsg("RegisterNameServers - unknown flag %d.  Skipping.", ptr->flag); }
		
		prev = ptr;
		ptr = ptr->next;
		}		
	
	return mStatus_NoError;
	}

// key must be kSCPropNetDNSServerAddresses or kSCPropNetDNSSearchDomains
mDNSlocal mStatus RegisterDNSConfig(mDNS *const m, CFDictionaryRef dict, const CFStringRef key)
	{	
	if (key == kSCPropNetDNSSearchDomains) return RegisterSearchDomains(m, dict);
	if (key == kSCPropNetDNSServerAddresses) return RegisterNameServers(m, dict);
	LogMsg("ERROR: RegisterDNSConfig - bad key"); return mStatus_UnknownErr;
	}


mDNSlocal void DNSConfigChanged(SCDynamicStoreRef session, CFArrayRef changes, void *context)
	{
	mDNS *m = context;
	CFDictionaryRef dict;
	CFStringRef     key;

	if (DNSConfigInitialized && (!changes || CFArrayGetCount(changes) == 0)) return;

	//!!!KRS fixme - we need a list of registerd servers. this wholesale
    // dereg doesn't work if there's an error and we bail out before registering the new list

	key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
	if (!key) {  LogMsg("ERROR: DNSConfigChanged - SCDynamicStoreKeyCreateNetworkGlobalEntity");  return;  }
	dict = SCDynamicStoreCopyValue(session, key);
	CFRelease(key);
	if (dict)
		{
		RegisterDNSConfig(m, dict, kSCPropNetDNSServerAddresses);
		RegisterDNSConfig(m, dict, kSCPropNetDNSSearchDomains);		
		CFRelease(dict);
		}		
	if (mDNS_DNSRegistered(m)) mDNS_GenerateGlobalFQDN(m);
	// no-op if label & domain are unchanged
	}

mDNSlocal mStatus WatchForDNSChanges(mDNS *const m)	
    {
    CFStringRef			    key;
    CFMutableArrayRef		keyList;
    CFRunLoopSourceRef		rls;
    SCDynamicStoreRef 		session;
	SCDynamicStoreContext context = { 0, m, NULL, NULL, NULL };
	
    session = SCDynamicStoreCreate(NULL, CFSTR("trackDNS"), DNSConfigChanged, &context);    
    if (!session) {  LogMsg("ERROR: WatchForDNSChanges - SCDynamicStoreCreate");  return mStatus_UnknownErr;  }

    keyList = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    if (!keyList) {  LogMsg("ERROR: WatchForDNSChanges - CFArrayCreateMutable");  return mStatus_UnknownErr;  }
    
    // create a pattern that matches the global DNS dictionary key
    key = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetDNS);
    if (!key) {  LogMsg("ERROR: WatchForDNSChanges - SCDynamicStoreKeyCreateNetworkGlobalEntity");  return mStatus_UnknownErr;  }
    
    CFArrayAppendValue(keyList, key);
    CFRelease(key);
    
    // set the keys for our DynamicStore session
    SCDynamicStoreSetNotificationKeys(session, keyList, NULL);
    CFRelease(keyList);
    
    // create a CFRunLoopSource for our DynamicStore session
    rls = SCDynamicStoreCreateRunLoopSource(NULL, session, 0);
    if (!rls) {  LogMsg("ERROR: WatchForDNSChanges - SCDynamicStoreCreateRunLoopSource");  return mStatus_UnknownErr;  }
    
    // add the run loop source to our current run loop 
    CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
    CFRelease(rls);
    
	// get initial configuration
    DNSConfigChanged(session, NULL, m);
    return mStatus_NoError;
    }

mDNSlocal void NetworkChanged(SCDynamicStoreRef store, CFArrayRef changedKeys, void *context)
	{
	(void)store;        // Parameter not used
	(void)changedKeys;  // Parameter not used
	debugf("***   Network Configuration Change   ***");

	mDNS *const m = (mDNS *const)context;
	MarkAllInterfacesInactive(m);
	UpdateInterfaceList(m);
	ClearInactiveInterfaces(m);
	SetupActiveInterfaces(m);
	
	if (m->MainCallback)
		m->MainCallback(m, mStatus_ConfigChanged);
	}

mDNSlocal mStatus WatchForNetworkChanges(mDNS *const m)
	{
	mStatus err = -1;
	SCDynamicStoreContext context = { 0, m, NULL, NULL, NULL };
	SCDynamicStoreRef     store    = SCDynamicStoreCreate(NULL, CFSTR("mDNSResponder"), NetworkChanged, &context);
	CFStringRef           key1     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetIPv4);
	CFStringRef           key2     = SCDynamicStoreKeyCreateNetworkGlobalEntity(NULL, kSCDynamicStoreDomainState, kSCEntNetIPv6);
	CFStringRef           key3     = SCDynamicStoreKeyCreateComputerName(NULL);
	CFStringRef           key4     = SCDynamicStoreKeyCreateHostNames(NULL);
	CFStringRef           pattern1 = SCDynamicStoreKeyCreateNetworkInterfaceEntity(NULL, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv4);
	CFStringRef           pattern2 = SCDynamicStoreKeyCreateNetworkInterfaceEntity(NULL, kSCDynamicStoreDomainState, kSCCompAnyRegex, kSCEntNetIPv6);

	CFMutableArrayRef     keys     = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	CFMutableArrayRef     patterns = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

	if (!store) { LogMsg("SCDynamicStoreCreate failed: %s\n", SCErrorString(SCError())); goto error; }
	if (!key1 || !key2 || !key3 || !key4 || !keys || !pattern1 || !pattern2 || !patterns) goto error;

	CFArrayAppendValue(keys, key1);
	CFArrayAppendValue(keys, key2);
	CFArrayAppendValue(keys, key3);
	CFArrayAppendValue(keys, key4);
	CFArrayAppendValue(patterns, pattern1);
	CFArrayAppendValue(patterns, pattern2);
	if (!SCDynamicStoreSetNotificationKeys(store, keys, patterns))
		{ LogMsg("SCDynamicStoreSetNotificationKeys failed: %s\n", SCErrorString(SCError())); goto error; }

	m->p->StoreRLS = SCDynamicStoreCreateRunLoopSource(NULL, store, 0);
	if (!m->p->StoreRLS) { LogMsg("SCDynamicStoreCreateRunLoopSource failed: %s\n", SCErrorString(SCError())); goto error; }

	CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
	m->p->Store = store;
	err = 0;
	goto exit;

error:
	if (store)    CFRelease(store);

exit:
	if (key1)     CFRelease(key1);
	if (key2)     CFRelease(key2);
	if (key3)     CFRelease(key3);
	if (key4)     CFRelease(key4);
	if (pattern1) CFRelease(pattern1);
	if (pattern2) CFRelease(pattern2);
	if (keys)     CFRelease(keys);
	if (patterns) CFRelease(patterns);
	
	return(err);
	}

mDNSlocal void PowerChanged(void *refcon, io_service_t service, natural_t messageType, void *messageArgument)
	{
	mDNS *const m = (mDNS *const)refcon;
	(void)service;    // Parameter not used
	switch(messageType)
		{
		case kIOMessageCanSystemPowerOff:     debugf("PowerChanged kIOMessageCanSystemPowerOff (no action)");                      break; // E0000240
		case kIOMessageSystemWillPowerOff:    debugf("PowerChanged kIOMessageSystemWillPowerOff"); mDNSCoreMachineSleep(m, true);  break; // E0000250
		case kIOMessageSystemWillNotPowerOff: debugf("PowerChanged kIOMessageSystemWillNotPowerOff (no action)");                  break; // E0000260
		case kIOMessageCanSystemSleep:        debugf("PowerChanged kIOMessageCanSystemSleep (no action)");                         break; // E0000270
		case kIOMessageSystemWillSleep:       debugf("PowerChanged kIOMessageSystemWillSleep");    mDNSCoreMachineSleep(m, true);  break; // E0000280
		case kIOMessageSystemWillNotSleep:    debugf("PowerChanged kIOMessageSystemWillNotSleep (no action)");                     break; // E0000290
		case kIOMessageSystemHasPoweredOn:    debugf("PowerChanged kIOMessageSystemHasPoweredOn"); mDNSCoreMachineSleep(m, false); break; // E0000300
		default:                              debugf("PowerChanged unknown message %X", messageType);                              break;
		}
	IOAllowPowerChange(m->p->PowerConnection, (long)messageArgument);
	}

mDNSlocal mStatus WatchForPowerChanges(mDNS *const m)
	{
	IONotificationPortRef thePortRef;
	m->p->PowerConnection = IORegisterForSystemPower(m, &thePortRef, PowerChanged, &m->p->PowerNotifier);
	if (m->p->PowerConnection)
		{
		m->p->PowerRLS = IONotificationPortGetRunLoopSource(thePortRef);
		CFRunLoopAddSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		return(mStatus_NoError);
		}
	return(-1);
	}

mDNSexport mDNSBool haveSecInfo = mDNSfalse;  // this must go away once we have full keychain integration
mDNSlocal void GetAuthInfoFromKeychainItem(mDNS *m, SecKeychainItemRef item)
	{
	OSStatus err;
	mDNSu32 infoTag = kSecAccountItemAttr;
	mDNSu32 infoFmt = 0; // string
    SecKeychainAttributeInfo info;		
	SecKeychainAttributeList *authAttrList = NULL; 
	void *data;
	mDNSu32 dataLen;
	
	mStatus regErr;		
	char accountName[MAX_ESCAPED_DOMAIN_NAME];
	domainname zone;
	AuthRecord *rrReg, *rrBrowse;

	info.count = 1;
	info.tag = &infoTag;
	info.format = &infoFmt;
		
	err = SecKeychainItemCopyAttributesAndData(item, &info, NULL, &authAttrList, &dataLen, &data);
	if (err) { LogMsg("SecKeychainItemCopyAttributesAndData returned error %d", err); return; }
	
	// copy account name
	if (!authAttrList->count || authAttrList->attr->tag != kSecAccountItemAttr)
	    { LogMsg("Received bad authAttrList"); return; }
	
	if (authAttrList->attr->length + strlen(LH_SUFFIX) > MAX_ESCAPED_DOMAIN_NAME)
		{ LogMsg("Account name too long (%d bytes)", authAttrList->attr->length); return; }
	memcpy(accountName, authAttrList->attr->data, authAttrList->attr->length);
	accountName[authAttrList->attr->length] = '\0';
	
	zone.c[0] = '\0';
	if (!AppendLiteralLabelString(&zone, accountName) ||
		!AppendDNSNameString(&zone, LH_SUFFIX))
		{ LogMsg("InitAuthInfo - bad account name"); return; }
	
	mDNS_UpdateDomainRequiresAuthentication(m, &zone, &zone, data, dataLen, mDNStrue);
	if(m->uDNS_info.NameRegDomain) { debugf("Overwriting config file options with KeyChain values"); }
	
	if (!ConvertDomainNameToCString(&zone, m->uDNS_info.NameRegDomain) ||
		!ConvertDomainNameToCString(&zone, m->uDNS_info.ServiceRegDomain))
		{ LogMsg("Couldn't set keychain username in uDNS global info"); }
	
	mDNS_GenerateGlobalFQDN(m);
	// normally we'd query the zone for _register/_browse domains, but to reduce server load we manually generate the records

	haveSecInfo = mDNStrue;
	//!!!KRS need to do better bookkeeping once we support multiple users
	rrReg = mallocL("AuthRecord", sizeof(AuthRecord));
	rrBrowse = mallocL("AuthRecord", sizeof(AuthRecord));
	if (!rrReg || !rrBrowse) { LogMsg("ERROR: Malloc"); return; }
	
	// set up _browse
	mDNS_SetupResourceRecord(rrBrowse, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, mDNSNULL, mDNSNULL);
	MakeDomainNameFromDNSNameString(&rrBrowse->resrec.name, "_browse._dns-sd._udp.local.");
	strcpy(rrBrowse->resrec.rdata->u.name.c, zone.c);
	
	// set up _register
	mDNS_SetupResourceRecord(rrReg, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, mDNSNULL, mDNSNULL);
	MakeDomainNameFromDNSNameString(&rrReg->resrec.name, "_register._dns-sd._udp.local.");
	strcpy(rrReg->resrec.rdata->u.name.c, zone.c);
	
	regErr = mDNS_Register(m, rrReg);
	if (regErr) LogMsg("Registration of local-only reg domain %s failed", zone.c);
	
	regErr = mDNS_Register(m, rrBrowse);
	if (regErr) LogMsg("Registration of local-only browse domain %s failed", zone.c);
	SecKeychainItemFreeContent(authAttrList, data);
	}

mDNSlocal void InitAuthInfo(mDNS *m);

mDNSlocal OSStatus KeychainCallback(SecKeychainEvent event, SecKeychainCallbackInfo *info, void *context)
	{
	(void)event;
	(void)info;
	// unused
	
	debugf("SecKeychainAddCallback received event %d", event);
	InitAuthInfo((mDNS *)context);  // keychain events happen rarely - just rebuild the list
	return 0;
	}

mDNSexport void InitAuthInfo(mDNS *m)
	{
	OSStatus err;
	
	SecKeychainSearchRef searchRef = NULL;
	SecKeychainRef sysKeychain = NULL;
	SecKeychainAttribute searchAttrs[] = { { kSecDescriptionItemAttr, strlen(LH_KEYCHAIN_DESC), LH_KEYCHAIN_DESC },
									  { kSecServiceItemAttr, strlen(LH_KEYCHAIN_SERVICE), LH_KEYCHAIN_SERVICE } };	
	SecKeychainAttributeList searchList = { sizeof(searchAttrs) / sizeof(*searchAttrs), searchAttrs };
	SecKeychainItemRef item;

	// clear any previous entries
	mDNS_ClearAuthenticationList(m);

	err = SecKeychainOpen(SYS_KEYCHAIN_PATH, &sysKeychain);
	if (err) { LogMsg("ERROR: InitAuthInfo - couldn't open system keychain - %d", err); goto release_refs; }
	err = SecKeychainSetDomainDefault(kSecPreferencesDomainSystem, sysKeychain);
	if (err) { LogMsg("ERROR: InitAuthInfo - couldn't set domain default for system keychain - %d", err); goto release_refs; }
	
	err = SecKeychainSearchCreateFromAttributes(sysKeychain, kSecGenericPasswordItemClass, &searchList, &searchRef);
	if (err) { LogMsg("ERROR: InitAuthInfo - SecKeychainSearchCreateFromAttributes %d", err); goto release_refs; }

	while (!SecKeychainSearchCopyNext(searchRef, &item))
		{
		GetAuthInfoFromKeychainItem(m, item);
		CFRelease(item);
		}
	err = SecKeychainAddCallback(KeychainCallback, kSecAddEventMask | kSecDeleteEventMask | kSecUpdateEventMask | kSecPasswordChangedEventMask, m);	
	if (err && err != errSecDuplicateCallback) { LogMsg("SecKeychainAddCallback returned error %d", err); }							 	

	release_refs:
	
	if (searchRef) CFRelease(searchRef);
	if (sysKeychain) CFRelease(sysKeychain);
	}

CF_EXPORT CFDictionaryRef _CFCopySystemVersionDictionary(void);
CF_EXPORT const CFStringRef _kCFSystemVersionProductNameKey;
CF_EXPORT const CFStringRef _kCFSystemVersionProductVersionKey;
CF_EXPORT const CFStringRef _kCFSystemVersionBuildVersionKey;

mDNSexport mDNSBool mDNSMacOSXSystemBuildNumber(char *HINFO_SWstring)
	{
	int major = 0, minor = 0;
	char letter = 0, prodname[256]="Mac OS X", prodvers[256]="", buildver[256]="?";
	CFDictionaryRef vers = _CFCopySystemVersionDictionary();
	if (vers)
		{
		CFStringRef cfprodname = CFDictionaryGetValue(vers, _kCFSystemVersionProductNameKey);
		CFStringRef cfprodvers = CFDictionaryGetValue(vers, _kCFSystemVersionProductVersionKey);
		CFStringRef cfbuildver = CFDictionaryGetValue(vers, _kCFSystemVersionBuildVersionKey);
		if (cfprodname) CFStringGetCString(cfprodname, prodname, sizeof(prodname), kCFStringEncodingUTF8);
		if (cfprodvers) CFStringGetCString(cfprodvers, prodvers, sizeof(prodvers), kCFStringEncodingUTF8);
		if (cfbuildver) CFStringGetCString(cfbuildver, buildver, sizeof(buildver), kCFStringEncodingUTF8);
		sscanf(buildver, "%d%c%d", &major, &letter, &minor);
		CFRelease(vers);
		}
	if (HINFO_SWstring) mDNS_snprintf(HINFO_SWstring, 256, "%s %s (%s), %s", prodname, prodvers, buildver, mDNSResponderVersionString);
	return(major);
	}

// Test to see if we're the first client running on UDP port 5353, by trying to bind to 5353 without using SO_REUSEPORT.
// If we fail, someone else got here first. That's not a big problem; we can share the port for multicast responses --
// we just need to be aware that we shouldn't expect to successfully receive unicast UDP responses.
mDNSlocal mDNSBool mDNSPlatformInit_ReceiveUnicast(void)
	{
	int err;
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in s5353;
	s5353.sin_family      = AF_INET;
	s5353.sin_port        = MulticastDNSPort.NotAnInteger;
	s5353.sin_addr.s_addr = 0;
	err = bind(s, (struct sockaddr *)&s5353, sizeof(s5353));
	close(s);
	if (err) debugf("No unicast UDP responses");
	else     debugf("Unicast UDP responses okay");
	return(err == 0);
	}


//!!!KRS this should be less order-dependent as we support more configuration options
mDNSlocal mDNSBool GetConfigOption(char *dst, const char *option, FILE *f)
	{
	char buf[1024];
	int len;
	
	if (!fgets(buf, 1024, f)) { LogMsg("Option %s not set", option); return mDNSfalse; }
	len = strlen(option);
	if (!strncmp(buf, option, len))
		{
		strcpy(dst, buf + len + 1);
		len = strlen(dst);
		if ( len && dst[len-1] == '\n') dst[len-1] = '\0';  // chop newline
		return mDNStrue;
		}
	LogMsg("Malformatted config file - %s not set", option);	
	return mDNSfalse;
	}
	

	
mDNSlocal void ReadRegDomainFromConfig(mDNS *const m)
	{
	FILE *f;
	uDNS_GlobalInfo *u = &m->uDNS_info;;
	char key[MAX_ESCAPED_DOMAIN_NAME];
	domainname key_d, name_d, service_d;
	char secret[1024];
	int slen;
	mStatus err;

    // read registration domain (for dynamic updates) from config file
    // !!!KRS these must go away once we can learn the reg domain from the network or prefs	
	if (m->uDNS_info.NameRegDomain[0] || m->uDNS_info.ServiceRegDomain[0])
		{ debugf("Options from config already set via keychain. Ignoring config file."); return; }
	
	f = fopen(CONFIG_FILE, "r");
	if (!f)
		{
		if (errno != ENOENT)  LogMsg("ERROR: Config file exists, but cannot be opened.");
		return;
		}

	if (!GetConfigOption(u->NameRegDomain, "name-reg", f)) goto end;
	if (!GetConfigOption(u->ServiceRegDomain, "service-reg", f)) goto end;
	if (!GetConfigOption(key, "key-name", f)) goto end;
	if (!GetConfigOption(secret, "secret-64", f)) { LogMsg("ERROR: config file contains key without secret"); goto end; }

	// we don't actually need this in domain-name format - just convert it to error check
	if (!MakeDomainNameFromDNSNameString(&service_d, u->ServiceRegDomain))
		{ LogMsg("ERROR: config file contains bad service reg domain %s", u->ServiceRegDomain); u->ServiceRegDomain[0] = '\0'; }	

	if (!MakeDomainNameFromDNSNameString(&name_d, u->NameRegDomain))
		{ LogMsg("ERROR: config file contains bad name reg domain %s", u->NameRegDomain); u->NameRegDomain[0] = '\0'; }	

	if (!MakeDomainNameFromDNSNameString(&key_d, key))
		{ LogMsg("ERROR: config file contains bad key %s", key); key[0] = '\0'; }

	if (key[0])
		{
		slen = strlen(secret);
		if (u->ServiceRegDomain[0]) 
			{
			err = mDNS_UpdateDomainRequiresAuthentication(m, &service_d, &key_d, secret, slen, mDNStrue);
			if (err) LogMsg("ERROR: mDNS_UpdateDomainRequiresAuthentication returned %d for domain ", err, u->ServiceRegDomain);
			}
		if (u->NameRegDomain[0])
			{
			err = mDNS_UpdateDomainRequiresAuthentication(m, &name_d, &key_d, secret, slen, mDNStrue);
			if (err) LogMsg("ERROR: mDNS_UpdateDomainRequiresAuthentication returned %d for domain ", err, u->NameRegDomain);
			}
		}
	
	end:
	fclose(f);
	}

mDNSexport DNameListElem *mDNSPlatformGetSearchDomainList(void)
	{
	return mDNS_CopyDNameList(DefBrowseList);
	}

mDNSexport DNameListElem *mDNSPlatformGetRegDomainList(void)
	{
	static DNameListElem tmp;
	static mDNSBool init = mDNSfalse;

	if (!init)
		{
		MakeDomainNameFromDNSNameString(&tmp.name, "local.");
		tmp.next = NULL;
		init = mDNStrue;
		}
	return mDNS_CopyDNameList(&tmp);
	}

	
mDNSlocal void FoundDefBrowseDomain(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
	{
	DNameListElem *ptr, *prev, *new;
	(void)m; // unused;
	(void)question;  // unused

	if (AddRecord)
		{
		new = mallocL("FoundDefBrowseDomain", sizeof(DNameListElem));
		if (!new) { LogMsg("ERROR: malloc"); return; }
		strcpy(new->name.c, answer->rdata->u.name.c);
		new->next = DefBrowseList;
		DefBrowseList = new;
		return;
		}
	else
		{
		ptr = DefBrowseList;
		prev = NULL;
		while (ptr)
			{
			if (SameDomainName(&ptr->name, &answer->rdata->u.name))
				{
				if (prev) prev->next = ptr->next;
				else DefBrowseList = ptr->next;
				freeL("FoundDefBrowseDomain", ptr);
				return;
				}
			prev = ptr;
			ptr = ptr->next;
			}
		LogMsg("FoundDefBrowseDomain: Got remove event for domain %s not in list", answer->rdata->u.name.c);
		}    
	}

// Construction of Default Browse domain list (i.e. when clients pass NULL) is as follows:
// 1) query for _browse._dns-sd._udp.local on LocalOnly interface
//    (.local manually generated via explicit callback)
// 2) for each search domain (from prefs pane), query for _browse._dns-sd._udp.<searchdomain>.
// 3) for each result from (2), register LocalOnly PTR record_browse._dns-sd._udp.local. -> <result>
// 4) result above should generate a callback from question in (1).  result added to global list
// 5) global list delivered to client via GetSearchDomainList()
// 6) client calls to enumerate domains now go over LocalOnly interface
//    (!!!KRS may add outgoing interface in addition)

mDNSlocal mStatus InitDNSConfig(mDNS *const m)
	{
	mStatus err;
	AuthRecord local;	
	DNSConfigInitialized = mDNStrue;

	// start query for domains to be used in default (empty string domain) browses
	err = mDNS_GetDomains(m, &DefBrowseDomainQ, mDNS_DomainTypeBrowse, NULL, mDNSInterface_LocalOnly, FoundDefBrowseDomain, NULL);

	// provide .local automatically
	mDNS_SetupResourceRecord(&local, mDNSNULL, mDNSInterface_LocalOnly, kDNSType_PTR, 7200,  kDNSRecordTypeShared, mDNSNULL, mDNSNULL);
	MakeDomainNameFromDNSNameString(&local.resrec.name, "_browse._dns-sd._udp.local.");
	MakeDomainNameFromDNSNameString(&local.resrec.rdata->u.name, "local.");
	// other fields ignored
	FoundDefBrowseDomain(m, &DefBrowseDomainQ, &local.resrec, 1);

    return mStatus_NoError;
	}
	
mDNSlocal mStatus mDNSPlatformInit_setup(mDNS *const m)
	{
	mStatus err;

   	m->hostlabel.c[0]        = 0;
	
	char *HINFO_HWstring = "Macintosh";
	char HINFO_HWstring_buffer[256];
	int    get_model[2] = { CTL_HW, HW_MODEL };
	size_t len_model = sizeof(HINFO_HWstring_buffer);
	if (sysctl(get_model, 2, HINFO_HWstring_buffer, &len_model, NULL, 0) == 0)
		HINFO_HWstring = HINFO_HWstring_buffer;

	char HINFO_SWstring[256] = "";
	if (mDNSMacOSXSystemBuildNumber(HINFO_SWstring) < 7) m->KnownBugs |= mDNS_KnownBug_PhantomInterfaces;
	if (mDNSPlatformInit_ReceiveUnicast())               m->CanReceiveUnicast = mDNStrue;

	mDNSu32 hlen = mDNSPlatformStrLen(HINFO_HWstring);
	mDNSu32 slen = mDNSPlatformStrLen(HINFO_SWstring);
	if (hlen + slen < 254)
		{
		m->HIHardware.c[0] = hlen;
		m->HISoftware.c[0] = slen;
		mDNSPlatformMemCopy(HINFO_HWstring, &m->HIHardware.c[1], hlen);
		mDNSPlatformMemCopy(HINFO_SWstring, &m->HISoftware.c[1], slen);
		}

 	m->p->unicastsockets.m     = m;
	m->p->unicastsockets.info  = NULL;
	m->p->unicastsockets.sktv4 = m->p->unicastsockets.sktv6 = -1;
	m->p->unicastsockets.cfsv4 = m->p->unicastsockets.cfsv6 = NULL;
	m->p->unicastsockets.rlsv4 = m->p->unicastsockets.rlsv6 = NULL;
	
	err = SetupSocket(&m->p->unicastsockets, zeroIPPort, &zeroAddr, AF_INET);
	err = SetupSocket(&m->p->unicastsockets, zeroIPPort, &zeroAddr, AF_INET6);

	m->p->InterfaceList      = mDNSNULL;
	m->p->userhostlabel.c[0] = 0;
	UpdateInterfaceList(m);
	SetupActiveInterfaces(m);

	err = WatchForNetworkChanges(m);
	if (err) return(err);
	
	err = WatchForPowerChanges(m);
	if (err) return err;
	
	err = WatchForDNSChanges(m);

	InitDNSConfig(m);

	m->uDNS_info.ServiceRegDomain[0] = '\0';
	m->uDNS_info.NameRegDomain[0] = '\0';
	InitAuthInfo(m);
	ReadRegDomainFromConfig(m);	
	
 	return(err);
	}

mDNSexport mStatus mDNSPlatformInit(mDNS *const m)
	{
	mStatus result = mDNSPlatformInit_setup(m);
	
	// We don't do asynchronous initialization on OS X, so by the time we get here the setup will already
	// have succeeded or failed -- so if it succeeded, we should just call mDNSCoreInitComplete() immediately
	if (result == mStatus_NoError) mDNSCoreInitComplete(m, mStatus_NoError);
	return(result);
	}

mDNSexport void mDNSPlatformClose(mDNS *const m)
	{
	if (m->p->PowerConnection)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->PowerRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->PowerRLS);
		CFRelease(m->p->PowerRLS);
		IODeregisterForSystemPower(&m->p->PowerNotifier);
		m->p->PowerConnection = NULL;
		m->p->PowerNotifier   = NULL;
		m->p->PowerRLS        = NULL;
		}
	
	if (m->p->Store)
		{
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(), m->p->StoreRLS, kCFRunLoopDefaultMode);
		CFRunLoopSourceInvalidate(m->p->StoreRLS);
		CFRelease(m->p->StoreRLS);
		CFRelease(m->p->Store);
		m->p->Store    = NULL;
		m->p->StoreRLS = NULL;
		}
	
	MarkAllInterfacesInactive(m);
	ClearInactiveInterfaces(m);
	CloseSocketSet(&m->p->unicastsockets);
	}

mDNSexport mDNSs32  mDNSPlatformOneSecond = 1000;

mDNSexport mStatus mDNSPlatformTimeInit(mDNSs32 *timenow)
	{
	// Notes: Typical values for mach_timebase_info:
	// tbi.numer = 1000 million
	// tbi.denom =   33 million
	// These are set such that (mach_absolute_time() * numer/denom) gives us nanoseconds;
	//          numer  / denom = nanoseconds per hardware clock tick (e.g. 30);
	//          denom  / numer = hardware clock ticks per nanosecond (e.g. 0.033)
	// (denom*1000000) / numer = hardware clock ticks per millisecond (e.g. 33333)
	// So: mach_absolute_time() / ((denom*1000000)/numer) = milliseconds
	//
	// Arithmetic notes:
	// tbi.denom is at least 1, and not more than 2^32-1.
	// Therefore (tbi.denom * 1000000) is at least one million, but cannot overflow a uint64_t.
	// tbi.denom is at least 1, and not more than 2^32-1.
	// Therefore clockdivisor should end up being a number roughly in the range 10^3 - 10^9.
	// If clockdivisor is less than 10^3 then that means that the native clock frequency is less than 1MHz,
	// which is unlikely on any current or future Macintosh.
	// If clockdivisor is greater than 10^9 then that means the native clock frequency is greater than 1000GHz.
	// When we ship Macs with clock frequencies above 1000GHz, we may have to update this code.
	struct mach_timebase_info tbi;
	kern_return_t result = mach_timebase_info(&tbi);
	if (result != KERN_SUCCESS) return(result);
	clockdivisor = ((uint64_t)tbi.denom * 1000000) / tbi.numer;
	*timenow = mDNSPlatformTimeNow();
	return(mStatus_NoError);
	}

mDNSexport mDNSs32  mDNSPlatformTimeNow(void)
	{
	if (clockdivisor == 0) { LogMsg("mDNSPlatformTimeNow called before mDNSPlatformTimeInit"); return(0); }
	return((mDNSs32)(mach_absolute_time() / clockdivisor));
	}

mDNSexport mDNSs32 mDNSPlatformUTC(void)
	{
	return time(NULL);
	}

// Locking is a no-op here, because we're single-threaded with a CFRunLoop, so we can never interrupt ourselves
mDNSexport void     mDNSPlatformLock   (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformUnlock (const mDNS *const m) { (void)m; }
mDNSexport void     mDNSPlatformStrCopy(const void *src,       void *dst)              { strcpy((char *)dst, (char *)src); }
mDNSexport mDNSu32  mDNSPlatformStrLen (const void *src)                               { return(strlen((char*)src)); }
mDNSexport void     mDNSPlatformMemCopy(const void *src,       void *dst, mDNSu32 len) { memcpy(dst, src, len); }
mDNSexport mDNSBool mDNSPlatformMemSame(const void *src, const void *dst, mDNSu32 len) { return(memcmp(dst, src, len) == 0); }
mDNSexport void     mDNSPlatformMemZero(                       void *dst, mDNSu32 len) { bzero(dst, len); }
mDNSexport void *   mDNSPlatformMemAllocate(mDNSu32 len) { return(mallocL("mDNSPlatformMemAllocate", len)); }
mDNSexport void     mDNSPlatformMemFree    (void *mem)   { freeL("mDNSPlatformMemFree", mem); }
