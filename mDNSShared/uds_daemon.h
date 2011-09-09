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

 	File:		uds_daemon.h

 	Contains:	Interfaces necessary to talk to uds_daemon.c.

 	Version:	1.0

    Change History (most recent first):

$Log: uds_daemon.h,v $
Revision 1.3  2004/01/25 00:03:21  cheshire
Change to use mDNSVal16() instead of private PORT_AS_NUM() macro

Revision 1.2  2004/01/24 08:46:26  bradley
Added InterfaceID<->Index platform interfaces since they are now used by all platforms for the DNS-SD APIs.

Revision 1.1  2003/12/08 21:11:42  rpantos;
Changes necessary to support mDNSResponder on Linux.

*/

#include "mDNSClientAPI.h"


/* Client interface: */

#define SRS_PORT(S) mDNSVal16((S)->RR_SRV.resrec.rdata->u.srv.port)

extern int udsserver_init( mDNS *globalInstance);

// takes the next scheduled event time, does idle work, and returns the updated nextevent time
extern mDNSs32 udsserver_idle(mDNSs32 nextevent);

extern void udsserver_info(void);	// print out info about current state

extern void udsserver_handle_configchange(void);

extern int udsserver_exit(void);	// should be called prior to app exit


/* Routines that uds_daemon expects to link against: */

typedef	void (*udsEventCallback)(void *context);

extern mStatus udsSupportAddFDToEventLoop( int fd, udsEventCallback callback, void *context);
extern mStatus udsSupportRemoveFDFromEventLoop( int fd);
