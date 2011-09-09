# Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
#
# @APPLE_LICENSE_HEADER_START@
# 
# This file contains Original Code and/or Modifications of Original Code
# as defined in and that are subject to the Apple Public Source License
# Version 2.0 (the 'License'). You may not use this file except in
# compliance with the License. Please obtain a copy of the License at
# http://www.opensource.apple.com/apsl/ and read it before using this
# file.
# 
# The Original Code and all software distributed under the License are
# distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
# Please see the License for the specific language governing rights and
# limitations under the License.
# 
# @APPLE_LICENSE_HEADER_END@
#
# $Log: Makefile,v $
# Revision 1.7  2006/01/06 01:06:17  cheshire
# <rdar://problem/3978979> Compile library and client programs in one pass
#
# Revision 1.6  2004/09/24 21:15:26  cheshire
# <rdar://problem/3724985> Library "libmdns" misnamed; should be "libdns_sd"
#
# Revision 1.5  2004/09/02 17:32:45  cheshire
# Look for headers in ../mDNSShared before we go to /usr/include
#
# Revision 1.4  2004/05/21 17:25:56  cheshire
# Fixes to make sample client work on Linux
#
# Revision 1.3  2004/03/12 08:05:32  cheshire
# Add a "make clean" target
#
# Revision 1.2  2004/02/11 20:59:26  cheshire
# Fix Makefile so it creates the "build" directory if necessary
#
# Revision 1.1  2004/02/06 03:19:09  cheshire
# Check in code to make command-line "dns-sd" testing tool
#
#
# Notes:
# $@ means "The file name of the target of the rule"
# $< means "The name of the first prerequisite"
# $+ means "The names of all the prerequisites, with spaces between them, exactly as given"
# For more magic automatic variables, see
# <http://www.gnu.org/software/make/manual/html_chapter/make_10.html#SEC111>

#############################################################################

# On OS X the dns_sd library functions are included in libSystem, which is implicitly linked with every executable
# If /usr/lib/libSystem.dylib exists, then we're on OS X, so we don't need also to link the "dns_sd" shared library
ifneq "$(wildcard /usr/lib/libSystem.dylib)" ""
LIBS =
else
LIBS = -L../mDNSPosix/build/prod/ -ldns_sd
endif

targets: build/dns-sd

clean:
	rm -rf build

build:
	mkdir build

build/dns-sd: build dns-sd.c
	cc $(filter %.c %.o, $+) $(LIBS) -I../mDNSShared -o $@
