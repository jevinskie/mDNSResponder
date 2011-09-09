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

$Log: RMxServer.h,v $
Revision 1.1  2004/01/30 02:35:13  bradley
Rendezvous Message Exchange implementation for DNS-SD IPC on Windows.

*/

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		RMxServer.h
	
	@abstract	Server-side implementation of the DNS-SD IPC API.
	
	@discussion	
	
	This listens for and accepts connections from IPC clients, starts server sessions, and acts as a mediator between the 
	"direct" (compiled-in mDNSCore) code and the IPC client.
*/

#ifndef __RMx_SERVER__
#define __RMx_SERVER__

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == RMx ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxServerFlags

	@abstract	Flags to control how the server is stopped.
	
	@constant	kRMxServerFlagsNone			No flags.
	@constant	kRMxServerFlagsAllowRemote	Allow remote connections.
*/

typedef uint32_t		RMxServerFlags;

#define	kRMxServerFlagsNone				0
#define	kRMxServerFlagsAllowRemote		( 1 << 0 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxServerStopFlags

	@abstract	Flags to control how the server is stopped.
	
	@constant	kRMxServerStopFlagsNone		No flags.
	@constant	kRMxServerStopFlagNoWait	Do not wait for the server to stop (signal and return).
*/

typedef uint32_t		RMxServerStopFlags;

#define	kRMxServerStopFlagsNone			0
#define	kRMxServerStopFlagsNoWait		( 1 << 0 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxServerInitialize

	@abstract	Initializes the RMx server.
*/

OSStatus	RMxServerInitialize( RMxServerFlags inFlags );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxServerFinalize

	@abstract	Finalizes the RMx server.
*/

void		RMxServerFinalize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxServerRun

	@abstract	Runs the RMx server. Does not return unless stopped or an error occurs.
*/

OSStatus	RMxServerRun( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxServerStop

	@abstract	Stops the RMx server.
*/

OSStatus	RMxServerStop( RMxServerStopFlags inFlags );

#ifdef	__cplusplus
	}
#endif

#endif  // __RMx_SERVER__
