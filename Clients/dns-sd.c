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
 *
 * Formatting notes:
 * This code follows the "Whitesmiths style" C indentation rules. Plenty of discussion
 * on C indentation can be found on the web, such as <http://www.kafejo.com/komp/1tbs.htm>,
 * but for the sake of brevity here I will say just this: Curly braces are not syntactially
 * part of an "if" statement; they are the beginning and ending markers of a compound statement;
 * therefore common sense dictates that if they are part of a compound statement then they
 * should be indented to the same level as everything else in that compound statement.
 * Indenting curly braces at the same level as the "if" implies that curly braces are
 * part of the "if", which is false. (This is as misleading as people who write "char* x,y;"
 * thinking that variables x and y are both of type "char*" -- and anyone who doesn't
 * understand why variable y is not of type "char*" just proves the point that poor code
 * layout leads people to unfortunate misunderstandings about how the C language really works.)

   Change History (most recent first):

$Log: dns-sd.c,v $
Revision 1.7  2004/05/28 02:20:06  cheshire
If we allow dot or empty string for domain when resolving a service,
it should be a synonym for "local"

Revision 1.6  2004/05/21 19:57:19  cheshire
Add -Q option to exercise DNSServiceQueryRecord() API call

Revision 1.5  2004/05/21 17:39:27  cheshire
Include extra headers to fix Xcode build

Revision 1.4  2004/05/21 17:25:56  cheshire
Fixes to make sample client work on Linux

Revision 1.3  2004/04/06 22:02:06  cheshire
Also show interface id when showing browse add/remove events

Revision 1.2  2004/03/25 05:40:56  cheshire
Changes from Marc Krochmal: Fix inconsistent use of space/tab

Revision 1.1  2004/02/06 03:19:09  cheshire
Check in code to make command-line "dns-sd" testing tool

*/

#include <stdio.h>			// For stdout, stderr
#include <stdlib.h>			// For exit()
#include <strings.h>		// For strlen(), strcpy(), bzero()
#include <unistd.h>         // For getopt() and optind
#include <errno.h>          // For errno, EINTR
#define BIND_8_COMPAT
#include <arpa/nameser.h>	// For T_HINFO, etc.
#include <sys/time.h>		// For struct timeval
#include <dns_sd.h>

//*************************************************************************************************************
// Globals

typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;

static int operation;
static DNSServiceRef client = NULL;
static int num_printed;
static char addtest = 0;
static DNSRecordRef record = NULL;
static char myhinfo9[11] = "\003Mac\006OS 9.2";
static char myhinfoX[ 9] = "\003Mac\004OS X";
static char updatetest[3] = "\002AA";
static char bigNULL[4096];

#define LONG_TIME 0x4000000
static volatile int stopNow = 0;
static volatile int timeOut = LONG_TIME;

//*************************************************************************************************************
// Supporting Utility Function

//*************************************************************************************************************
// Sample callback functions for each of the operation types

static void printtimestamp(void)
	{
	struct timeval tv;
	struct tm tm;
	gettimeofday(&tv, NULL);
	localtime_r((time_t*)&tv.tv_sec, &tm);
	printf("%2d:%02d:%02d.%03d  ", tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
	}

#define DomainMsg(X) (((X) & kDNSServiceFlagsDefault) ? "(Default)" : \
                      ((X) & kDNSServiceFlagsAdd)     ? "Added"     : "Removed")

static void regdom_reply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interface,
	DNSServiceErrorType errorCode, const char *replyDomain, void *context)
	{
	(void)interface;    // Unused
	(void)errorCode;    // Unused
	(void)context;      // Unused
	printtimestamp();
	printf("Recommended Registration Domain %s %s", replyDomain, DomainMsg(flags));
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

static void browsedom_reply(DNSServiceRef client, DNSServiceFlags flags, uint32_t interface,
	DNSServiceErrorType errorCode, const char *replyDomain, void *context)
	{
	(void)client;       // Unused
	(void)interface;    // Unused
	(void)errorCode;    // Unused
	(void)context;      // Unused
	printtimestamp();
	printf("Recommended Browsing Domain %s %s", replyDomain, DomainMsg(flags));
	if (flags) printf(" Flags: %X", flags);
	printf("\n");
	}

static void browse_reply(DNSServiceRef client, DNSServiceFlags flags, uint32_t interface, DNSServiceErrorType errorCode,
	const char *replyName, const char *replyType, const char *replyDomain, void *context)
	{
	char *op = (flags & kDNSServiceFlagsAdd) ? "Add" : "Rmv";
	(void)client;       // Unused
	(void)interface;    // Unused
	(void)errorCode;    // Unused
	(void)context;      // Unused
	if (num_printed++ == 0) printf("Timestamp     A/R Flags if %-24s %-24s %s\n", "Domain", "Service Type", "Instance Name");
	printtimestamp();
	printf("%s%6X%3d %-24s %-24s %s\n", op, flags, interface, replyDomain, replyType, replyName);
	}

static void resolve_reply(DNSServiceRef client, DNSServiceFlags flags, uint32_t interface, DNSServiceErrorType errorCode,
	const char *fullname, const char *hosttarget, uint16_t opaqueport, uint16_t txtLen, const char *txtRecord, void *context)
	{
	const char *src = txtRecord;
	union { uint16_t s; u_char b[2]; } port = { opaqueport };
	uint16_t PortAsNumber = ((uint16_t)port.b[0]) << 8 | port.b[1];

	(void)client;       // Unused
	(void)interface;    // Unused
	(void)errorCode;    // Unused
	(void)context;      // Unused

	printtimestamp();
	printf("%s can be reached at %s:%u", fullname, hosttarget, PortAsNumber);

	if (flags) printf(" Flags: %X", flags);
	if (*src)
		{
		char txtInfo[64];                               // Display at most first 64 characters of TXT record
		char *dst = txtInfo;
		const char *const lim = &txtInfo[sizeof(txtInfo)];
		while (*src && dst < lim-1)
			{
			if (*src == '\\') *dst++ = '\\';            // '\' displays as "\\"
			if (*src >= ' ') *dst++ = *src++;           // Display normal characters as-is
			else
				{
				*dst++ = '\\';                          // Display a backslash
				if (*src ==    1) *dst++ = ' ';         // String boundary displayed as "\ "
				else                                    // Other chararacters displayed as "\0xHH"
					{
					static const char hexchars[16] = "0123456789ABCDEF";
					*dst++ = '0';
					*dst++ = 'x';
					*dst++ = hexchars[*src >> 4];
					*dst++ = hexchars[*src & 0xF];
					}
				src++;
				}
			}
		*dst++ = 0;
		printf(" TXT %s", txtInfo);
		}
	printf("\n");
	}

static void myTimerCallBack(void)
	{
	DNSServiceErrorType err;

	switch (operation)
		{
		case 'A':
			{
			switch (addtest)
				{
				case 0: printf("Adding Test HINFO record\n");
						err = DNSServiceAddRecord(client, &record, 0, T_HINFO, sizeof(myhinfo9), &myhinfo9[0], 120);
						addtest = 1;
						break;
				case 1: printf("Updating Test HINFO record\n");
						err = DNSServiceUpdateRecord(client, record, 0, sizeof(myhinfoX), &myhinfoX[0], 120);
						addtest = 2;
						break;
				case 2: printf("Removing Test HINFO record\n");
						err = DNSServiceRemoveRecord(client, record, 0);
						addtest = 0;
						break;
				}
			}
			break;

		case 'U':
			{
			if (updatetest[1] != 'Z') updatetest[1]++;
			else                      updatetest[1] = 'A';
			updatetest[0] = 3 - updatetest[0];
			updatetest[2] = updatetest[1];
			printf("Updating Test TXT record to %c\n", updatetest[1]);
			err = DNSServiceUpdateRecord(client, NULL, 0, 1+updatetest[0], &updatetest[0], 120);
			}
			break;

		case 'N':
			{
			printf("Adding big NULL record\n");
			err = DNSServiceAddRecord(client, &record, 0, T_NULL, sizeof(bigNULL), &bigNULL[0], 120);
			timeOut = LONG_TIME;
			}
			break;
		}

	if (err != kDNSServiceErr_NoError)
		{
		fprintf(stderr, "DNSService call failed %ld\n", (long int)err);
		stopNow = 1;
		}
	}

static void reg_reply(DNSServiceRef client, DNSServiceFlags flags, DNSServiceErrorType errorCode,
	const char *name, const char *regtype, const char *domain, void *context)
	{
	(void)client;   // Unused
	(void)flags;    // Unused
	(void)context;  // Unused

	printf("Got a reply for %s.%s%s: ", name, regtype, domain);
	switch (errorCode)
		{
		case kDNSServiceErr_NoError:      printf("Name now registered and active\n"); break;
		case kDNSServiceErr_NameConflict: printf("Name in use, please choose another\n"); exit(-1);
		default:                          printf("Error %d\n", errorCode); return;
		}

	if (operation == 'A' || operation == 'U' || operation == 'N') timeOut = 5;
	}

void qr_reply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
	const char *fullname, uint16_t rrtype, uint16_t rrclass, uint16_t rdlen, const void *rdata, uint32_t ttl, void *context)
	{
	const unsigned char *rd = rdata;
	char rdb[1000];
	switch (rrtype)
		{
		case T_A: snprintf(rdb, sizeof(rdb), "%d.%d.%d.%d", rd[0], rd[1], rd[2], rd[3]); break;
		default : snprintf(rdb, sizeof(rdb), "Unknown rdata: %d bytes", rdlen);          break;
		}
	printf("%-30s%4d%4d  %s\n", fullname, rrtype, rrclass, rdb);
	}

//*************************************************************************************************************
// The main test function

static void HandleEvents(void)
	{
	int dns_sd_fd = DNSServiceRefSockFD(client);
	int nfds = dns_sd_fd + 1;
	fd_set readfds;
	struct timeval tv;
	int result;

	while (!stopNow)
		{
		// 1. Set up the fd_set as usual here.
		// This example client has no file descriptors of its own,
		// but a real application would call FD_SET to add them to the set here
		FD_ZERO(&readfds);

		// 2. Add the fd for our client(s) to the fd_set
		FD_SET(dns_sd_fd, &readfds);

		// 3. Set up the timeout.
		tv.tv_sec = timeOut;
		tv.tv_usec = 0;

		result = select(nfds, &readfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
		if (result > 0)
			{
			if (FD_ISSET(dns_sd_fd, &readfds)) DNSServiceProcessResult(client);
			}
		else if (result == 0)
			myTimerCallBack();
		else
			{
			printf("select() returned %d errno %d %s\n", result, errno, strerror(errno));
			if (errno != EINTR) stopNow = 1;
			}
		}
	}

int main(int argc, char **argv)
	{
	DNSServiceErrorType err;
	char *dom;
	setlinebuf(stdout);             // Want to see lines as they appear, not block buffered

	if (argc < 2) goto Fail;        // Minimum command line is the command name and one argument
	operation = getopt(argc, (char * const *)argv, "EFBLQRAUNTMI");
	if (operation == -1) goto Fail;

	switch (operation)
		{
		case 'E':	printf("Looking for recommended registration domains:\n");
					err = DNSServiceEnumerateDomains(&client, kDNSServiceFlagsRegistrationDomains, 0, regdom_reply, NULL);
					break;

		case 'F':	printf("Looking for recommended browsing domains:\n");
					err = DNSServiceEnumerateDomains(&client, kDNSServiceFlagsBrowseDomains, 0, browsedom_reply, NULL);
					break;

		case 'B':	if (argc < optind+1) goto Fail;
					dom = (argc < optind+2) ? "" : argv[optind+1];
					if (dom[0] == '.' && dom[1] == 0) dom[0] = 0;   // We allow '.' on the command line as a synonym for empty string
					printf("Browsing for %s%s\n", argv[optind+0], dom);
					err = DNSServiceBrowse(&client, 0, 0, argv[optind+0], dom, browse_reply, NULL);
					break;

		case 'L':	if (argc < optind+2) goto Fail;
					dom = (argc < optind+3) ? "local" : argv[optind+2];
					if (dom[0] == '.' && dom[1] == 0) dom = "local";   // We allow '.' on the command line as a synonym for "local"
					printf("Lookup %s.%s.%s\n", argv[optind+0], argv[optind+1], dom);
					err = DNSServiceResolve(&client, 0, 0, argv[optind+0], argv[optind+1], dom, resolve_reply, NULL);
					break;

		case 'R':	if (argc < optind+4) goto Fail;
					{
					char *nam = argv[optind+0];
					char *typ = argv[optind+1];
					char *dom = argv[optind+2];
					uint16_t PortAsNumber = atoi(argv[optind+3]);
					Opaque16 registerPort = { { PortAsNumber >> 8, PortAsNumber & 0xFF } };
					char txt[2048];
					char *ptr = txt;
					int i;

					if (nam[0] == '.' && nam[1] == 0) nam[0] = 0;   // We allow '.' on the command line as a synonym for empty string
					if (dom[0] == '.' && dom[1] == 0) dom[0] = 0;   // We allow '.' on the command line as a synonym for empty string

					for (i = optind+4; i < argc; i++)
						{
						char *len = ptr++;
						*len = strlen(argv[i]);
						strcpy(ptr, argv[i]);
						ptr += *len;
						}

					printf("Registering Service %s.%s%s port %s %s\n", nam, typ, dom, argv[optind+3], txt);
					err = DNSServiceRegister(&client, 0, 0, nam, typ, dom, NULL, registerPort.NotAnInteger, ptr-txt, txt, reg_reply, NULL);
					break;
					}

		case 'Q':	{
					if (argc < optind+1) goto Fail;
					uint16_t rrtype  = (argc <= optind+1) ? T_A  : atoi(argv[optind+1]);
					uint16_t rrclass = (argc <= optind+2) ? C_IN : atoi(argv[optind+2]);
					err = DNSServiceQueryRecord(&client, 0, 0, argv[optind+0], rrtype, rrclass, qr_reply, NULL);
					break;
					}

		case 'A':
		case 'U':
		case 'N':	{
					Opaque16 registerPort = { { 0x12, 0x34 } };
					static const char TXT[] = "\xC" "First String" "\xD" "Second String" "\xC" "Third String";
					printf("Registering Service Test._testupdate._tcp.local.\n");
					err = DNSServiceRegister(&client, 0, 0, "Test", "_testupdate._tcp.", "", NULL, registerPort.NotAnInteger, sizeof(TXT)-1, TXT, reg_reply, NULL);
					break;
					}

		case 'T':	{
					Opaque16 registerPort = { { 0x23, 0x45 } };
					char TXT[1024];
					unsigned int i;
					for (i=0; i<sizeof(TXT); i++)
						if ((i & 0x1F) == 0) TXT[i] = 0x1F; else TXT[i] = 'A' + (i >> 5);
					printf("Registering Service Test._testlargetxt._tcp.local.\n");
					err = DNSServiceRegister(&client, 0, 0, "Test", "_testlargetxt._tcp.", "", NULL, registerPort.NotAnInteger, sizeof(TXT), TXT, reg_reply, NULL);
					break;
					}

		case 'M':	{
					pid_t pid = getpid();
					Opaque16 registerPort = { { pid >> 8, pid & 0xFF } };
					static const char TXT1[] = "\xC" "First String"  "\xD" "Second String" "\xC" "Third String";
					static const char TXT2[] = "\xD" "Fourth String" "\xC" "Fifth String"  "\xC" "Sixth String";
					printf("Registering Service Test._testdualtxt._tcp.local.\n");
					err = DNSServiceRegister(&client, 0, 0, "Test", "_testdualtxt._tcp.", "", NULL, registerPort.NotAnInteger, sizeof(TXT1)-1, TXT1, reg_reply, NULL);
					if (!err) err = DNSServiceAddRecord(client, &record, 0, T_TXT, sizeof(TXT2)-1, TXT2, 120);
					break;
					}

		case 'I':	{
					pid_t pid = getpid();
					Opaque16 registerPort = { { pid >> 8, pid & 0xFF } };
					static const char TXT[] = "\x09" "Test Data";
					printf("Registering Service Test._testtxt._tcp.local.\n");
					err = DNSServiceRegister(&client, 0, 0, "Test", "_testtxt._tcp.", "", NULL, registerPort.NotAnInteger, 0, NULL, reg_reply, NULL);
					if (!err) err = DNSServiceUpdateRecord(client, NULL, 0, sizeof(TXT)-1, TXT, 120);
					break;
					}

		default: goto Fail;
		}

	if (!client || err != kDNSServiceErr_NoError) { fprintf(stderr, "DNSService call failed %ld\n", (long int)err); return (-1); }
	HandleEvents();

	// Be sure to deallocate the DNSServiceRef when you're finished
	DNSServiceRefDeallocate(client);
	return 0;

Fail:
	fprintf(stderr, "%s -E                  (Enumerate recommended registration domains)\n", argv[0]);
	fprintf(stderr, "%s -F                      (Enumerate recommended browsing domains)\n", argv[0]);
	fprintf(stderr, "%s -B        <Type> <Domain>        (Browse for services instances)\n", argv[0]);
	fprintf(stderr, "%s -L <Name> <Type> <Domain>           (Look up a service instance)\n", argv[0]);
	fprintf(stderr, "%s -R <Name> <Type> <Domain> <Port> [<TXT>...] (Register a service)\n", argv[0]);
	fprintf(stderr, "%s -Q <FQDN> <rrtype> <rrclass> (Generic query for any record type)\n", argv[0]);
	fprintf(stderr, "%s -A                      (Test Adding/Updating/Deleting a record)\n", argv[0]);
	fprintf(stderr, "%s -U                                  (Test updating a TXT record)\n", argv[0]);
	fprintf(stderr, "%s -N                             (Test adding a large NULL record)\n", argv[0]);
	fprintf(stderr, "%s -T                            (Test creating a large TXT record)\n", argv[0]);
	fprintf(stderr, "%s -M      (Test creating a registration with multiple TXT records)\n", argv[0]);
	fprintf(stderr, "%s -I   (Test registering and then immediately updating TXT record)\n", argv[0]);
	return 0;
	}
