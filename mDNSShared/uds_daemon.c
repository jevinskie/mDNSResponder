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

    Change History (most recent first):

$Log: uds_daemon.c,v $
Revision 1.55.2.1  2004/06/13 22:32:24  ksekar
WWDC Branch

Revision 1.55  2004/06/05 00:04:27  cheshire
<rdar://problem/3668639>: wide-area domains should be returned in reg. domain enumeration

Revision 1.54  2004/06/01 22:22:52  ksekar
<rdar://problem/3668635>: wide-area default registrations should be in
.local too

Revision 1.53  2004/05/28 23:42:37  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.52  2004/05/26 00:39:49  ksekar
<rdar://problem/3667105>: wide-area rendezvous servers don't appear in
Finder
Use local-only InterfaceID for GetDomains calls for sockets-API

Revision 1.51  2004/05/18 23:51:27  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.50  2004/05/14 16:39:47  ksekar
Browse for iChat locally for now.

Revision 1.49  2004/05/13 21:33:52  ksekar
Clean up non-local registration control via config file.  Force iChat
registrations to be local for now.

Revision 1.48  2004/05/13 04:13:19  ksekar
Updated SIGINFO handler for multi-domain browses

Revision 1.47  2004/05/12 22:04:01  ksekar
Implemented multi-domain browsing by default for uds_daemon.

Revision 1.46  2004/05/06 18:42:58  ksekar
General dns_sd.h API cleanup, including the following radars:
<rdar://problem/3592068>: Remove flags with zero value
<rdar://problem/3479569>: Passing in NULL causes a crash.

Revision 1.45  2004/03/12 08:49:28  cheshire
#include <sys/socket.h>

Revision 1.44  2004/02/25 01:25:27  ksekar
<rdar://problem/3569212>: DNSServiceRegisterRecord flags not error-checked

Revision 1.43  2004/02/24 01:46:40  cheshire
Manually reinstate lost checkin 1.36

Revision 1.42  2004/02/05 19:39:29  cheshire
Move creation of /var/run/mDNSResponder.pid to uds_daemon.c,
so that all platforms get this functionality

Revision 1.41  2004/02/03 18:59:02  cheshire
Change "char *domain" parameter for format_enumeration_reply to "const char *domain"

Revision 1.40  2004/01/28 03:41:00  cheshire
<rdar://problem/3541946>: Need ability to do targeted queries as well as multicast queries

Revision 1.39  2004/01/25 00:03:21  cheshire
Change to use mDNSVal16() instead of private PORT_AS_NUM() macro

Revision 1.38  2004/01/19 19:51:46  cheshire
Fix compiler error (mixed declarations and code) on some versions of Linux

Revision 1.37  2003/12/08 21:11:42  rpantos
Changes necessary to support mDNSResponder on Linux.

Revision 1.36  2003/12/04 23:40:57  cheshire
<rdar://problem/3484766>: Security: Crashing bug in mDNSResponder
Fix some more code that should use buffer size MAX_ESCAPED_DOMAIN_NAME (1005) instead of 256-byte buffers.

Revision 1.35  2003/12/03 19:10:22  ksekar
<rdar://problem/3498644>: malloc'd data not zero'd

Revision 1.34  2003/12/03 02:00:01  ksekar
<rdar://problem/3498644>: malloc'd data not zero'd

Revision 1.33  2003/11/22 01:18:46  ksekar
<rdar://problem/3486646>: config change handler not called for dns-sd services

Revision 1.32  2003/11/20 21:46:12  ksekar
<rdar://problem/3486635>: leak: DNSServiceRegisterRecord

Revision 1.31  2003/11/20 20:33:05  ksekar
<rdar://problem/3486635>: leak: DNSServiceRegisterRecord

Revision 1.30  2003/11/20 02:10:55  ksekar
<rdar://problem/3486643>: cleanup DNSServiceAdd/RemoveRecord

Revision 1.29  2003/11/14 21:18:32  cheshire
<rdar://problem/3484766>: Security: Crashing bug in mDNSResponder
Fix code that should use buffer size MAX_ESCAPED_DOMAIN_NAME (1005) instead of 256-byte buffers.

Revision 1.28  2003/11/08 22:18:29  cheshire
<rdar://problem/3477870>: Don't need to show process ID in *every* mDNSResponder syslog message

Revision 1.27  2003/11/05 22:44:57  ksekar
<rdar://problem/3335230>: No bounds checking when reading data from client
Reviewed by: Stuart Cheshire

Revision 1.26  2003/10/23 17:51:04  ksekar
<rdar://problem/3335216>: handle blocked clients more efficiently
Changed gettimeofday() to mDNSPlatformTimeNow()

Revision 1.25  2003/10/22 23:37:49  ksekar
<rdar://problem/3459141>: crash/hang in abort_client

Revision 1.24  2003/10/21 20:59:40  ksekar
<rdar://problem/3335216>: handle blocked clients moreefficiently

Revision 1.23  2003/09/23 02:12:43  cheshire
Also include port number in list of services registered via new UDS API

Revision 1.22  2003/08/19 16:03:55  ksekar
<rdar://problem/3380097>: ER: SIGINFO dump should include resolves started by DNSServiceQueryRecord
Check termination_context for NULL before dereferencing.

Revision 1.21  2003/08/19 05:39:43  cheshire
<rdar://problem/3380097> SIGINFO dump should include resolves started by DNSServiceQueryRecord

Revision 1.20  2003/08/16 03:39:01  cheshire
<rdar://problem/3338440> InterfaceID -1 indicates "local only"

Revision 1.19  2003/08/15 20:16:03  cheshire
<rdar://problem/3366590> mDNSResponder takes too much RPRVT
We want to avoid touching the rdata pages, so we don't page them in.
1. RDLength was stored with the rdata, which meant touching the page just to find the length.
   Moved this from the RData to the ResourceRecord object.
2. To avoid unnecessarily touching the rdata just to compare it,
   compute a hash of the rdata and store the hash in the ResourceRecord object.

Revision 1.18  2003/08/15 00:38:00  ksekar
<rdar://problem/3377005>: Bug: buffer overrun when reading long rdata from client

Revision 1.17  2003/08/14 02:18:21  cheshire
<rdar://problem/3375491> Split generic ResourceRecord type into two separate types: AuthRecord and CacheRecord

Revision 1.16  2003/08/13 23:58:52  ksekar
<rdar://problem/3374911>: Bug: UDS Sub-type browsing works, but not sub-type registration
Fixed pointer increment error, moved subtype reading for-loop for easier error bailout.

Revision 1.15  2003/08/13 17:30:33  ksekar
<rdar://problem/3374671>: DNSServiceAddRecord doesn't work
Fixed various problems with handling the AddRecord request and freeing the ExtraResourceRecords.

Revision 1.14  2003/08/12 19:56:25  cheshire
Update to APSL 2.0

 */

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>

#include "mDNSClientAPI.h"
#include "uds_daemon.h"
#include "dns_sd.h"
#include "dnssd_ipc.h"

// convenience definition 
#define	_UNUSED	__attribute__ ((unused))
// Types and Data Structures
// ----------------------------------------------------------------------

typedef enum
    {
    t_uninitialized,
    t_morecoming,
    t_complete,
    t_error,
    t_terminated
    } transfer_state;

typedef void (*req_termination_fn)(void *);


typedef struct registered_record_entry
    {
    int key;
    AuthRecord *rr;
    struct registered_record_entry *next;
    } registered_record_entry;
    
typedef struct extra_record_entry
    {
    int key;
    struct extra_record_entry *next;
    ExtraResourceRecord e;
    } extra_record_entry;

typedef struct registered_service
    {
    int autoname;
    int renameonconflict;
    int rename_on_memfree;  	// set flag on config change when we deregister original name
    domainlabel name;
    ServiceRecordSet *srs;
    struct request_state *request;
    AuthRecord *subtypes;
    extra_record_entry *extras;
    } registered_service;

typedef struct
	{
    registered_service *local;
    registered_service *global;
	} servicepair_t;

typedef struct 
    {
    mStatus err;
    int nwritten;
    int sd;
    } undelivered_error_t;

typedef struct request_state
    {
    // connection structures
    int sd;	
    int errfd;		
                                
    // state of read (in case message is read over several recv() calls)                            
    transfer_state ts;
    uint32_t hdr_bytes;		// bytes of header already read
    ipc_msg_hdr hdr;
    uint32_t data_bytes;	// bytes of message data already read
    char *msgbuf;		// pointer to data storage to pass to free()
    char *msgdata;		// pointer to data to be read from (may be modified)
    int bufsize;		// size of data storage

    // reply, termination, error, and client context info
    int no_reply;		// don't send asynchronous replies to client
    int time_blocked;           // record time of a blocked client
    void *client_context;	// don't touch this - pointer only valid in client's addr space
    struct reply_state *replies;  // corresponding (active) reply list
    undelivered_error_t *u_err;
    void *termination_context;
    req_termination_fn terminate;
    
    //!!!KRS toss these pointers in a union
    // registration context associated with this request (null if not applicable)
    registered_record_entry *reg_recs;  // muliple registrations for a connection-oriented request
    servicepair_t servicepair;
    struct resolve_result_t *resolve_results;
    
    struct request_state *next;
    } request_state;

// struct physically sits between ipc message header and call-specific fields in the message buffer
typedef struct
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    DNSServiceErrorType error;
    } reply_hdr;
    

typedef struct reply_state
    {
    // state of the transmission
    int sd;
    transfer_state ts;
    uint32_t nwriten;
    uint32_t len;
    // context of the reply
    struct request_state *request;  // the request that this answers
    struct reply_state *next;   // if there are multiple unsent replies
    // pointer into message buffer - allows fields to be changed after message is formatted
    ipc_msg_hdr *mhdr;
    reply_hdr *rhdr;
    char *sdata;  // pointer to start of call-specific data
    // pointer to malloc'd buffer
    char *msgbuf;	
    } reply_state;


// domain enumeration and resolv calls require 2 mDNSCore calls, so we need separate interconnected
// structures to handle callbacks
typedef struct
    {
    DNSQuestion question;
    mDNS_DomainType type;
    request_state *rstate;
    } domain_enum_t;

typedef struct
    {
    domain_enum_t *all;
    domain_enum_t *def;
    request_state *rstate;
    } enum_termination_t;

typedef struct
    {
    DNSQuestion question;
    uint16_t	qtype;
    request_state *rstate;
    } resolve_t;
    
typedef struct
    {
    resolve_t *txt;
    resolve_t *srv;
    request_state *rstate;
    } resolve_termination_t;
    
typedef struct resolve_result_t
    {
    const ResourceRecord *txt;
    const ResourceRecord *srv;
    } resolve_result_t;

typedef struct
    {
    request_state *rstate;
    client_context_t client_context;
    } regrecord_callback_context;

// for multi-domain browsing
typedef struct qlist_t
	{
    DNSQuestion q;
    struct qlist_t *next;
	} qlist_t;

typedef struct
	{
    qlist_t *qlist;
    request_state *rstate;
	} browse_termination_context;


// globals
static mDNS *gmDNS = NULL;
static int listenfd = -1;  
static request_state *all_requests = NULL;  
//!!!KRS we should keep a separate list containing only the requests that need to be examined
//in the idle() routine.


#define MAX_OPENFILES 1024
#define MAX_TIME_BLOCKED 60 * mDNSPlatformOneSecond   // try to send data to a blocked client for 60 seconds before
                              // terminating connection
#define MSG_PAD_BYTES 5       // pad message buffer (read from client) with n zero'd bytes to guarantee
                              // n get_string() calls w/o buffer overrun    
// private function prototypes
static void connect_callback(void *info);
static int read_msg(request_state *rs);
static int send_msg(reply_state *rs);
static void abort_request(request_state *rs);
static void request_callback(void *info);
static void handle_resolve_request(request_state *rstate);
static void question_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
static void question_termination_callback(void *context);
static void handle_browse_request(request_state *request);
static void browse_termination_callback(void *context);
static void browse_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
static void handle_regservice_request(request_state *request);
static void regservice_termination_callback(void *context);
static void process_service_registration(ServiceRecordSet *const srs);
static void regservice_callback(mDNS *const m, ServiceRecordSet *const srs, mStatus result);
static void handle_add_request(request_state *rstate);
static void handle_update_request(request_state *rstate);
static mStatus gen_rr_response(domainname *servicename, mDNSInterfaceID id, request_state *request, reply_state **rep);
static void append_reply(request_state *req, reply_state *rep);
static int build_domainname_from_strings(domainname *srv, char *name, char *regtype, char *domain);
static void enum_termination_callback(void *context);
static void enum_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
static void handle_query_request(request_state *rstate);
static reply_state *format_enumeration_reply(request_state *rstate, const char *domain, DNSServiceFlags flags, uint32_t ifi, DNSServiceErrorType err);
static void handle_enum_request(request_state *rstate);
static void handle_regrecord_request(request_state *rstate);
static void regrecord_callback(mDNS *const m, AuthRecord *const rr, mStatus result);
static void connected_registration_termination(void *context);
static void handle_reconfirm_request(request_state *rstate);
static AuthRecord *read_rr_from_ipc_msg(char *msgbuf, int ttl, int validate_flags);
static void handle_removerecord_request(request_state *rstate);
static void reset_connected_rstate(request_state *rstate);
static int deliver_error(request_state *rstate, mStatus err);
static int deliver_async_error(request_state *rs, reply_op_t op, mStatus err);
static transfer_state send_undelivered_error(request_state *rs);
static reply_state *create_reply(reply_op_t op, int datalen, request_state *request);
static void update_callback(mDNS *const m, AuthRecord *const rr, RData *oldrd);
static void my_perror(char *errmsg);
static void unlink_request(request_state *rs);
static void resolve_result_callback(mDNS *const m, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord);
static void resolve_termination_callback(void *context);
static int validate_message(request_state *rstate);
static mStatus remove_extra_rr_from_service(request_state *rstate);
static mStatus remove_record(request_state *rstate);
static void free_service_registration(registered_service *srv);

// initialization, setup/teardown functions

// If a platform specifies its own PID file name, we use that
#ifndef PID_FILE
#define PID_FILE "/var/run/mDNSResponder.pid"
#endif

int udsserver_init( mDNS *globalInstance)  
    {
    mode_t mask;
    struct sockaddr_un laddr;
    struct rlimit maxfds;

    if ( !globalInstance)
        goto error;
	gmDNS = globalInstance;

	// If a particular platform wants to opt out of having a PID file, define PID_FILE to be ""
	if (PID_FILE[0])
		{
		FILE *fp = fopen(PID_FILE, "w");
		if (fp != NULL)
			{
			fprintf(fp, "%d\n", getpid());
			fclose(fp);
			}
		}

    if ((listenfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) 
        goto error;
    unlink(MDNS_UDS_SERVERPATH);  //OK if this fails
    bzero(&laddr, sizeof(laddr));
    laddr.sun_family = AF_LOCAL;
#ifndef NOT_HAVE_SA_LEN		// According to Stevens (section 3.2), there is no portable way to 
							// determine whether sa_len is defined on a particular platform. 
    laddr.sun_len = sizeof(struct sockaddr_un);
#endif
    strcpy(laddr.sun_path, MDNS_UDS_SERVERPATH);
    mask = umask(0);
    if (bind(listenfd, (struct sockaddr *)&laddr, sizeof(laddr)) < 0)
        goto error;
    umask(mask);

    if (fcntl(listenfd, F_SETFL, O_NONBLOCK) < 0)
        {
        my_perror("ERROR: could not set listen socket to non-blocking mode");
        goto error;
        }
    listen(listenfd, LISTENQ);
    
    if (mStatus_NoError != udsSupportAddFDToEventLoop(listenfd, connect_callback, (void *) NULL))
        {
        my_perror("ERROR: could not add listen socket to event loop");
        goto error;
        }
    
    // set maximum file descriptor to 1024
    if (getrlimit(RLIMIT_NOFILE, &maxfds) < 0)
        {
        my_perror("ERROR: Unable to get file descriptor limit");
        return 0;
        }
    if (maxfds.rlim_max >= MAX_OPENFILES && maxfds.rlim_cur == maxfds.rlim_max)
        {
        // proper values already set
        return 0;
        }
    maxfds.rlim_max = MAX_OPENFILES;
    maxfds.rlim_cur = MAX_OPENFILES;	
    if (setrlimit(RLIMIT_NOFILE, &maxfds) < 0)
        my_perror("ERROR: Unable to set maximum file descriptor limit");
    return 0;
	
error:
    my_perror("ERROR: udsserver_init");
    return -1;
    }

int udsserver_exit(void)
    {
    close(listenfd);
    unlink(MDNS_UDS_SERVERPATH);
    return 0;
    }


mDNSs32 udsserver_idle(mDNSs32 nextevent)
    {
    request_state *req = all_requests, *tmp, *prev = NULL;
    reply_state *fptr;
    transfer_state result; 
    mDNSs32 now = mDNSPlatformTimeNow();

    while(req)
        {
        result = t_uninitialized;
        if (req->u_err) 
            result = send_undelivered_error(req);
        if (result != t_error && result != t_morecoming &&		// don't try to send msg if send_error failed
            (req->ts == t_complete || req->ts == t_morecoming))
            {
            while(req->replies)
                {
                if (req->replies->next) req->replies->rhdr->flags |= kDNSServiceFlagsMoreComing;
                result = send_msg(req->replies);
                if (result == t_complete)
                    {
                    fptr = req->replies;
                    req->replies = req->replies->next;
                    freeL("udsserver_idle", fptr);
                    req->time_blocked = 0;                              // reset failure counter after successful send
                    }
                else if (result == t_terminated || result == t_error)
                    {
                    abort_request(req);
                    break;
                    }
                else if (result == t_morecoming) break;	   		// client's queues are full, move to next
                }
            }
        if (result == t_morecoming)
        {	
	    if (!req->time_blocked) req->time_blocked = now;
	    debugf("udsserver_idle: client has been blocked for %d seconds", now - req->time_blocked);
	    if (now - req->time_blocked >= MAX_TIME_BLOCKED)
		{
		LogMsg("Could not write data to client after %d seconds - aborting connection", MAX_TIME_BLOCKED / mDNSPlatformOneSecond);
		abort_request(req);
		result = t_terminated;
		}
	    else if (nextevent - now > mDNSPlatformOneSecond) nextevent = now + mDNSPlatformOneSecond;  // try again in a second
	}
        if (result == t_terminated || result == t_error)  
        //since we're already doing a list traversal, we unlink the request manunally instead of calling unlink_request()
            {
            tmp = req;
            if (prev) prev->next = req->next;
            if (req == all_requests) all_requests = all_requests->next;
            req = req->next;
            freeL("udsserver_idle", tmp);
            }
        else 
            {
            prev = req;
            req = req->next;
            }
        }
    return nextevent;
    }

void udsserver_info(void)
    {
    request_state *req;
	qlist_t *qlist;
    for (req = all_requests; req; req=req->next)
        {
        void *t = req->termination_context;
        if (!t) continue;
        if (req->terminate == regservice_termination_callback)
            LogMsgNoIdent("DNSServiceRegister         %##s %u", ((registered_service *)t)->srs->RR_SRV.resrec.name.c, SRS_PORT(((registered_service *)t)->srs));
        else if (req->terminate == browse_termination_callback)
			{
			for (qlist = ((browse_termination_context *)t)->qlist; qlist; qlist = qlist->next)
				LogMsgNoIdent("DNSServiceBrowse           %##s", qlist->q.qname.c);
			}
        else if (req->terminate == resolve_termination_callback)
            LogMsgNoIdent("DNSServiceResolve          %##s", ((resolve_termination_t *)t)->srv->question.qname.c);
        else if (req->terminate == question_termination_callback)
            LogMsgNoIdent("DNSServiceQueryRecord      %##s", ((DNSQuestion *)          t)->qname.c);
        else if (req->terminate == enum_termination_callback)
            LogMsgNoIdent("DNSServiceEnumerateDomains %##s", ((enum_termination_t *)   t)->all->question.qname.c);
        }
    }


static void rename_service(registered_service *srv)
	{
    mStatus err;
	
	if (srv->autoname && !SameDomainLabel(srv->name.c, gmDNS->nicelabel.c))
		{
		srv->rename_on_memfree = 1;
		err = mDNS_DeregisterService(gmDNS, srv->srs);
		if (err) LogMsg("ERROR: udsserver_handle_configchange: DeregisterService returned error %d.  Continuing.", err);
		// error should never occur - safest to log and continue
		}	
	}

void udsserver_handle_configchange(void)
    {
    request_state *req;

    
    for (req = all_requests; req; req = req->next)
        {
		if (req->servicepair.local) rename_service(req->servicepair.local);
		if (req->servicepair.global) rename_service(req->servicepair.global);
		}
    }

static void connect_callback(void *info _UNUSED)
    {
    int sd, clilen, optval;
    struct sockaddr_un cliaddr;
    request_state *rstate;
//    int errpipe[2];
    
    clilen = sizeof(cliaddr);
    sd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    if (sd < 0)
        {
        if (errno == EWOULDBLOCK) return; 
        my_perror("ERROR: accept");
        return;
    	}
    optval = 1;
#ifdef SO_NOSIGPIPE
	// Some environments (e.g. OS X) support turning off SIGPIPE for a socket
    if (setsockopt(sd, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) < 0)
    	{
        my_perror("ERROR: setsockopt - SOL_NOSIGPIPE - aborting client");  
        close(sd);
        return;
    	}
#endif

    if (fcntl(sd, F_SETFL, O_NONBLOCK) < 0)
    	{
        my_perror("ERROR: could not set connected socket to non-blocking mode - aborting client");
        close(sd);    	
        return;
        }

/*
    // open a pipe to deliver error messages, pass descriptor to client
    if (pipe(errpipe) < 0)
        {
        my_perror("ERROR: could not create pipe");
        exit(1);
        }
    
    if (ioctl(sd, I_SENDFD, errpipe[0]) < 0)
        {
        my_perror("ERROR: could not pass pipe descriptor to client.  Aborting client.\n");
        close(sd);
        return;
        }
    if (fcntl(errpipe[1], F_SETFL, O_NONBLOCK) < 0)
    	{
        my_perror("ERROR: could not set error pipe to non-blocking mode - aborting client");
        close(sd);    	
        close(errpipe[1]);
        return;
        }
  */
  
      // allocate a request_state struct that will live with the socket
    rstate = mallocL("connect_callback", sizeof(request_state));
    if (!rstate)
    	{
        my_perror("ERROR: malloc");
        exit(1);
    	}
    bzero(rstate, sizeof(request_state));
    rstate->ts = t_morecoming;
    rstate->sd = sd;
    //rstate->errfd = errpipe[1];
    
    if ( mStatus_NoError != udsSupportAddFDToEventLoop( sd, request_callback, rstate))
        return;
    rstate->next = all_requests;
    all_requests = rstate;
    }


// handler
static void request_callback(void *info)
    {
    request_state *rstate = info;
    transfer_state result;
    struct sockaddr_un cliaddr;
    char ctrl_path[MAX_CTLPATH];
    
    result = read_msg(rstate);
    if (result == t_morecoming)
    	{
        return;
    	}
    if (result == t_terminated)
    	{
        abort_request(rstate);
        unlink_request(rstate);
        return;
    	}
    if (result == t_error)
    	{
        abort_request(rstate);
        unlink_request(rstate);
        return;
    	}

    if (rstate->hdr.version != VERSION)
    {
        LogMsg("ERROR: client incompatible with daemon (client version = %d, "
                "daemon version = %d)\n", rstate->hdr.version, VERSION);
        abort_request(rstate);
        unlink_request(rstate);
        return;
    }
    
    if (validate_message(rstate) < 0)
	{
	// note that we cannot deliver an error message if validation fails, since the path to the error socket
	// may be contained in the (invalid) message body for some message types
	abort_request(rstate);
	unlink_request(rstate);
	LogMsg("Invalid message sent by client - may indicate a malicious program running on this machine!");
	return;
	}    
    
    // check if client wants silent operation
    if (rstate->hdr.flags & IPC_FLAGS_NOREPLY) rstate->no_reply = 1;

    // check if primary socket is to be used for synchronous errors, else open new socket
    if (rstate->hdr.flags & IPC_FLAGS_REUSE_SOCKET)
        rstate->errfd = rstate->sd;
    else
       {
        if ((rstate->errfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
            {
            my_perror("ERROR: socket");	
            exit(1);
            }
        if (fcntl(rstate->errfd, F_SETFL, O_NONBLOCK) < 0)
            {
            my_perror("ERROR: could not set control socket to non-blocking mode");
            abort_request(rstate);
            unlink_request(rstate);
            return;
            }        
        get_string(&rstate->msgdata, ctrl_path, 256);	// path is first element in message buffer
        bzero(&cliaddr, sizeof(cliaddr));
        cliaddr.sun_family = AF_LOCAL;
        strcpy(cliaddr.sun_path, ctrl_path);
        if (connect(rstate->errfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0)
            {
            my_perror("ERROR: connect");
            abort_request(rstate);
            unlink_request(rstate);
            }
        }

        


    switch(rstate->hdr.op.request_op)
    	{
        case resolve_request: handle_resolve_request(rstate); break;
        case query_request: handle_query_request(rstate);  break;
        case browse_request: handle_browse_request(rstate); break;
        case reg_service_request: handle_regservice_request(rstate);  break;
        case enumeration_request: handle_enum_request(rstate); break;
        case reg_record_request: handle_regrecord_request(rstate);  break;
        case add_record_request: handle_add_request(rstate); break;
        case update_record_request: handle_update_request(rstate); break;
        case remove_record_request: handle_removerecord_request(rstate); break;
        case reconfirm_record_request: handle_reconfirm_request(rstate); break;
        default:
            debugf("ERROR: udsserver_recv_request - unsupported request type: %d", rstate->hdr.op.request_op);
    	}
    }

// mDNS operation functions.  Each operation has 3 associated functions - a request handler that parses
// the client's request and makes the appropriate mDNSCore call, a result handler (passed as a callback
// to the mDNSCore routine) that sends results back to the client, and a termination routine that aborts
// the mDNSCore operation if the client dies or closes its socket.


// query and resolve calls have separate request handlers that parse the arguments from the client and
// massage the name parameters appropriately, but the rest of the operations (making the query call,
// delivering the result to the client, and termination) are identical.

static void handle_query_request(request_state *rstate)
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    char name[256];
    uint16_t rrtype, rrclass;
    char *ptr;
    mStatus result;
    mDNSInterfaceID InterfaceID;
	DNSQuestion *q;
	
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_query_request - transfer state != t_complete");
        goto error;
        }
    ptr = rstate->msgdata;
    if (!ptr)
        {
        LogMsg("ERROR: handle_query_request - NULL msgdata");
        goto error;
        }
	
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    if (get_string(&ptr, name, 256) < 0) goto bad_param;
    rrtype = get_short(&ptr);
    rrclass = get_short(&ptr);
	InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, ifi);
    if (ifi && !InterfaceID) goto bad_param;

    q = mallocL("DNSQuestion", sizeof(DNSQuestion));
    if (!q)
    	{
        my_perror("ERROR: handle_query - malloc");
        exit(1);
    	}
    bzero(q, sizeof(DNSQuestion));	

    if (!MakeDomainNameFromDNSNameString(&q->qname, name)) { freeL("DNSQuestion", q); goto bad_param; }	
    q->QuestionContext = rstate;
    q->QuestionCallback = question_result_callback;
    q->qtype = rrtype;
    q->qclass = rrclass;
    q->InterfaceID = InterfaceID;
    q->Target = zeroAddr;
	if (flags & kDNSServiceFlagsLongLivedQuery) q->LongLived = mDNStrue;
    rstate->termination_context = q;
    rstate->terminate = question_termination_callback;

    result = mDNS_StartQuery(gmDNS, q);
    if (result != mStatus_NoError) LogMsg("ERROR: mDNS_StartQuery: %d", (int)result);
	
    if (result) rstate->terminate = NULL;
    if (deliver_error(rstate, result) < 0) goto error;
    return;
    
bad_param:
    deliver_error(rstate, mStatus_BadParamErr);
    rstate->terminate = NULL;	// don't try to terminate insuccessful Core calls
error:
    abort_request(rstate);
    unlink_request(rstate);
    return;
    }

static void handle_resolve_request(request_state *rstate)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    mDNSInterfaceID InterfaceID;
    char name[256], regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
    char *ptr;  // message data pointer
    domainname fqdn;
    resolve_t *srv, *txt;
    resolve_termination_t *term;
    mStatus err;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_resolve_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
        
    // extract the data from the message
    ptr = rstate->msgdata;
    if (!ptr)
        {
        LogMsg("ERROR: handle_resolve_request - NULL msgdata");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, interfaceIndex);
    if (interfaceIndex && !InterfaceID) goto bad_param;
    if (get_string(&ptr, name, 256) < 0 ||
        get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0)
        goto bad_param;

    // free memory in rstate since we don't need it anymore
    freeL("handle_resolve_request", rstate->msgbuf);
    rstate->msgbuf = NULL;
        
    if (build_domainname_from_strings(&fqdn, name, regtype, domain) < 0)
        goto bad_param;

    // allocate question wrapper structs
    srv = mallocL("handle_resolve_request", sizeof(resolve_t));
    txt = mallocL("handle_resolve_request", sizeof(resolve_t));
    if (!srv || !txt) goto malloc_error;
    srv->qtype = kDNSType_SRV;
    txt->qtype = kDNSType_TXT;
    srv->rstate = rstate;
    txt->rstate = rstate;
    
    // format questions
    srv->question.QuestionContext = rstate;
    srv->question.QuestionCallback = resolve_result_callback;
    memcpy(&srv->question.qname, &fqdn, MAX_DOMAIN_NAME);
    srv->question.qtype = kDNSType_SRV;
    srv->question.qclass = kDNSClass_IN;
    srv->question.InterfaceID = InterfaceID;
    srv->question.Target      = zeroAddr;

    txt->question.QuestionContext = rstate;
    txt->question.QuestionCallback = resolve_result_callback;
    memcpy(&txt->question.qname, &fqdn, MAX_DOMAIN_NAME);
    txt->question.qtype = kDNSType_TXT;
    txt->question.qclass = kDNSClass_IN;
    txt->question.InterfaceID = InterfaceID;
    txt->question.Target      = zeroAddr;

    // set up termination info
    term = mallocL("handle_resolve_request", sizeof(resolve_termination_t));
    if (!term) goto malloc_error;
    term->srv = srv;
    term->txt = txt;
    term->rstate = rstate;
    rstate->termination_context = term;
    rstate->terminate = resolve_termination_callback;
    
    // set up reply wrapper struct (since answer will come via 2 callbacks)
    rstate->resolve_results = mallocL("handle_resolve_response", sizeof(resolve_result_t));
    if (!rstate->resolve_results) goto malloc_error;
    bzero(rstate->resolve_results, sizeof(resolve_result_t));

    // ask the questions
    err = mDNS_StartQuery(gmDNS, &srv->question);
    if (!err) err = mDNS_StartQuery(gmDNS, &txt->question);

    if (err)
        {
        freeL("handle_resolve_request", txt);
        freeL("handle_resolve_request", srv);
        freeL("handle_resolve_request", term);
        freeL("handle_resolve_request", rstate->resolve_results);
        rstate->terminate = NULL;  // prevent abort_request() from invoking termination callback
        }
    if (deliver_error(rstate, err) < 0 || err) 
        {
        abort_request(rstate);
        unlink_request(rstate);
        }
    return;

bad_param:
    deliver_error(rstate, mStatus_BadParamErr);
    abort_request(rstate);
    unlink_request(rstate);
    return;
    
malloc_error:
    my_perror("ERROR: malloc");
    exit(1);
    }
    
static void resolve_termination_callback(void *context)
    {
    resolve_termination_t *term = context;
    request_state *rs;
    
    if (!term) 
        {
        LogMsg("ERROR: resolve_termination_callback: double termination");
        return;
        }
    rs = term->rstate;
    
    mDNS_StopQuery(gmDNS, &term->txt->question);
    mDNS_StopQuery(gmDNS, &term->srv->question);
    
    freeL("resolve_termination_callback", term->txt);
    freeL("resolve_termination_callback", term->srv);
    freeL("resolve_termination_callback", term);
    rs->termination_context = NULL;
    freeL("resolve_termination_callback", rs->resolve_results);
    rs->resolve_results = NULL;
    }
    
    

static void resolve_result_callback(mDNS *const m _UNUSED, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
{
    int len = 0;
    char fullname[MAX_ESCAPED_DOMAIN_NAME], target[MAX_ESCAPED_DOMAIN_NAME];
    char *data;
    transfer_state result;
    reply_state *rep;
    request_state *rs = question->QuestionContext;
    resolve_result_t *res = rs->resolve_results;
    
	if (!AddRecord)
		{
		if (answer->rrtype == kDNSType_TXT && res->txt == answer) res->txt = mDNSNULL;
		if (answer->rrtype == kDNSType_SRV && res->srv == answer) res->srv = mDNSNULL;
		return;
		}

    if (answer->rrtype == kDNSType_TXT) res->txt = answer;
    if (answer->rrtype == kDNSType_SRV) res->srv = answer;

    if (!res->txt || !res->srv) return;		// only deliver result to client if we have both answers
    
    ConvertDomainNameToCString(&answer->name, fullname);
    ConvertDomainNameToCString(&res->srv->rdata->u.srv.target, target);

    // calculate reply length
    len += sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);  // interface index
    len += sizeof(DNSServiceErrorType);
    len += strlen(fullname) + 1;
    len += strlen(target) + 1;
    len += 2 * sizeof(uint16_t);  // port, txtLen
    len += res->txt->rdlength;
    
    // allocate/init reply header
    rep =  create_reply(resolve_reply, len, rs);
    rep->rhdr->flags = 0;
    rep->rhdr->ifi =  mDNSPlatformInterfaceIndexfromInterfaceID(gmDNS, answer->InterfaceID);
    rep->rhdr->error = kDNSServiceErr_NoError;
    data = rep->sdata;
    
    // write reply data to message
    put_string(fullname, &data);
    put_string(target, &data);
    put_short(res->srv->rdata->u.srv.port.NotAnInteger, &data);
    put_short(res->txt->rdlength, &data);
    put_rdata(res->txt->rdlength, res->txt->rdata->u.txt.c, &data);
    
    result = send_msg(rep);
    if (result == t_error || result == t_terminated) 
        {  
        abort_request(rs);  
        unlink_request(rs);
        freeL("resolve_result_callback", rep);  
        }
    else if (result == t_complete) freeL("resolve_result_callback", rep);
    else append_reply(rs, rep);
    }
 
// what gets called when a resolve is completed and we need to send the data back to the client
static void question_result_callback(mDNS *const m _UNUSED, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
    {
    char *data;
    char name[MAX_ESCAPED_DOMAIN_NAME];
    request_state *req;
    reply_state *rep;
    int len;

    //mDNS_StopQuery(m, question);
    req = question->QuestionContext;
    
    // calculate reply data length
    len = sizeof(DNSServiceFlags);
    len += 2 * sizeof(uint32_t);  // if index + ttl
    len += sizeof(DNSServiceErrorType);
    len += 3 * sizeof(uint16_t); // type, class, rdlen
    len += answer->rdlength;
    ConvertDomainNameToCString(&answer->name, name);
    len += strlen(name) + 1;
    
    rep =  create_reply(query_reply, len, req);
    rep->rhdr->flags = AddRecord ? kDNSServiceFlagsAdd : 0;
    rep->rhdr->ifi =  mDNSPlatformInterfaceIndexfromInterfaceID(gmDNS, answer->InterfaceID);
    rep->rhdr->error = kDNSServiceErr_NoError;
    data = rep->sdata;
    
    put_string(name, &data);
    put_short(answer->rrtype, &data);
    put_short(answer->rrclass, &data);
    put_short(answer->rdlength, &data);
    put_rdata(answer->rdlength, (char *)&answer->rdata->u, &data);
    put_long(AddRecord ? answer->rroriginalttl : 0, &data);

    append_reply(req, rep);
    return;
    }

static void question_termination_callback(void *context)
    {
    DNSQuestion *q = context;


    mDNS_StopQuery(gmDNS, q);  // no need to error check
    freeL("question_termination_callback", q);
    }


static void handle_browse_request(request_state *request)
    {
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    mDNSInterfaceID InterfaceID;
    char regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME];
	qlist_t *qlist = NULL, *qlist_elem;
    domainname typedn;
    char *ptr;
    mStatus result;
	DNameListElem *search_domain_list, *sdom, tmp;
	browse_termination_context *term;
	
    if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_browse_request - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

    // extract data from message
    ptr = request->msgdata;
    flags = get_flags(&ptr);
    interfaceIndex = get_long(&ptr);
    if (get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 || 
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0)
        goto bad_param;        
    freeL("handle_browse_request", request->msgbuf);
    request->msgbuf = NULL;

    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, interfaceIndex);
    if (interfaceIndex && !InterfaceID) goto bad_param;

	if (!MakeDomainNameFromDNSNameString(&typedn, regtype)) goto bad_param;

	//!!!KRS browse locally for ichat
	if (!domain[0] && (!strcmp(regtype, "_ichat._tcp.") || !strcmp(regtype, "_presence._tcp.")))
		strcpy(domain,"local.");
	
	if (domain[0])
		{
		// generate a fake list of one elem to reduce number of code paths
		if (!MakeDomainNameFromDNSNameString(&tmp.name, domain)) goto bad_param;
		tmp.next = NULL;
		search_domain_list = &tmp;
		}
	else search_domain_list = mDNSPlatformGetSearchDomainList();

	for (sdom = search_domain_list; sdom; sdom = sdom->next)
		{
		qlist_elem = mallocL("handle_browse_request", sizeof(qlist_t));
		if (!qlist_elem)
			{
			my_perror("ERROR: handle_browse_request - malloc");
			exit(1);
			}
		bzero(qlist_elem, sizeof(qlist_t));
		qlist_elem->q.QuestionContext = request;
		qlist_elem->q.QuestionCallback = browse_result_callback;
		qlist_elem->next = qlist;
		qlist = qlist_elem;
		}
	
	// setup termination context
	term = (browse_termination_context *)mallocL("handle_browse_request", sizeof(browse_termination_context));
	if (!term)
    	{
        my_perror("ERROR: handle_browse_request - malloc");
        exit(1);
    	}
	term->qlist = qlist;
	term->rstate = request;
	request->termination_context = term;
    request->terminate = browse_termination_callback;
	
	// start the browses
	sdom = search_domain_list;
	for (qlist_elem = qlist; qlist_elem; qlist_elem = qlist_elem->next)
		{		
		result = mDNS_StartBrowse(gmDNS, &qlist_elem->q, &typedn, &sdom->name, InterfaceID, browse_result_callback, request);
		if (result)
			{
			// bail here on error.  questions not yet issued are in no core lists, so they can be deallocated lazily
			if (search_domain_list != &tmp) mDNS_FreeDNameList(search_domain_list);
			deliver_error(request, result);
			return;
			}
		sdom = sdom->next;
		}
	if (search_domain_list != &tmp) mDNS_FreeDNameList(search_domain_list);
	deliver_error(request, mStatus_NoError);
	return;
    
bad_param:
    deliver_error(request, mStatus_BadParamErr);
    abort_request(request);
    unlink_request(request);
    }

static void browse_result_callback(mDNS *const m _UNUSED, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
    {
    request_state *req;
    reply_state *rep;
    mStatus err;
        
    req = question->QuestionContext;

    err = gen_rr_response(&answer->rdata->u.name, answer->InterfaceID, req, &rep);
    if (err)
        {
        if (deliver_async_error(req, browse_reply, err) < 0) 
            {
            abort_request(req);
            unlink_request(req);
            }
        return;
        }
    if (AddRecord) rep->rhdr->flags |= kDNSServiceFlagsAdd;  // non-zero TTL indicates add
    append_reply(req, rep);
    return;
    }

static void browse_termination_callback(void *context)
    {
	browse_termination_context *t = context;
	qlist_t *ptr, *fptr;
	
	if (!t) return;

	ptr = t->qlist;
	t->qlist = NULL;

	while(ptr)
		{
		mDNS_StopBrowse(gmDNS, &ptr->q);  // no need to error-check result
		fptr = ptr;
		ptr = ptr->next;
		freeL("browse_termination_callback", fptr);
		}
	t->rstate->termination_context = NULL;
	freeL("browse_termination_callback", t);
	}

static mStatus register_service(request_state *request, registered_service **srv_ptr, DNSServiceFlags flags,
	uint16_t txtlen, void *txtdata, mDNSIPPort port, domainlabel *n, char *type_as_string,
    domainname *t, domainname *d, domainname *h, mDNSBool autoname, int num_subtypes, mDNSInterfaceID InterfaceID)
	{
	registered_service *r_srv;
    int srs_size, i;
	char *sub;
	mStatus result;	
	*srv_ptr = NULL;
	
	r_srv = mallocL("handle_regservice_request", sizeof(registered_service));
    if (!r_srv) goto malloc_error;
    srs_size = sizeof(ServiceRecordSet) + (sizeof(RDataBody) > txtlen ? 0 : txtlen - sizeof(RDataBody));
    r_srv->srs = mallocL("handle_regservice_request", srs_size);
    if (!r_srv->srs) goto malloc_error;
    if (num_subtypes > 0)
        {
        r_srv->subtypes = mallocL("handle_regservice_request", num_subtypes * sizeof(AuthRecord));
        if (!r_srv->subtypes) goto malloc_error;
        sub = type_as_string + strlen(type_as_string) + 1;
        for (i = 0; i < num_subtypes; i++)
            {
            if (!MakeDomainNameFromDNSNameString(&(r_srv->subtypes + i)->resrec.name, sub))
                {
				free_service_registration(r_srv);
                return mStatus_BadParamErr;
                }
            sub += strlen(sub) + 1;
            }
        }
    else r_srv->subtypes = NULL;
    r_srv->request = request;
    
    r_srv->extras = NULL;
    r_srv->autoname = autoname;
    r_srv->rename_on_memfree = 0;
    r_srv->renameonconflict = !(flags & kDNSServiceFlagsNoAutoRename);
	memcpy(r_srv->name.c, n->c, n->c[0]);

    result = mDNS_RegisterService(gmDNS, r_srv->srs, n, t, d, h, port,
    	txtdata, txtlen, r_srv->subtypes, num_subtypes, InterfaceID, regservice_callback, r_srv);

	if (result)
		free_service_registration(r_srv);
	else *srv_ptr = r_srv;

	return result;

malloc_error:
    my_perror("ERROR: malloc");
    exit(1);
	}


// service registration
static void handle_regservice_request(request_state *request)
    {
    DNSServiceFlags flags;
    uint32_t ifi;
    char name[256], regtype[MAX_ESCAPED_DOMAIN_NAME], domain[MAX_ESCAPED_DOMAIN_NAME], host[MAX_ESCAPED_DOMAIN_NAME];
    uint16_t txtlen;
    mDNSIPPort port;
    void *txtdata;
    char *ptr, *sub;
    domainlabel n;
    domainname d, h, t, srv;
    mStatus result;
    mDNSInterfaceID InterfaceID;
	int num_subtypes;
    char *rtype_ptr;
    
	if (request->ts != t_complete)
        {
        LogMsg("ERROR: handle_regservice_request - transfer state != t_complete");
        abort_request(request);
        unlink_request(request);
        return;
        }

    // extract data from message
    ptr = request->msgdata;
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, ifi);
    if (ifi && !InterfaceID) goto bad_param;
    if (get_string(&ptr, name, 256) < 0 ||
        get_string(&ptr, regtype, MAX_ESCAPED_DOMAIN_NAME) < 0 || 
        get_string(&ptr, domain, MAX_ESCAPED_DOMAIN_NAME) < 0 ||
        get_string(&ptr, host, MAX_ESCAPED_DOMAIN_NAME) < 0)
        goto bad_param;
        
    port.NotAnInteger = get_short(&ptr);
    txtlen = get_short(&ptr);
    txtdata = get_rdata(&ptr, txtlen);

    if (!*regtype || !MakeDomainNameFromDNSNameString(&t, regtype)) goto bad_param;

    // count subtypes, replacing commas w/ whitespace
    rtype_ptr = regtype;
    num_subtypes = -1;
    while((sub = strsep(&rtype_ptr, ",")))
        if (*sub) num_subtypes++;
        
    if (!name[0]) n = (gmDNS)->nicelabel;
    else if (!MakeDomainLabelFromLiteralString(&n, name))  
        goto bad_param;
	
    if ((!MakeDomainNameFromDNSNameString(&d, *domain ? domain : "local.")) ||
		(!ConstructServiceName(&srv, &n, &t, &d)))
        goto bad_param;

	if (host[0] && !MakeDomainNameFromDNSNameString(&h, host)) goto bad_param;

	result = register_service(request, &request->servicepair.local, flags, txtlen, txtdata, port, &n,  &regtype[0], &t, &d, host[0] ? &h : NULL, !name[0], num_subtypes, InterfaceID);

	//!!!KRS if we got a dynamic reg domain from the config file, use it for default (except for iChat)
	if (!domain[0] && gmDNS->uDNS_info.ServiceRegDomain[0] && strcmp(regtype, "_presence._tcp.") && strcmp(regtype, "_ichat._tcp."))
		{
		MakeDomainNameFromDNSNameString(&d, gmDNS->uDNS_info.ServiceRegDomain);
		register_service(request, &request->servicepair.global, flags, txtlen, txtdata, port, &n, &regtype[0], &t, &d, host[0] ? &h : NULL, !name[0], num_subtypes, InterfaceID);
		// don't return default global errors - it will confuse legacy clients, and we want .local to still work for them
		}
		
    request->termination_context = &request->servicepair;
    request->terminate = regservice_termination_callback;
    
    deliver_error(request, result);
    if (result != mStatus_NoError) 
        {
        abort_request(request);
        unlink_request(request);
        }
    else 
        reset_connected_rstate(request);  // reset to receive add/remove messages

    return;

bad_param:
    deliver_error(request, mStatus_BadParamErr);
    abort_request(request);
    unlink_request(request);
    }




// service registration callback performs three duties - frees memory for deregistered services,
// handles name conflicts, and delivers completed registration information to the client (via
// process_service_registraion())

static void regservice_callback(mDNS *const m _UNUSED, ServiceRecordSet *const srs, mStatus result)
    {
    mStatus err;
    registered_service *r_srv = srs->ServiceContext;
    request_state *rs = r_srv->request;
    
    if (!rs && (result != mStatus_MemFree && !r_srv->rename_on_memfree))
        {     
        // error should never happen - safest to log and continue
        LogMsg("ERROR: regservice_callback: received result %d with a NULL request pointer\n");
        return;
        }

    if (result == mStatus_NoError)
        return process_service_registration(srs);
    else if (result == mStatus_MemFree)
        {
        if (r_srv->rename_on_memfree)
            {
            r_srv->rename_on_memfree = 0;
            r_srv->name = gmDNS->nicelabel;
            err = mDNS_RenameAndReregisterService(gmDNS, srs, &r_srv->name);
            if (err) LogMsg("ERROR: regservice_callback - RenameAndReregisterService returned %d", err);
            // error should never happen - safest to log and continue
            }
        else 
            {
			free_service_registration(r_srv);
            return;
            }
        }
    else if (result == mStatus_NameConflict)
    	{
        if (r_srv->autoname || r_srv->renameonconflict)
            {
            mDNS_RenameAndReregisterService(gmDNS, srs, mDNSNULL);
            return;
            }
        else
            {
			free_service_registration(r_srv);
			if (deliver_async_error(rs, reg_service_reply, result) < 0) 
                {
                abort_request(rs);
                unlink_request(rs);
                }
            return;
            }
    	} 
    else 
        {
        LogMsg("ERROR: unknown result in regservice_callback: %d", result);
        if (deliver_async_error(rs, reg_service_reply, result) < 0) 
            {
            abort_request(rs);
            unlink_request(rs);
            }
        return;
        }
    }

static mStatus add_record_to_service(request_state *rstate, registered_service *r_srv, uint16_t rrtype, uint16_t rdlen, char *rdata, uint32_t ttl)
	{
	ServiceRecordSet *srs = r_srv->srs;
    ExtraResourceRecord *extra;
    extra_record_entry *ere;
	mStatus result;
	int size;
	
	if (rdlen > sizeof(RDataBody)) size = rdlen;
    else size = sizeof(RDataBody);
	
    ere = mallocL("hanle_add_request", sizeof(extra_record_entry) - sizeof(RDataBody) + size);
    if (!ere)
        {
        my_perror("ERROR: malloc");
        exit(1);
        }
        
    bzero(ere, sizeof(ExtraResourceRecord));  // OK if oversized rdata not zero'd
    extra = &ere->e;
    extra->r.resrec.rrtype = rrtype;
    extra->r.rdatastorage.MaxRDLength = size;
    extra->r.resrec.rdlength = rdlen;
    memcpy(&extra->r.rdatastorage.u.data, rdata, rdlen);

    result =  mDNS_AddRecordToService(gmDNS, srs , extra, &extra->r.rdatastorage, ttl);
	if (result) { freeL("handle_add_request", ere); return result; }

    ere->key = rstate->hdr.reg_index;
    ere->next = r_srv->extras;
    r_srv->extras = ere;
	return result;
	}


static void handle_add_request(request_state *rstate)
    {
    uint32_t ttl;
    uint16_t rrtype, rdlen;
    char *ptr, *rdata;
    mStatus result;
    DNSServiceFlags flags;
	registered_service *local, *global;

	local = rstate->servicepair.local;
	global = rstate->servicepair.global;

	if (!local)
        {
        LogMsg("ERROR: handle_add_request - no service registered");
        deliver_error(rstate, mStatus_UnknownErr);
        return;
        }	

    ptr = rstate->msgdata;
    flags = get_flags(&ptr);
    rrtype = get_short(&ptr);
    rdlen = get_short(&ptr);
    rdata = get_rdata(&ptr, rdlen);
    ttl = get_long(&ptr);
    
	result = add_record_to_service(rstate, local, rrtype, rdlen, rdata, ttl);
	if (global) add_record_to_service(rstate, global, rrtype, rdlen, rdata, ttl); // don't report global errors to client
	
    deliver_error(rstate, result);
    reset_connected_rstate(rstate);
    }

static mStatus update_record(AuthRecord *rr, uint16_t rdlen, char *rdata, uint32_t ttl)
	{	
	int rdsize;
	RData *newrd;
	mStatus result;
	
	if (rdlen > sizeof(RDataBody)) rdsize = rdlen;
    else rdsize = sizeof(RDataBody);
    newrd = mallocL("handle_update_request", sizeof(RData) - sizeof(RDataBody) + rdsize);
    if (!newrd)
        {
        my_perror("ERROR: malloc");
        exit(1);
        }
    newrd->MaxRDLength = rdsize;
    memcpy(&newrd->u, rdata, rdlen);
    result = mDNS_Update(gmDNS, rr, ttl, rdlen, newrd, update_callback);
	if (result) { LogMsg("ERROR: mDNS_Update - %d", result); freeL("handle_update_request", newrd); }
	return result;
	}

static mStatus find_extras_by_key(request_state *rstate, AuthRecord **lRR, AuthRecord **gRR)	
	{
	extra_record_entry *e;
	
	// find the extra record for the local service
	for (e = rstate->servicepair.local->extras; e; e = e->next)
		if (e->key == rstate->hdr.reg_index) break;
	if (!e) return mStatus_BadReferenceErr;

	*lRR = &e->e.r;

	// find the corresponding global record, if it exists
	if (rstate->servicepair.global)
		{
		for (e = rstate->servicepair.global->extras; e; e = e->next)
			if (e->key == rstate->hdr.reg_index) break;
		if (e) *gRR = &e->e.r;
		else *gRR = NULL;
		}
	return mStatus_NoError;
	}

static void handle_update_request(request_state *rstate)
    {
    registered_record_entry *reptr;
    AuthRecord *lRR, *gRR = NULL;
    uint16_t rdlen;
    char *ptr, *rdata;
    uint32_t ttl;
    mStatus result;
	
    if (rstate->hdr.reg_index == TXT_RECORD_INDEX)
        {
		if (!rstate->servicepair.local)
            {
            deliver_error(rstate, mStatus_BadParamErr);
            return;
            }
		lRR = &rstate->servicepair.local->srs->RR_TXT;
		if (rstate->servicepair.global) gRR = &rstate->servicepair.global->srs->RR_TXT;
        }
    else
        {
		if (rstate->servicepair.local) // registered service
			{
			if (find_extras_by_key(rstate, &lRR, &gRR))
				{ deliver_error(rstate, mStatus_BadReferenceErr); return; }
			}
		else
			{
			// record created via RegisterRecord			
			reptr = rstate->reg_recs;
			while(reptr && reptr->key != rstate->hdr.reg_index) reptr = reptr->next;			
			if (!reptr) { deliver_error(rstate, mStatus_BadReferenceErr); return; }
			lRR = reptr->rr;
			}
		}

	// get the message data
	ptr = rstate->msgdata;
    get_flags(&ptr);	// flags unused
    rdlen = get_short(&ptr);
    rdata = get_rdata(&ptr, rdlen);
    ttl = get_long(&ptr);

	result = update_record(lRR, rdlen, rdata, ttl);
	if (gRR) update_record(gRR, rdlen, rdata, ttl);  // don't report errors for global registration
	
    deliver_error(rstate, result);
    reset_connected_rstate(rstate);
    }
    
static void update_callback(mDNS *const m _UNUSED, AuthRecord *const rr, RData *oldrd)
    {    
    if (oldrd != &rr->rdatastorage) freeL("update_callback", oldrd);
    }
    
static void process_service_registration(ServiceRecordSet *const srs)
    {
    reply_state *rep;
    transfer_state send_result;
    mStatus err;
    registered_service *r_srv = srs->ServiceContext;
    request_state *req = r_srv->request;


    err = gen_rr_response(&srs->RR_SRV.resrec.name, srs->RR_SRV.resrec.InterfaceID, req, &rep);
    if (err) 
        {
        if (deliver_async_error(req, reg_service_reply, err) < 0)
            {
            abort_request(req);
            unlink_request(req);
            }
        return;
        }
    send_result = send_msg(rep);
    if (send_result == t_error || send_result == t_terminated) 
        {  
        abort_request(req);  
        unlink_request(req);
        freeL("process_service_registration", rep);  
        }
    else if (send_result == t_complete) freeL("process_service_registration", rep);
    else append_reply(req, rep);
    }

static void free_service_registration(registered_service *srv)
	{
	request_state *rstate = srv->request;
	extra_record_entry *extra;
	
	// clear pointers from parent struct
	if (rstate)
		{
		if (rstate->servicepair.local == srv) rstate->servicepair.local = NULL;
		else if (rstate->servicepair.global == srv) rstate->servicepair.global = NULL;
		}

	while (srv->extras)
		{
		extra = srv->extras;
		srv->extras = srv->extras->next;
		if (extra->e.r.resrec.rdata != &extra->e.r.rdatastorage)
			freeL("free_service_registration", extra->e.r.resrec.rdata);
		freeL("regservice_callback", extra);
		}
	
	if (srv->subtypes) { freeL("regservice_callback", srv->subtypes); srv->subtypes = NULL; }
	freeL("regservice_callback", srv->srs);
	srv->srs = NULL;
	freeL("regservice_callback", srv);
	}

static void regservice_termination_callback(void *context)
    {
    servicepair_t *pair = context;

	if (!pair->local && !pair->global) { LogMsg("ERROR: regservice_termination_callback called with null services"); return; }

	// clear service pointers to parent request state
	if (pair->local) pair->local->request = NULL;
	if (pair->global) pair->global->request = NULL;
	
    // only safe to free memory if registration is not valid, ie deregister fails
    if (pair->local && mDNS_DeregisterService(gmDNS, pair->local->srs) != mStatus_NoError)
		free_service_registration(pair->local);
	if (pair->global && mDNS_DeregisterService(gmDNS, pair->global->srs) != mStatus_NoError)
		free_service_registration(pair->global);

	// clear pointers to services - they'll get cleaned by MemFree callback
	pair->local = NULL;
	pair->global = NULL;	
	}


static void handle_regrecord_request(request_state *rstate)
    {
    AuthRecord *rr;
    regrecord_callback_context *rcc;
    registered_record_entry *re;
    mStatus result;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_regrecord_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
        
    rr = read_rr_from_ipc_msg(rstate->msgdata, 1, 1);
    if (!rr) 
        {
        deliver_error(rstate, mStatus_BadParamErr);
        return;
        }
	
    rcc = mallocL("hanlde_regrecord_request", sizeof(regrecord_callback_context));
    if (!rcc) goto malloc_error;
    rcc->rstate = rstate;
    rcc->client_context = rstate->hdr.client_context;
    rr->RecordContext = rcc;
    rr->RecordCallback = regrecord_callback;

    // allocate registration entry, link into list
    re = mallocL("hanlde_regrecord_request", sizeof(registered_record_entry));
    if (!re) goto malloc_error;
    re->key = rstate->hdr.reg_index;
    re->rr = rr;
    re->next = rstate->reg_recs;
    rstate->reg_recs = re;

    if (!rstate->terminate)
    	{
        rstate->terminate = connected_registration_termination;
        rstate->termination_context = rstate;
    	}
    
    result = mDNS_Register(gmDNS, rr);
    deliver_error(rstate, result); 
    reset_connected_rstate(rstate);
    return;

malloc_error:
    my_perror("ERROR: malloc");
    return;
    }

static void regrecord_callback(mDNS *const m _UNUSED, AuthRecord *const rr, mStatus result)
    {
    regrecord_callback_context *rcc = rr->RecordContext;
    int len;
    reply_state *reply;
    transfer_state ts;

    if (result == mStatus_MemFree) 	
        { 
        freeL("regrecord_callback", rcc);
        rr->RecordContext = NULL;        
        freeL("regrecord_callback", rr);
        return; 
        }

    // format result, add to the list for the request, including the client context in the header
    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);                //interfaceIndex
    len += sizeof(DNSServiceErrorType);
    
    reply = create_reply(reg_record_reply, len, rcc->rstate);
    reply->mhdr->client_context = rcc->client_context;
    reply->rhdr->flags = 0;
    reply->rhdr->ifi = mDNSPlatformInterfaceIndexfromInterfaceID(gmDNS, rr->resrec.InterfaceID);
    reply->rhdr->error = result;

    ts = send_msg(reply);
    if (ts == t_error || ts == t_terminated) 
        {
        abort_request(rcc->rstate);
        unlink_request(rcc->rstate);
        }
    else if (ts == t_complete) freeL("regrecord_callback", reply);
    else if (ts == t_morecoming) append_reply(rcc->rstate, reply);   // client is blocked, link reply into list
    }

static void connected_registration_termination(void *context)
    {
    int shared;
    registered_record_entry *fptr, *ptr = ((request_state *)context)->reg_recs;
    while(ptr)
        {
        fptr = ptr;
        ptr = ptr->next;
        shared = fptr->rr->resrec.RecordType == kDNSRecordTypeShared;
        mDNS_Deregister(gmDNS, fptr->rr);
        if (!shared)
            // shared records free'd via callback w/ mStatus_MemFree
            {
            freeL("connected_registration_termination", fptr->rr->RecordContext);
            fptr->rr->RecordContext = NULL;            
            freeL("connected_registration_termination", fptr->rr);
            fptr->rr = NULL;
            }
        freeL("connected_registration_termination", fptr);
        }
    }
    


static void handle_removerecord_request(request_state *rstate)
    {
    mStatus err;
    char *ptr;

    ptr = rstate->msgdata;
    get_flags(&ptr);	// flags unused

    if (rstate->servicepair.local) err = remove_extra_rr_from_service(rstate);
    else err = remove_record(rstate);

    reset_connected_rstate(rstate);
    if (deliver_error(rstate, err) < 0)
        {
        abort_request(rstate);
        unlink_request(rstate);
        }
    }

// remove a resource record registered via DNSServiceRegisterRecord()
static mStatus remove_record(request_state *rstate)
    {
    int shared;
    registered_record_entry *reptr, *prev = NULL;
    mStatus err = mStatus_UnknownErr;
    reptr = rstate->reg_recs;

    while(reptr)
    	{
        if (reptr->key == rstate->hdr.reg_index)  // found match
            {
            if (prev) prev->next = reptr->next;
            else rstate->reg_recs = reptr->next;
            shared = reptr->rr->resrec.RecordType == kDNSRecordTypeShared;
            err  = mDNS_Deregister(gmDNS, reptr->rr);
	        if (err) 
	            {
	            LogMsg("ERROR: remove_record, mDNS_Deregister: %d", err);
	            return err;	// this should not happen.  don't try to free memory if there's an error
	            }
            if (!shared)
	            // shared records free'd via callback w/ mStatus_MemFree		
	            {
                freeL("remove_record", reptr->rr->RecordContext);
                reptr->rr->RecordContext = NULL;
	            freeL("remove_record", reptr->rr);
	            reptr->rr = NULL;
	            }
            freeL("remove_record", reptr);  	    
            break;
            }
        prev = reptr;
        reptr = reptr->next;
    	}
    return err;
    }


static mStatus remove_extra(request_state *rstate, registered_service *serv)
	{
	mStatus err = mStatus_BadReferenceErr;
	extra_record_entry *ptr, *prev = NULL;
    
    ptr = serv->extras;
    while (ptr)
		{
		if (ptr->key == rstate->hdr.reg_index) // found match
			{
			if (prev) prev->next = ptr->next;
			else serv->extras = ptr->next;
			err = mDNS_RemoveRecordFromService(gmDNS, serv->srs, &ptr->e);
			if (err) return err;
			freeL("remove_extra_rr_from_service", ptr);
			break;
			}
		prev = ptr;
		ptr = ptr->next;
		}
	return err;
	}

static mStatus remove_extra_rr_from_service(request_state *rstate)
    {
    mStatus err = mStatus_UnknownErr;

	err = remove_extra(rstate, rstate->servicepair.local);
	if (rstate->servicepair.global) remove_extra(rstate, rstate->servicepair.global); // don't return error for global

	return err;
    }



// domain enumeration
static void handle_enum_request(request_state *rstate)
    {
    DNSServiceFlags flags, add_default;
    uint32_t ifi;
    mDNSInterfaceID InterfaceID;
    char *ptr = rstate->msgdata;
    domain_enum_t *def, *all;
    enum_termination_t *term;
    reply_state *reply;  // initial default reply
    transfer_state tr;
    mStatus err;
    int result;
    
    if (rstate->ts != t_complete)
        {
        LogMsg("ERROR: handle_enum_request - transfer state != t_complete");
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
        
    flags = get_flags(&ptr);
    ifi = get_long(&ptr);
    InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, ifi);
    if (ifi && !InterfaceID)
    	{
		deliver_error(rstate, mStatus_BadParamErr);
		abort_request(rstate);
		unlink_request(rstate);
    	}

    // allocate context structures
    def = mallocL("hanlde_enum_request", sizeof(domain_enum_t));
    all = mallocL("handle_enum_request", sizeof(domain_enum_t));
    term = mallocL("handle_enum_request", sizeof(enum_termination_t));
    if (!def || !all || !term)
    	{
        my_perror("ERROR: malloc");
        exit(1);
    	}

    // enumeration requires multiple questions, so we must link all the context pointers so that
    // necessary context can be reached from the callbacks
    def->rstate = rstate;
    all->rstate = rstate;
    term->def = def;
    term->all = all;
    term->rstate = rstate;
    rstate->termination_context = term;
    rstate->terminate = enum_termination_callback;
    def->question.QuestionContext = def;
    def->type = (flags & kDNSServiceFlagsRegistrationDomains) ? 
        mDNS_DomainTypeRegistrationDefault: mDNS_DomainTypeBrowseDefault;
    all->question.QuestionContext = all;
    all->type = (flags & kDNSServiceFlagsRegistrationDomains) ? 
        mDNS_DomainTypeRegistration : mDNS_DomainTypeBrowse;

	// if the caller hasn't specified an explicit interface, we use local-only to get the system-wide list.
	if (!InterfaceID) InterfaceID = mDNSInterface_LocalOnly;
	
    // make the calls
    err = mDNS_GetDomains(gmDNS, &all->question, all->type, NULL, InterfaceID, enum_result_callback, all);
    if (err == mStatus_NoError)
        err = mDNS_GetDomains(gmDNS, &def->question, def->type, NULL, InterfaceID, enum_result_callback, def);
    result = deliver_error(rstate, err);  // send error *before* returning local domain
    
    if (result < 0 || err)
        {
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }

    // provide local. as the first domain automatically
    add_default = kDNSServiceFlagsDefault | kDNSServiceFlagsAdd;
    reply = format_enumeration_reply(rstate, "local.", add_default, ifi, 0);
    tr = send_msg(reply);
    if (tr == t_error || tr == t_terminated) 
        {
        freeL("handle_enum_request", def);
        freeL("handle_enum_request", all);
        abort_request(rstate);
        unlink_request(rstate);
        return;
        }
    if (tr == t_complete) freeL("handle_enum_request", reply);
    if (tr == t_morecoming) append_reply(rstate, reply); // couldn't send whole reply because client is blocked - link into list
    }

static void enum_result_callback(mDNS *const m _UNUSED, DNSQuestion *question, const ResourceRecord *const answer, mDNSBool AddRecord)
    {
    char domain[MAX_ESCAPED_DOMAIN_NAME];
    domain_enum_t *de = question->QuestionContext;
    DNSServiceFlags flags = 0;
    reply_state *reply;

    if (answer->rrtype != kDNSType_PTR) return;
    if (AddRecord)
    	{
        flags |= kDNSServiceFlagsAdd;
        if (de->type == mDNS_DomainTypeRegistrationDefault || de->type == mDNS_DomainTypeBrowseDefault)
            flags |= kDNSServiceFlagsDefault;
    	}
    ConvertDomainNameToCString(&answer->rdata->u.name, domain);
    reply = format_enumeration_reply(de->rstate, domain, flags, mDNSPlatformInterfaceIndexfromInterfaceID(gmDNS, answer->InterfaceID), kDNSServiceErr_NoError);
    if (!reply)
    	{
        LogMsg("ERROR: enum_result_callback, format_enumeration_reply");
        return;
    	}
    reply->next = NULL;
    append_reply(de->rstate, reply);
    return;
    }

static reply_state *format_enumeration_reply(request_state *rstate, const char *domain, DNSServiceFlags flags, uint32_t ifi, DNSServiceErrorType err)
    {
    int len;
    reply_state *reply;
    char *data;
    
    
    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);
    len += sizeof(DNSServiceErrorType);
    len += strlen(domain) + 1;
  
    reply = create_reply(enumeration_reply, len, rstate);
    reply->rhdr->flags = flags;
    reply->rhdr->ifi = ifi;  
    reply->rhdr->error = err;
    data = reply->sdata;
    put_string(domain, &data);
    return reply;
    }

static void enum_termination_callback(void *context)
    {
    enum_termination_t *t = context;
    mDNS *coredata = gmDNS;

    mDNS_StopGetDomains(coredata, &t->all->question);
    mDNS_StopGetDomains(coredata, &t->def->question);
    freeL("enum_termination_callback", t->all);
    freeL("enum_termination_callback", t->def);
    t->rstate->termination_context = NULL;
    freeL("enum_termination_callback", t);
    }

static void handle_reconfirm_request(request_state *rstate)
    {
    AuthRecord *rr;

    rr = read_rr_from_ipc_msg(rstate->msgdata, 0, 1);
    if (!rr) return;
    mDNS_ReconfirmByValue(gmDNS, &rr->resrec);
    abort_request(rstate);
    unlink_request(rstate);
    freeL("handle_reconfirm_request", rr);
    }


// setup rstate to accept new reg/dereg requests
static void reset_connected_rstate(request_state *rstate)
    {
    rstate->ts = t_morecoming;
    rstate->hdr_bytes = 0;
    rstate->data_bytes = 0;
    if (rstate->msgbuf) freeL("reset_connected_rstate", rstate->msgbuf);
    rstate->msgbuf = NULL;
    rstate->bufsize = 0;
    }



// returns a resource record (allocated w/ malloc) containing the data found in an IPC message
// data must be in format flags, interfaceIndex, name, rrtype, rrclass, rdlen, rdata, (optional)ttl
// (ttl only extracted/set if ttl argument is non-zero).  returns NULL for a bad-parameter error
static AuthRecord *read_rr_from_ipc_msg(char *msgbuf, int ttl, int validate_flags)
    {
    char *rdata, name[256];
    AuthRecord *rr;
    DNSServiceFlags flags;
    uint32_t interfaceIndex;
    uint16_t type, class, rdlen;
    int storage_size;

    flags = get_flags(&msgbuf);
	if (validate_flags &&
		!((flags & kDNSServiceFlagsShared) == kDNSServiceFlagsShared) &&
		!((flags & kDNSServiceFlagsUnique) == kDNSServiceFlagsUnique))
		{
		LogMsg("ERROR: Bad resource record flags (must be kDNSServiceFlagsShared or kDNSServiceFlagsUnique)");
		return NULL;
		}
	
	interfaceIndex = get_long(&msgbuf);
    if (get_string(&msgbuf, name, 256) < 0)
        {
        LogMsg("ERROR: read_rr_from_ipc_msg - get_string");
        return NULL;
        }
    type = get_short(&msgbuf);    
    class = get_short(&msgbuf);
    rdlen = get_short(&msgbuf);

    if (rdlen > sizeof(RDataBody)) storage_size = rdlen;
    else storage_size = sizeof(RDataBody);
    
    rr = mallocL("read_rr_from_ipc_msg", sizeof(AuthRecord) - sizeof(RDataBody) + storage_size);
    if (!rr) 	
        { 
        my_perror("ERROR: malloc");  
        exit(1);
        }
    bzero(rr, sizeof(AuthRecord));  // ok if oversized rdata not zero'd
    rr->resrec.rdata = &rr->rdatastorage;
    rr->resrec.InterfaceID = mDNSPlatformInterfaceIDfromInterfaceIndex(gmDNS, interfaceIndex);
    if (!MakeDomainNameFromDNSNameString(&rr->resrec.name, name))
    	{
        LogMsg("ERROR: bad name: %s", name);
        freeL("read_rr_from_ipc_msg", rr);
        return NULL;
    	}
    rr->resrec.rrtype = type;
	if ((flags & kDNSServiceFlagsShared) == kDNSServiceFlagsShared)
        rr->resrec.RecordType = kDNSRecordTypeShared;
    if ((flags & kDNSServiceFlagsUnique) == kDNSServiceFlagsUnique)
        rr->resrec.RecordType = kDNSRecordTypeUnique;
    rr->resrec.rrclass = class;
    rr->resrec.rdlength = rdlen;
    rr->resrec.rdata->MaxRDLength = rdlen;
    rdata = get_rdata(&msgbuf, rdlen);
    memcpy(rr->resrec.rdata->u.data, rdata, rdlen);
    if (ttl)	
    	{
        rr->resrec.rroriginalttl = get_long(&msgbuf);
    	}
    return rr;
    }


// generate a response message for a browse result, service registration result, or any other call with the
// identical callback signature.  on successful completion rep is set to point to a malloc'd reply_state struct,
// and mStatus_NoError is returned.  otherwise the appropriate error is returned.

static mStatus gen_rr_response(domainname *servicename, mDNSInterfaceID id, request_state *request, reply_state **rep)
    {
    char *data;
    int len;
    domainlabel name;
    domainname type, dom;
	char namestr[MAX_DOMAIN_LABEL+1];		// Unescaped name: up to 63 bytes plus C-string terminating NULL.
	char typestr[MAX_ESCAPED_DOMAIN_NAME];
	char domstr [MAX_ESCAPED_DOMAIN_NAME];

    *rep = NULL;
    
    if (!DeconstructServiceName(servicename, &name, &type, &dom))
        return kDNSServiceErr_Unknown;

    ConvertDomainLabelToCString_unescaped(&name, namestr);
    ConvertDomainNameToCString(&type, typestr);
    ConvertDomainNameToCString(&dom, domstr);

    // calculate reply data length
    len = sizeof(DNSServiceFlags);
    len += sizeof(uint32_t);  // if index
    len += sizeof(DNSServiceErrorType);
    len += strlen(namestr) + 1;
    len += strlen(typestr) + 1;
    len += strlen(domstr) + 1;
    
    *rep = create_reply(query_reply, len, request);
    (*rep)->rhdr->flags = 0;
    (*rep)->rhdr->ifi = mDNSPlatformInterfaceIndexfromInterfaceID(gmDNS, id);
    (*rep)->rhdr->error = kDNSServiceErr_NoError;    
    data = (*rep)->sdata;
    
    put_string(namestr, &data);
    put_string(typestr, &data);
    put_string(domstr, &data);
    return mStatus_NoError;
    }


static int build_domainname_from_strings(domainname *srv, char *name, char *regtype, char *domain)
    {
    domainlabel n;
    domainname d, t;

    if (!MakeDomainLabelFromLiteralString(&n, name)) return -1;
    if (!MakeDomainNameFromDNSNameString(&t, regtype)) return -1;
    if (!MakeDomainNameFromDNSNameString(&d, domain)) return -1;
    if (!ConstructServiceName(srv, &n, &t, &d)) return -1;
    return 0;
    }


// append a reply to the list in a request object
static void append_reply(request_state *req, reply_state *rep)
    {
    reply_state *ptr;

    if (!req->replies) req->replies = rep;
    else
    	{
        ptr = req->replies;
        while (ptr->next) ptr = ptr->next;
        ptr->next = rep;
    	}
    rep->next = NULL;
    }


// read_msg may be called any time when the transfer state (rs->ts) is t_morecoming.
// returns the current state of the request (morecoming, error, complete, terminated.)
// if there is no data on the socket, the socket will be closed and t_terminated will be returned
static int read_msg(request_state *rs)
    {
    uint32_t nleft;
    int nread;
    char buf[4];   // dummy for death notification 
    
    if (rs->ts == t_terminated || rs->ts == t_error)
        {
        LogMsg("ERROR: read_msg called with transfer state terminated or error");
        rs->ts = t_error;
        return t_error;
        }
        
    if (rs->ts == t_complete)
    	{  // this must be death or something is wrong
        nread = recv(rs->sd, buf, 4, 0);
        if (!nread) 	{  rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        LogMsg("ERROR: read data from a completed request.");
        rs->ts = t_error;
        return t_error;
    	}

    if (rs->ts != t_morecoming)
        {
        LogMsg("ERROR: read_msg called with invalid transfer state (%d)", rs->ts);
        rs->ts = t_error;
        return t_error;
        }
        
    if (rs->hdr_bytes < sizeof(ipc_msg_hdr))
    	{
        nleft = sizeof(ipc_msg_hdr) - rs->hdr_bytes;
        nread = recv(rs->sd, (char *)&rs->hdr + rs->hdr_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated;  	}
        if (nread < 0) goto rerror;
        rs->hdr_bytes += nread;
        if (rs->hdr_bytes > sizeof(ipc_msg_hdr))
            {
            LogMsg("ERROR: read_msg - read too many header bytes");
            rs->ts = t_error;
            return t_error;
            }
    	}

    // only read data if header is complete
    if (rs->hdr_bytes == sizeof(ipc_msg_hdr))
    	{
        if (rs->hdr.datalen == 0)  // ok in removerecord requests
            {
            rs->ts = t_complete;
            rs->msgbuf = NULL;
            return t_complete;
            }
        
        if (!rs->msgbuf)  // allocate the buffer first time through
            {
            rs->msgbuf = mallocL("read_msg", rs->hdr.datalen + MSG_PAD_BYTES);
            if (!rs->msgbuf)
            	{
                my_perror("ERROR: malloc");
                rs->ts = t_error;
                return t_error;
            	}
            rs->msgdata = rs->msgbuf;
            }
            bzero(rs->msgbuf, rs->hdr.datalen + MSG_PAD_BYTES);
        nleft = rs->hdr.datalen - rs->data_bytes;
        nread = recv(rs->sd, rs->msgbuf + rs->data_bytes, nleft, 0);
        if (nread == 0)  	{ rs->ts = t_terminated;  return t_terminated; 	}
        if (nread < 0)	goto rerror;
        rs->data_bytes += nread;
        if (rs->data_bytes > rs->hdr.datalen)
            {
            LogMsg("ERROR: read_msg - read too many data bytes");
            rs->ts = t_error;
            return t_error;
            }
        }

    if (rs->hdr_bytes == sizeof(ipc_msg_hdr) && rs->data_bytes == rs->hdr.datalen)
        rs->ts = t_complete;
    else rs->ts = t_morecoming;

    return rs->ts;

rerror:
    if (errno == EAGAIN || errno == EINTR) return rs->ts;	
    my_perror("ERROR: read_msg");
    rs->ts = t_error;
    return t_error;
    }


static int send_msg(reply_state *rs)
    {
    ssize_t nwriten;
    
    if (!rs->msgbuf)
        {
        LogMsg("ERROR: send_msg called with NULL message buffer");
        return t_error;
        }
    
    if (rs->request->no_reply)	//!!!KRS this behavior should be optimized if it becomes more common
        {
        rs->ts = t_complete;
        freeL("send_msg", rs->msgbuf);
        return t_complete;
        }

    nwriten = send(rs->sd, rs->msgbuf + rs->nwriten, rs->len - rs->nwriten, 0);
    if (nwriten < 0)
    	{
        if (errno == EINTR || errno == EAGAIN) nwriten = 0;
        else
            {
            if (errno == EPIPE)
            	{
                LogMsg("broken pipe - cleanup should be handled by run-loop read wakeup");
                rs->ts = t_terminated;
                rs->request->ts = t_terminated;
                return t_terminated;  
            	}
            else
            	{
                my_perror("ERROR: send\n");
                rs->ts = t_error;
                return t_error;
            	}
            }
        }
    rs->nwriten += nwriten;

    if (rs->nwriten == rs->len)
    	{
        rs->ts = t_complete;
        freeL("send_msg", rs->msgbuf);
    	}
    return rs->ts;
    }



static reply_state *create_reply(reply_op_t op, int datalen, request_state *request)
{
    reply_state *reply;
    int totallen;

    
    if ((unsigned)datalen < sizeof(reply_hdr))
        {
        LogMsg("ERROR: create_reply - data length less than lenght of required fields");
        return NULL;
        }
    
    totallen = datalen + sizeof(ipc_msg_hdr);
    reply = mallocL("create_reply", sizeof(reply_state));
    if (!reply) 
        {
        my_perror("ERROR: malloc");
        exit(1);
        }
    bzero(reply, sizeof(reply_state));
    reply->ts = t_morecoming;
    reply->sd = request->sd;
    reply->request = request;
    reply->len = totallen;
    reply->msgbuf = mallocL("create_reply", totallen);
    if (!reply->msgbuf)
        {
        my_perror("ERROR: malloc");
        exit(1);
        }
    bzero(reply->msgbuf, totallen);
    reply->mhdr = (ipc_msg_hdr *)reply->msgbuf;
    reply->rhdr = (reply_hdr *)(reply->msgbuf + sizeof(ipc_msg_hdr));
    reply->sdata = reply->msgbuf + sizeof(ipc_msg_hdr) + sizeof(reply_hdr);
    reply->mhdr->version = VERSION;
    reply->mhdr->op.reply_op = op;
    reply->mhdr->datalen = totallen - sizeof(ipc_msg_hdr);
    return reply;
    }


static int deliver_error(request_state *rstate, mStatus err)
    {
    int nwritten = -1;
    undelivered_error_t *undeliv;
    
    nwritten = send(rstate->errfd, &err, sizeof(mStatus), 0);
    if (nwritten < (int)sizeof(mStatus))
        {
        if (errno == EINTR || errno == EAGAIN)   
            nwritten = 0;
        if (nwritten < 0)
            {
            my_perror("ERROR: send - unable to deliver error to client");
            goto error;
            }
        //client blocked - store result and come backr
        undeliv = mallocL("deliver_error", sizeof(undelivered_error_t));
        if (!undeliv)
            {
            my_perror("ERROR: malloc");
            exit(1);
            }
        undeliv->err = err;
        undeliv->nwritten = nwritten;
        undeliv->sd = rstate->errfd;
        rstate->u_err = undeliv;
        return 0;
    }
    if (rstate->errfd != rstate->sd) close(rstate->errfd);
    return 0;
    
error:
    if (rstate->errfd != rstate->sd) close(rstate->errfd);
    return -1;
    
    }
           

// returns 0 on success, -1 if send is incomplete, or on terminal failre (request is aborted)
static transfer_state send_undelivered_error(request_state *rs)
    {
    int nwritten;
    
    nwritten = send(rs->u_err->sd, (char *)(&rs->u_err) + rs->u_err->nwritten, sizeof(mStatus) - rs->u_err->nwritten, 0);
    if (nwritten < 0)
        {
        if (errno == EINTR || errno == EAGAIN)
            nwritten = 0;
        else
            {
            my_perror("ERROR: send - unable to deliver error to client\n");
            if (rs->u_err->sd == rs->sd) close (rs->u_err->sd);
            return t_error;
            }
        }
    if (nwritten + rs->u_err->nwritten == sizeof(mStatus))
        {
        if (rs->u_err->sd == rs->sd) close(rs->u_err->sd);
        freeL("send_undelivered_error", rs->u_err);
        rs->u_err = NULL;
        return t_complete;
        }
    rs->u_err->nwritten += nwritten;
    return t_morecoming;
    }


// send bogus data along with an error code to the app callback
// returns 0 on success (linking reply into list of not fully delivered),
// -1 on failure (request should be aborted)
static int deliver_async_error(request_state *rs, reply_op_t op, mStatus err)
    {
    int len;
    reply_state *reply;
    transfer_state ts;
    
    if (rs->no_reply) return 0;
    len = 256;		// long enough for any reply handler to read all args w/o buffer overrun
    reply = create_reply(op, len, rs);
    reply->rhdr->error = err;
    ts = send_msg(reply);
    if (ts == t_error || ts == t_terminated)
        {
        freeL("deliver_async_error", reply);
        return -1;
        }
    else if (ts == t_complete) freeL("deliver_async_error", reply);
    else if (ts == t_morecoming) append_reply(rs, reply);   // client is blocked, link reply into list
    return 0;
    }


static void abort_request(request_state *rs)
    {
    reply_state *rep, *ptr;

    if (rs->terminate) rs->terminate(rs->termination_context);  // terminate field may not be set yet
    if (rs->msgbuf) freeL("abort_request", rs->msgbuf);
    udsSupportRemoveFDFromEventLoop(rs->sd);
    rs->sd = -1;
    if (rs->errfd >= 0) close(rs->errfd);
    rs->errfd = -1;

    // free pending replies
    rep = rs->replies;
    while(rep)
    	{
        if (rep->msgbuf) freeL("abort_request", rep->msgbuf);
        ptr = rep;
        rep = rep->next;
        freeL("abort_request", ptr);
    	}
    
    if (rs->u_err)
        {
        freeL("abort_request", rs->u_err);
        rs->u_err = NULL;
        }
    }


static void unlink_request(request_state *rs)
    {
    request_state *ptr;
    
    if (rs == all_requests)
        {
        all_requests = all_requests->next;
        freeL("unlink_request", rs);
        return;
        }
    for(ptr = all_requests; ptr->next; ptr = ptr->next)
        if (ptr->next == rs)
            {
            ptr->next = rs->next;
            freeL("unlink_request", rs);
            return;
        }
    }
    


//hack to search-replace perror's to LogMsg's
static void my_perror(char *errmsg)
    {
    LogMsg("%s: %s", errmsg, strerror(errno));
    }

// check that the message delivered by the client is sufficiently long to extract the required data from the buffer
// without overrunning it.
// returns 0 on success, -1 on error.

static int validate_message(request_state *rstate)
    {
    uint32_t min_size;
    
    switch(rstate->hdr.op.request_op)
    	{
        case resolve_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						(3 * sizeof(char));           	// name, regtype, domain
						break;  
        case query_request: min_size = 		sizeof(DNSServiceFlags) + 	// flags
						sizeof(uint32_t) +		// interface
						sizeof(char) + 			// fullname
						(2 * sizeof(uint16_t)); 	// type, class
						break;
        case browse_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) +		// interface
						(2 * sizeof(char)); 		// regtype, domain
						break;
        case reg_service_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) +		// interface
						(4 * sizeof(char)) + 		// name, type, domain, host
						(2 * sizeof(uint16_t));		// port, textlen	
						break;	
        case enumeration_request: min_size =	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t); 		// interface
						break;
        case reg_record_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						sizeof(char) + 			// fullname
						(3 * sizeof(uint16_t)) +	// type, class, rdlen
						sizeof(uint32_t);		// ttl
						break;
        case add_record_request: min_size = 	sizeof(DNSServiceFlags) +	// flags
						(2 * sizeof(uint16_t)) + 	// type, rdlen
						sizeof(uint32_t);		// ttl
						break;
        case update_record_request: min_size =	sizeof(DNSServiceFlags) +	// flags
						sizeof(uint16_t) +		// rdlen
						sizeof(uint32_t); 		// ttl
						break;
        case remove_record_request: min_size =	sizeof(DNSServiceFlags);	// flags
						break;
        case reconfirm_record_request: min_size=sizeof(DNSServiceFlags) +	// flags
						sizeof(uint32_t) + 		// interface
						sizeof(char) + 			// fullname
						(3 * sizeof(uint16_t));		// type, class, rdlen
        default:
            LogMsg("ERROR: validate_message - unsupported request type: %d", rstate->hdr.op.request_op);	    
	    return -1;
	}    
    
	return (rstate->data_bytes >= min_size ? 0 : -1);
    
    }


