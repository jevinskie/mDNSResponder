/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
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

 	File:		daemon.c

 	Contains:	main & associated Application layer for mDNSResponder on Linux.

 	Version:	1.0
 	Tabs:		4 spaces

    Change History (most recent first):

$Log: PosixDaemon.c,v $
Revision 1.10  2004/06/08 04:59:40  cheshire
Tidy up wording -- log messages are already prefixed with "mDNSResponder", so don't need to repeat it

Revision 1.9  2004/05/29 00:14:20  rpantos
<rdar://problem/3508093> Runtime check to disable prod mdnsd on OS X.

Revision 1.8  2004/04/07 01:19:04  cheshire
Hash slot value should be unsigned

Revision 1.7  2004/02/14 06:34:57  cheshire
Use LogMsg instead of fprintf( stderr

Revision 1.6  2004/02/14 01:10:42  rpantos
Allow daemon to run if 'nobody' is not defined, with a warning. (For Roku HD1000.)

Revision 1.5  2004/02/05 07:45:43  cheshire
Add Log header

Revision 1.4  2004/01/28 21:14:23  cheshire
Reconcile debug_mode and gDebugLogging into a single flag (mDNS_DebugMode)

Revision 1.3  2004/01/19 19:51:46  cheshire
Fix compiler error (mixed declarations and code) on some versions of Linux

Revision 1.2  2003/12/11 03:03:51  rpantos
Clean up mDNSPosix so that it builds on OS X again.

Revision 1.1  2003/12/08 20:47:02  rpantos
Add support for mDNSResponder on Linux.

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>

#include "mDNSClientAPI.h"
#include "mDNSDebug.h"
#include "mDNSPosix.h"
#include "uds_daemon.h"


static void		ParseCmdLinArgs( int argc, char **argv);
static void		DumpStateLog( mDNS *m);
static mStatus	MainLoop( mDNS *m);


#define RR_CACHE_SIZE 500
static CacheRecord gRRCache[RR_CACHE_SIZE];

extern const char mDNSResponderVersionString[];

int		main( int argc, char **argv)
{
	mDNS					mDNSRecord;
	mDNS_PlatformSupport	platformStorage;
	mStatus					err;

	bzero( &mDNSRecord, sizeof mDNSRecord);
	bzero( &platformStorage, sizeof platformStorage);

	ParseCmdLinArgs( argc, argv);

	err = mDNS_Init( &mDNSRecord, &platformStorage, gRRCache, RR_CACHE_SIZE, mDNS_Init_AdvertiseLocalAddresses, 
					mDNS_Init_NoInitCallback, mDNS_Init_NoInitCallbackContext); 

	if ( mStatus_NoError == err)
		err = udsserver_init( &mDNSRecord);

	// Now that we're finished with anything privileged, switch over to running as "nobody"
	if ( mStatus_NoError == err)
	{
		const struct passwd *pw = getpwnam("nobody");
		if ( pw != NULL)
			setuid( pw->pw_uid);
		else
			LogMsg("WARNING: mdnsd continuing as root because user \"nobody\" does not exist");
	}

	if ( mStatus_NoError == err)
	 	err = MainLoop( &mDNSRecord);
 
 	mDNS_Close( &mDNSRecord);

	if (udsserver_exit() < 0)
		LogMsg("ExitCallback: udsserver_exit failed");
 
 #if MDNS_DEBUGMSGS > 0
 	printf( "mDNSResponder exiting normally with %ld\n", err);
 #endif
 
 	return err;
}


static void	ParseCmdLinArgs( int argc, char **argv)
// Do appropriate things at startup with command line arguments. Calls exit() if unhappy.
{
	if ( argc > 1)
	{
		if ( 0 == strcmp( argv[1], "-debug"))
		{
			mDNS_DebugMode = mDNStrue;
		}
		else
			printf( "Usage: mDNSResponder [-debug]\n");
	}

	if ( !mDNS_DebugMode)
	{
		int		result = daemon( 0, 0);
		
		if ( result != 0)
		{
			LogMsg("Could not run as daemon - exiting");
			exit( result);
		}

#if __APPLE__
		{
			LogMsg("The POSIX mDNSResponder should only be used on OS X for testing - exiting");
			exit( -1);
		}
#endif
	}
}


static void		DumpStateLog( mDNS *m)
// Dump a little log of what we've been up to.
{
	mDNSu32 slot;
	CacheRecord *rr;
	mDNSu32 CacheUsed = 0, CacheActive = 0;
	mDNSs32 now = mDNSPlatformTimeNow();

	LogMsgIdent(mDNSResponderVersionString, "---- BEGIN STATE LOG ----");

	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		{
		mDNSu32 SlotUsed = 0;
		for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
			{
			mDNSs32 remain = rr->resrec.rroriginalttl - (now - rr->TimeRcvd) / mDNSPlatformOneSecond;
			CacheUsed++;
			SlotUsed++;
			if (rr->CRActiveQuestion) CacheActive++;
			LogMsgNoIdent("%s%6ld %-6s%-6s%s", rr->CRActiveQuestion ? "*" : " ", remain, DNSTypeName(rr->resrec.rrtype),
				((PosixNetworkInterface *)rr->resrec.InterfaceID)->intfName, GetRRDisplayString(m, rr));
			usleep(1000);	// Limit rate a little so we don't flood syslog too fast
			}
		if (m->rrcache_used[slot] != SlotUsed)
			LogMsgNoIdent("Cache use mismatch: rrcache_used[slot] is %lu, true count %lu", m->rrcache_used[slot], SlotUsed);
		}
	if (m->rrcache_totalused != CacheUsed)
		LogMsgNoIdent("Cache use mismatch: rrcache_totalused is %lu, true count %lu", m->rrcache_totalused, CacheUsed);
	if (m->rrcache_active != CacheActive)
		LogMsgNoIdent("Cache use mismatch: rrcache_active is %lu, true count %lu", m->rrcache_active, CacheActive);
	LogMsgNoIdent("Cache currently contains %lu records; %lu referenced by active questions", CacheUsed, CacheActive);

	udsserver_info();

	LogMsgIdent(mDNSResponderVersionString, "----  END STATE LOG  ----");
}

static mStatus	MainLoop( mDNS *m)
// Loop until we quit.
{
	sigset_t	signals;
	mDNSBool	gotData = mDNSfalse;

	mDNSPosixListenForSignalInEventLoop( SIGINT);
	mDNSPosixListenForSignalInEventLoop( SIGTERM);
	mDNSPosixListenForSignalInEventLoop( SIGUSR1);
	mDNSPosixListenForSignalInEventLoop( SIGPIPE);

	for ( ; ;)
	{
		// Work out how long we expect to sleep before the next scheduled task
		struct timeval	timeout;
		mDNSs32			ticks;

		// Only idle if we didn't find any data the last time around
		if ( !gotData)
		{
			mDNSs32			nextTimerEvent = mDNS_Execute(m);
		
			nextTimerEvent = udsserver_idle( nextTimerEvent);
	
			ticks = nextTimerEvent - mDNSPlatformTimeNow();
			if (ticks < 1) ticks = 1;
		}
		else	// otherwise call EventLoop again with 0 timemout
			ticks = 0;

		timeout.tv_sec = ticks / mDNSPlatformOneSecond;
		timeout.tv_usec = (ticks % mDNSPlatformOneSecond) * 1000000 / mDNSPlatformOneSecond;

		(void) mDNSPosixRunEventLoopOnce( m, &timeout, &signals, &gotData);

		if ( sigismember( &signals, SIGUSR1))
			DumpStateLog( m);
		if ( sigismember( &signals, SIGPIPE))	// happens when we try to write to a dead client; death should be detected soon in request_callback() and cleaned up.
			LogMsg("Received SIGPIPE - ignoring");
		if ( sigismember( &signals, SIGINT) || sigismember( &signals, SIGTERM))
			break;
	}

	return EINTR;
}


//		uds_daemon support		////////////////////////////////////////////////////////////

#if MDNS_MALLOC_DEBUGGING >= 2
#define LogMalloc LogMsg
#else
#define	LogMalloc(ARGS...) ((void)0)
#endif


mStatus udsSupportAddFDToEventLoop( int fd, udsEventCallback callback, void *context)
/* Support routine for uds_daemon.c */
{
	// Depends on the fact that udsEventCallback == mDNSPosixEventCallback
	return mDNSPosixAddFDToEventLoop( fd, callback, context);
}

mStatus udsSupportRemoveFDFromEventLoop( int fd)
{
	return mDNSPosixRemoveFDFromEventLoop( fd);
}

#if MACOSX_MDNS_MALLOC_DEBUGGING >= 1

void *mallocL(char *msg, unsigned int size)
{
	unsigned long *mem = malloc(size+8);
	if (!mem)
	{
		LogMsg("malloc( %s : %d ) failed", msg, size);
		return(NULL); 
	}
	else
	{
		LogMalloc("malloc( %s : %lu ) = %p", msg, size, &mem[2]);
		mem[0] = 0xDEAD1234;
		mem[1] = size;
		//bzero(&mem[2], size);
		memset(&mem[2], 0xFF, size);
//		validatelists(&mDNSStorage);
		return(&mem[2]);
	}
}

void freeL(char *msg, void *x)
{
	if (!x)
		LogMsg("free( %s @ NULL )!", msg);
	else
	{
		unsigned long *mem = ((unsigned long *)x) - 2;
		if (mem[0] != 0xDEAD1234)
			{ LogMsg("free( %s @ %p ) !!!! NOT ALLOCATED !!!!", msg, &mem[2]); return; }
		if (mem[1] > 8000)
			{ LogMsg("free( %s : %ld @ %p) too big!", msg, mem[1], &mem[2]); return; }
		LogMalloc("free( %s : %ld @ %p)", msg, mem[1], &mem[2]);
		//bzero(mem, mem[1]+8);
		memset(mem, 0xDD, mem[1]+8);
//		validatelists(&mDNSStorage);
		free(mem);
	}
}

#endif // MACOSX_MDNS_MALLOC_DEBUGGING >= 1



// For convenience when using the "strings" command, this is the last thing in the file
#if mDNSResponderVersion > 1
mDNSexport const char mDNSResponderVersionString[] = "mDNSResponder-" STRINGIFY(mDNSResponderVersion) " (" __DATE__ " " __TIME__ ") ";
#else
mDNSexport const char mDNSResponderVersionString[] = "mDNSResponder (Engineering Build) (" __DATE__ " " __TIME__ ") ";
#endif
