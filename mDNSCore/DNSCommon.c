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

$Log: DNSCommon.c,v $
Revision 1.35  2004/06/05 00:14:44  cheshire
Fix signed/unsigned and other compiler warnings

Revision 1.34  2004/06/04 00:25:25  cheshire
Fix misaligned write exception that occurs on some platforms

Revision 1.33  2004/06/04 00:16:18  cheshire
Remove non-portable use of 'inline'

Revision 1.32  2004/06/03 03:09:58  ksekar
<rdar://problem/3668626>: Garbage Collection for Dynamic Updates

Revision 1.31  2004/05/28 23:42:36  ksekar
<rdar://problem/3258021>: Feature: DNS server->client notification on record changes (#7805)

Revision 1.30  2004/05/26 09:08:04  bradley
Added cast to correct structure pointer when allocating domain name list element to fix C++ builds.

Revision 1.29  2004/05/18 23:51:25  cheshire
Tidy up all checkin comments to use consistent "<rdar://problem/xxxxxxx>" format for bug numbers

Revision 1.28  2004/05/13 04:54:20  ksekar
Unified list copy/free code.  Added symetric list for

Revision 1.27  2004/04/22 20:29:07  cheshire
Log error message if no count field passed to PutResourceRecordTTL()

Revision 1.26  2004/04/22 04:07:01  cheshire
Fix from Bob Bradley: Don't try to do inline functions on compilers that don't support it

Revision 1.25  2004/04/22 03:05:28  cheshire
kDNSClass_ANY should be kDNSQClass_ANY

Revision 1.24  2004/04/22 02:51:20  cheshire
Use common code for HINFO/TXT and TSIG cases in putRData

Revision 1.23  2004/04/15 00:51:28  bradley
Minor tweaks for Windows and C++ builds. Added casts for signed/unsigned integers and 64-bit pointers.
Prefix some functions with mDNS to avoid conflicts. Disable benign warnings on Microsoft compilers.

Revision 1.22  2004/04/14 23:09:28  ksekar
Support for TSIG signed dynamic updates.

Revision 1.21  2004/04/09 16:47:28  cheshire
<rdar://problem/3617655>: mDNSResponder escape handling inconsistent with BIND

Revision 1.20  2004/04/09 16:37:15  cheshire
Suggestion from Bob Bradley:
Move NumCacheRecordsForInterfaceID() to DNSCommon.c so it's available to all platform layers

Revision 1.19  2004/04/02 19:34:38  cheshire
Fix broken comment

Revision 1.18  2004/03/30 06:45:00  cheshire
Compiler warning fixes from Don Woodward at Roku Labs

Revision 1.17  2004/03/19 22:25:20  cheshire
<rdar://problem/3579561>: Need to limit service types to fourteen characters
Won't actually do this for now, but keep the code around just in case

Revision 1.16  2004/03/08 02:45:35  cheshire
Minor change to make a couple of the log messages a bit shorter

Revision 1.15  2004/03/08 02:44:09  cheshire
<rdar://problem/3579561>: Need to limit service types to fourteen characters

Revision 1.14  2004/02/21 02:06:24  cheshire
Can't use anonymous unions -- they're non-standard and don't work on all compilers

Revision 1.13  2004/02/06 23:04:18  ksekar
Basic Dynamic Update support via mDNS_Register (dissabled via
UNICAST_REGISTRATION #define)

Revision 1.12  2004/02/03 22:37:10  cheshire
Delete unused (commented-out) code

Revision 1.11  2004/02/03 22:35:34  cheshire
<rdar://problem/3548256>: Should not allow empty string for resolve domain

Revision 1.10  2004/02/03 19:47:36  ksekar
Added an asyncronous state machine mechanism to uDNS.c, including
calls to find the parent zone for a domain name.  Changes include code
in repository previously dissabled via "#if 0 //incomplete".  Codepath
is currently unused, and will be called to create update records, etc.

Revision 1.9  2004/01/27 20:15:22  cheshire
<rdar://problem/3541288>: Time to prune obsolete code for listening on port 53

Revision 1.8  2004/01/24 23:24:36  cheshire
Expanded out the list of local domains to reduce risk of mistakes in future

Revision 1.7  2004/01/24 08:32:30  bradley
Mask values with 0xFF before casting to avoid runtime truncation errors on Windows debug builds.
Separated octal-escaped sequences preceding decimal digits to avoid errors with some compilers wanting
to signal potentially hidden errors about the subsequent digit not being part of the octal sequence.

Revision 1.6  2004/01/24 04:59:15  cheshire
Fixes so that Posix/Linux, OS9, Windows, and VxWorks targets build again

Revision 1.5  2004/01/23 23:23:14  ksekar
Added TCP support for truncated unicast messages.

Revision 1.4  2004/01/22 02:15:33  cheshire
<rdar://problem/3536597>: Link-local reverse-mapping domains need to be resolved using link-local multicast

Revision 1.3  2004/01/21 21:16:29  cheshire
Minor tidy-up: Deleted a bunch of blank lines, trailing spaces, tabs, etc.

Revision 1.2  2003/12/13 05:47:48  bradley
Made local ptr const to fix error when assigning from const structure. Disable benign conditional
expression is constant warning when building with Microsoft compilers.

Revision 1.1  2003/12/13 03:05:27  ksekar
<rdar://problem/3192548>: DynDNS: Unicast query of service records

 */

// Set mDNS_InstantiateInlines to tell mDNSClientAPI.h to instantiate inline functions, if necessary
#define mDNS_InstantiateInlines 1
#include "DNSCommon.h"

// Disable certain benign warnings with Microsoft compilers
#if (defined(_MSC_VER))
	// Disable "conditional expression is constant" warning for debug macros.
	// Otherwise, this generates warnings for the perfectly natural construct "while(1)"
	// If someone knows a variant way of writing "while(1)" that doesn't generate warning messages, please let us know
	#pragma warning(disable:4127)
#endif

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNameList copy/deallocation routines
#endif

mDNSexport DNameListElem *mDNS_CopyDNameList(const DNameListElem *orig)
	{
	DNameListElem *copy = mDNSNULL, *newelem;
	const DNameListElem *ptr;

	for (ptr = orig; ptr; ptr = ptr->next)
		{
		newelem = (DNameListElem*)mDNSPlatformMemAllocate(sizeof(DNameListElem));
		if (!newelem) { LogMsg("ERROR: malloc"); return mDNSNULL; }
		mDNSPlatformStrCopy(ptr->name.c, newelem->name.c);
		newelem->next = copy;
		copy = newelem;
		}
	return copy;
	}

mDNSexport void mDNS_FreeDNameList(DNameListElem *list)
	{
	DNameListElem *fptr;

	while (list)
		{
		fptr = list;
		list = list->next;
		mDNSPlatformMemFree(fptr);
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - General Utility Functions
#endif

mDNSexport const NetworkInterfaceInfo *GetFirstActiveInterface(const NetworkInterfaceInfo *intf)
	{
	while (intf && !intf->InterfaceActive) intf = intf->next;
	return(intf);
	}

mDNSexport mDNSInterfaceID GetNextActiveInterfaceID(const NetworkInterfaceInfo *intf)
	{
	const NetworkInterfaceInfo *next = GetFirstActiveInterface(intf->next);
	if (next) return(next->InterfaceID); else return(mDNSNULL);
	}

mDNSexport mDNSu32 NumCacheRecordsForInterfaceID(const mDNS *const m, mDNSInterfaceID id)
	{
	mDNSu32 slot, used = 0;
	CacheRecord *rr;
	for (slot = 0; slot < CACHE_HASH_SLOTS; slot++)
		for (rr = m->rrcache_hash[slot]; rr; rr=rr->next)
			if (rr->resrec.InterfaceID == id) used++;
	return(used);
	}

mDNSexport char *DNSTypeName(mDNSu16 rrtype)
	{
	switch (rrtype)
		{
		case kDNSType_A:    return("Addr");
		case kDNSType_CNAME:return("CNAME");
		case kDNSType_NULL: return("NULL");
		case kDNSType_PTR:  return("PTR");
		case kDNSType_HINFO:return("HINFO");
		case kDNSType_TXT:  return("TXT");
		case kDNSType_AAAA: return("AAAA");
		case kDNSType_SRV:  return("SRV");
		case kDNSQType_ANY: return("ANY");
		default:			{
							static char buffer[16];
							mDNS_snprintf(buffer, sizeof(buffer), "(%d)", rrtype);
							return(buffer);
							}
		}
	}

mDNSexport char *GetRRDisplayString_rdb(mDNS *const m, const ResourceRecord *rr, RDataBody *rd)
	{
	char *ptr = m->MsgBuffer;
	mDNSu32 length = mDNS_snprintf(m->MsgBuffer, 79, "%4d %##s %s ", rr->rdlength, rr->name.c, DNSTypeName(rr->rrtype));
	switch (rr->rrtype)
		{
		case kDNSType_A:	mDNS_snprintf(m->MsgBuffer+length, 79-length, "%.4a", &rd->ip);         break;
		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	mDNS_snprintf(m->MsgBuffer+length, 79-length, "%##s", &rd->name);       break;
		case kDNSType_HINFO:// Display this the same as TXT (just show first string)
		case kDNSType_TXT:  mDNS_snprintf(m->MsgBuffer+length, 79-length, "%#s", rd->txt.c);        break;
		case kDNSType_AAAA:	mDNS_snprintf(m->MsgBuffer+length, 79-length, "%.16a", &rd->ipv6);      break;
		case kDNSType_SRV:	mDNS_snprintf(m->MsgBuffer+length, 79-length, "%##s", &rd->srv.target); break;
		default:			mDNS_snprintf(m->MsgBuffer+length, 79-length, "RDLen %d: %s",
								rr->rdlength, rd->data);  break;
		}
	for (ptr = m->MsgBuffer; *ptr; ptr++) if (*ptr < ' ') *ptr='.';
	return(m->MsgBuffer);
	}

mDNSexport mDNSu32 mDNSRandom(mDNSu32 max)
	{
	static mDNSu32 seed = 0;
	mDNSu32 mask = 1;

	if (!seed) seed = (mDNSu32)mDNSPlatformTimeNow();
	while (mask < max) mask = (mask << 1) | 1;
	do seed = seed * 21 + 1; while ((seed & mask) > max);
	return (seed & mask);
	}

mDNSexport mDNSBool mDNSSameAddress(const mDNSAddr *ip1, const mDNSAddr *ip2)
	{
	if (ip1->type == ip2->type)
		{
		switch (ip1->type)
			{
			case mDNSAddrType_IPv4 : return(mDNSBool)(mDNSSameIPv4Address(ip1->ip.v4, ip2->ip.v4));
			case mDNSAddrType_IPv6 : return(mDNSBool)(mDNSSameIPv6Address(ip1->ip.v6, ip2->ip.v6));
			}
		}
	return(mDNSfalse);
	}

mDNSexport mDNSBool mDNSAddrIsDNSMulticast(const mDNSAddr *ip)
	{
	switch(ip->type)
		{
		case mDNSAddrType_IPv4: return(mDNSBool)(ip->ip.v4.NotAnInteger == AllDNSLinkGroup.NotAnInteger);
		case mDNSAddrType_IPv6: return(mDNSBool)(ip->ip.v6.l[0] == AllDNSLinkGroupv6.l[0] &&
												 ip->ip.v6.l[1] == AllDNSLinkGroupv6.l[1] &&
												 ip->ip.v6.l[2] == AllDNSLinkGroupv6.l[2] &&
												 ip->ip.v6.l[3] == AllDNSLinkGroupv6.l[3] );
		default: return(mDNSfalse);
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Domain Name Utility Functions
#endif

mDNSexport mDNSBool SameDomainLabel(const mDNSu8 *a, const mDNSu8 *b)
	{
	int i;
	const int len = *a++;

	if (len > MAX_DOMAIN_LABEL)
		{ debugf("Malformed label (too long)"); return(mDNSfalse); }

	if (len != *b++) return(mDNSfalse);
	for (i=0; i<len; i++)
		{
		mDNSu8 ac = *a++;
		mDNSu8 bc = *b++;
		if (mDNSIsUpperCase(ac)) ac += 'a' - 'A';
		if (mDNSIsUpperCase(bc)) bc += 'a' - 'A';
		if (ac != bc) return(mDNSfalse);
		}
	return(mDNStrue);
	}

mDNSexport mDNSBool SameDomainName(const domainname *const d1, const domainname *const d2)
	{
	const mDNSu8 *      a   = d1->c;
	const mDNSu8 *      b   = d2->c;
	const mDNSu8 *const max = d1->c + MAX_DOMAIN_NAME;			// Maximum that's valid

	while (*a || *b)
		{
		if (a + 1 + *a >= max)
			{ debugf("Malformed domain name (more than 255 characters)"); return(mDNSfalse); }
		if (!SameDomainLabel(a, b)) return(mDNSfalse);
		a += 1 + *a;
		b += 1 + *b;
		}

	return(mDNStrue);
	}

mDNSexport mDNSBool IsLocalDomain(const domainname *d)
	{
	// Domains that are defined to be resolved via link-local multicast are:
	// local., 254.169.in-addr.arpa., and 0.8.E.F.ip6.arpa.
	static const domainname *n0 = (domainname*)"\x5" "local";
	static const domainname *n1 = (domainname*)"\x3" "254" "\x3" "169"                     "\x7" "in-addr" "\x4" "arpa";
	static const domainname *n2 = (domainname*)"\x1" "0"   "\x1" "8"   "\x1" "e" "\x1" "f" "\x3" "ip6"     "\x4" "arpa";

	const domainname *d1, *d2, *d3, *d4, *d5, *d6;	// Top-level domain, second-level domain, etc.
	d1 = d2 = d3 = d4 = d5 = d6 = mDNSNULL;
	while (d->c[0])
		{
		d6 = d5; d5 = d4; d4 = d3; d3 = d2; d2 = d1; d1 = d;
		d = (domainname*)(d->c + 1 + d->c[0]);
		}

	if (d1 && SameDomainName(d1, n0)) return(mDNStrue);
	if (d4 && SameDomainName(d4, n1)) return(mDNStrue);
	if (d6 && SameDomainName(d6, n2)) return(mDNStrue);
	return(mDNSfalse);
	}

// Returns length of a domain name INCLUDING the byte for the final null label
// i.e. for the root label "." it returns one
// For the FQDN "com." it returns 5 (length byte, three data bytes, final zero)
// Legal results are 1 (just root label) to 255 (MAX_DOMAIN_NAME)
// If the given domainname is invalid, result is 256
mDNSexport mDNSu16 DomainNameLength(const domainname *const name)
	{
	const mDNSu8 *src = name->c;
	while (*src)
		{
		if (*src > MAX_DOMAIN_LABEL) return(MAX_DOMAIN_NAME+1);
		src += 1 + *src;
		if (src - name->c >= MAX_DOMAIN_NAME) return(MAX_DOMAIN_NAME+1);
		}
	return((mDNSu16)(src - name->c + 1));
	}

// CompressedDomainNameLength returns the length of a domain name INCLUDING the byte
// for the final null label i.e. for the root label "." it returns one.
// E.g. for the FQDN "foo.com." it returns 9
// (length, three data bytes, length, three more data bytes, final zero).
// In the case where a parent domain name is provided, and the given name is a child
// of that parent, CompressedDomainNameLength returns the length of the prefix portion
// of the child name, plus TWO bytes for the compression pointer.
// E.g. for the name "foo.com." with parent "com.", it returns 6
// (length, three data bytes, two-byte compression pointer).
mDNSexport mDNSu16 CompressedDomainNameLength(const domainname *const name, const domainname *parent)
	{
	const mDNSu8 *src = name->c;
	if (parent && parent->c[0] == 0) parent = mDNSNULL;
	while (*src)
		{
		if (*src > MAX_DOMAIN_LABEL) return(MAX_DOMAIN_NAME+1);
		if (parent && SameDomainName((domainname *)src, parent)) return((mDNSu16)(src - name->c + 2));
		src += 1 + *src;
		if (src - name->c >= MAX_DOMAIN_NAME) return(MAX_DOMAIN_NAME+1);
		}
	return((mDNSu16)(src - name->c + 1));
	}

// AppendLiteralLabelString appends a single label to an existing (possibly empty) domainname.
// The C string contains the label as-is, with no escaping, etc.
// Any dots in the name are literal dots, not label separators
// If successful, AppendLiteralLabelString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendLiteralLabelString returns mDNSNULL.
mDNSexport mDNSu8 *AppendLiteralLabelString(domainname *const name, const char *cstr)
	{
	mDNSu8       *      ptr  = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim1 = name->c + MAX_DOMAIN_NAME - 1;			// Limit of how much we can add (not counting final zero)
	const mDNSu8 *const lim2 = ptr + 1 + MAX_DOMAIN_LABEL;
	const mDNSu8 *const lim  = (lim1 < lim2) ? lim1 : lim2;
	mDNSu8       *lengthbyte = ptr++;									// Record where the length is going to go

	while (*cstr && ptr < lim) *ptr++ = (mDNSu8)*cstr++;	// Copy the data
	*lengthbyte = (mDNSu8)(ptr - lengthbyte - 1);			// Fill in the length byte
	*ptr++ = 0;												// Put the null root label on the end
	if (*cstr) return(mDNSNULL);							// Failure: We didn't successfully consume all input
	else return(ptr);										// Success: return new value of ptr
	}

// AppendDNSNameString appends zero or more labels to an existing (possibly empty) domainname.
// The C string is in conventional DNS syntax:
// Textual labels, escaped as necessary using the usual DNS '\' notation, separated by dots.
// If successful, AppendDNSNameString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendDNSNameString returns mDNSNULL.
mDNSexport mDNSu8 *AppendDNSNameString(domainname *const name, const char *cstr)
	{
	mDNSu8       *      ptr = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim = name->c + MAX_DOMAIN_NAME - 1;		// Limit of how much we can add (not counting final zero)
	while (*cstr && ptr < lim)										// While more characters, and space to put them...
		{
		mDNSu8 *lengthbyte = ptr++;									// Record where the length is going to go
		while (*cstr && *cstr != '.' && ptr < lim)					// While we have characters in the label...
			{
			mDNSu8 c = (mDNSu8)*cstr++;								// Read the character
			if (c == '\\')											// If escape character, check next character
				{
				c = (mDNSu8)*cstr++;								// Assume we'll just take the next character
				if (mdnsIsDigit(cstr[-1]) && mdnsIsDigit(cstr[0]) && mdnsIsDigit(cstr[1]))
					{												// If three decimal digits,
					int v0 = cstr[-1] - '0';						// then interpret as three-digit decimal
					int v1 = cstr[ 0] - '0';
					int v2 = cstr[ 1] - '0';
					int val = v0 * 100 + v1 * 10 + v2;
					if (val <= 255) { c = (mDNSu8)val; cstr += 2; }	// If valid three-digit decimal value, use it
					}
				}
			*ptr++ = c;												// Write the character
			}
		if (*cstr) cstr++;											// Skip over the trailing dot (if present)
		if (ptr - lengthbyte - 1 > MAX_DOMAIN_LABEL)				// If illegal label, abort
			return(mDNSNULL);
		*lengthbyte = (mDNSu8)(ptr - lengthbyte - 1);				// Fill in the length byte
		}

	*ptr++ = 0;														// Put the null root label on the end
	if (*cstr) return(mDNSNULL);									// Failure: We didn't successfully consume all input
	else return(ptr);												// Success: return new value of ptr
	}

// AppendDomainLabel appends a single label to a name.
// If successful, AppendDomainLabel returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// AppendDomainLabel returns mDNSNULL.
mDNSexport mDNSu8 *AppendDomainLabel(domainname *const name, const domainlabel *const label)
	{
	int i;
	mDNSu8 *ptr = name->c + DomainNameLength(name) - 1;

	// Check label is legal
	if (label->c[0] > MAX_DOMAIN_LABEL) return(mDNSNULL);

	// Check that ptr + length byte + data bytes + final zero does not exceed our limit
	if (ptr + 1 + label->c[0] + 1 > name->c + MAX_DOMAIN_NAME) return(mDNSNULL);

	for (i=0; i<=label->c[0]; i++) *ptr++ = label->c[i];	// Copy the label data
	*ptr++ = 0;								// Put the null root label on the end
	return(ptr);
	}

mDNSexport mDNSu8 *AppendDomainName(domainname *const name, const domainname *const append)
	{
	mDNSu8       *      ptr = name->c + DomainNameLength(name) - 1;	// Find end of current name
	const mDNSu8 *const lim = name->c + MAX_DOMAIN_NAME - 1;		// Limit of how much we can add (not counting final zero)
	const mDNSu8 *      src = append->c;
	while(src[0])
		{
		int i;
		if (ptr + 1 + src[0] > lim) return(mDNSNULL);
		for (i=0; i<=src[0]; i++) *ptr++ = src[i];
		*ptr = 0;	// Put the null root label on the end
		src += i;
		}
	return(ptr);
	}

// MakeDomainLabelFromLiteralString makes a single domain label from a single literal C string (with no escaping).
// If successful, MakeDomainLabelFromLiteralString returns mDNStrue.
// If unable to convert the whole string to a legal domain label (i.e. because length is more than 63 bytes) then
// MakeDomainLabelFromLiteralString makes a legal domain label from the first 63 bytes of the string and returns mDNSfalse.
// In some cases silently truncated oversized names to 63 bytes is acceptable, so the return result may be ignored.
// In other cases silent truncation may not be acceptable, so in those cases the calling function needs to check the return result.
mDNSexport mDNSBool MakeDomainLabelFromLiteralString(domainlabel *const label, const char *cstr)
	{
	mDNSu8       *      ptr   = label->c + 1;						// Where we're putting it
	const mDNSu8 *const limit = label->c + 1 + MAX_DOMAIN_LABEL;	// The maximum we can put
	while (*cstr && ptr < limit) *ptr++ = (mDNSu8)*cstr++;			// Copy the label
	label->c[0] = (mDNSu8)(ptr - label->c - 1);						// Set the length byte
	return(*cstr == 0);												// Return mDNStrue if we successfully consumed all input
	}

// MakeDomainNameFromDNSNameString makes a native DNS-format domainname from a C string.
// The C string is in conventional DNS syntax:
// Textual labels, escaped as necessary using the usual DNS '\' notation, separated by dots.
// If successful, MakeDomainNameFromDNSNameString returns a pointer to the next unused byte
// in the domainname bufer (i.e., the next byte after the terminating zero).
// If unable to construct a legal domain name (i.e. label more than 63 bytes, or total more than 255 bytes)
// MakeDomainNameFromDNSNameString returns mDNSNULL.
mDNSexport mDNSu8 *MakeDomainNameFromDNSNameString(domainname *const name, const char *cstr)
	{
	name->c[0] = 0;									// Make an empty domain name
	return(AppendDNSNameString(name, cstr));		// And then add this string to it
	}

mDNSexport char *ConvertDomainLabelToCString_withescape(const domainlabel *const label, char *ptr, char esc)
	{
	const mDNSu8 *      src = label->c;							// Domain label we're reading
	const mDNSu8        len = *src++;							// Read length of this (non-null) label
	const mDNSu8 *const end = src + len;						// Work out where the label ends
	if (len > MAX_DOMAIN_LABEL) return(mDNSNULL);				// If illegal label, abort
	while (src < end)											// While we have characters in the label
		{
		mDNSu8 c = *src++;
		if (esc)
			{
			if (c == '.' || c == esc)							// If character is a dot or the escape character
				*ptr++ = esc;									// Output escape character
			else if (c <= ' ')									// If non-printing ascii,
				{												// Output decimal escape sequence
				*ptr++ = esc;
				*ptr++ = (char)  ('0' + (c / 100)     );
				*ptr++ = (char)  ('0' + (c /  10) % 10);
				c      = (mDNSu8)('0' + (c      ) % 10);
				}
			}
		*ptr++ = (char)c;										// Copy the character
		}
	*ptr = 0;													// Null-terminate the string
	return(ptr);												// and return
	}

// Note: To guarantee that there will be no possible overrun, cstr must be at least MAX_ESCAPED_DOMAIN_NAME (1005 bytes)
mDNSexport char *ConvertDomainNameToCString_withescape(const domainname *const name, char *ptr, char esc)
	{
	const mDNSu8 *src         = name->c;								// Domain name we're reading
	const mDNSu8 *const max   = name->c + MAX_DOMAIN_NAME;			// Maximum that's valid

	if (*src == 0) *ptr++ = '.';									// Special case: For root, just write a dot

	while (*src)													// While more characters in the domain name
		{
		if (src + 1 + *src >= max) return(mDNSNULL);
		ptr = ConvertDomainLabelToCString_withescape((const domainlabel *)src, ptr, esc);
		if (!ptr) return(mDNSNULL);
		src += 1 + *src;
		*ptr++ = '.';												// Write the dot after the label
		}

	*ptr++ = 0;														// Null-terminate the string
	return(ptr);													// and return
	}

// RFC 1034 rules:
// Host names must start with a letter, end with a letter or digit,
// and have as interior characters only letters, digits, and hyphen.
// This was subsequently modified in RFC 1123 to allow the first character to be either a letter or a digit

mDNSexport void ConvertUTF8PstringToRFC1034HostLabel(const mDNSu8 UTF8Name[], domainlabel *const hostlabel)
	{
	const mDNSu8 *      src  = &UTF8Name[1];
	const mDNSu8 *const end  = &UTF8Name[1] + UTF8Name[0];
	      mDNSu8 *      ptr  = &hostlabel->c[1];
	const mDNSu8 *const lim  = &hostlabel->c[1] + MAX_DOMAIN_LABEL;
	while (src < end)
		{
		// Delete apostrophes from source name
		if (src[0] == '\'') { src++; continue; }		// Standard straight single quote
		if (src + 2 < end && src[0] == 0xE2 && src[1] == 0x80 && src[2] == 0x99)
			{ src += 3; continue; }	// Unicode curly apostrophe
		if (ptr < lim)
			{
			if (mdnsValidHostChar(*src, (ptr > &hostlabel->c[1]), (src < end-1))) *ptr++ = *src;
			else if (ptr > &hostlabel->c[1] && ptr[-1] != '-') *ptr++ = '-';
			}
		src++;
		}
	while (ptr > &hostlabel->c[1] && ptr[-1] == '-') ptr--;	// Truncate trailing '-' marks
	hostlabel->c[0] = (mDNSu8)(ptr - &hostlabel->c[1]);
	}

#if MDNS_ENFORCE_SERVICE_TYPE_LENGTH
mDNSlocal mDNSBool AllowedServiceNameException(const mDNSu8 *const src)
	{
	if (SameDomainLabel(src, (mDNSu8*)"\x12_MacOSXDupSuppress")) return(mDNStrue);
	LogMsg("Application protocol name %#s too long; see <http://www.dns-sd.org/ServiceTypes.html>", src);
	return(mDNSfalse);
	}
#endif

mDNSexport mDNSu8 *ConstructServiceName(domainname *const fqdn,
	const domainlabel *name, const domainname *type, const domainname *const domain)
	{
	int i, len;
	mDNSu8 *dst = fqdn->c;
	const mDNSu8 *src;
	const char *errormsg;

	// In the case where there is no name (and ONLY in that case),
	// a single-label subtype is allowed as the first label of a three-part "type"
	if (!name)
		{
		const mDNSu8 *s2 = type->c + 1 + type->c[0];
		if (type->c[0]  > 0 && type->c[0]  < 0x40 &&
			s2[0]       > 0 && s2[0]       < 0x40 &&
			s2[1+s2[0]] > 0 && s2[1+s2[0]] < 0x40)
			{
			name = (domainlabel *)type;
			type = (domainname  *)s2;
			}
		}

	if (name && name->c[0])
		{
		src = name->c;									// Put the service name into the domain name
		len = *src;
		if (len >= 0x40) { errormsg="Service instance name too long"; goto fail; }
		for (i=0; i<=len; i++) *dst++ = *src++;
		}
	else
		name = (domainlabel*)"";	// Set this up to be non-null, to avoid errors if we have to call LogMsg() below

	src = type->c;										// Put the service type into the domain name
	len = *src;
#if MDNS_ENFORCE_SERVICE_TYPE_LENGTH
	if (len < 2 || len > 15)
		if (!AllowedServiceNameException(src))			// If length not legal, check our grandfather-exceptions list
			{ errormsg="Application protocol name must be underscore plus 1-14 characters"; goto fail; }
#else
	if (len < 2 || len >= 0x40) { errormsg="Application protocol name should be underscore plus 1-14 characters"; goto fail; }
#endif
	if (src[1] != '_') { errormsg="Application protocol name must begin with underscore"; goto fail; }
	for (i=2; i<=len; i++)
		if (!mdnsIsLetter(src[i]) && !mdnsIsDigit(src[i]) && src[i] != '-' && src[i] != '_')
			{ errormsg="Application protocol name must contain only letters, digits, and hyphens"; goto fail; }
	for (i=0; i<=len; i++) *dst++ = *src++;

	len = *src;
	if (!(len == 4 && src[1] == '_' &&
		(((src[2] | 0x20) == 'u' && (src[3] | 0x20) == 'd') || ((src[2] | 0x20) == 't' && (src[3] | 0x20) == 'c')) &&
		(src[4] | 0x20) == 'p'))
		{ errormsg="Service transport protocol name must be _udp or _tcp"; goto fail; }
	for (i=0; i<=len; i++) *dst++ = *src++;

	if (*src) { errormsg="Service type must have only two labels"; goto fail; }

	*dst = 0;
	if (!domain->c[0]) { errormsg="Service domain must be non-empty"; goto fail; }
	dst = AppendDomainName(fqdn, domain);
	if (!dst) { errormsg="Service domain too long"; goto fail; }
	return(dst);

fail:
	LogMsg("ConstructServiceName: %s: %#s.%##s%##s", errormsg, name->c, type->c, domain->c);
	return(mDNSNULL);
	}

mDNSexport mDNSBool DeconstructServiceName(const domainname *const fqdn,
	domainlabel *const name, domainname *const type, domainname *const domain)
	{
	int i, len;
	const mDNSu8 *src = fqdn->c;
	const mDNSu8 *max = fqdn->c + MAX_DOMAIN_NAME;
	mDNSu8 *dst;

	dst = name->c;										// Extract the service name from the domain name
	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service name too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;

	dst = type->c;										// Extract the service type from the domain name
	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service type too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;

	len = *src;
	if (len >= 0x40) { debugf("DeconstructServiceName: service type too long"); return(mDNSfalse); }
	for (i=0; i<=len; i++) *dst++ = *src++;
	*dst++ = 0;		// Put the null root label on the end of the service type

	dst = domain->c;									// Extract the service domain from the domain name
	while (*src)
		{
		len = *src;
		if (len >= 0x40)
			{ debugf("DeconstructServiceName: service domain label too long"); return(mDNSfalse); }
		if (src + 1 + len + 1 >= max)
			{ debugf("DeconstructServiceName: service domain too long"); return(mDNSfalse); }
		for (i=0; i<=len; i++) *dst++ = *src++;
		}
	*dst++ = 0;		// Put the null root label on the end

	return(mDNStrue);
	}

// Returns true if a rich text label ends in " (nnn)", or if an RFC 1034
// name ends in "-nnn", where n is some decimal number.
mDNSexport mDNSBool LabelContainsSuffix(const domainlabel *const name, const mDNSBool RichText)
	{
	mDNSu16 l = name->c[0];

	if (RichText)
		{
		if (l < 4) return mDNSfalse;							// Need at least " (2)"
		if (name->c[l--] != ')') return mDNSfalse;				// Last char must be ')'
		if (!mdnsIsDigit(name->c[l])) return mDNSfalse;			// Preceeded by a digit
		l--;
		while (l > 2 && mdnsIsDigit(name->c[l])) l--;			// Strip off digits
		return (name->c[l] == '(' && name->c[l - 1] == ' ');
		}
	else
		{
		if (l < 2) return mDNSfalse;							// Need at least "-2"
		if (!mdnsIsDigit(name->c[l])) return mDNSfalse;			// Last char must be a digit
		l--;
		while (l > 2 && mdnsIsDigit(name->c[l])) l--;			// Strip off digits
		return (name->c[l] == '-');
		}
	}

// removes an auto-generated suffix (appended on a name collision) from a label.  caller is
// responsible for ensuring that the label does indeed contain a suffix.  returns the number
// from the suffix that was removed.
mDNSexport mDNSu32 RemoveLabelSuffix(domainlabel *name, mDNSBool RichText)
	{
	mDNSu32 val = 0, multiplier = 1;

	// Chop closing parentheses from RichText suffix
	if (RichText && name->c[0] >= 1 && name->c[name->c[0]] == ')') name->c[0]--;

	// Get any existing numerical suffix off the name
	while (mdnsIsDigit(name->c[name->c[0]]))
		{ val += (name->c[name->c[0]] - '0') * multiplier; multiplier *= 10; name->c[0]--; }

	// Chop opening parentheses or dash from suffix
	if (RichText)
		{
		if (name->c[0] >= 2 && name->c[name->c[0]] == '(' && name->c[name->c[0]-1] == ' ') name->c[0] -= 2;
		}
	else
		{
		if (name->c[0] >= 1 && name->c[name->c[0]] == '-') name->c[0] -= 1;
		}

	return(val);
	}

// appends a numerical suffix to a label, with the number following a whitespace and enclosed
// in parentheses (rich text) or following two consecutive hyphens (RFC 1034 domain label).
mDNSexport void AppendLabelSuffix(domainlabel *name, mDNSu32 val, mDNSBool RichText)
	{
	mDNSu32 divisor = 1, chars = 2;	// Shortest possible RFC1034 name suffix is 2 characters ("-2")
	if (RichText) chars = 4;		// Shortest possible RichText suffix is 4 characters (" (2)")

	// Truncate trailing spaces from RichText names
	if (RichText) while (name->c[name->c[0]] == ' ') name->c[0]--;

	while (val >= divisor * 10) { divisor *= 10; chars++; }

	if (name->c[0] > (mDNSu8)(MAX_DOMAIN_LABEL - chars))
		{
		name->c[0] = (mDNSu8)(MAX_DOMAIN_LABEL - chars);
		// If the following character is a UTF-8 continuation character,
		// we just chopped a multi-byte UTF-8 character in the middle, so strip back to a safe truncation point
		while (name->c[0] > 0 && (name->c[name->c[0]+1] & 0xC0) == 0x80) name->c[0]--;
		}

	if (RichText) { name->c[++name->c[0]] = ' '; name->c[++name->c[0]] = '('; }
	else          { name->c[++name->c[0]] = '-'; }

	while (divisor)
		{
		name->c[++name->c[0]] = (mDNSu8)('0' + val / divisor);
		val     %= divisor;
		divisor /= 10;
		}

	if (RichText) name->c[++name->c[0]] = ')';
	}

mDNSexport void IncrementLabelSuffix(domainlabel *name, mDNSBool RichText)
	{
	mDNSu32 val = 0;

	if (LabelContainsSuffix(name, RichText))
		val = RemoveLabelSuffix(name, RichText);

	// If no existing suffix, start by renaming "Foo" as "Foo (2)" or "Foo-2" as appropriate.
	// If existing suffix in the range 2-9, increment it.
	// If we've had ten conflicts already, there are probably too many hosts trying to use the same name,
	// so add a random increment to improve the chances of finding an available name next time.
	if      (val == 0) val = 2;
	else if (val < 10) val++;
	else               val += 1 + mDNSRandom(99);

	AppendLabelSuffix(name, val, RichText);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - Resource Record Utility Functions
#endif

mDNSexport mDNSu32 RDataHashValue(mDNSu16 const rdlength, const RDataBody *const rdb)
	{
	mDNSu32 sum = 0;
	int i;
	for (i=0; i+1 < rdlength; i+=2)
		{
		sum += (((mDNSu32)(rdb->data[i])) << 8) | rdb->data[i+1];
		sum = (sum<<3) | (sum>>29);
		}
	if (i < rdlength)
		{
		sum += ((mDNSu32)(rdb->data[i])) << 8;
		}
	return(sum);
	}

mDNSexport mDNSBool SameRData(const ResourceRecord *const r1, const ResourceRecord *const r2)
	{
	if (r1->rrtype     != r2->rrtype)   return(mDNSfalse);
	if (r1->rdlength   != r2->rdlength) return(mDNSfalse);
	if (r1->rdatahash  != r2->rdatahash) return(mDNSfalse);
	if (r1->rdnamehash != r2->rdnamehash) return(mDNSfalse);
	switch(r1->rrtype)
		{
		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	return(SameDomainName(&r1->rdata->u.name, &r2->rdata->u.name));

		case kDNSType_SRV:	return(mDNSBool)(  	r1->rdata->u.srv.priority          == r2->rdata->u.srv.priority          &&
												r1->rdata->u.srv.weight            == r2->rdata->u.srv.weight            &&
												r1->rdata->u.srv.port.NotAnInteger == r2->rdata->u.srv.port.NotAnInteger &&
												SameDomainName(&r1->rdata->u.srv.target, &r2->rdata->u.srv.target)       );

		default:			return(mDNSPlatformMemSame(r1->rdata->u.data, r2->rdata->u.data, r1->rdlength));
		}
	}

mDNSexport mDNSBool ResourceRecordAnswersQuestion(const ResourceRecord *const rr, const DNSQuestion *const q)
	{
	if (rr->InterfaceID &&
		q ->InterfaceID &&
		rr->InterfaceID != q->InterfaceID) return(mDNSfalse);

	// RR type CNAME matches any query type. QTYPE ANY matches any RR type. QCLASS ANY matches any RR class.
	if (rr->rrtype != kDNSType_CNAME && rr->rrtype  != q->qtype  && q->qtype  != kDNSQType_ANY ) return(mDNSfalse);
	if (                                rr->rrclass != q->qclass && q->qclass != kDNSQClass_ANY) return(mDNSfalse);
	return(rr->namehash == q->qnamehash && SameDomainName(&rr->name, &q->qname));
	}

mDNSexport mDNSu16 GetRDLength(const ResourceRecord *const rr, mDNSBool estimate)
	{
	RDataBody *rd = &rr->rdata->u;
	const domainname *const name = estimate ? &rr->name : mDNSNULL;
	switch (rr->rrtype)
		{
		case kDNSType_A:	return(sizeof(rd->ip));
		case kDNSType_CNAME:// Same as PTR
		case kDNSType_NS:   // Same as PTR
		case kDNSType_PTR:	return(CompressedDomainNameLength(&rd->name, name));
		case kDNSType_HINFO:return(mDNSu16)(2 + (int)rd->data[0] + (int)rd->data[1 + (int)rd->data[0]]);
		case kDNSType_NULL:	// Same as TXT -- not self-describing, so have to just trust rdlength
		case kDNSType_TXT:  return(rr->rdlength); // TXT is not self-describing, so have to just trust rdlength
		case kDNSType_AAAA:	return(sizeof(rd->ipv6));
		case kDNSType_SRV:	return(mDNSu16)(6 + CompressedDomainNameLength(&rd->srv.target, name));
		case kDNSType_SOA:  return (mDNSu16)(CompressedDomainNameLength(&rd->soa.mname, name) +
											CompressedDomainNameLength(&rd->soa.rname, name) +
											5 * sizeof(mDNSOpaque32));
		case kDNSType_OPT:  return(rr->rdlength);
		default:			debugf("Warning! Don't know how to get length of resource type %d", rr->rrtype);
							return(rr->rdlength);
		}
	}

mDNSexport mDNSBool ValidateRData(const mDNSu16 rrtype, const mDNSu16 rdlength, const RData *const rd)
	{
	mDNSu16 len;
	switch(rrtype)
		{
		case kDNSType_A:	return(rdlength == sizeof(mDNSv4Addr));

		case kDNSType_NS:	// Same as PTR
		case kDNSType_MD:	// Same as PTR
		case kDNSType_MF:	// Same as PTR
		case kDNSType_CNAME:// Same as PTR
		//case kDNSType_SOA not checked
		case kDNSType_MB:	// Same as PTR
		case kDNSType_MG:	// Same as PTR
		case kDNSType_MR:	// Same as PTR
		//case kDNSType_NULL not checked (no specified format, so always valid)
		//case kDNSType_WKS not checked
		case kDNSType_PTR:	len = DomainNameLength(&rd->u.name);
							return(len <= MAX_DOMAIN_NAME && rdlength == len);

		case kDNSType_HINFO:// Same as TXT (roughly)
		case kDNSType_MINFO:// Same as TXT (roughly)
		case kDNSType_TXT:  {
							const mDNSu8 *ptr = rd->u.txt.c;
							const mDNSu8 *end = rd->u.txt.c + rdlength;
							while (ptr < end) ptr += 1 + ptr[0];
							return (ptr == end);
							}

		case kDNSType_AAAA:	return(rdlength == sizeof(mDNSv6Addr));

		case kDNSType_MX:   len = DomainNameLength(&rd->u.mx.exchange);
							return(len <= MAX_DOMAIN_NAME && rdlength == 2+len);

		case kDNSType_SRV:	len = DomainNameLength(&rd->u.srv.target);
							return(len <= MAX_DOMAIN_NAME && rdlength == 6+len);

		default:			return(mDNStrue);	// Allow all other types without checking
		}
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - DNS Message Creation Functions
#endif

mDNSexport void InitializeDNSMessage(DNSMessageHeader *h, mDNSOpaque16 id, mDNSOpaque16 flags)
	{
	h->id             = id;
	h->flags          = flags;
	h->numQuestions   = 0;
	h->numAnswers     = 0;
	h->numAuthorities = 0;
	h->numAdditionals = 0;
	}

mDNSexport const mDNSu8 *FindCompressionPointer(const mDNSu8 *const base, const mDNSu8 *const end, const mDNSu8 *const domname)
	{
	const mDNSu8 *result = end - *domname - 1;

	if (*domname == 0) return(mDNSNULL);	// There's no point trying to match just the root label

	// This loop examines each possible starting position in packet, starting end of the packet and working backwards
	while (result >= base)
		{
		// If the length byte and first character of the label match, then check further to see
		// if this location in the packet will yield a useful name compression pointer.
		if (result[0] == domname[0] && result[1] == domname[1])
			{
			const mDNSu8 *name = domname;
			const mDNSu8 *targ = result;
			while (targ + *name < end)
				{
				// First see if this label matches
				int i;
				const mDNSu8 *pointertarget;
				for (i=0; i <= *name; i++) if (targ[i] != name[i]) break;
				if (i <= *name) break;							// If label did not match, bail out
				targ += 1 + *name;								// Else, did match, so advance target pointer
				name += 1 + *name;								// and proceed to check next label
				if (*name == 0 && *targ == 0) return(result);	// If no more labels, we found a match!
				if (*name == 0) break;							// If no more labels to match, we failed, so bail out

				// The label matched, so now follow the pointer (if appropriate) and then see if the next label matches
				if (targ[0] < 0x40) continue;					// If length value, continue to check next label
				if (targ[0] < 0xC0) break;						// If 40-BF, not valid
				if (targ+1 >= end) break;						// Second byte not present!
				pointertarget = base + (((mDNSu16)(targ[0] & 0x3F)) << 8) + targ[1];
				if (targ < pointertarget) break;				// Pointertarget must point *backwards* in the packet
				if (pointertarget[0] >= 0x40) break;			// Pointertarget must point to a valid length byte
				targ = pointertarget;
				}
			}
		result--;	// We failed to match at this search position, so back up the tentative result pointer and try again
		}
	return(mDNSNULL);
	}

// Put a string of dot-separated labels as length-prefixed labels
// domainname is a fully-qualified name (i.e. assumed to be ending in a dot, even if it doesn't)
// msg points to the message we're building (pass mDNSNULL if we don't want to use compression pointers)
// end points to the end of the message so far
// ptr points to where we want to put the name
// limit points to one byte past the end of the buffer that we must not overrun
// domainname is the name to put
mDNSexport mDNSu8 *putDomainNameAsLabels(const DNSMessage *const msg,
	mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name)
	{
	const mDNSu8 *const base        = (const mDNSu8 *)msg;
	const mDNSu8 *      np          = name->c;
	const mDNSu8 *const max         = name->c + MAX_DOMAIN_NAME;	// Maximum that's valid
	const mDNSu8 *      pointer     = mDNSNULL;
	const mDNSu8 *const searchlimit = ptr;

	while (*np && ptr < limit-1)		// While we've got characters in the name, and space to write them in the message...
		{
		if (*np > MAX_DOMAIN_LABEL)
			{ LogMsg("Malformed domain name %##s (label more than 63 bytes)", name->c); return(mDNSNULL); }

		// This check correctly allows for the final trailing root label:
		// e.g.
		// Suppose our domain name is exactly 255 bytes long, including the final trailing root label.
		// Suppose np is now at name->c[248], and we're about to write our last non-null label ("local").
		// We know that max will be at name->c[255]
		// That means that np + 1 + 5 == max - 1, so we (just) pass the "if" test below, write our
		// six bytes, then exit the loop, write the final terminating root label, and the domain
		// name we've written is exactly 255 bytes long, exactly at the correct legal limit.
		// If the name is one byte longer, then we fail the "if" test below, and correctly bail out.
		if (np + 1 + *np >= max)
			{ LogMsg("Malformed domain name %##s (more than 255 bytes)", name->c); return(mDNSNULL); }

		if (base) pointer = FindCompressionPointer(base, searchlimit, np);
		if (pointer)					// Use a compression pointer if we can
			{
			mDNSu16 offset = (mDNSu16)(pointer - base);
			*ptr++ = (mDNSu8)(0xC0 | (offset >> 8));
			*ptr++ = (mDNSu8)(        offset &  0xFF);
			return(ptr);
			}
		else							// Else copy one label and try again
			{
			int i;
			mDNSu8 len = *np++;
			if (ptr + 1 + len >= limit) return(mDNSNULL);
			*ptr++ = len;
			for (i=0; i<len; i++) *ptr++ = *np++;
			}
		}

	if (ptr < limit)												// If we didn't run out of space
		{
		*ptr++ = 0;													// Put the final root label
		return(ptr);												// and return
		}

	return(mDNSNULL);
	}

mDNSlocal mDNSu8 *putVal16(mDNSu8 *ptr, mDNSu16 val)
	{
	ptr[0] = (mDNSu8)((val >> 8 ) & 0xFF);
	ptr[1] = (mDNSu8)((val      ) & 0xFF);
	return ptr + sizeof(mDNSOpaque16);
	}

mDNSlocal mDNSu8 *putOptRData(mDNSu8 *ptr, const mDNSu8 *limit, ResourceRecord *rr)
	{
	int nput = 0;
	rdataOpt *opt;
	
	while (nput < rr->rdlength)
		{
		// check if space for opt/optlen
		if (ptr + (2 * sizeof(mDNSu16)) > limit) goto space_err;
		(mDNSu8 *)opt = rr->rdata->u.data + nput;
		ptr = putVal16(ptr, opt->opt);
		ptr = putVal16(ptr, opt->optlen);
		nput += 2 * sizeof(mDNSu16);
		if (opt->opt == kDNSOpt_LLQ)
			{
			if (ptr + sizeof(LLQOptData) > limit) goto space_err;
			ptr = putVal16(ptr, opt->OptData.llq.vers);
			ptr = putVal16(ptr, opt->OptData.llq.llqOp);
			ptr = putVal16(ptr, opt->OptData.llq.err);
			mDNSPlatformMemCopy(opt->OptData.llq.id, ptr, 8);  // 8-byte id
			ptr += 8;
			*(mDNSOpaque32 *)ptr = mDNSOpaque32fromIntVal(opt->OptData.llq.lease);
			ptr += sizeof(mDNSOpaque32);
			nput += sizeof(LLQOptData);
			}
		else if (opt->opt == kDNSOpt_Lease)
			{
			if (ptr + sizeof(mDNSs32) > limit) goto space_err;
			*(mDNSOpaque32 *)ptr = mDNSOpaque32fromIntVal(opt->OptData.lease);
			ptr += sizeof(mDNSs32);
			nput += sizeof(mDNSs32);
			}
		else { LogMsg("putOptRData - unknown option %d", opt->opt); return mDNSNULL; }
		}
	
	return ptr;

	space_err:
	LogMsg("ERROR: putOptRData - out of space");
	return mDNSNULL;
	}

mDNSlocal mDNSu16 getVal16(const mDNSu8 **ptr)
	{
	mDNSu16 val = (mDNSu16)(((mDNSu16)(*ptr)[0]) << 8 | (*ptr)[1]);
	*ptr += sizeof(mDNSOpaque16);
	return val;
	}

mDNSlocal const mDNSu8 *getOptRdata(const mDNSu8 *ptr, const mDNSu8 *limit, ResourceRecord *rr, mDNSu16 pktRDLen)
	{
	int nread = 0;
	rdataOpt *opt;
	
	while (nread < pktRDLen)
		{
		opt = (rdataOpt *)(rr->rdata->u.data + nread);
		// space for opt + optlen
		if (nread + (2 * sizeof(mDNSu16)) > rr->rdata->MaxRDLength) goto space_err;
		opt->opt = getVal16(&ptr);
		opt->optlen = getVal16(&ptr);
		nread += 2 * sizeof(mDNSu16);
		if (opt->opt == kDNSOpt_LLQ)
			{
			if ((unsigned)(limit - ptr) < sizeof(LLQOptData)) goto space_err;
			opt->OptData.llq.vers = getVal16(&ptr);
			opt->OptData.llq.llqOp = getVal16(&ptr);
			opt->OptData.llq.err = getVal16(&ptr);
			mDNSPlatformMemCopy(ptr, opt->OptData.llq.id, 8);
			ptr += 8;
			opt->OptData.llq.lease = (mDNSu32) ((mDNSu32)ptr[0] << 24 | (mDNSu32)ptr[1] << 16 | (mDNSu32)ptr[2] << 8 | ptr[3]);
			if (opt->OptData.llq.lease > 0x70000000UL / mDNSPlatformOneSecond)
				opt->OptData.llq.lease = 0x70000000UL / mDNSPlatformOneSecond;
			ptr += sizeof(mDNSOpaque32);
			nread += sizeof(LLQOptData);
			}
		else if (opt->opt == kDNSOpt_Lease)
			{
			if ((unsigned)(limit - ptr) < sizeof(mDNSs32)) goto space_err;

			opt->OptData.lease = (mDNSu32) ((mDNSu32)ptr[0] << 24 | (mDNSu32)ptr[1] << 16 | (mDNSu32)ptr[2] << 8 | ptr[3]);
			if (opt->OptData.lease > 0x70000000UL / mDNSPlatformOneSecond)
				opt->OptData.lease = 0x70000000UL / mDNSPlatformOneSecond;
			ptr += sizeof(mDNSs32);
			nread += sizeof(mDNSs32);
			}
		else { LogMsg("ERROR: getOptRdata - unknown opt %d", opt->opt); return mDNSNULL; }
		}
	
	rr->rdlength = pktRDLen;
	return ptr;

	space_err:
	LogMsg("ERROR: getLLQRdata - out of space");
	return mDNSNULL;
	}

mDNSexport mDNSu8 *putRData(const DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, ResourceRecord *rr)
	{
	switch (rr->rrtype)
		{
		case kDNSType_A:	if (rr->rdlength != 4)
								{
								debugf("putRData: Illegal length %d for kDNSType_A", rr->rdlength);
								return(mDNSNULL);
								}
							if (ptr + 4 > limit) return(mDNSNULL);
							*ptr++ = rr->rdata->u.ip.b[0];
							*ptr++ = rr->rdata->u.ip.b[1];
							*ptr++ = rr->rdata->u.ip.b[2];
							*ptr++ = rr->rdata->u.ip.b[3];
							return(ptr);

		case kDNSType_CNAME:// Same as PTR
		case kDNSType_PTR:	return(putDomainNameAsLabels(msg, ptr, limit, &rr->rdata->u.name));

		case kDNSType_AAAA:	if (rr->rdlength != sizeof(rr->rdata->u.ipv6))
								{
								debugf("putRData: Illegal length %d for kDNSType_AAAA", rr->rdlength);
								return(mDNSNULL);
								}
							if (ptr + sizeof(rr->rdata->u.ipv6) > limit) return(mDNSNULL);
							mDNSPlatformMemCopy(&rr->rdata->u.ipv6, ptr, sizeof(rr->rdata->u.ipv6));
							return(ptr + sizeof(rr->rdata->u.ipv6));

		case kDNSType_SRV:	if (ptr + 6 > limit) return(mDNSNULL);
							*ptr++ = (mDNSu8)(rr->rdata->u.srv.priority >> 8);
							*ptr++ = (mDNSu8)(rr->rdata->u.srv.priority &  0xFF);
							*ptr++ = (mDNSu8)(rr->rdata->u.srv.weight   >> 8);
							*ptr++ = (mDNSu8)(rr->rdata->u.srv.weight   &  0xFF);
							*ptr++ = rr->rdata->u.srv.port.b[0];
							*ptr++ = rr->rdata->u.srv.port.b[1];
							return(putDomainNameAsLabels(msg, ptr, limit, &rr->rdata->u.srv.target));
		case kDNSType_OPT:	return putOptRData(ptr, limit, rr);
							
		default:			debugf("putRData: Warning! Writing unknown resource type %d as raw data", rr->rrtype);
							// Fall through to common code below
		case kDNSType_HINFO:
		case kDNSType_TXT:
		case kDNSType_TSIG:	if (ptr + rr->rdlength > limit) return(mDNSNULL);
							mDNSPlatformMemCopy(rr->rdata->u.data, ptr, rr->rdlength);
							return(ptr + rr->rdlength);
		}
	}

mDNSexport mDNSu8 *PutResourceRecordTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32 ttl)
	{
	mDNSu8 *endofrdata;
	mDNSu16 actualLength;
	const mDNSu8 *limit = msg->data + AbsoluteMaxDNSMessageData;

	// If we have a single large record to put in the packet, then we allow the packet to be up to 9K bytes,
	// but in the normal case we try to keep the packets below 1500 to avoid IP fragmentation on standard Ethernet
	if (msg->h.numAnswers || msg->h.numAuthorities || msg->h.numAdditionals)
		limit = msg->data + NormalMaxDNSMessageData;

	if (rr->RecordType == kDNSRecordTypeUnregistered)
		{
		LogMsg("PutResourceRecord ERROR! Attempt to put kDNSRecordTypeUnregistered %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
		return(ptr);
		}

	ptr = putDomainNameAsLabels(msg, ptr, limit, &rr->name);
	if (!ptr || ptr + 10 >= limit) return(mDNSNULL);	// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rr->rrtype  >> 8);
	ptr[1] = (mDNSu8)(rr->rrtype  &  0xFF);
	ptr[2] = (mDNSu8)(rr->rrclass >> 8);
	ptr[3] = (mDNSu8)(rr->rrclass &  0xFF);
	ptr[4] = (mDNSu8)((ttl >> 24) &  0xFF);
	ptr[5] = (mDNSu8)((ttl >> 16) &  0xFF);
	ptr[6] = (mDNSu8)((ttl >>  8) &  0xFF);
	ptr[7] = (mDNSu8)( ttl        &  0xFF);
	endofrdata = putRData(msg, ptr+10, limit, rr);
	if (!endofrdata) { verbosedebugf("Ran out of space in PutResourceRecord for %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype)); return(mDNSNULL); }

	// Go back and fill in the actual number of data bytes we wrote
	// (actualLength can be less than rdlength when domain name compression is used)
	actualLength = (mDNSu16)(endofrdata - ptr - 10);
	ptr[8] = (mDNSu8)(actualLength >> 8);
	ptr[9] = (mDNSu8)(actualLength &  0xFF);

	if (count) (*count)++;
	else LogMsg("PutResourceRecordTTL: ERROR: No target count to update for %##s (%s)", rr->name.c, DNSTypeName(rr->rrtype));
	return(endofrdata);
	}

mDNSexport mDNSu8 *PutResourceRecordCappedTTL(DNSMessage *const msg, mDNSu8 *ptr, mDNSu16 *count, ResourceRecord *rr, mDNSu32
											   maxttl)
	{
	if (maxttl > rr->rroriginalttl) maxttl = rr->rroriginalttl;
	return(PutResourceRecordTTL(msg, ptr, count, rr, maxttl));
	}

mDNSexport mDNSu8 *putEmptyResourceRecord(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit,
	mDNSu16 *count, const AuthRecord *rr)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, &rr->resrec.name);
	if (!ptr || ptr + 10 > limit) return(mDNSNULL);		// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rr->resrec.rrtype  >> 8);				// Put type
	ptr[1] = (mDNSu8)(rr->resrec.rrtype  &  0xFF);
	ptr[2] = (mDNSu8)(rr->resrec.rrclass >> 8);				// Put class
	ptr[3] = (mDNSu8)(rr->resrec.rrclass &  0xFF);
	ptr[4] = ptr[5] = ptr[6] = ptr[7] = 0;				// TTL is zero
	ptr[8] = ptr[9] = 0;								// RDATA length is zero
	(*count)++;
	return(ptr + 10);
	}

mDNSexport mDNSu8 *putQuestion(DNSMessage *const msg, mDNSu8 *ptr, const mDNSu8 *const limit, const domainname *const name, mDNSu16 rrtype, mDNSu16 rrclass)
	{
	ptr = putDomainNameAsLabels(msg, ptr, limit, name);
	if (!ptr || ptr+4 >= limit) return(mDNSNULL);			// If we're out-of-space, return mDNSNULL
	ptr[0] = (mDNSu8)(rrtype  >> 8);
	ptr[1] = (mDNSu8)(rrtype  &  0xFF);
	ptr[2] = (mDNSu8)(rrclass >> 8);
	ptr[3] = (mDNSu8)(rrclass &  0xFF);
	msg->h.numQuestions++;
	return(ptr+4);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark - DNS Message Parsing Functions
#endif

mDNSexport mDNSu32 DomainNameHashValue(const domainname *const name)
	{
	mDNSu32 sum = 0;
	const mDNSu8 *c;

	for (c = name->c; c[0] != 0 && c[1] != 0; c += 2)
		{
		sum += ((mDNSIsUpperCase(c[0]) ? c[0] + 'a' - 'A' : c[0]) << 8) |
				(mDNSIsUpperCase(c[1]) ? c[1] + 'a' - 'A' : c[1]);
		sum = (sum<<3) | (sum>>29);
		}
	if (c[0]) sum += ((mDNSIsUpperCase(c[0]) ? c[0] + 'a' - 'A' : c[0]) << 8);
	return(sum);
	}

mDNSexport void SetNewRData(ResourceRecord *const rr, RData *NewRData, mDNSu16 rdlength)
	{
	domainname *target;
	if (NewRData)
		{
		rr->rdata    = NewRData;
		rr->rdlength = rdlength;
		}
	// Must not try to get target pointer until after updating rr->rdata
	target = GetRRDomainNameTarget(rr);
	rr->rdlength   = GetRDLength(rr, mDNSfalse);
	rr->rdestimate = GetRDLength(rr, mDNStrue);
	rr->rdatahash  = RDataHashValue(rr->rdlength, &rr->rdata->u);
	rr->rdnamehash = target ? DomainNameHashValue(target) : 0;
	}

mDNSexport const mDNSu8 *skipDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end)
	{
	mDNSu16 total = 0;

	if (ptr < (mDNSu8*)msg || ptr >= end)
		{ debugf("skipDomainName: Illegal ptr not within packet boundaries"); return(mDNSNULL); }

	while (1)						// Read sequence of labels
		{
		const mDNSu8 len = *ptr++;	// Read length of this label
		if (len == 0) return(ptr);	// If length is zero, that means this name is complete
		switch (len & 0xC0)
			{
			case 0x00:	if (ptr + len >= end)					// Remember: expect at least one more byte for the root label
							{ debugf("skipDomainName: Malformed domain name (overruns packet end)"); return(mDNSNULL); }
						if (total + 1 + len >= MAX_DOMAIN_NAME)	// Remember: expect at least one more byte for the root label
							{ debugf("skipDomainName: Malformed domain name (more than 255 characters)"); return(mDNSNULL); }
						ptr += len;
						total += 1 + len;
						break;

			case 0x40:	debugf("skipDomainName: Extended EDNS0 label types 0x%X not supported", len); return(mDNSNULL);
			case 0x80:	debugf("skipDomainName: Illegal label length 0x%X", len); return(mDNSNULL);
			case 0xC0:	return(ptr+1);
			}
		}
	}

// Routine to fetch an FQDN from the DNS message, following compression pointers if necessary.
mDNSexport const mDNSu8 *getDomainName(const DNSMessage *const msg, const mDNSu8 *ptr, const mDNSu8 *const end,
	domainname *const name)
	{
	const mDNSu8 *nextbyte = mDNSNULL;					// Record where we got to before we started following pointers
	mDNSu8       *np = name->c;							// Name pointer
	const mDNSu8 *const limit = np + MAX_DOMAIN_NAME;	// Limit so we don't overrun buffer

	if (ptr < (mDNSu8*)msg || ptr >= end)
		{ debugf("getDomainName: Illegal ptr not within packet boundaries"); return(mDNSNULL); }

	*np = 0;						// Tentatively place the root label here (may be overwritten if we have more labels)

	while (1)						// Read sequence of labels
		{
		const mDNSu8 len = *ptr++;	// Read length of this label
		if (len == 0) break;		// If length is zero, that means this name is complete
		switch (len & 0xC0)
			{
			int i;
			mDNSu16 offset;

			case 0x00:	if (ptr + len >= end)		// Remember: expect at least one more byte for the root label
							{ debugf("getDomainName: Malformed domain name (overruns packet end)"); return(mDNSNULL); }
						if (np + 1 + len >= limit)	// Remember: expect at least one more byte for the root label
							{ debugf("getDomainName: Malformed domain name (more than 255 characters)"); return(mDNSNULL); }
						*np++ = len;
						for (i=0; i<len; i++) *np++ = *ptr++;
						*np = 0;	// Tentatively place the root label here (may be overwritten if we have more labels)
						break;

			case 0x40:	debugf("getDomainName: Extended EDNS0 label types 0x%X not supported in name %##s", len, name->c);
						return(mDNSNULL);

			case 0x80:	debugf("getDomainName: Illegal label length 0x%X in domain name %##s", len, name->c); return(mDNSNULL);

			case 0xC0:	offset = (mDNSu16)((((mDNSu16)(len & 0x3F)) << 8) | *ptr++);
						if (!nextbyte) nextbyte = ptr;	// Record where we got to before we started following pointers
						ptr = (mDNSu8 *)msg + offset;
						if (ptr < (mDNSu8*)msg || ptr >= end)
							{ debugf("getDomainName: Illegal compression pointer not within packet boundaries"); return(mDNSNULL); }
						if (*ptr & 0xC0)
							{ debugf("getDomainName: Compression pointer must point to real label"); return(mDNSNULL); }
						break;
			}
		}

	if (nextbyte) return(nextbyte);
	else return(ptr);
	}

mDNSexport const mDNSu8 *skipResourceRecord(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end)
	{
	mDNSu16 pktrdlength;

	ptr = skipDomainName(msg, ptr, end);
	if (!ptr) { debugf("skipResourceRecord: Malformed RR name"); return(mDNSNULL); }

	if (ptr + 10 > end) { debugf("skipResourceRecord: Malformed RR -- no type/class/ttl/len!"); return(mDNSNULL); }
	pktrdlength = (mDNSu16)((mDNSu16)ptr[8] <<  8 | ptr[9]);
	ptr += 10;
	if (ptr + pktrdlength > end) { debugf("skipResourceRecord: RDATA exceeds end of packet"); return(mDNSNULL); }

	return(ptr + pktrdlength);
	}

mDNSexport const mDNSu8 *GetResourceRecord(mDNS *const m, const DNSMessage * const msg, const mDNSu8 *ptr,
    const mDNSu8 * const end, const mDNSInterfaceID InterfaceID, mDNSu8 RecordType, CacheRecord *rr, RData *RDataStorage)
	{
	mDNSu16 pktrdlength;

	rr->next              = mDNSNULL;
	rr->resrec.RecordType = RecordType;

	rr->NextInKAList      = mDNSNULL;
	rr->TimeRcvd          = m->timenow;
	rr->NextRequiredQuery = m->timenow;		// Will be updated to the real value when we call SetNextCacheCheckTime()
	rr->LastUsed          = m->timenow;
	rr->UseCount          = 0;
	rr->CRActiveQuestion  = mDNSNULL;
	rr->UnansweredQueries = 0;
	rr->LastUnansweredTime= 0;
	rr->MPUnansweredQ     = 0;
	rr->MPLastUnansweredQT= 0;
	rr->MPUnansweredKA    = 0;
	rr->MPExpectingKA     = mDNSfalse;
	rr->NextInCFList      = mDNSNULL;

	rr->resrec.InterfaceID       = InterfaceID;
	ptr = getDomainName(msg, ptr, end, &rr->resrec.name);
	if (!ptr) { debugf("GetResourceRecord: Malformed RR name"); return(mDNSNULL); }

	if (ptr + 10 > end) { debugf("GetResourceRecord: Malformed RR -- no type/class/ttl/len!"); return(mDNSNULL); }

	rr->resrec.rrtype            = (mDNSu16) ((mDNSu16)ptr[0] <<  8 | ptr[1]);
	rr->resrec.rrclass           = (mDNSu16)(((mDNSu16)ptr[2] <<  8 | ptr[3]) & kDNSClass_Mask);
	rr->resrec.rroriginalttl     = (mDNSu32) ((mDNSu32)ptr[4] << 24 | (mDNSu32)ptr[5] << 16 | (mDNSu32)ptr[6] << 8 | ptr[7]);
	if (rr->resrec.rroriginalttl > 0x70000000UL / mDNSPlatformOneSecond && (mDNSs32)rr->resrec.rroriginalttl != -1)
		rr->resrec.rroriginalttl = 0x70000000UL / mDNSPlatformOneSecond;
	// Note: We don't have to adjust m->NextCacheCheck here -- this is just getting a record into memory for
	// us to look at. If we decide to copy it into the cache, then we'll update m->NextCacheCheck accordingly.
	pktrdlength           = (mDNSu16)((mDNSu16)ptr[8] <<  8 | ptr[9]);
	if (ptr[2] & (kDNSClass_UniqueRRSet >> 8))
		rr->resrec.RecordType |= kDNSRecordTypePacketUniqueMask;
	ptr += 10;
	if (ptr + pktrdlength > end) { debugf("GetResourceRecord: RDATA exceeds end of packet"); return(mDNSNULL); }

	if (RDataStorage)
		rr->resrec.rdata = RDataStorage;
	else
		{
		rr->resrec.rdata = (RData*)&rr->rdatastorage;
		rr->resrec.rdata->MaxRDLength = sizeof(RDataBody);
		}

	switch (rr->resrec.rrtype)
		{
		case kDNSType_A:	rr->resrec.rdata->u.ip.b[0] = ptr[0];
							rr->resrec.rdata->u.ip.b[1] = ptr[1];
							rr->resrec.rdata->u.ip.b[2] = ptr[2];
							rr->resrec.rdata->u.ip.b[3] = ptr[3];
							break;

		case kDNSType_CNAME:// Same as PTR
		case kDNSType_NS:
		case kDNSType_PTR:	if (!getDomainName(msg, ptr, end, &rr->resrec.rdata->u.name))
								{ debugf("GetResourceRecord: Malformed CNAME/PTR RDATA name"); return(mDNSNULL); }
							//debugf("%##s PTR %##s rdlen %d", rr->resrec.name.c, rr->resrec.rdata->u.name.c, pktrdlength);
							break;

		case kDNSType_NULL:	//Same as TXT
		case kDNSType_HINFO://Same as TXT
		case kDNSType_TXT:  if (pktrdlength > rr->resrec.rdata->MaxRDLength)
								{
								debugf("GetResourceRecord: %s rdata size (%d) exceeds storage (%d)",
									DNSTypeName(rr->resrec.rrtype), pktrdlength, rr->resrec.rdata->MaxRDLength);
								return(mDNSNULL);
								}
							rr->resrec.rdlength = pktrdlength;
							mDNSPlatformMemCopy(ptr, rr->resrec.rdata->u.data, pktrdlength);
							break;

		case kDNSType_AAAA:	mDNSPlatformMemCopy(ptr, &rr->resrec.rdata->u.ipv6, sizeof(rr->resrec.rdata->u.ipv6));
							break;

		case kDNSType_SRV:	rr->resrec.rdata->u.srv.priority = (mDNSu16)((mDNSu16)ptr[0] <<  8 | ptr[1]);
							rr->resrec.rdata->u.srv.weight   = (mDNSu16)((mDNSu16)ptr[2] <<  8 | ptr[3]);
							rr->resrec.rdata->u.srv.port.b[0] = ptr[4];
							rr->resrec.rdata->u.srv.port.b[1] = ptr[5];
							if (!getDomainName(msg, ptr+6, end, &rr->resrec.rdata->u.srv.target))
								{ debugf("GetResourceRecord: Malformed SRV RDATA name"); return(mDNSNULL); }
							//debugf("%##s SRV %##s rdlen %d", rr->resrec.name.c, rr->resrec.rdata->u.srv.target.c, pktrdlength);
							break;

		case kDNSType_SOA:  if (!getDomainName(msg, ptr, end, &rr->resrec.rdata->u.soa.mname) ||
							   !getDomainName(msg, ptr, end, &rr->resrec.rdata->u.soa.rname))
                			   { debugf("GetResourceRecord: Malformed SOA RDATA mname/rname"); return mDNSNULL; }
			                if ((unsigned)(end - ptr) < 5 * sizeof(mDNSOpaque32))
								{ debugf("GetResourceRecord: Malformed SOA RDATA"); return mDNSNULL; }
                			rr->resrec.rdata->u.soa.serial.NotAnInteger = ((mDNSOpaque32 *)ptr)->NotAnInteger;   ptr += 4;
			                rr->resrec.rdata->u.soa.refresh.NotAnInteger = ((mDNSOpaque32 *)ptr)->NotAnInteger;  ptr += 4;
			                rr->resrec.rdata->u.soa.retry.NotAnInteger = ((mDNSOpaque32 *)ptr)->NotAnInteger;    ptr += 4;
			                rr->resrec.rdata->u.soa.expire.NotAnInteger = ((mDNSOpaque32 *)ptr)->NotAnInteger;   ptr += 4;
			                rr->resrec.rdata->u.soa.min.NotAnInteger = ((mDNSOpaque32 *)ptr)->NotAnInteger;
			                break;

		case kDNSType_OPT:  getOptRdata(ptr, end, &rr->resrec, pktrdlength); break;

		default:			if (pktrdlength > rr->resrec.rdata->MaxRDLength)
								{
								debugf("GetResourceRecord: rdata %d (%s) size (%d) exceeds storage (%d)",
									rr->resrec.rrtype, DNSTypeName(rr->resrec.rrtype), pktrdlength, rr->resrec.rdata->MaxRDLength);
								return(mDNSNULL);
								}
							debugf("GetResourceRecord: Warning! Reading resource type %d (%s) as opaque data",
								rr->resrec.rrtype, DNSTypeName(rr->resrec.rrtype));
							// Note: Just because we don't understand the record type, that doesn't
							// mean we fail. The DNS protocol specifies rdlength, so we can
							// safely skip over unknown records and ignore them.
							// We also grab a binary copy of the rdata anyway, since the caller
							// might know how to interpret it even if we don't.
							rr->resrec.rdlength = pktrdlength;
							mDNSPlatformMemCopy(ptr, rr->resrec.rdata->u.data, pktrdlength);
							break;
		}

	rr->resrec.namehash = DomainNameHashValue(&rr->resrec.name);
	SetNewRData(&rr->resrec, mDNSNULL, 0);

	return(ptr + pktrdlength);
	}

mDNSexport const mDNSu8 *skipQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end)
	{
	ptr = skipDomainName(msg, ptr, end);
	if (!ptr) { debugf("skipQuestion: Malformed domain name in DNS question section"); return(mDNSNULL); }
	if (ptr+4 > end) { debugf("skipQuestion: Malformed DNS question section -- no query type and class!"); return(mDNSNULL); }
	return(ptr+4);
	}

mDNSexport const mDNSu8 *getQuestion(const DNSMessage *msg, const mDNSu8 *ptr, const mDNSu8 *end, const mDNSInterfaceID InterfaceID,
	DNSQuestion *question)
	{
	question->InterfaceID = InterfaceID;
	ptr = getDomainName(msg, ptr, end, &question->qname);
	if (!ptr) { debugf("Malformed domain name in DNS question section"); return(mDNSNULL); }
	if (ptr+4 > end) { debugf("Malformed DNS question section -- no query type and class!"); return(mDNSNULL); }

	question->qnamehash = DomainNameHashValue(&question->qname);
	question->qtype  = (mDNSu16)((mDNSu16)ptr[0] << 8 | ptr[1]);			// Get type
	question->qclass = (mDNSu16)((mDNSu16)ptr[2] << 8 | ptr[3]);			// and class
	return(ptr+4);
	}

mDNSexport const mDNSu8 *LocateAnswers(const DNSMessage *const msg, const mDNSu8 *const end)
	{
	int i;
	const mDNSu8 *ptr = msg->data;
	for (i = 0; i < msg->h.numQuestions && ptr; i++) ptr = skipQuestion(msg, ptr, end);
	return(ptr);
	}

mDNSexport const mDNSu8 *LocateAuthorities(const DNSMessage *const msg, const mDNSu8 *const end)
	{
	int i;
	const mDNSu8 *ptr = LocateAnswers(msg, end);
	for (i = 0; i < msg->h.numAnswers && ptr; i++) ptr = skipResourceRecord(msg, ptr, end);
	return(ptr);
	}

mDNSexport const mDNSu8 *LocateAdditionals(const DNSMessage *const msg, const mDNSu8 *const end)
	{
	int i;
	const mDNSu8 *ptr = LocateAuthorities(msg, end);
	for (i = 0; i < msg->h.numAuthorities; i++) ptr = skipResourceRecord(msg, ptr, end);
	return (ptr);
	}

// ***************************************************************************
#if COMPILER_LIKES_PRAGMA_MARK
#pragma mark -
#pragma mark -
#pragma mark - Packet Sending Functions
#endif

mDNSlocal mStatus sendDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 *end,
    mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport, int sd, uDNS_AuthInfo *authInfo)
	{
	mStatus status;
	int nsent;
	mDNSs32 msglen;
	mDNSu8 lenbuf[2];
	mDNSu16 numQuestions   = msg->h.numQuestions;
	mDNSu16 numAnswers     = msg->h.numAnswers;
	mDNSu16 numAuthorities = msg->h.numAuthorities;
	mDNSu16 numAdditionals = msg->h.numAdditionals;
	mDNSu8 *ptr = (mDNSu8 *)&msg->h.numQuestions;

	// Put all the integer values in IETF byte-order (MSB first, LSB second)
	*ptr++ = (mDNSu8)(numQuestions   >> 8);
	*ptr++ = (mDNSu8)(numQuestions   &  0xFF);
	*ptr++ = (mDNSu8)(numAnswers     >> 8);
	*ptr++ = (mDNSu8)(numAnswers     &  0xFF);
	*ptr++ = (mDNSu8)(numAuthorities >> 8);
	*ptr++ = (mDNSu8)(numAuthorities &  0xFF);
	*ptr++ = (mDNSu8)(numAdditionals >> 8);
	*ptr++ = (mDNSu8)(numAdditionals &  0xFF);

	if (authInfo)
		{
		end = DNSDigest_SignMessage(msg, &end, &numAdditionals, authInfo);
		if (!end) return mStatus_UnknownErr;
		}

	// Send the packet on the wire

	if (sd >= 0)
		{
		msglen = (mDNSu16)(end - (mDNSu8 *)msg);
		lenbuf[0] = (mDNSu8)(msglen >> 8);  // host->network byte conversion
		lenbuf[1] = (mDNSu8)(msglen &  0xFF);
		nsent = mDNSPlatformWriteTCP(sd, (char*)lenbuf, 2);
		//!!!KRS make sure kernel is sending these as 1 packet!
		if (nsent != 2) goto tcp_error;
		nsent = mDNSPlatformWriteTCP(sd, (char *)msg, msglen);
		if (nsent != msglen) goto tcp_error;
		status = mStatus_NoError;
		}
	else
		{
		status = mDNSPlatformSendUDP(m, msg, end, InterfaceID, dst, dstport);
		}

	// Put all the integer values back the way they were before we return
	msg->h.numQuestions   = numQuestions;
	msg->h.numAnswers     = numAnswers;
	msg->h.numAuthorities = numAuthorities;
	msg->h.numAdditionals = (mDNSu16)(authInfo ? numAdditionals - 1 : numAdditionals);

	return(status);

	tcp_error:
	LogMsg("sendDNSMessage: error sending message over tcp");
	return mStatus_UnknownErr;

	}

mDNSexport mStatus mDNSSendDNSMessage_tcp(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end, int sd)
	{
	if (sd < 0) { LogMsg("mDNSSendDNSMessage_tcp: invalid desciptor %d", sd); return mStatus_UnknownErr; }
	return sendDNSMessage(m, msg, end, mDNSInterface_Any, &zeroAddr, zeroIPPort, sd, mDNSNULL);
	}

mDNSexport mStatus mDNSSendDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end,
	mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport)
	{
	return sendDNSMessage(m, msg, end, InterfaceID, dst, dstport, -1, mDNSNULL);
	}

mDNSexport mStatus mDNSSendSignedDNSMessage(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end,
    mDNSInterfaceID InterfaceID, const mDNSAddr *dst, mDNSIPPort dstport, uDNS_AuthInfo *authInfo)
	{
	return sendDNSMessage(m, msg, end, InterfaceID, dst, dstport, -1, authInfo);
	}

mDNSexport mStatus mDNSSendSignedDNSMessage_tcp(const mDNS *const m, DNSMessage *const msg, mDNSu8 * end, int sd, uDNS_AuthInfo *authInfo)
	{
	if (sd < 0) { LogMsg("mDNSSendDNSMessage_tcp: invalid desciptor %d", sd); return mStatus_UnknownErr; }
	return sendDNSMessage(m, msg, end, mDNSInterface_Any, &zeroAddr, zeroIPPort, sd, authInfo);
	}
