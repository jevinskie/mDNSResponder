/*
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
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
    
$Log: mDNSWin32.c,v $
Revision 1.63  2004/11/23 03:39:47  cheshire
Let interface name/index mapping capability live directly in JNISupport.c,
instead of having to call through to the daemon via IPC to get this information.

Revision 1.62  2004/11/12 03:16:41  rpantos
rdar://problem/3809541 Add mDNSPlatformGetInterfaceByName, mDNSPlatformGetInterfaceName

Revision 1.61  2004/11/05 22:54:38  shersche
Change registry key flags from KEY_ALL_ACCESS to KEY_READ to support mDNSResponder running with limited access rights
Submitted by: Pavel Repin <prepin@gmail.com>

Revision 1.60  2004/11/05 22:41:56  shersche
Determine subnet mask when populating network interface data structures
Submitted by: Pavel Repin <prepin@gmail.com>
Reviewed by:

Revision 1.59  2004/10/28 03:24:42  cheshire
Rename m->CanReceiveUnicastOn as m->CanReceiveUnicastOn5353

Revision 1.58  2004/10/16 00:17:01  cheshire
<rdar://problem/3770558> Replace IP TTL 255 check with local subnet source address check

Revision 1.57  2004/10/11 21:53:15  shersche
<rdar://problem/3832450> Change GetWindowsVersionString link scoping from static to non-static so that it can be accessed from other compilation units. The information returned in this function will be used to determine what service dependencies to use when calling CreateService().
Bug #: 3832450

Revision 1.56  2004/09/26 23:20:36  ksekar
<rdar://problem/3813108> Allow default registrations in multiple wide-area domains

Revision 1.55  2004/09/21 21:02:57  cheshire
Set up ifname before calling mDNS_RegisterInterface()

Revision 1.54  2004/09/17 01:08:57  cheshire
Renamed mDNSClientAPI.h to mDNSEmbeddedAPI.h
  The name "mDNSClientAPI.h" is misleading to new developers looking at this code. The interfaces
  declared in that file are ONLY appropriate to single-address-space embedded applications.
  For clients on general-purpose computers, the interfaces defined in dns_sd.h should be used.

Revision 1.53  2004/09/17 00:19:11  cheshire
For consistency with AllDNSLinkGroupv6, rename AllDNSLinkGroup to AllDNSLinkGroupv4

Revision 1.52  2004/09/16 00:24:50  cheshire
<rdar://problem/3803162> Fix unsafe use of mDNSPlatformTimeNow()

Revision 1.51  2004/09/14 23:42:37  cheshire
<rdar://problem/3801296> Need to seed random number generator from platform-layer data

Revision 1.50  2004/08/25 23:36:56  shersche
<rdar://problem/3658379> Remove code that retrieves TTL from received packets
Bug #: 3658379

Revision 1.49  2004/08/25 16:43:29  ksekar
Fix Windows build - change mDNS_SetFQDNs to mDNS_SetFQDN, remove unicast
hostname parameter.

Revision 1.48  2004/08/14 03:22:43  cheshire
<rdar://problem/3762579> Dynamic DNS UI <-> mDNSResponder glue
Add GetUserSpecifiedDDNSName() routine
Convert ServiceRegDomain to domainname instead of C string
Replace mDNS_GenerateFQDN/mDNS_GenerateGlobalFQDN with mDNS_SetFQDNs

Revision 1.47  2004/08/06 17:33:02  shersche
<rdar://problem/3753797> Put correct length of string in first byte of nicelabel
Bug #: 3753797

Revision 1.46  2004/08/05 05:43:01  shersche
<rdar://problem/3751566> Add HostDescriptionChangedCallback so callers can choose to handle it when mDNSWin32 core detects that the computer description string has changed
Bug #: 3751566

Revision 1.45  2004/07/26 22:49:31  ksekar
<rdar://problem/3651409>: Feature #9516: Need support for NATPMP in client

Revision 1.44  2004/07/26 05:42:50  shersche
use "Computer Description" for nicename if available, track dynamic changes to "Computer Description"

Revision 1.43  2004/07/13 21:24:25  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.42  2004/06/24 15:23:24  shersche
Add InterfaceListChanged callback.  This callback is used in Service.c to add link local routes to the routing table
Submitted by: herscher

Revision 1.41  2004/06/18 05:22:16  rpantos
Integrate Scott's changes

Revision 1.40  2004/05/26 09:06:07  bradley
Retry while building the interface list if it returns an error since the two-step process required to
get the interface list could allow a subsequent interface change to come in that window and change the
needed size after getting the size, but before getting the list, causing it to return an error.
Fixed structure name typo in search domain list stuff. Fixed spelling error in global for GAA.

Revision 1.39  2004/05/18 23:51:27  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.38  2004/05/13 04:57:48  ksekar
Removed unnecessary FreeSearchList function

Revision 1.37  2004/05/13 04:54:20  ksekar
Unified list copy/free code.  Added symetric list for

Revision 1.36  2004/05/12 22:03:09  ksekar
Made GetSearchDomainList a true platform-layer call (declaration moved
from mDNSMacOSX.h to mDNSEmbeddedAPI.h), impelemted to return "local"
only on non-OSX platforms.  Changed call to return a copy of the list
to avoid shared memory issues.  Added a routine to free the list.

Revision 1.35  2004/04/21 02:49:12  cheshire
To reduce future confusion, renamed 'TxAndRx' to 'McastTxRx'

Revision 1.34  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.33  2004/04/14 23:09:29  ksekar
Support for TSIG signed dynamic updates.

Revision 1.32  2004/04/09 17:40:26  cheshire
Remove unnecessary "Multicast" field -- it duplicates the semantics of the existing McastTxRx field

Revision 1.31  2004/04/09 00:40:46  bradley
Re-enable IPv6 support, AAAA records over IPv4, and IPv4 routable IPv6 exclusion support.

Revision 1.30  2004/04/09 00:33:58  bradley
Turn on Multicast flag for interfaces to tell mDNSCore that the interfaces are multicast capable.

Revision 1.29  2004/03/15 02:07:46  bradley
Changed interface index handling to use the upper 24 bits for IPv4 and the lower 8 bits for IPv6 to
handle some IPv4 interface indexes that are greater than 16-bit. This is not perfect because Windows
does not provide a consistent index for IPv4 and IPv6, but it seems to handle the known cases.

Revision 1.28  2004/03/07 00:26:39  bradley
Allow non-NULL PlatformSupport ptr when initializing so non-Apple clients can provide their own storage.
Added count assert when building the wait list to catch underruns/overruns if the code is changed.

Revision 1.27  2004/01/30 02:44:32  bradley
Added support for IPv6 (v4 & v6, v4-only, v6-only, AAAA over v4, etc.). Added support for DNS-SD
InterfaceID<->Interface Index mappings. Added support for loopback usage when no other interfaces
are available. Updated unlock signaling to no longer require timenow - NextScheduledTime to be >= 0
(it no longer is). Added unicast-capable detection to avoid using unicast when there is other mDNS
software running on the same machine. Removed unneeded sock_XtoY routines. Added support for
reporting HINFO records with the  Windows and mDNSResponder version information.

Revision 1.26  2004/01/24 04:59:16  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.25  2003/11/14 20:59:09  cheshire
Clients can't use AssignDomainName macro because mDNSPlatformMemCopy is defined in mDNSPlatformFunctions.h.
Best solution is just to combine mDNSEmbeddedAPI.h and mDNSPlatformFunctions.h into a single file.

Revision 1.24  2003/10/24 23:23:02  bradley
Removed legacy port 53 support as it is no longer needed.

Revision 1.23  2003/10/14 03:26:12  bradley
Clear interface list buffer to workaround Windows CE bug where interfaces are not reported correctly.

Revision 1.22  2003/08/20 06:21:25  bradley
Updated to latest internal version of the mDNSWindows platform layer: Added support
for Windows CE/PocketPC 2003; re-did interface-related code to emulate getifaddrs/freeifaddrs for
restricting usage to only active, multicast-capable, and non-point-to-point interfaces and to ease
the addition of IPv6 support in the future; Changed init code to serialize thread initialization to
enable ThreadID improvement to wakeup notification; Define platform support structure locally to
allow portable mDNS_Init usage; Removed dependence on modified mDNSCore: define interface ID<->name
structures/prototypes locally; Changed to use _beginthreadex()/_endthreadex() on non-Windows CE
platforms (re-mapped to CreateThread on Window CE) to avoid a leak in the Microsoft C runtime;
Added IPv4/IPv6 string<->address conversion routines; Cleaned up some code and added HeaderDoc.

Revision 1.21  2003/08/18 23:09:57  cheshire
<rdar://problem/3382647> mDNSResponder divide by zero in mDNSPlatformRawTime()

Revision 1.20  2003/08/12 19:56:27  cheshire
Update to APSL 2.0

Revision 1.19  2003/08/05 23:58:18  cheshire
Update code to compile with the new mDNSCoreReceive() function that requires a TTL
Right now this platform layer just reports 255 instead of returning the real value -- we should fix this

Revision 1.18  2003/07/23 21:16:30  cheshire
Removed a couple of debugfs

Revision 1.17  2003/07/23 02:23:01  cheshire
Updated mDNSPlatformUnlock() to work correctly, now that <rdar://problem/3160248>
"ScheduleNextTask needs to be smarter" has refined the way m->NextScheduledEvent is set

Revision 1.16  2003/07/19 03:15:16  cheshire
Add generic MemAllocate/MemFree prototypes to mDNSPlatformFunctions.h,
and add the obvious trivial implementations to each platform support layer

Revision 1.15  2003/07/02 21:20:04  cheshire
<rdar://problem/3313413> Update copyright notices, etc., in source code comments

Revision 1.14  2003/05/26 03:21:30  cheshire
Tidy up address structure naming:
mDNSIPAddr         => mDNSv4Addr (for consistency with mDNSv6Addr)
mDNSAddr.addr.ipv4 => mDNSAddr.ip.v4
mDNSAddr.addr.ipv6 => mDNSAddr.ip.v6

Revision 1.13  2003/05/26 03:01:28  cheshire
<rdar://problem/3268904> sprintf/vsprintf-style functions are unsafe; use snprintf/vsnprintf instead

Revision 1.12  2003/05/06 21:06:05  cheshire
<rdar://problem/3242673> mDNSWindows needs a wakeupEvent object to signal the main thread

Revision 1.11  2003/05/06 00:00:51  cheshire
<rdar://problem/3248914> Rationalize naming of domainname manipulation functions

Revision 1.10  2003/04/29 00:06:09  cheshire
<rdar://problem/3242673> mDNSWindows needs a wakeupEvent object to signal the main thread

Revision 1.9  2003/04/26 02:40:01  cheshire
Add void LogMsg( const char *format, ... )

Revision 1.8  2003/03/22 02:57:44  cheshire
Updated mDNSWindows to use new "mDNS_Execute" model (see "mDNSCore/Implementer Notes.txt")

Revision 1.7  2003/03/15 04:40:38  cheshire
Change type called "mDNSOpaqueID" to the more descriptive name "mDNSInterfaceID"

Revision 1.6  2003/02/21 01:54:10  cheshire
<rdar://problem/3099194> mDNSResponder needs performance improvements
Switched to using new "mDNS_Execute" model (see "Implementer Notes.txt")

Revision 1.5  2003/02/20 00:59:03  cheshire
Brought Windows code up to date so it complies with
Josh Graessley's interface changes for IPv6 support.
(Actual support for IPv6 on Windows will come later.)

Revision 1.4  2002/09/21 20:44:54  zarzycki
Added APSL info

Revision 1.3  2002/09/20 05:50:45  bradley
Multicast DNS platform plugin for Win32

	To Do:
	
	- Get unicode name of machine for nice name instead of just the host name.
	- Use the IPv6 Internet Connection Firewall API to allow IPv6 mDNS without manually changing the firewall.
	- Get DNS server address(es) from Windows and provide them to the uDNS layer.
	- Implement TCP support for truncated packets (only stubs now).	
*/

#include	<stdarg.h>
#include	<stddef.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"CommonServices.h"
#include	"DebugServices.h"

#include	<Iphlpapi.h>
#if( !TARGET_OS_WINDOWS_CE )
	#include	<mswsock.h>
	#include	<process.h>
#endif

#include	"mDNSEmbeddedAPI.h"

#include	"mDNSWin32.h"

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

#define	DEBUG_NAME									"[mDNSWin32] "

#define	MDNS_WINDOWS_USE_IPV6_IF_ADDRS				1
#define	MDNS_WINDOWS_ENABLE_IPV4					1
#define	MDNS_WINDOWS_ENABLE_IPV6					1
#define	MDNS_WINDOWS_EXCLUDE_IPV4_ROUTABLE_IPV6		1
#define	MDNS_WINDOWS_AAAA_OVER_IPV4					1

#define	kMDNSDefaultName							"My Computer"

#define	kWinSockMajorMin							2
#define	kWinSockMinorMin							2

#define	kWaitListCancelEvent						( WAIT_OBJECT_0 + 0 )
#define	kWaitListInterfaceListChangedEvent			( WAIT_OBJECT_0 + 1 )
#define	kWaitListWakeupEvent						( WAIT_OBJECT_0 + 2 )
#define kWaitListRegEvent							( WAIT_OBJECT_0 + 3 )
#define	kWaitListFixedItemCount						4

#if( !TARGET_OS_WINDOWS_CE )
	static GUID										kWSARecvMsgGUID = WSAID_WSARECVMSG;
#endif

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

mDNSlocal mStatus			SetupSynchronizationObjects( mDNS * const inMDNS );
mDNSlocal mStatus			TearDownSynchronizationObjects( mDNS * const inMDNS );
mDNSlocal mStatus			SetupNiceName( mDNS * const inMDNS );
mDNSlocal mStatus			SetupHostName( mDNS * const inMDNS );
mDNSlocal mStatus			SetupName( mDNS * const inMDNS );
mDNSlocal mStatus			SetupInterfaceList( mDNS * const inMDNS );
mDNSlocal mStatus			TearDownInterfaceList( mDNS * const inMDNS );
mDNSlocal mStatus			SetupInterface( mDNS * const inMDNS, const struct ifaddrs *inIFA, mDNSInterfaceData **outIFD );
mDNSlocal mStatus			TearDownInterface( mDNS * const inMDNS, mDNSInterfaceData *inIFD );
mDNSlocal mStatus			SetupSocket( mDNS * const inMDNS, const struct sockaddr *inAddr, SocketRef *outSocketRef  );
mDNSlocal mStatus			SockAddrToMDNSAddr( const struct sockaddr * const inSA, mDNSAddr *outIP, mDNSIPPort *outPort );
mDNSlocal mStatus			SetupNotifications( mDNS * const inMDNS );
mDNSlocal mStatus			TearDownNotifications( mDNS * const inMDNS );

mDNSlocal mStatus			SetupThread( mDNS * const inMDNS );
mDNSlocal mStatus			TearDownThread( const mDNS * const inMDNS );
mDNSlocal unsigned WINAPI	ProcessingThread( LPVOID inParam );
mDNSlocal mStatus 			ProcessingThreadInitialize( mDNS * const inMDNS );
mDNSlocal mStatus			ProcessingThreadSetupWaitList( mDNS * const inMDNS, HANDLE **outWaitList, int *outWaitListCount );
mDNSlocal void				ProcessingThreadProcessPacket( mDNS *inMDNS, mDNSInterfaceData *inIFD, SocketRef inSock );
mDNSlocal void				ProcessingThreadInterfaceListChanged( mDNS *inMDNS );
mDNSlocal void				ProcessingThreadRegistryChanged( mDNS * inMDNS );

// Platform Accessors

#ifdef	__cplusplus
	extern "C" {
#endif

typedef struct mDNSPlatformInterfaceInfo	mDNSPlatformInterfaceInfo;
struct	mDNSPlatformInterfaceInfo
{
	const char *		name;
	mDNSAddr			ip;
};

mDNSexport mStatus	mDNSPlatformInterfaceNameToID( mDNS * const inMDNS, const char *inName, mDNSInterfaceID *outID );
mDNSexport mStatus	mDNSPlatformInterfaceIDToInfo( mDNS * const inMDNS, mDNSInterfaceID inID, mDNSPlatformInterfaceInfo *outInfo );

// Utilities

#if( MDNS_WINDOWS_USE_IPV6_IF_ADDRS )
	mDNSlocal int	getifaddrs_ipv6( struct ifaddrs **outAddrs );
	mDNSlocal int	getifnetmask_ipv6( struct ifaddrs * ifa );
#endif

#if( !TARGET_OS_WINDOWS_CE )
	mDNSlocal int	getifaddrs_ipv4( struct ifaddrs **outAddrs );
#endif

#if( TARGET_OS_WINDOWS_CE )
	mDNSlocal int	getifaddrs_ce( struct ifaddrs **outAddrs );
#endif

mDNSlocal mDNSBool	CanReceiveUnicast( void );

#ifdef	__cplusplus
	}
#endif

#if 0
#pragma mark == Globals ==
#endif

//===========================================================================================================================
//	Globals
//===========================================================================================================================

mDNSlocal mDNS_PlatformSupport		gMDNSPlatformSupport;
mDNSs32								mDNSPlatformOneSecond = 0;

#if( MDNS_WINDOWS_USE_IPV6_IF_ADDRS )

	typedef DWORD
		( WINAPI * GetAdaptersAddressesFunctionPtr )( 
			ULONG 					inFamily, 
			DWORD 					inFlags, 
			PVOID 					inReserved, 
			PIP_ADAPTER_ADDRESSES 	inAdapter, 
			PULONG					outBufferSize );

	mDNSlocal HMODULE								gIPHelperLibraryInstance			= NULL;
	mDNSlocal GetAdaptersAddressesFunctionPtr		gGetAdaptersAddressesFunctionPtr	= NULL;

#endif

#if 0
#pragma mark -
#pragma mark == Platform Support ==
#endif

//===========================================================================================================================
//	mDNSPlatformInit
//===========================================================================================================================

mStatus	mDNSPlatformInit( mDNS * const inMDNS )
{
	mStatus		err;
	WSADATA		wsaData;
	int			supported;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "platform init\n" );
	
	// Initialize variables. If the PlatformSupport pointer is not null then just assume that a non-Apple client is 
	// calling mDNS_Init and wants to provide its own storage for the platform-specific data so do not overwrite it.
	
	memset( &gMDNSPlatformSupport, 0, sizeof( gMDNSPlatformSupport ) );
	if( !inMDNS->p ) inMDNS->p				= &gMDNSPlatformSupport;
	inMDNS->p->interfaceListChangedSocket	= kInvalidSocketRef;
	mDNSPlatformOneSecond 					= 1000;		// Use milliseconds as the quantum of time
	
	// Startup WinSock 2.2 or later.
	
	err = WSAStartup( MAKEWORD( kWinSockMajorMin, kWinSockMinorMin ), &wsaData );
	require_noerr( err, exit );
	
	supported = ( ( LOBYTE( wsaData.wVersion ) == kWinSockMajorMin ) && ( HIBYTE( wsaData.wVersion ) == kWinSockMinorMin ) );
	require_action( supported, exit, err = mStatus_UnsupportedErr );
	
	inMDNS->CanReceiveUnicastOn5353 = CanReceiveUnicast();
	
	// Setup the HINFO HW/SW strings.
	
	err = GetWindowsVersionString( (char *) &inMDNS->HIHardware.c[ 1 ], sizeof( inMDNS->HIHardware.c ) - 2 );
	check_noerr( err );
	inMDNS->HIHardware.c[ 0 ] = (mDNSu8) mDNSPlatformStrLen( &inMDNS->HIHardware.c[ 1 ] );
	dlog( kDebugLevelInfo, DEBUG_NAME "HIHardware: %#s\n", inMDNS->HIHardware.c );
	
	mDNS_snprintf( (char *) &inMDNS->HISoftware.c[ 1 ], sizeof( inMDNS->HISoftware.c ) - 2, 
		"mDNSResponder (%s %s)", __DATE__, __TIME__ );
	inMDNS->HISoftware.c[ 0 ] = (mDNSu8) mDNSPlatformStrLen( &inMDNS->HISoftware.c[ 1 ] );
	dlog( kDebugLevelInfo, DEBUG_NAME "HISoftware: %#s\n", inMDNS->HISoftware.c );
	
	// Set up the mDNS thread.
	
	err = SetupSynchronizationObjects( inMDNS );
	require_noerr( err, exit );
	
	err = SetupThread( inMDNS );
	require_noerr( err, exit );
	
	// Success!
	
	mDNSCoreInitComplete( inMDNS, err );
	
exit:
	if( err )
	{
		mDNSPlatformClose( inMDNS );
	}
	dlog( kDebugLevelTrace, DEBUG_NAME "platform init done (err=%d %m)\n", err, err );
	return( err );
}

//===========================================================================================================================
//	mDNSPlatformClose
//===========================================================================================================================

void	mDNSPlatformClose( mDNS * const inMDNS )
{
	mStatus		err;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "platform close\n" );
	check( inMDNS );
	
	// Tear everything down in reverse order to how it was set up.
		
	err = TearDownThread( inMDNS );
	check_noerr( err );
	
	err = TearDownInterfaceList( inMDNS );
	check_noerr( err );
	check( !inMDNS->p->inactiveInterfaceList );
		
	err = TearDownSynchronizationObjects( inMDNS );
	check_noerr( err );

	// Free the DLL needed for IPv6 support.
	
#if( MDNS_WINDOWS_USE_IPV6_IF_ADDRS )
	if( gIPHelperLibraryInstance )
	{
		gGetAdaptersAddressesFunctionPtr = NULL;
		
		FreeLibrary( gIPHelperLibraryInstance );
		gIPHelperLibraryInstance = NULL;
	}
#endif

	WSACleanup();
	
	dlog( kDebugLevelTrace, DEBUG_NAME "platform close done\n" );
}

//===========================================================================================================================
//	mDNSPlatformSendUDP
//===========================================================================================================================

mStatus
	mDNSPlatformSendUDP( 
		const mDNS * const			inMDNS, 
		const void * const	        inMsg, 
		const mDNSu8 * const		inMsgEnd, 
		mDNSInterfaceID 			inInterfaceID, 
		const mDNSAddr *			inDstIP, 
		mDNSIPPort 					inDstPort )
{
	mStatus						err;
	mDNSInterfaceData *			ifd;
	struct sockaddr_storage		addr;
	int							n;
	
	DEBUG_USE_ONLY( inMDNS );
	
	n = (int)( inMsgEnd - ( (const mDNSu8 * const) inMsg ) );
	check( inMDNS );
	check( inMsg );
	check( inMsgEnd );
	check( inInterfaceID );
	check( inDstIP );
	
	ifd = (mDNSInterfaceData *) inInterfaceID;
	require_action_quiet( ifd->interfaceInfo.McastTxRx, exit, err = mStatus_Invalid );					// Silent Interface
	require_action_quiet( inDstIP->type == ifd->interfaceInfo.ip.type, exit, err = mStatus_NoError );	// Wrong Type
	check( IsValidSocket( ifd->sock ) );
	
	dlog( kDebugLevelChatty, DEBUG_NAME "platform send %d bytes to %#a:%u\n", n, inDstIP, ntohs( inDstPort.NotAnInteger ) );
	
	if( inDstIP->type == mDNSAddrType_IPv4 )
	{
		struct sockaddr_in *		sa4;
		
		sa4						= (struct sockaddr_in *) &addr;
		sa4->sin_family			= AF_INET;
		sa4->sin_port			= inDstPort.NotAnInteger;
		sa4->sin_addr.s_addr	= inDstIP->ip.v4.NotAnInteger;
	}
	else if( inDstIP->type == mDNSAddrType_IPv6 )
	{
		struct sockaddr_in6 *		sa6;
		
		sa6					= (struct sockaddr_in6 *) &addr;
		sa6->sin6_family	= AF_INET6;
		sa6->sin6_port		= inDstPort.NotAnInteger;
		sa6->sin6_flowinfo	= 0;
		sa6->sin6_addr		= *( (struct in6_addr *) &inDstIP->ip.v6 );
		sa6->sin6_scope_id	= 0;	// Windows requires the scope ID to be zero. IPV6_MULTICAST_IF specifies interface.
	}
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "%s: dst is not an IPv4 or IPv6 address (type=%d)\n", __ROUTINE__, inDstIP->type );
		err = mStatus_BadParamErr;
		goto exit;
	}
	
	n = sendto( ifd->sock, (char *) inMsg, n, 0, (struct sockaddr *) &addr, sizeof( addr ) );
	err = translate_errno( n > 0, errno_compat(), kWriteErr );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	mDNSPlatformLock
//===========================================================================================================================

void	mDNSPlatformLock( const mDNS * const inMDNS )
{
	check( inMDNS );
	check( inMDNS->p->lockInitialized );
	
	EnterCriticalSection( &inMDNS->p->lock );
}

//===========================================================================================================================
//	mDNSPlatformUnlock
//===========================================================================================================================

void	mDNSPlatformUnlock( const mDNS * const inMDNS )
{
	check( inMDNS );
	check( inMDNS->p );
	check( inMDNS->p->lockInitialized );
	check( inMDNS->p->threadID );
	
	// Signal a wakeup event if when called from a task other than the mDNS task since if we are called from mDNS task, 
	// we'll loop back and call mDNS_Execute anyway. Signaling is needed to re-evaluate the wakeup via mDNS_Execute.
	
	if( GetCurrentThreadId() != inMDNS->p->threadID )
	{
		BOOL		wasSet;
		
		wasSet = SetEvent( inMDNS->p->wakeupEvent );
		check_translated_errno( wasSet, GetLastError(), kUnknownErr );
	}
	LeaveCriticalSection( &inMDNS->p->lock );
}

//===========================================================================================================================
//	mDNSPlatformStrCopy
//===========================================================================================================================

void	mDNSPlatformStrCopy( const void *inSrc, void *inDst )
{
	check( inSrc );
	check( inDst );
	
	strcpy( (char *) inDst, (const char*) inSrc );
}

//===========================================================================================================================
//	mDNSPlatformStrLen
//===========================================================================================================================

mDNSu32	mDNSPlatformStrLen( const void *inSrc )
{
	check( inSrc );
	
	return( (mDNSu32) strlen( (const char *) inSrc ) );
}

//===========================================================================================================================
//	mDNSPlatformMemCopy
//===========================================================================================================================

void	mDNSPlatformMemCopy( const void *inSrc, void *inDst, mDNSu32 inSize )
{
	check( inSrc );
	check( inDst );
	
	memcpy( inDst, inSrc, inSize );
}

//===========================================================================================================================
//	mDNSPlatformMemSame
//===========================================================================================================================

mDNSBool	mDNSPlatformMemSame( const void *inSrc, const void *inDst, mDNSu32 inSize )
{
	check( inSrc );
	check( inDst );
	
	return( (mDNSBool)( memcmp( inSrc, inDst, inSize ) == 0 ) );
}

//===========================================================================================================================
//	mDNSPlatformMemZero
//===========================================================================================================================

void	mDNSPlatformMemZero( void *inDst, mDNSu32 inSize )
{
	check( inDst );
	
	memset( inDst, 0, inSize );
}

//===========================================================================================================================
//	mDNSPlatformMemAllocate
//===========================================================================================================================

mDNSexport void *	mDNSPlatformMemAllocate( mDNSu32 inSize )
{
	void *		mem;
	
	check( inSize > 0 );
	
	mem = malloc( inSize );
	check( mem );
	
	return( mem );
}

//===========================================================================================================================
//	mDNSPlatformMemFree
//===========================================================================================================================

mDNSexport void	mDNSPlatformMemFree( void *inMem )
{
	check( inMem );
	
	free( inMem );
}

//===========================================================================================================================
//	mDNSPlatformRandomSeed
//===========================================================================================================================

mDNSexport mDNSu32 mDNSPlatformRandomSeed(void)
{
	return( GetTickCount() );
}

//===========================================================================================================================
//	mDNSPlatformTimeInit
//===========================================================================================================================

mDNSexport mStatus	mDNSPlatformTimeInit( void )
{
	// No special setup is required on Windows -- we just use GetTickCount().
	return( mStatus_NoError );
}

//===========================================================================================================================
//	mDNSPlatformRawTime
//===========================================================================================================================

mDNSs32	mDNSPlatformRawTime( void )
{
	return( (mDNSs32) GetTickCount() );
}

//===========================================================================================================================
//	mDNSPlatformUTC
//===========================================================================================================================

mDNSexport mDNSs32	mDNSPlatformUTC( void )
{
	return( -1 );
}

//===========================================================================================================================
//	mDNSPlatformInterfaceNameToID
//===========================================================================================================================

mStatus	mDNSPlatformInterfaceNameToID( mDNS * const inMDNS, const char *inName, mDNSInterfaceID *outID )
{
	mStatus					err;
	mDNSInterfaceData *		ifd;
	
	check( inMDNS );
	check( inMDNS->p );
	check( inName );
	
	// Search for an interface with the specified name,
	
	for( ifd = inMDNS->p->interfaceList; ifd; ifd = ifd->next )
	{
		if( strcmp( ifd->name, inName ) == 0 )
		{
			break;
		}
	}
	require_action_quiet( ifd, exit, err = mStatus_NoSuchNameErr );
	
	// Success!
	
	if( outID )
	{
		*outID = (mDNSInterfaceID) ifd;
	}
	err = mStatus_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	mDNSPlatformInterfaceIDToInfo
//===========================================================================================================================

mStatus	mDNSPlatformInterfaceIDToInfo( mDNS * const inMDNS, mDNSInterfaceID inID, mDNSPlatformInterfaceInfo *outInfo )
{
	mStatus					err;
	mDNSInterfaceData *		ifd;
	
	check( inMDNS );
	check( inID );
	check( outInfo );
	
	// Search for an interface with the specified ID,
	
	for( ifd = inMDNS->p->interfaceList; ifd; ifd = ifd->next )
	{
		if( ifd == (mDNSInterfaceData *) inID )
		{
			break;
		}
	}
	require_action_quiet( ifd, exit, err = mStatus_NoSuchNameErr );
	
	// Success!
	
	outInfo->name 	= ifd->name;
	outInfo->ip 	= ifd->interfaceInfo.ip;
	err 			= mStatus_NoError;
	
exit:
	return( err );
}

//===========================================================================================================================
//	mDNSPlatformInterfaceIDfromInterfaceIndex
//===========================================================================================================================

mDNSInterfaceID	mDNSPlatformInterfaceIDfromInterfaceIndex( const mDNS * const inMDNS, mDNSu32 inIndex )
{
	mDNSInterfaceID		id;
	
	id = mDNSNULL;
	if( inIndex == (mDNSu32) ~0 )
	{
		id = mDNSInterface_LocalOnly;
	}
	else if( inIndex != 0 )
	{
		mDNSInterfaceData *		ifd;
		
		for( ifd = inMDNS->p->interfaceList; ifd; ifd = ifd->next )
		{
			if( ( ifd->scopeID == inIndex ) && ifd->interfaceInfo.InterfaceActive )
			{
				id = ifd->interfaceInfo.InterfaceID;
				break;
			}
		}
		check( ifd );
	}
	return( id );
}

//===========================================================================================================================
//	mDNSPlatformInterfaceIndexfromInterfaceID
//===========================================================================================================================
	
mDNSu32	mDNSPlatformInterfaceIndexfromInterfaceID( const mDNS * const inMDNS, mDNSInterfaceID inID )
{
	mDNSu32		index;
	
	index = 0;
	if( inID == mDNSInterface_LocalOnly )
	{
		index = (mDNSu32) ~0;
	}
	else if( inID )
	{
		mDNSInterfaceData *		ifd;
		
		// Search active interfaces.
		for( ifd = inMDNS->p->interfaceList; ifd; ifd = ifd->next )
		{
			if( (mDNSInterfaceID) ifd == inID )
			{
				index = ifd->scopeID;
				break;
			}
		}
		
		// Search inactive interfaces too so remove events for inactive interfaces report the old interface index.
		
		if( !ifd )
		{
			for( ifd = inMDNS->p->inactiveInterfaceList; ifd; ifd = ifd->next )
			{
				if( (mDNSInterfaceID) ifd == inID )
				{
					index = ifd->scopeID;
					break;
				}
			}
		}
		check( ifd );
	}
	return( index );
}

//===========================================================================================================================
//	mDNSPlatformTCPConnect
//===========================================================================================================================

mStatus
	mDNSPlatformTCPConnect( 
		const mDNSAddr *		inDstIP, 
		mDNSOpaque16 			inDstPort, 
		mDNSInterfaceID			inInterfaceID,
		TCPConnectionCallback	inCallback, 
		void *					inContext, 
		int *					outSock )
{
	DEBUG_UNUSED( inDstIP );
	DEBUG_UNUSED( inDstPort );
	DEBUG_UNUSED( inInterfaceID );
	DEBUG_UNUSED( inCallback );
	DEBUG_UNUSED( inContext );
	DEBUG_UNUSED( outSock );
	
	return( mStatus_UnsupportedErr );
}

//===========================================================================================================================
//	mDNSPlatformTCPCloseConnection
//===========================================================================================================================

void	mDNSPlatformTCPCloseConnection( int inSock )
{
	DEBUG_UNUSED( inSock );
}

//===========================================================================================================================
//	mDNSPlatformReadTCP
//===========================================================================================================================

int	mDNSPlatformReadTCP( int inSock, void *inBuffer, int inBufferSize )
{
	DEBUG_UNUSED( inSock );
	DEBUG_UNUSED( inBuffer );
	DEBUG_UNUSED( inBufferSize );
	
	return( -1 );
}

//===========================================================================================================================
//	mDNSPlatformWriteTCP
//===========================================================================================================================

int	mDNSPlatformWriteTCP( int inSock, const char *inMsg, int inMsgSize )
{
	DEBUG_UNUSED( inSock );
	DEBUG_UNUSED( inMsg );
	DEBUG_UNUSED( inMsgSize );
	
	return( -1 );
}


//===========================================================================================================================
//	mDNSPlatformGetSearchDomainList
//===========================================================================================================================


mDNSexport DNameListElem *mDNSPlatformGetSearchDomainList(void)
	{
	static DNameListElem tmp;
	static int init = 0;

	if (!init)
		{
		MakeDomainNameFromDNSNameString(&tmp.name, "local.");
		tmp.next = NULL;
		init = 1;
		}
	return mDNS_CopyDNameList(&tmp);
	}

//===========================================================================================================================
//	mDNSPlatformGetRegDomainList
//===========================================================================================================================

mDNSexport DNameListElem *mDNSPlatformGetRegDomainList(void)
	{
	return NULL;
	}


#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	debugf_
//===========================================================================================================================

#if( MDNS_DEBUGMSGS )
void	debugf_( const char *inFormat, ... )
{
	char		buffer[ 512 ];
    va_list		args;
    mDNSu32		length;
	
	va_start( args, inFormat );
	length = mDNS_vsnprintf( buffer, sizeof( buffer ), inFormat, args );
	va_end( args );
	
	dlog( kDebugLevelInfo, "%s\n", buffer );
}
#endif

//===========================================================================================================================
//	verbosedebugf_
//===========================================================================================================================

#if( MDNS_DEBUGMSGS > 1 )
void	verbosedebugf_( const char *inFormat, ... )
{
	char		buffer[ 512 ];
    va_list		args;
    mDNSu32		length;
	
	va_start( args, inFormat );
	length = mDNS_vsnprintf( buffer, sizeof( buffer ), inFormat, args );
	va_end( args );
	
	dlog( kDebugLevelVerbose, "%s\n", buffer );
}
#endif

//===========================================================================================================================
//	LogMsg
//===========================================================================================================================

/*
void	LogMsg( const char *inFormat, ... )
{
	char		buffer[ 512 ];
    va_list		args;
    mDNSu32		length;
	
	va_start( args, inFormat );
	length = mDNS_vsnprintf( buffer, sizeof( buffer ), inFormat, args );
	va_end( args );
	
	dlog( kDebugLevelWarning, "%s\n", buffer );
}
*/

#if 0
#pragma mark -
#pragma mark == Platform Internals  ==
#endif

//===========================================================================================================================
//	SetupSynchronizationObjects
//===========================================================================================================================

mDNSlocal mStatus	SetupSynchronizationObjects( mDNS * const inMDNS )
{
	mStatus		err;
		
	InitializeCriticalSection( &inMDNS->p->lock );
	inMDNS->p->lockInitialized = mDNStrue;
	
	inMDNS->p->cancelEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inMDNS->p->cancelEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	inMDNS->p->quitEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inMDNS->p->quitEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	inMDNS->p->interfaceListChangedEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inMDNS->p->interfaceListChangedEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	inMDNS->p->wakeupEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inMDNS->p->wakeupEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
exit:
	if( err )
	{
		TearDownSynchronizationObjects( inMDNS );
	}
	return( err );
}

//===========================================================================================================================
//	TearDownSynchronizationObjects
//===========================================================================================================================

mDNSlocal mStatus	TearDownSynchronizationObjects( mDNS * const inMDNS )
{
	if( inMDNS->p->quitEvent )
	{
		CloseHandle( inMDNS->p->quitEvent );
		inMDNS->p->quitEvent = 0;
	}
	if( inMDNS->p->cancelEvent )
	{
		CloseHandle( inMDNS->p->cancelEvent );
		inMDNS->p->cancelEvent = 0;
	}
	if( inMDNS->p->interfaceListChangedEvent )
	{
		CloseHandle( inMDNS->p->interfaceListChangedEvent );
		inMDNS->p->interfaceListChangedEvent = 0;
	}
	if( inMDNS->p->wakeupEvent )
	{
		CloseHandle( inMDNS->p->wakeupEvent );
		inMDNS->p->wakeupEvent = 0;
	}
	if( inMDNS->p->lockInitialized )
	{
		DeleteCriticalSection( &inMDNS->p->lock );
		inMDNS->p->lockInitialized = mDNSfalse;
	}
	return( mStatus_NoError );
}


//===========================================================================================================================
//	SetupNiceName
//===========================================================================================================================

mDNSlocal mStatus	SetupNiceName( mDNS * const inMDNS )
{
	mStatus		err = 0;
	char		tempString[ 256 ];
	
	check( inMDNS );
	
	// Set up the nice name.
	tempString[ 0 ] = '\0';

	// First try and open the registry key that contains the computer description value
	if (inMDNS->p->regKey == NULL)
	{
		const char * s = "SYSTEM\\CurrentControlSet\\Services\\lanmanserver\\parameters";
		err = RegOpenKeyEx( HKEY_LOCAL_MACHINE, s, 0, KEY_READ, &inMDNS->p->regKey);
		check_translated_errno( err == 0, errno_compat(), kNameErr );

		if (err)
		{
			inMDNS->p->regKey = NULL;
		}
	}

	// if we opened it...
	if (inMDNS->p->regKey != NULL)
	{
		DWORD type;
		DWORD valueLen = sizeof(tempString);

		// look for the computer description
		err = RegQueryValueEx(inMDNS->p->regKey, "srvcomment", 0, &type, (LPBYTE) &tempString, &valueLen);
		check_translated_errno( err == 0, errno_compat(), kNameErr );
	}

	// if we can't find it in the registry, then use the hostname of the machine
	if (err || ( tempString[ 0] == '\0' ) )
	{
		err = gethostname( tempString, sizeof( tempString ) - 1 );
		check_translated_errno( err == 0, errno_compat(), kNameErr );
	}

	// if we can't get the hostname
	if( err || ( tempString[ 0 ] == '\0' ) )
	{
		// Invalidate name so fall back to a default name.
		
		strcpy( tempString, kMDNSDefaultName );
	}

	tempString[ sizeof( tempString ) - 1 ] = '\0';
	
	inMDNS->nicelabel.c[ 0 ] = (mDNSu8) (strlen( tempString ) < MAX_DOMAIN_LABEL ? strlen( tempString ) : MAX_DOMAIN_LABEL);
	memcpy( &inMDNS->nicelabel.c[ 1 ], tempString, inMDNS->nicelabel.c[ 0 ] );
	
	dlog( kDebugLevelInfo, DEBUG_NAME "nice name \"%.*s\"\n", inMDNS->nicelabel.c[ 0 ], &inMDNS->nicelabel.c[ 1 ] );
	
	return( err );
}


//===========================================================================================================================
//	SetupHostName
//===========================================================================================================================

mDNSlocal mStatus	SetupHostName( mDNS * const inMDNS )
{
	mStatus		err = 0;
	char		tempString[ 256 ];
	domainlabel tempLabel;
	
	check( inMDNS );

	// Set up the nice name.
	tempString[ 0 ] = '\0';

	// use the hostname of the machine
	err = gethostname( tempString, sizeof( tempString ) - 1 );
	check_translated_errno( err == 0, errno_compat(), kNameErr );

	// if we can't get the hostname
	if( err || ( tempString[ 0 ] == '\0' ) )
	{
		// Invalidate name so fall back to a default name.
		
		strcpy( tempString, kMDNSDefaultName );
	}

	tempString[ sizeof( tempString ) - 1 ] = '\0';
	tempLabel.c[ 0 ] = (mDNSu8) (strlen( tempString ) < MAX_DOMAIN_LABEL ? strlen( tempString ) : MAX_DOMAIN_LABEL );
	memcpy( &tempLabel.c[ 1 ], tempString, tempLabel.c[ 0 ] );
	
	// Set up the host name.
	
	ConvertUTF8PstringToRFC1034HostLabel( tempLabel.c, &inMDNS->hostlabel );
	if( inMDNS->hostlabel.c[ 0 ] == 0 )
	{
		// Nice name has no characters that are representable as an RFC1034 name (e.g. Japanese) so use the default.
		
		MakeDomainLabelFromLiteralString( &inMDNS->hostlabel, kMDNSDefaultName );
	}

	check( inMDNS->hostlabel.c[ 0 ] != 0 );
	
	mDNS_SetFQDN( inMDNS );
	
	dlog( kDebugLevelInfo, DEBUG_NAME "host name \"%.*s\"\n", inMDNS->hostlabel.c[ 0 ], &inMDNS->hostlabel.c[ 1 ] );
	
	return( err );
}

//===========================================================================================================================
//	SetupName
//===========================================================================================================================

mDNSlocal mStatus	SetupName( mDNS * const inMDNS )
{
	mStatus		err = 0;
	
	check( inMDNS );
	
	err = SetupNiceName( inMDNS );
	check_noerr( err );

	err = SetupHostName( inMDNS );
	check_noerr( err );

	return err;
}


//===========================================================================================================================
//	SetupInterfaceList
//===========================================================================================================================

mDNSlocal mStatus	SetupInterfaceList( mDNS * const inMDNS )
{
	mStatus						err;
	mDNSInterfaceData **		next;
	mDNSInterfaceData *			ifd;
	struct ifaddrs *			addrs;
	struct ifaddrs *			p;
	struct ifaddrs *			loopback;
	u_int						flagMask;
	u_int						flagTest;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up interface list\n" );
	check( inMDNS );
	check( inMDNS->p );
	
	addrs = NULL;
	
	// Tear down any existing interfaces that may be set up.
	
	TearDownInterfaceList( inMDNS );

	// Set up the name of this machine.
	
	err = SetupName( inMDNS );
	check_noerr( err );
	
	// Set up the interface list change notification.
	
	err = SetupNotifications( inMDNS );
	check_noerr( err );
	
	// Set up IPv4 interface(s). We have to set up IPv4 first so any IPv6 interface with an IPv4-routable address
	// can refer to the IPv4 interface when it registers to allow DNS AAAA records over the IPv4 interface.
	
	err = getifaddrs( &addrs );
	require_noerr( err, exit );
	
	loopback	= NULL;
	next		= &inMDNS->p->interfaceList;
	
	flagMask = IFF_UP | IFF_MULTICAST | IFF_POINTTOPOINT;
	flagTest = IFF_UP | IFF_MULTICAST;
	
#if( MDNS_WINDOWS_ENABLE_IPV4 )
	for( p = addrs; p; p = p->ifa_next )
	{
		if( !p->ifa_addr || ( p->ifa_addr->sa_family != AF_INET ) || ( ( p->ifa_flags & flagMask ) != flagTest ) )
		{
			continue;
		}
		if( p->ifa_flags & IFF_LOOPBACK )
		{
			if( !loopback )
			{
				loopback = p;
			}
			continue;
		}
		dlog( kDebugLevelVerbose, DEBUG_NAME "Interface %40s (0x%08X) %##a\n", 
			p->ifa_name ? p->ifa_name : "<null>", p->ifa_extra.index, p->ifa_addr );
		
		err = SetupInterface( inMDNS, p, &ifd );
		require_noerr( err, exit );
						
		*next = ifd;
		next  = &ifd->next;
		++inMDNS->p->interfaceCount;
	}
#endif
	
	// Set up IPv6 interface(s) after IPv4 is set up (see IPv4 notes above for reasoning).
	
#if( MDNS_WINDOWS_ENABLE_IPV6 )
	for( p = addrs; p; p = p->ifa_next )
	{
		if( !p->ifa_addr || ( p->ifa_addr->sa_family != AF_INET6 ) || ( ( p->ifa_flags & flagMask ) != flagTest ) )
		{
			continue;
		}
		if( p->ifa_flags & IFF_LOOPBACK )
		{
			if( !loopback )
			{
				loopback = p;
			}
			continue;
		}
		dlog( kDebugLevelVerbose, DEBUG_NAME "Interface %40s (0x%08X) %##a\n", 
			p->ifa_name ? p->ifa_name : "<null>", p->ifa_extra.index, p->ifa_addr );
		
		err = SetupInterface( inMDNS, p, &ifd );
		require_noerr( err, exit );
						
		*next = ifd;
		next  = &ifd->next;
		++inMDNS->p->interfaceCount;
	}
#endif

	// If there are no real interfaces, but there is a loopback interface, use that so same-machine operations work.

#if( !MDNS_WINDOWS_ENABLE_IPV4 && !MDNS_WINDOWS_ENABLE_IPV6 )
	
	flagMask |= IFF_LOOPBACK;
	flagTest |= IFF_LOOPBACK;
	
	for( p = addrs; p; p = p->ifa_next )
	{
		if( !p->ifa_addr || ( ( p->ifa_flags & flagMask ) != flagTest ) )
		{
			continue;
		}
		if( ( p->ifa_addr->sa_family != AF_INET ) && ( p->ifa_addr->sa_family != AF_INET6 ) )
		{
			continue;
		}
		loopback = p;
		break;
	}
	
#endif
	
	if( !inMDNS->p->interfaceList && loopback )
	{
		dlog( kDebugLevelVerbose, DEBUG_NAME "Interface %40s (0x%08X) %##a\n", 
			loopback->ifa_name ? loopback->ifa_name : "<null>", loopback->ifa_extra.index, loopback->ifa_addr );
		
		err = SetupInterface( inMDNS, loopback, &ifd );
		require_noerr( err, exit );
		
		*next = ifd;
		next  = &ifd->next;
		++inMDNS->p->interfaceCount;
	}
	
exit:
	if( err )
	{
		TearDownInterfaceList( inMDNS );
	}
	if( addrs )
	{
		freeifaddrs( addrs );
	}
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up interface list done (err=%d %m)\n", err, err );
	return( err );
}

//===========================================================================================================================
//	TearDownInterfaceList
//===========================================================================================================================

mDNSlocal mStatus	TearDownInterfaceList( mDNS * const inMDNS )
{
	mStatus					err;
	mDNSInterfaceData **		p;
	mDNSInterfaceData *		ifd;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "tearing down interface list\n" );
	check( inMDNS );
	check( inMDNS->p );
	
	// Free any interfaces that were previously marked inactive and are no longer referenced by the mDNS cache.
	// Interfaces are marked inactive, but not deleted immediately if they were still referenced by the mDNS cache
	// so that remove events that occur after an interface goes away can still report the correct interface.

	p = &inMDNS->p->inactiveInterfaceList;
	while( *p )
	{
		ifd = *p;
		if( NumCacheRecordsForInterfaceID( inMDNS, (mDNSInterfaceID) ifd ) > 0 )
		{
			p = &ifd->next;
			continue;
		}
		
		dlog( kDebugLevelInfo, DEBUG_NAME "freeing unreferenced, inactive interface %#p %#a\n", ifd, &ifd->interfaceInfo.ip );
		*p = ifd->next;
		free( ifd );
	}
	
	// Tear down interface list change notifications.
	
	err = TearDownNotifications( inMDNS );
	check_noerr( err );
	
	// Tear down all the interfaces.
	
	while( inMDNS->p->interfaceList )
	{
		ifd = inMDNS->p->interfaceList;
		inMDNS->p->interfaceList = ifd->next;
		
		TearDownInterface( inMDNS, ifd );
	}
	inMDNS->p->interfaceCount = 0;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "tearing down interface list done\n" );
	return( mStatus_NoError );
}

//===========================================================================================================================
//	SetupInterface
//===========================================================================================================================

mDNSlocal mStatus	SetupInterface( mDNS * const inMDNS, const struct ifaddrs *inIFA, mDNSInterfaceData **outIFD )
{
	mStatus					err;
	mDNSInterfaceData *		ifd;
	SocketRef				sock;
	
	ifd = NULL;
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up interface\n" );
	check( inMDNS );
	check( inMDNS->p );
	check( inIFA );
	check( inIFA->ifa_addr );
	check( outIFD );
	
	// Allocate memory for the interface and initialize it.
	
	ifd = (mDNSInterfaceData *) calloc( 1, sizeof( *ifd ) );
	require_action( ifd, exit, err = mStatus_NoMemoryErr );
	ifd->sock		= kInvalidSocketRef;
	ifd->index		= inIFA->ifa_extra.index;
	ifd->scopeID	= inIFA->ifa_extra.index;
	
	check( strlen( inIFA->ifa_name ) < sizeof( ifd->name ) );
	strncpy( ifd->name, inIFA->ifa_name, sizeof( ifd->name ) - 1 );
	ifd->name[ sizeof( ifd->name ) - 1 ] = '\0';
	
	strncpy(ifd->interfaceInfo.ifname, inIFA->ifa_name, sizeof(ifd->interfaceInfo.ifname));
	ifd->interfaceInfo.ifname[sizeof(ifd->interfaceInfo.ifname)-1] = 0;
	
	// We always send and receive using IPv4, but to reduce traffic, we send and receive using IPv6 only on interfaces 
	// that have no routable IPv4 address. Having a routable IPv4 address assigned is a reasonable indicator of being 
	// on a large configured network, which means there's a good chance that most or all the other devices on that 
	// network should also have v4. By doing this we lose the ability to talk to true v6-only devices on that link, 
	// but we cut the packet rate in half. At this time, reducing the packet rate is more important than v6-only 
	// devices on a large configured network, so we are willing to make that sacrifice.
	
	ifd->interfaceInfo.McastTxRx		= mDNStrue;
	
#if( MDNS_WINDOWS_EXCLUDE_IPV4_ROUTABLE_IPV6 )
	if( inIFA->ifa_addr->sa_family != AF_INET )
	{
		const mDNSInterfaceData *		p;
		
		for( p = inMDNS->p->interfaceList; p; p = p->next )
		{
			if( ( p->interfaceInfo.ip.type == mDNSAddrType_IPv4 ) &&
				( ( p->interfaceInfo.ip.ip.v4.b[ 0 ] != 169 ) && ( p->interfaceInfo.ip.ip.v4.b[ 1 ] != 254 ) ) &&
				( strcmp( p->name, inIFA->ifa_name ) == 0 ) )
			{
				ifd->interfaceInfo.McastTxRx = mDNSfalse;
				break;
			}
		}
	}
#endif

	// If this is an IPv6 interface, search for its IPv4 equivalent and use that InterfaceID. This causes the IPv4
	// interface to send both A and AAAA records so we can publish IPv6 support without doubling the packet rate.
	// Note: this search only works because we register all IPv4 interfaces before IPv6 interfaces.
	
	ifd->interfaceInfo.InterfaceID = (mDNSInterfaceID) ifd;
	
#if( MDNS_WINDOWS_AAAA_OVER_IPV4 )
	if( inIFA->ifa_addr->sa_family != AF_INET )
	{
		mDNSInterfaceData *		ipv4IFD;
		
		for( ipv4IFD = inMDNS->p->interfaceList; ipv4IFD; ipv4IFD = ipv4IFD->next )
		{
			if( strcmp( ipv4IFD->name, ifd->name ) == 0 )
			{
				ipv4IFD->scopeID				= ifd->scopeID;
				ifd->interfaceInfo.McastTxRx	= mDNSfalse;
				ifd->interfaceInfo.InterfaceID	= (mDNSInterfaceID) ipv4IFD;
				break;
			}
		}
	}
#endif
	
	// Set up a socket for this interface (if needed).
	
	if( ifd->interfaceInfo.McastTxRx )
	{
		err = SetupSocket( inMDNS, inIFA->ifa_addr, &sock );
		require_noerr( err, exit );
		ifd->sock = sock;
		ifd->defaultAddr = ( inIFA->ifa_addr->sa_family == AF_INET6 ) ? AllDNSLinkGroup_v6 : AllDNSLinkGroup_v4;
		
		// Get a ptr to the WSARecvMsg function, if supported. Otherwise, we'll fallback to recvfrom.

		#if( !TARGET_OS_WINDOWS_CE )
		{
			DWORD		size;

			err = WSAIoctl( sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &kWSARecvMsgGUID, sizeof( kWSARecvMsgGUID ),
				&ifd->wsaRecvMsgFunctionPtr, sizeof( ifd->wsaRecvMsgFunctionPtr ), &size, NULL, NULL );
			if( err != 0 )
			{
				ifd->wsaRecvMsgFunctionPtr = NULL;
			}
		}
		#endif

		// Set up the read pending event and associate it so we can block until data is available for this socket.
		
		ifd->readPendingEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		err = translate_errno( ifd->readPendingEvent, (mStatus) GetLastError(), kUnknownErr );
		require_noerr( err, exit );
		
		err = WSAEventSelect( ifd->sock, ifd->readPendingEvent, FD_READ );
		require_noerr( err, exit );
	}
	else
	{
		// Create a placeholder event so WaitForMultipleObjects Handle slot for this interface is valid.
		
		ifd->readPendingEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
		err = translate_errno( ifd->readPendingEvent, (mStatus) GetLastError(), kUnknownErr );
		require_noerr( err, exit );
	}
	
	// Register this interface with mDNS.
	
	err = SockAddrToMDNSAddr( inIFA->ifa_addr, &ifd->interfaceInfo.ip, NULL );
	require_noerr( err, exit );
	
	err = SockAddrToMDNSAddr( inIFA->ifa_netmask, &ifd->interfaceInfo.mask, NULL );
	require_noerr( err, exit );
	
	ifd->interfaceInfo.Advertise = inMDNS->AdvertiseLocalAddresses;
	
	err = mDNS_RegisterInterface( inMDNS, &ifd->interfaceInfo );
	require_noerr( err, exit );
	ifd->hostRegistered = mDNStrue;
	
	dlog( kDebugLevelInfo, DEBUG_NAME "Registered interface %##a with mDNS\n", inIFA->ifa_addr );
	
	// Success!
	
	*outIFD = ifd;
	ifd = NULL;
	
exit:
	if( ifd )
	{
		TearDownInterface( inMDNS, ifd );
	}
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up interface done (err=%d %m)\n", err, err );
	return( err );
}

//===========================================================================================================================
//	TearDownInterface
//===========================================================================================================================

mDNSlocal mStatus	TearDownInterface( mDNS * const inMDNS, mDNSInterfaceData *inIFD )
{
	SocketRef		sock;
	
	check( inMDNS );
	check( inIFD );
	
	// Deregister this interface with mDNS.
	
	dlog( kDebugLevelInfo, DEBUG_NAME "Deregistering interface %#a with mDNS\n", &inIFD->interfaceInfo.ip );
	
	if( inIFD->hostRegistered )
	{
		inIFD->hostRegistered = mDNSfalse;
		mDNS_DeregisterInterface( inMDNS, &inIFD->interfaceInfo );
	}
	
	// Tear down the multicast socket.
	
	if( inIFD->readPendingEvent )
	{
		CloseHandle( inIFD->readPendingEvent );
		inIFD->readPendingEvent = 0;
	}
	
	sock = inIFD->sock;
	inIFD->sock = kInvalidSocketRef;
	if( IsValidSocket( sock ) )
	{
		close_compat( sock );
	}
	
	// If the interface is still referenced by items in the mDNS cache then put it on the inactive list. This keeps 
	// the InterfaceID valid so remove events report the correct interface. If it is no longer referenced, free it.

	if( NumCacheRecordsForInterfaceID( inMDNS, (mDNSInterfaceID) inIFD ) > 0 )
	{
		inIFD->next = inMDNS->p->inactiveInterfaceList;
		inMDNS->p->inactiveInterfaceList = inIFD;
		dlog( kDebugLevelInfo, DEBUG_NAME "deferring free of interface %#p %#a\n", inIFD, &inIFD->interfaceInfo.ip );
	}
	else
	{
		dlog( kDebugLevelInfo, DEBUG_NAME "freeing interface %#p %#a immediately\n", inIFD, &inIFD->interfaceInfo.ip );
		free( inIFD );
	}
	return( mStatus_NoError );
}

//===========================================================================================================================
//	SetupSocket
//===========================================================================================================================

mDNSlocal mStatus	SetupSocket( mDNS * const inMDNS, const struct sockaddr *inAddr, SocketRef *outSocketRef  )
{
	mStatus			err;
	SocketRef		sock;
	int				option;
	
	DEBUG_UNUSED( inMDNS );
	
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up socket %##a\n", inAddr );
	check( inMDNS );
	check( outSocketRef );
	
	// Set up an IPv4 or IPv6 UDP socket.
	
	sock = socket( inAddr->sa_family, SOCK_DGRAM, IPPROTO_UDP );
	err = translate_errno( IsValidSocket( sock ), errno_compat(), kUnknownErr );
	require_noerr( err, exit );
		
	// Turn on reuse address option so multiple servers can listen for Multicast DNS packets.
	
	option = 1;
	err = setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof( option ) );
	check_translated_errno( err == 0, errno_compat(), kOptionErr );
	
	if( inAddr->sa_family == AF_INET )
	{
		mDNSv4Addr				ipv4;
		struct sockaddr_in		sa4;
		struct ip_mreq			mreqv4;
		
		// Bind to the multicast DNS port 5353.
		
		ipv4.NotAnInteger 	= ( (const struct sockaddr_in *) inAddr )->sin_addr.s_addr;
		memset( &sa4, 0, sizeof( sa4 ) );
		sa4.sin_family 		= AF_INET;
		sa4.sin_port 		= MulticastDNSPort.NotAnInteger;
		sa4.sin_addr.s_addr	= ipv4.NotAnInteger;
		
		err = bind( sock, (struct sockaddr *) &sa4, sizeof( sa4 ) );
		check_translated_errno( err == 0, errno_compat(), kUnknownErr );
		
		// Turn on option to receive destination addresses and receiving interface.
		
		option = 1;
		err = setsockopt( sock, IPPROTO_IP, IP_PKTINFO, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Join the all-DNS multicast group so we receive Multicast DNS packets.
		
		mreqv4.imr_multiaddr.s_addr = AllDNSLinkGroupv4.NotAnInteger;
		mreqv4.imr_interface.s_addr = ipv4.NotAnInteger;
		err = setsockopt( sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreqv4, sizeof( mreqv4 ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Specify the interface to send multicast packets on this socket.
		
		sa4.sin_addr.s_addr = ipv4.NotAnInteger;
		err = setsockopt( sock, IPPROTO_IP, IP_MULTICAST_IF, (char *) &sa4.sin_addr, sizeof( sa4.sin_addr ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Send unicast packets with TTL 255 (helps against spoofing).
		
		option = 255;
		err = setsockopt( sock, IPPROTO_IP, IP_TTL, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Send multicast packets with TTL 255 (helps against spoofing).
		
		option = 255;
		err = setsockopt( sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Enable multicast loopback so we receive multicast packets we send (for same-machine operations).
		
		option = 1;
		err = setsockopt( sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
	}
	else if( inAddr->sa_family == AF_INET6 )
	{
		struct sockaddr_in6 *		sa6p;
		struct sockaddr_in6			sa6;
		struct ipv6_mreq			mreqv6;
		
		sa6p = (struct sockaddr_in6 *) inAddr;
		
		// Bind to the multicast DNS port 5353.
		
		memset( &sa6, 0, sizeof( sa6 ) );
		sa6.sin6_family		= AF_INET6;
		sa6.sin6_port		= MulticastDNSPort.NotAnInteger;
		sa6.sin6_flowinfo	= 0;
		sa6.sin6_addr		= sa6p->sin6_addr;
		sa6.sin6_scope_id	= sa6p->sin6_scope_id;
		
		err = bind( sock, (struct sockaddr *) &sa6, sizeof( sa6 ) );
		check_translated_errno( err == 0, errno_compat(), kUnknownErr );
		
		// Turn on option to receive destination addresses and receiving interface.
		
		option = 1;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_PKTINFO, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// We only want to receive IPv6 packets (not IPv4-mapped IPv6 addresses) because we have a separate socket 
		// for IPv4, but the IPv6 stack in Windows currently doesn't support IPv4-mapped IPv6 addresses and doesn't
		// support the IPV6_V6ONLY socket option so the following code would typically not be executed (or needed).
		
		#if( defined( IPV6_V6ONLY ) )
			option = 1;
			err = setsockopt( sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &option, sizeof( option ) );
			check_translated_errno( err == 0, errno_compat(), kOptionErr );		
		#endif
		
		// Join the all-DNS multicast group so we receive Multicast DNS packets.
		
		mreqv6.ipv6mr_multiaddr = *( (struct in6_addr *) &AllDNSLinkGroupv6 );
		mreqv6.ipv6mr_interface = sa6p->sin6_scope_id;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *) &mreqv6, sizeof( mreqv6 ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Specify the interface to send multicast packets on this socket.
		
		option = (int) sa6p->sin6_scope_id;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Send unicast packets with TTL 255 (helps against spoofing).
		
		option = 255;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Send multicast packets with TTL 255 (helps against spoofing).
		
		option = 255;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
		
		// Enable multicast loopback so we receive multicast packets we send (for same-machine operations).
		
		option = 1;
		err = setsockopt( sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char *) &option, sizeof( option ) );
		check_translated_errno( err == 0, errno_compat(), kOptionErr );
	}
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "%s: unsupport socket family (%d)\n", __ROUTINE__, inAddr->sa_family );
		err = kUnsupportedErr;
		goto exit;
	}
	
	// Success!
	
	*outSocketRef = sock;
	sock = kInvalidSocketRef;
	err = mStatus_NoError;
	
exit:
	if( IsValidSocket( sock ) )
	{
		close_compat( sock );
	}
	return( err );
}

//===========================================================================================================================
//	SetupSocket
//===========================================================================================================================

mDNSlocal mStatus	SockAddrToMDNSAddr( const struct sockaddr * const inSA, mDNSAddr *outIP, mDNSIPPort *outPort )
{
	mStatus		err;
	
	check( inSA );
	check( outIP );
	
	if( inSA->sa_family == AF_INET )
	{
		struct sockaddr_in *		sa4;
		
		sa4 						= (struct sockaddr_in *) inSA;
		outIP->type 				= mDNSAddrType_IPv4;
		outIP->ip.v4.NotAnInteger	= sa4->sin_addr.s_addr;
		if( outPort )
		{
			outPort->NotAnInteger	= sa4->sin_port;
		}
		err = mStatus_NoError;
	}
	else if( inSA->sa_family == AF_INET6 )
	{
		struct sockaddr_in6 *		sa6;
		
		sa6 			= (struct sockaddr_in6 *) inSA;
		outIP->type 	= mDNSAddrType_IPv6;
		outIP->ip.v6 	= *( (mDNSv6Addr *) &sa6->sin6_addr );
		if( IN6_IS_ADDR_LINKLOCAL( &sa6->sin6_addr ) )
		{
			outIP->ip.v6.w[ 1 ] = 0;
		}
		if( outPort )
		{
			outPort->NotAnInteger = sa6->sin6_port;
		}
		err = mStatus_NoError;
	}
	else
	{
		dlog( kDebugLevelError, DEBUG_NAME "%s: invalid sa_family %d", __ROUTINE__, inSA->sa_family );
		err = mStatus_BadParamErr;
	}
	return( err );
}

//===========================================================================================================================
//	SetupNotifications
//===========================================================================================================================

mDNSlocal mStatus	SetupNotifications( mDNS * const inMDNS )
{
	mStatus				err;
	SocketRef			sock;
	unsigned long		param;
	int					inBuffer;
	int					outBuffer;
	DWORD				outSize;
	
	// Register to listen for address list changes.
	
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	err = translate_errno( IsValidSocket( sock ), errno_compat(), kUnknownErr );
	require_noerr( err, exit );
	inMDNS->p->interfaceListChangedSocket = sock;
	
	// Make the socket non-blocking so the WSAIoctl returns immediately with WSAEWOULDBLOCK. It will set the event 
	// when a change to the interface list is detected.
	
	param = 1;
	err = ioctlsocket( sock, FIONBIO, &param );
	err = translate_errno( err == 0, errno_compat(), kUnknownErr );
	require_noerr( err, exit );
	
	inBuffer	= 0;
	outBuffer	= 0;
	err = WSAIoctl( sock, SIO_ADDRESS_LIST_CHANGE, &inBuffer, 0, &outBuffer, 0, &outSize, NULL, NULL );
	if( err < 0 )
	{
		check( errno_compat() == WSAEWOULDBLOCK );
	}
	
	err = WSAEventSelect( sock, inMDNS->p->interfaceListChangedEvent, FD_ADDRESS_LIST_CHANGE );
	err = translate_errno( err == 0, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	inMDNS->p->regEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	err = translate_errno( inMDNS->p->regEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

	if (inMDNS->p->regKey != NULL)
	{
		err = RegNotifyChangeKeyValue(inMDNS->p->regKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, inMDNS->p->regEvent, TRUE);
		require_noerr( err, exit );
	}

exit:
	if( err )
	{
		TearDownNotifications( inMDNS );
	}
	return( err );
}

//===========================================================================================================================
//	TearDownNotifications
//===========================================================================================================================

mDNSlocal mStatus	TearDownNotifications( mDNS * const inMDNS )
{
	if( IsValidSocket( inMDNS->p->interfaceListChangedSocket ) )
	{
		close_compat( inMDNS->p->interfaceListChangedSocket );
		inMDNS->p->interfaceListChangedSocket = kInvalidSocketRef;
	}

	if ( inMDNS->p->regEvent != NULL )
	{
		CloseHandle( inMDNS->p->regEvent );
		inMDNS->p->regEvent = NULL;
	}

	if ( inMDNS->p->regKey != NULL )
	{
		RegCloseKey( inMDNS->p->regKey );
		inMDNS->p->regKey = NULL;
	}

	return( mStatus_NoError );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	SetupThread
//===========================================================================================================================

mDNSlocal mStatus	SetupThread( mDNS * const inMDNS )
{
	mStatus			err;
	HANDLE			threadHandle;
	unsigned		threadID;
	DWORD			result;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up thread\n" );
	
	// To avoid a race condition with the thread ID needed by the unlocking code, we need to make sure the
	// thread has fully initialized. To do this, we create the thread then wait for it to signal it is ready.
	
	inMDNS->p->initEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	err = translate_errno( inMDNS->p->initEvent, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );

	inMDNS->p->initStatus = mStatus_Invalid;
	
	// Create thread with _beginthreadex() instead of CreateThread() to avoid memory leaks when using static run-time 
	// libraries. See <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp>.
	
	threadHandle = (HANDLE) _beginthreadex_compat( NULL, 0, ProcessingThread, inMDNS, 0, &threadID );
	err = translate_errno( threadHandle, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
		
	result = WaitForSingleObject( inMDNS->p->initEvent, INFINITE );
	err = translate_errno( result == WAIT_OBJECT_0, (mStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	err = inMDNS->p->initStatus;
	require_noerr( err, exit );
	
exit:
	if( inMDNS->p->initEvent )
	{
		CloseHandle( inMDNS->p->initEvent );
		inMDNS->p->initEvent = 0;
	}
	dlog( kDebugLevelTrace, DEBUG_NAME "setting up thread done (err=%d %m)\n", err, err );
	return( err );
}

//===========================================================================================================================
//	TearDownThread
//===========================================================================================================================

mDNSlocal mStatus	TearDownThread( const mDNS * const inMDNS )
{
	// Signal the cancel event to cause the thread to exit. Then wait for the quit event to be signal indicating it did 
	// exit. If the quit event is not signal in 5 seconds, just give up and close anyway sinec the thread is probably hung.
	
	if( inMDNS->p->cancelEvent )
	{
		BOOL		wasSet;
		DWORD		result;
		
		wasSet = SetEvent( inMDNS->p->cancelEvent );
		check_translated_errno( wasSet, GetLastError(), kUnknownErr );
		
		if( inMDNS->p->quitEvent )
		{
			result = WaitForSingleObject( inMDNS->p->quitEvent, 5 * 1000 );
			check_translated_errno( result == WAIT_OBJECT_0, GetLastError(), kUnknownErr );
		}
	}
	return( mStatus_NoError );
}

//===========================================================================================================================
//	ProcessingThread
//===========================================================================================================================

mDNSlocal unsigned WINAPI	ProcessingThread( LPVOID inParam )
{
	mDNS *			m;
	int				done;
	mStatus			err;
	HANDLE *		waitList;
	int				waitListCount;
	DWORD			result;
	BOOL			wasSet;
	
	check( inParam );
		
	m = (mDNS *) inParam;
	err = ProcessingThreadInitialize( m );
	require_noerr( err, exit );
	
	done = 0;
	while( !done )
	{
		// Set up the list of objects we'll be waiting on.
		
		waitList 		= NULL;
		waitListCount	= 0;
		err = ProcessingThreadSetupWaitList( m, &waitList, &waitListCount );
		require_noerr( err, exit );
		
		// Main processing loop.
		
		for( ;; )
		{
			// Give the mDNS core a chance to do its work and determine next event time.
			
			mDNSs32 interval = mDNS_Execute(m) - mDNS_TimeNow(m);
			if (m->p->idleThreadCallback)
			{
				interval = m->p->idleThreadCallback(m, interval);
			}
			if      (interval < 0)						interval = 0;
			else if (interval > (0x7FFFFFFF / 1000))	interval = 0x7FFFFFFF / mDNSPlatformOneSecond;
			else										interval = (interval * 1000) / mDNSPlatformOneSecond;
			
			// Wait until something occurs (e.g. cancel, incoming packet, or timeout).
						
			result = WaitForMultipleObjects( (DWORD) waitListCount, waitList, FALSE, (DWORD) interval );
			if( result == WAIT_TIMEOUT )
			{
				// Next task timeout occurred. Loop back up to give mDNS core a chance to work.
				
				dlog( kDebugLevelChatty - 1, DEBUG_NAME "timeout\n" );
				continue;
			}
			else if( result == kWaitListCancelEvent )
			{
				// Cancel event. Set the done flag and break to exit.
				
				dlog( kDebugLevelVerbose, DEBUG_NAME "canceling...\n" );
				done = 1;
				break;
			}
			else if( result == kWaitListInterfaceListChangedEvent )
			{
				// Interface list changed event. Break out of the inner loop to re-setup the wait list.
				
				ProcessingThreadInterfaceListChanged( m );
				break;
			}
			else if( result == kWaitListWakeupEvent )
			{
				// Wakeup event due to an mDNS API call. Loop back to call mDNS_Execute.
				
				dlog( kDebugLevelChatty - 1, DEBUG_NAME "wakeup for mDNS_Execute\n" );
				continue;
			}
			else if ( result == kWaitListRegEvent )
			{
				//
				// The computer description might have changed
				//
				ProcessingThreadRegistryChanged( m );
				break;
			}
			else
			{
				int		waitItemIndex;
				
				// Socket data available event. Determine which socket and process the packet.
				
				waitItemIndex = (int)( ( (int) result ) - WAIT_OBJECT_0 );
				dlog( kDebugLevelChatty, DEBUG_NAME "socket data available on socket index %d\n", waitItemIndex );
				check( ( waitItemIndex >= 0 ) && ( waitItemIndex < waitListCount ) );
				if( ( waitItemIndex >= 0 ) && ( waitItemIndex < waitListCount ) )
				{
					HANDLE					signaledObject;
					int						n;
					mDNSInterfaceData *		ifd;
					
					signaledObject = waitList[ waitItemIndex ];
					
					n = 0;
					for( ifd = m->p->interfaceList; ifd; ifd = ifd->next )
					{
						if( ifd->readPendingEvent == signaledObject )
						{
							ProcessingThreadProcessPacket( m, ifd, ifd->sock );
							++n;
						}
					}
					check( n > 0 );
				}
				else
				{
					// Unexpected wait result.
				
					dlog( kDebugLevelWarning, DEBUG_NAME "%s: unexpected wait result (result=0x%08X)\n", __ROUTINE__, result );
				}
			}
		}
		
		// Release the wait list.
		
		if( waitList )
		{
			free( waitList );
			waitList = NULL;
			waitListCount = 0;
		}
	}
	
	// Signal the quit event to indicate that the thread is finished.

exit:
	wasSet = SetEvent( m->p->quitEvent );
	check_translated_errno( wasSet, GetLastError(), kUnknownErr );
	
	// Call _endthreadex() explicitly instead of just exiting normally to avoid memory leaks when using static run-time
	// libraries. See <http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp>.
	
	_endthreadex_compat( 0 );
	return( 0 );
}

//===========================================================================================================================
//	ProcessingThreadInitialize
//===========================================================================================================================

mDNSlocal mStatus ProcessingThreadInitialize( mDNS * const inMDNS )
{
	mStatus		err;
	BOOL		wasSet;
	
	inMDNS->p->threadID = GetCurrentThreadId();
	
	err = SetupInterfaceList( inMDNS );
	require_noerr( err, exit );
	
exit:
	if( err )
	{
		TearDownInterfaceList( inMDNS );
	}
	inMDNS->p->initStatus = err;
	
	wasSet = SetEvent( inMDNS->p->initEvent );
	check_translated_errno( wasSet, GetLastError(), kUnknownErr );
	return( err );
}

//===========================================================================================================================
//	ProcessingThreadSetupWaitList
//===========================================================================================================================

mDNSlocal mStatus	ProcessingThreadSetupWaitList( mDNS * const inMDNS, HANDLE **outWaitList, int *outWaitListCount )
{
	mStatus					err;
	int						waitListCount;
	HANDLE *				waitList;
	HANDLE *				waitItemPtr;
	mDNSInterfaceData *		ifd;
	
	dlog( kDebugLevelTrace, DEBUG_NAME "thread setting up wait list\n" );
	check( inMDNS );
	check( inMDNS->p );
	check( outWaitList );
	check( outWaitListCount );
	
	// Allocate an array to hold all the objects to wait on.
	
	waitListCount = kWaitListFixedItemCount + inMDNS->p->interfaceCount;
	waitList = (HANDLE *) malloc( waitListCount * sizeof( *waitList ) );
	require_action( waitList, exit, err = mStatus_NoMemoryErr );
	waitItemPtr = waitList;
	
	// Add the fixed wait items to the beginning of the list.
	
	*waitItemPtr++ = inMDNS->p->cancelEvent;
	*waitItemPtr++ = inMDNS->p->interfaceListChangedEvent;
	*waitItemPtr++ = inMDNS->p->wakeupEvent;
	*waitItemPtr++ = inMDNS->p->regEvent;
	
	// Append all the dynamic wait items to the list.
	
	for( ifd = inMDNS->p->interfaceList; ifd; ifd = ifd->next )
	{
		*waitItemPtr++ = ifd->readPendingEvent;
	}
	check( (int)( waitItemPtr - waitList ) == waitListCount );
	
	*outWaitList 		= waitList;
	*outWaitListCount	= waitListCount;
	waitList			= NULL;
	err					= mStatus_NoError;
	
exit:
	if( waitList )
	{
		free( waitList );
	}
	dlog( kDebugLevelTrace, DEBUG_NAME "thread setting up wait list done (err=%d %m)\n", err, err );
	return( err );
}

//===========================================================================================================================
//	ProcessingThreadProcessPacket
//===========================================================================================================================

mDNSlocal void	ProcessingThreadProcessPacket( mDNS *inMDNS, mDNSInterfaceData *inIFD, SocketRef inSock )
{
	OSStatus					err;
	mDNSAddr					srcAddr;
	mDNSIPPort					srcPort;
	mDNSAddr					dstAddr;
	mDNSIPPort					dstPort;
	mDNSu8						ttl;
	struct sockaddr_storage		addr;
	DNSMessage					packet;
	mDNSu8 *					end;
	int							n;
	
	check( inMDNS );
	check( inIFD );
	check( IsValidSocket( inSock ) );
	
	// Set up the default in case the packet info options are not supported or reported correctly.
	
	dstAddr	= inIFD->defaultAddr;
	dstPort	= MulticastDNSPort;
	ttl		= 255;

#if( !TARGET_OS_WINDOWS_CE )
	if( inIFD->wsaRecvMsgFunctionPtr )
	{
		WSAMSG				msg;
		WSABUF				buf;
		uint8_t				controlBuffer[ 128 ];
		DWORD				size;
		LPWSACMSGHDR		header;
		
		// Set up the buffer and read the packet.
		
		msg.name			= (LPSOCKADDR) &addr;
		msg.namelen			= (INT) sizeof( addr );
		buf.buf				= (char *) &packet;
		buf.len				= (u_long) sizeof( packet );
		msg.lpBuffers		= &buf;
		msg.dwBufferCount	= 1;
		msg.Control.buf		= (char *) controlBuffer;
		msg.Control.len		= (u_long) sizeof( controlBuffer );
		msg.dwFlags			= 0;
				
		err = inIFD->wsaRecvMsgFunctionPtr( inSock, &msg, &size, NULL, NULL );
		err = translate_errno( err == 0, (OSStatus) GetLastError(), kUnknownErr );
		require_noerr( err, exit );
		n = (int) size;
		
		// Parse the control information. Reject packets received on the wrong interface.
		
		for( header = WSA_CMSG_FIRSTHDR( &msg ); header; header = WSA_CMSG_NXTHDR( &msg, header ) )
		{
			if( ( header->cmsg_level == IPPROTO_IP ) && ( header->cmsg_type == IP_PKTINFO ) )
			{
				IN_PKTINFO *		ipv4PacketInfo;
				
				ipv4PacketInfo = (IN_PKTINFO *) WSA_CMSG_DATA( header );
				require_action( ipv4PacketInfo->ipi_ifindex == ( inIFD->index >> 8 ), exit, err = kMismatchErr );
				
				dstAddr.type 				= mDNSAddrType_IPv4;
				dstAddr.ip.v4.NotAnInteger	= ipv4PacketInfo->ipi_addr.s_addr;
			}
			else if( ( header->cmsg_level == IPPROTO_IPV6 ) && ( header->cmsg_type == IPV6_PKTINFO ) )
			{
				IN6_PKTINFO *		ipv6PacketInfo;
				
				ipv6PacketInfo = (IN6_PKTINFO *) WSA_CMSG_DATA( header );
				require_action( ipv6PacketInfo->ipi6_ifindex == inIFD->index, exit, err = kMismatchErr );
				
				dstAddr.type	= mDNSAddrType_IPv6;
				dstAddr.ip.v6	= *( (mDNSv6Addr *) &ipv6PacketInfo->ipi6_addr );
			}
		}
	}
	else
#endif
	{
		int		addrSize;
		
		addrSize = sizeof( addr );
		n = recvfrom( inSock, (char *) &packet, sizeof( packet ), 0, (struct sockaddr *) &addr, &addrSize );
		err = translate_errno( n > 0, errno_compat(), kUnknownErr );
		require_noerr( err, exit );
	}
	SockAddrToMDNSAddr( (struct sockaddr *) &addr, &srcAddr, &srcPort );
	
	// Dispatch the packet to mDNS.
	
	dlog( kDebugLevelChatty, DEBUG_NAME "packet received\n" );
	dlog( kDebugLevelChatty, DEBUG_NAME "    size      = %d\n", n );
	dlog( kDebugLevelChatty, DEBUG_NAME "    src       = %#a:%u\n", &srcAddr, ntohs( srcPort.NotAnInteger ) );
	dlog( kDebugLevelChatty, DEBUG_NAME "    dst       = %#a:%u\n", &dstAddr, ntohs( dstPort.NotAnInteger ) );
	dlog( kDebugLevelChatty, DEBUG_NAME "    interface = %#a (index=0x%08X)\n", &inIFD->interfaceInfo.ip, (int) inIFD->index );
	dlog( kDebugLevelChatty, DEBUG_NAME "\n" );
	
	end = ( (mDNSu8 *) &packet ) + n;
	mDNSCoreReceive( inMDNS, &packet, end, &srcAddr, srcPort, &dstAddr, dstPort, inIFD->interfaceInfo.InterfaceID );
	
exit:
	return;
}

//===========================================================================================================================
//	ProcessingThreadInterfaceListChanged
//===========================================================================================================================

mDNSlocal void	ProcessingThreadInterfaceListChanged( mDNS *inMDNS )
{
	mStatus		err;
	
	dlog( kDebugLevelInfo, DEBUG_NAME "interface list changed\n" );
	check( inMDNS );

	if (inMDNS->p->interfaceListChangedCallback)
	{
		inMDNS->p->interfaceListChangedCallback(inMDNS);
	}
	
	mDNSPlatformLock( inMDNS );
	
	// Tear down the existing interfaces and set up new ones using the new IP info.
	
	err = TearDownInterfaceList( inMDNS );
	check_noerr( err );
	
	err = SetupInterfaceList( inMDNS );
	check_noerr( err );
		
	mDNSPlatformUnlock( inMDNS );
	
	// Inform clients of the change.
	
	if( inMDNS->MainCallback )
	{
		inMDNS->MainCallback( inMDNS, mStatus_ConfigChanged );
	}
	
	// Force mDNS to update.
	
	mDNSCoreMachineSleep( inMDNS, mDNSfalse );
}


//===========================================================================================================================
//	ProcessingThreadRegistryChanged
//===========================================================================================================================
mDNSlocal void	ProcessingThreadRegistryChanged( mDNS *inMDNS )
{
	mStatus		err;
	
	dlog( kDebugLevelInfo, DEBUG_NAME "registry has changed\n" );
	check( inMDNS );

	mDNSPlatformLock( inMDNS );

	// redo the names
	SetupNiceName( inMDNS );

	if (inMDNS->p->hostDescriptionChangedCallback)
	{
		inMDNS->p->hostDescriptionChangedCallback(inMDNS);
	}
	
	// and reset the event handler
	if ((inMDNS->p->regKey != NULL) && (inMDNS->p->regEvent))
	{
		err = RegNotifyChangeKeyValue(inMDNS->p->regKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, inMDNS->p->regEvent, TRUE);
		check_noerr( err );
	}

	mDNSPlatformUnlock( inMDNS );
}


#if 0
#pragma mark -
#pragma mark == Utilities ==
#endif

//===========================================================================================================================
//	getifaddrs
//===========================================================================================================================

int	getifaddrs( struct ifaddrs **outAddrs )
{
	int		err;
	
#if( MDNS_WINDOWS_USE_IPV6_IF_ADDRS && !TARGET_OS_WINDOWS_CE )
	
	// Try to the load the GetAdaptersAddresses function from the IP Helpers DLL. This API is only available on Windows
	// XP or later. Looking up the symbol at runtime allows the code to still work on older systems without that API.
	
	if( !gIPHelperLibraryInstance )
	{
		gIPHelperLibraryInstance = LoadLibrary( TEXT( "Iphlpapi" ) );
		if( gIPHelperLibraryInstance )
		{
			gGetAdaptersAddressesFunctionPtr = 
				(GetAdaptersAddressesFunctionPtr) GetProcAddress( gIPHelperLibraryInstance, "GetAdaptersAddresses" );
			if( !gGetAdaptersAddressesFunctionPtr )
			{
				BOOL		ok;
				
				ok = FreeLibrary( gIPHelperLibraryInstance );
				check_translated_errno( ok, GetLastError(), kUnknownErr );
				gIPHelperLibraryInstance = NULL;
			}
		}
	}
	
	// Use the new IPv6-capable routine if supported. Otherwise, fall back to the old and compatible IPv4-only code.
	
	if( gGetAdaptersAddressesFunctionPtr )
	{
		err = getifaddrs_ipv6( outAddrs );
		require_noerr( err, exit );
	}
	else
	{
		err = getifaddrs_ipv4( outAddrs );
		require_noerr( err, exit );
	}
	
#elif( !TARGET_OS_WINDOWS_CE )

	err = getifaddrs_ipv4( outAddrs );
	require_noerr( err, exit );

#else

	err = getifaddrs_ce( outAddrs );
	require_noerr( err, exit );

#endif

exit:
	return( err );
}

#if( MDNS_WINDOWS_USE_IPV6_IF_ADDRS )
//===========================================================================================================================
//	getifaddrs_ipv6
//===========================================================================================================================

mDNSlocal int	getifaddrs_ipv6( struct ifaddrs **outAddrs )
{
	DWORD						err;
	int							i;
	DWORD						flags;
	struct ifaddrs *			head;
	struct ifaddrs **			next;
	IP_ADAPTER_ADDRESSES *		iaaList;
	ULONG						iaaListSize;
	IP_ADAPTER_ADDRESSES *		iaa;
	size_t						size;
	struct ifaddrs *			ifa;
	
	check( gGetAdaptersAddressesFunctionPtr );
	
	head	= NULL;
	next	= &head;
	iaaList	= NULL;
	
	// Get the list of interfaces. The first call gets the size and the second call gets the actual data.
	// This loops to handle the case where the interface changes in the window after getting the size, but before the
	// second call completes. A limit of 100 retries is enforced to prevent infinite loops if something else is wrong.
	
	flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;
	i = 0;
	for( ;; )
	{
		iaaListSize = 0;
		err = gGetAdaptersAddressesFunctionPtr( AF_UNSPEC, flags, NULL, NULL, &iaaListSize );
		check( err == ERROR_BUFFER_OVERFLOW );
		check( iaaListSize >= sizeof( IP_ADAPTER_ADDRESSES ) );
		
		iaaList = (IP_ADAPTER_ADDRESSES *) malloc( iaaListSize );
		require_action( iaaList, exit, err = ERROR_NOT_ENOUGH_MEMORY );
		
		err = gGetAdaptersAddressesFunctionPtr( AF_UNSPEC, flags, NULL, iaaList, &iaaListSize );
		if( err == ERROR_SUCCESS ) break;
		
		free( iaaList );
		iaaList = NULL;
		++i;
		require( i < 100, exit );
		dlog( kDebugLevelWarning, "%s: retrying GetAdaptersAddresses after %d failure(s) (%d %m)\n", __ROUTINE__, i, err, err );
	}
	
	for( iaa = iaaList; iaa; iaa = iaa->Next )
	{
		IP_ADAPTER_UNICAST_ADDRESS *		addr;

		if( iaa->IfIndex > 0xFFFFFF )
		{
			dlog( kDebugLevelAlert, DEBUG_NAME "%s: IPv4 ifindex out-of-range (0x%08X)\n", __ROUTINE__, iaa->IfIndex );
		}
		if( iaa->Ipv6IfIndex > 0xFF )
		{
			dlog( kDebugLevelAlert, DEBUG_NAME "%s: IPv6 ifindex out-of-range (0x%08X)\n", __ROUTINE__, iaa->Ipv6IfIndex );
		}
		
		// Skip psuedo and tunnel interfaces.
		
		if( ( iaa->Ipv6IfIndex == 1 ) || ( iaa->IfType == IF_TYPE_TUNNEL ) )
		{
			continue;
		}
		
		// Add each address as a separate interface to emulate the way getifaddrs works.
		
		for( addr = iaa->FirstUnicastAddress; addr; addr = addr->Next )
		{			
			ifa = (struct ifaddrs *) calloc( 1, sizeof( struct ifaddrs ) );
			require_action( ifa, exit, err = WSAENOBUFS );
			
			*next = ifa;
			next  = &ifa->ifa_next;
			
			// Get the name.
			
			size = strlen( iaa->AdapterName ) + 1;
			ifa->ifa_name = (char *) malloc( size );
			require_action( ifa->ifa_name, exit, err = WSAENOBUFS );
			memcpy( ifa->ifa_name, iaa->AdapterName, size );
			
			// Get interface flags.
			
			ifa->ifa_flags = 0;
			if( iaa->OperStatus == IfOperStatusUp )
			{
				ifa->ifa_flags |= IFF_UP;
			}
			if( iaa->IfType == IF_TYPE_SOFTWARE_LOOPBACK )
			{
				ifa->ifa_flags |= IFF_LOOPBACK;
			}
			if( !( iaa->Flags & IP_ADAPTER_NO_MULTICAST ) )
			{
				ifa->ifa_flags |= IFF_MULTICAST;
			}
			
			// Get the interface index. Windows does not have a uniform scheme for IPv4 and IPv6 interface indexes
			// so the following is a hack to put IPv4 interface indexes in the upper 16-bits and IPv6 interface indexes
			// in the lower 16-bits. This allows the IPv6 interface index to be usable as an IPv6 scope ID directly.
			
			switch( addr->Address.lpSockaddr->sa_family )
			{
				case AF_INET:
					ifa->ifa_extra.index = iaa->IfIndex << 8;
					break;
					
				case AF_INET6:
					ifa->ifa_extra.index = iaa->Ipv6IfIndex;
					break;
				
				default:
					break;
			}
			
			// Get addresses.
			
			switch( addr->Address.lpSockaddr->sa_family )
			{
				case AF_INET:
				case AF_INET6:
					ifa->ifa_addr = (struct sockaddr *) calloc( 1, (size_t) addr->Address.iSockaddrLength );
					require_action( ifa->ifa_addr, exit, err = WSAENOBUFS );
					memcpy( ifa->ifa_addr, addr->Address.lpSockaddr, (size_t) addr->Address.iSockaddrLength );

					ifa->ifa_netmask = (struct sockaddr *) calloc( 1, sizeof(struct sockaddr) );
					require_action( ifa->ifa_netmask, exit, err = WSAENOBUFS );
					err = getifnetmask_ipv6(ifa);
					require_noerr(err, exit);

					break;
				
				default:
					break;
			}
		}
	}
	
	// Success!
	
	if( outAddrs )
	{
		*outAddrs = head;
		head = NULL;
	}
	err = ERROR_SUCCESS;
	
exit:
	if( head )
	{
		freeifaddrs( head );
	}
	if( iaaList )
	{
		free( iaaList );
	}
	return( (int) err );
}

mDNSlocal int
getifnetmask_ipv6( struct ifaddrs * ifa )
{
	PMIB_IPADDRTABLE	pIPAddrTable	=	NULL;
	DWORD				dwSize			=	0;
	DWORD				dwRetVal;
	DWORD				i;
	int					err				=	0;

	// Make an initial call to GetIpAddrTable to get the
	// necessary size into the dwSize variable

	dwRetVal = GetIpAddrTable(NULL, &dwSize, 0);
	require_action( dwRetVal == ERROR_INSUFFICIENT_BUFFER, exit, err = WSAENOBUFS );

	pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
	require_action( pIPAddrTable != NULL, exit, err = WSAENOBUFS );

	// Make a second call to GetIpAddrTable to get the
	// actual data we want

	dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 );
	require_action(dwRetVal == NO_ERROR, exit, err = WSAENOBUFS);

	// Now try and find the correct IP Address

	for (i = 0; i < pIPAddrTable->dwNumEntries; i++)
	{
		struct sockaddr_in sa;

		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = pIPAddrTable->table[i].dwAddr;

		if (memcmp(ifa->ifa_addr, &sa, sizeof(sa)) == 0)
		{
			// Found the right one, so copy the subnet mask information

			sa.sin_addr.s_addr = pIPAddrTable->table[i].dwMask;
			memcpy( ifa->ifa_netmask, &sa, sizeof(sa) );
			break;
		}
	}

exit:

	if ( pIPAddrTable != NULL )
	{
		free(pIPAddrTable);
	}

	return err;
}
#endif	// MDNS_WINDOWS_USE_IPV6_IF_ADDRS

#if( !TARGET_OS_WINDOWS_CE )
//===========================================================================================================================
//	getifaddrs_ipv4
//===========================================================================================================================

mDNSlocal int	getifaddrs_ipv4( struct ifaddrs **outAddrs )
{
	int						err;
	SOCKET					sock;
	DWORD					size;
	DWORD					actualSize;
	INTERFACE_INFO *		buffer;
	INTERFACE_INFO *		tempBuffer;
	INTERFACE_INFO *		ifInfo;
	int						n;
	int						i;
	struct ifaddrs *		head;
	struct ifaddrs **		next;
	struct ifaddrs *		ifa;
	
	sock	= INVALID_SOCKET;
	buffer	= NULL;
	head	= NULL;
	next	= &head;
	
	// Get the interface list. WSAIoctl is called with SIO_GET_INTERFACE_LIST, but since this does not provide a 
	// way to determine the size of the interface list beforehand, we have to start with an initial size guess and
	// call WSAIoctl repeatedly with increasing buffer sizes until it succeeds. Limit this to 100 tries for safety.
	
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	err = translate_errno( IsValidSocket( sock ), errno_compat(), kUnknownErr );
	require_noerr( err, exit );
		
	n = 0;
	size = 16 * sizeof( INTERFACE_INFO );
	for( ;; )
	{
		tempBuffer = (INTERFACE_INFO *) realloc( buffer, size );
		require_action( tempBuffer, exit, err = WSAENOBUFS );
		buffer = tempBuffer;
		
		err = WSAIoctl( sock, SIO_GET_INTERFACE_LIST, NULL, 0, buffer, size, &actualSize, NULL, NULL );
		if( err == 0 )
		{
			break;
		}
		
		++n;
		require_action( n < 100, exit, err = WSAEADDRNOTAVAIL );
		
		size += ( 16 * sizeof( INTERFACE_INFO ) );
	}
	check( actualSize <= size );
	check( ( actualSize % sizeof( INTERFACE_INFO ) ) == 0 );
	n = (int)( actualSize / sizeof( INTERFACE_INFO ) );
	
	// Process the raw interface list and build a linked list of IPv4 interfaces.
	
	for( i = 0; i < n; ++i )
	{
		ifInfo = &buffer[ i ];
		if( ifInfo->iiAddress.Address.sa_family != AF_INET )
		{
			continue;
		}
		
		ifa = (struct ifaddrs *) calloc( 1, sizeof( struct ifaddrs ) );
		require_action( ifa, exit, err = WSAENOBUFS );
		
		*next = ifa;
		next  = &ifa->ifa_next;
		
		// Get the name.
		
		ifa->ifa_name = (char *) malloc( 16 );
		require_action( ifa->ifa_name, exit, err = WSAENOBUFS );
		sprintf( ifa->ifa_name, "%d", i + 1 );
		
		// Get interface flags.
		
		ifa->ifa_flags = (u_int) ifInfo->iiFlags;
		
		// Get addresses.
		
		switch( ifInfo->iiAddress.Address.sa_family )
		{
			case AF_INET:
			{
				struct sockaddr_in *		sa4;
				
				sa4 = &ifInfo->iiAddress.AddressIn;
				ifa->ifa_addr = (struct sockaddr *) calloc( 1, sizeof( *sa4 ) );
				require_action( ifa->ifa_addr, exit, err = WSAENOBUFS );
				memcpy( ifa->ifa_addr, sa4, sizeof( *sa4 ) );

				sa4 = &ifInfo->iiNetmask.AddressIn;
				ifa->ifa_netmask = (struct sockaddr*) calloc(1, sizeof( *sa4 ) );
				require_action( ifa->ifa_netmask, exit, err = WSAENOBUFS );
				memcpy( ifa->ifa_netmask, sa4, sizeof( *sa4 ) );

				break;
			}
			
			default:
				break;
		}
		
		// Emulate an interface index.
		
		ifa->ifa_extra.index = (uint32_t)( i + 1 );
	}
	
	// Success!
	
	if( outAddrs )
	{
		*outAddrs = head;
		head = NULL;
	}
	err = 0;
	
exit:
	if( head )
	{
		freeifaddrs( head );
	}
	if( buffer )
	{
		free( buffer );
	}
	if( sock != INVALID_SOCKET )
	{
		closesocket( sock );
	}
	return( err );
}
#endif	// !TARGET_OS_WINDOWS_CE )

#if( TARGET_OS_WINDOWS_CE )
//===========================================================================================================================
//	getifaddrs_ce
//===========================================================================================================================

mDNSlocal int	getifaddrs_ce( struct ifaddrs **outAddrs )
{
	int							err;
	SocketRef					sock;
	DWORD						size;
	void *						buffer;
	SOCKET_ADDRESS_LIST *		addressList;
	struct ifaddrs *			head;
	struct ifaddrs **			next;
	struct ifaddrs *			ifa;
	int							n;
	int							i;

	sock 	= kInvalidSocketRef;
	buffer	= NULL;
	head	= NULL;
	next	= &head;
	
	// Open a temporary socket because one is needed to use WSAIoctl (we'll close it before exiting this function).
	
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	err = translate_errno( IsValidSocket( sock ), errno_compat(), kUnknownErr );
	require_noerr( err, exit );
		
	// Call WSAIoctl with SIO_ADDRESS_LIST_QUERY and pass a null buffer. This call will fail, but the size needed to 
	// for the request will be filled in. Once we know the size, allocate a buffer to hold the entire list.
	//
	// NOTE: Due to a bug in Windows CE, the size returned by WSAIoctl is not enough so double it as a workaround.
	
	size = 0;
	WSAIoctl( sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, NULL, 0, &size, NULL, NULL );
	require_action( size > 0, exit, err = -1 );
	size *= 2;
	
	buffer = calloc( 1, size );
	require_action( buffer, exit, err = -1 );
	
	// We now know the size of the list and have a buffer to hold so call WSAIoctl again to get it.
	
	err = WSAIoctl( sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, size, &size, NULL, NULL );
	require_noerr( err, exit );
	addressList = (SOCKET_ADDRESS_LIST *) buffer;
	
	// Process the raw interface list and build a linked list of interfaces.
	//
	// NOTE: Due to a bug in Windows CE, the iAddressCount field is always 0 so use 1 in that case.
	
	n = addressList->iAddressCount;
	if( n == 0 )
	{
		n = 1;
	}
	for( i = 0; i < n; ++i )
	{
		ifa = (struct ifaddrs *) calloc( 1, sizeof( struct ifaddrs ) );
		require_action( ifa, exit, err = WSAENOBUFS );
		
		*next = ifa;
		next  = &ifa->ifa_next;
		
		// Get the name.
		
		ifa->ifa_name = (char *) malloc( 16 );
		require_action( ifa->ifa_name, exit, err = WSAENOBUFS );
		sprintf( ifa->ifa_name, "%d", i + 1 );
		
		// Get flags. Note: SIO_ADDRESS_LIST_QUERY does not report flags so just fake IFF_UP and IFF_MULTICAST.
		
		ifa->ifa_flags = IFF_UP | IFF_MULTICAST;
		
		// Get addresses.
		
		switch( addressList->Address[ i ].lpSockaddr->sa_family )
		{
			case AF_INET:
			{
				struct sockaddr_in *		sa4;
				
				sa4 = (struct sockaddr_in *) addressList->Address[ i ].lpSockaddr;
				ifa->ifa_addr = (struct sockaddr *) calloc( 1, sizeof( *sa4 ) );
				require_action( ifa->ifa_addr, exit, err = WSAENOBUFS );
				memcpy( ifa->ifa_addr, sa4, sizeof( *sa4 ) );
				break;
			}
			
			default:
				break;
		}
	}
	
	// Success!
	
	if( outAddrs )
	{
		*outAddrs = head;
		head = NULL;
	}
	err = 0;
	
exit:
	if( head )
	{
		freeifaddrs( head );
	}
	if( buffer )
	{
		free( buffer );
	}
	if( sock != INVALID_SOCKET )
	{
		closesocket( sock );
	}
	return( err );
}
#endif	// TARGET_OS_WINDOWS_CE )

//===========================================================================================================================
//	freeifaddrs
//===========================================================================================================================

void	freeifaddrs( struct ifaddrs *inIFAs )
{
	struct ifaddrs *		p;
	struct ifaddrs *		q;
	
	// Free each piece of the structure. Set to null after freeing to handle macro-aliased fields.
	
	for( p = inIFAs; p; p = q )
	{
		q = p->ifa_next;
		
		if( p->ifa_name )
		{
			free( p->ifa_name );
			p->ifa_name = NULL;
		}
		if( p->ifa_addr )
		{
			free( p->ifa_addr );
			p->ifa_addr = NULL;
		}
		if( p->ifa_netmask )
		{
			free( p->ifa_netmask );
			p->ifa_netmask = NULL;
		}
		if( p->ifa_broadaddr )
		{
			free( p->ifa_broadaddr );
			p->ifa_broadaddr = NULL;
		}
		if( p->ifa_dstaddr )
		{
			free( p->ifa_dstaddr );
			p->ifa_dstaddr = NULL;
		}
		if( p->ifa_data )
		{
			free( p->ifa_data );
			p->ifa_data = NULL;
		}
		free( p );
	}
}

//===========================================================================================================================
//	CanReceiveUnicast
//===========================================================================================================================

mDNSlocal mDNSBool	CanReceiveUnicast( void )
{
	mDNSBool				ok;
	SocketRef				sock;
	struct sockaddr_in		addr;
	
	// Try to bind to the port without the SO_REUSEADDR option to test if someone else has already bound to it.
	
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	check_translated_errno( IsValidSocket( sock ), errno_compat(), kUnknownErr );
	ok = IsValidSocket( sock );
	if( ok )
	{
		memset( &addr, 0, sizeof( addr ) );
		addr.sin_family			= AF_INET;
		addr.sin_port			= MulticastDNSPort.NotAnInteger;
		addr.sin_addr.s_addr	= htonl( INADDR_ANY );
		
		ok = ( bind( sock, (struct sockaddr *) &addr, sizeof( addr ) ) == 0 );
		close_compat( sock );
	}
	
	dlog( kDebugLevelInfo, DEBUG_NAME "Unicast UDP responses %s\n", ok ? "okay" : "*not allowed*" );
	return( ok );
}

//===========================================================================================================================
//	GetWindowsVersionString
//===========================================================================================================================

OSStatus	GetWindowsVersionString( char *inBuffer, size_t inBufferSize )
{
#if( !defined( VER_PLATFORM_WIN32_CE ) )
	#define VER_PLATFORM_WIN32_CE		3
#endif

	OSStatus				err;
	OSVERSIONINFO			osInfo;
	BOOL					ok;
	const char *			versionString;
	DWORD					platformID;
	DWORD					majorVersion;
	DWORD					minorVersion;
	DWORD					buildNumber;
	
	versionString = "unknown Windows version";
	
	osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	ok = GetVersionEx( &osInfo );
	err = translate_errno( ok, (OSStatus) GetLastError(), kUnknownErr );
	require_noerr( err, exit );
	
	platformID		= osInfo.dwPlatformId;
	majorVersion	= osInfo.dwMajorVersion;
	minorVersion	= osInfo.dwMinorVersion;
	buildNumber		= osInfo.dwBuildNumber & 0xFFFF;
	
	if( ( platformID == VER_PLATFORM_WIN32_WINDOWS ) && ( majorVersion == 4 ) )
	{
		if( ( minorVersion < 10 ) && ( buildNumber == 950 ) )
		{
			versionString	= "Windows 95";
		}
		else if( ( minorVersion < 10 ) && ( ( buildNumber > 950 ) && ( buildNumber <= 1080 ) ) )
		{
			versionString	= "Windows 95 SP1";
		}
		else if( ( minorVersion < 10 ) && ( buildNumber > 1080 ) )
		{
			versionString	= "Windows 95 OSR2";
		}
		else if( ( minorVersion == 10 ) && ( buildNumber == 1998 ) )
		{
			versionString	= "Windows 98";
		}
		else if( ( minorVersion == 10 ) && ( ( buildNumber > 1998 ) && ( buildNumber < 2183 ) ) )
		{
			versionString	= "Windows 98 SP1";
		}
		else if( ( minorVersion == 10 ) && ( buildNumber >= 2183 ) )
		{
			versionString	= "Windows 98 SE";
		}
		else if( minorVersion == 90 )
		{
			versionString	= "Windows ME";
		}
	}
	else if( platformID == VER_PLATFORM_WIN32_NT )
	{
		if( ( majorVersion == 3 ) && ( minorVersion == 51 ) )
		{
			versionString	= "Windows NT 3.51";
		}
		else if( ( majorVersion == 4 ) && ( minorVersion == 0 ) )
		{
			versionString	= "Windows NT 4";
		}
		else if( ( majorVersion == 5 ) && ( minorVersion == 0 ) )
		{
			versionString	= "Windows 2000";
		}
		else if( ( majorVersion == 5 ) && ( minorVersion == 1 ) )
		{
			versionString	= "Windows XP";
		}
		else if( ( majorVersion == 5 ) && ( minorVersion == 2 ) )
		{
			versionString	= "Windows Server 2003";
		}
	}
	else if( platformID == VER_PLATFORM_WIN32_CE )
	{
		versionString		= "Windows CE";
	}
	
exit:
	if( inBuffer && ( inBufferSize > 0 ) )
	{
		inBufferSize -= 1;
		strncpy( inBuffer, versionString, inBufferSize );
		inBuffer[ inBufferSize ] = '\0';
	}
	return( err );
}
