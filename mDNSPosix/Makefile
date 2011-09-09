# Copyright (c) 2002-2003 Apple Computer, Inc. All rights reserved.
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
# Revision 1.34  2004/05/25 18:29:33  cheshire
# Move DNSServiceConstructFullName() from dnssd_clientstub.c to dnssd_clientlib.c,
# so that it's also accessible to dnssd_clientshim.c (single address space) clients.
#
# Revision 1.33  2004/04/30 16:46:32  rpantos
# Add support for building Java libraries.
#
# Revision 1.32  2004/04/14 23:09:29  ksekar
# Support for TSIG signed dynamic updates.
#
# Revision 1.31  2004/03/15 19:07:06  cheshire
# Fix error message
#
# Revision 1.30  2004/03/11 18:58:29  rpantos
# Fix Kill /etc/rc scripts so they run at halt & reboot.
#
# Revision 1.29  2004/03/04 23:35:41  cheshire
# Instead of using a dummy target to generate an error message, use "$(error text...)"
#
# Revision 1.28  2004/03/04 23:33:42  cheshire
# Fixes from Alfred Perlstein for FreeBSD's benefit
#
# Revision 1.27  2004/02/11 21:00:21  cheshire
# Update URL for GNU Make manual page
#
# Revision 1.26  2004/02/05 21:28:30  cheshire
# Fixes so that "sudo make install" works on *BSD
#
# Revision 1.25  2004/02/05 20:00:22  cheshire
# Define mdnsd's PID file to be /var/run/mdnsd.pid on Posix builds
#
# Revision 1.24  2004/02/05 01:00:01  rpantos
# Fix some issues that turned up when building for FreeBSD.
#
# Revision 1.23  2004/02/04 01:50:54  cheshire
# Make InstalledStartup conditional, so it automatically installs into
# either /etc/init.d/ or /etc/rc.d/init.d/ as appropriate
#
# Revision 1.22  2004/01/20 01:41:21  rpantos
# Define USES_NETLINK for Linux builds.
#
# Revision 1.21  2003/12/17 00:51:22  cheshire
# Changed mDNSNetMonitor and mDNSIdentify to link the object files
# instead of #including the "DNSCommon.c" "uDNS.c" and source files
#
# Revision 1.20  2003/12/13 03:05:28  ksekar
# Bug #: <rdar://problem/3192548>: DynDNS: Unicast query of service records
#
# Revision 1.19  2003/12/11 19:42:13  cheshire
# Change name "mDNSResponderd" to "mdnsd" for consistency with standard Linux (Unix) naming conventions
#
# Revision 1.18  2003/12/11 19:38:34  cheshire
# Add APSL
#
# Revision 1.17  2003/12/11 03:16:49  rpantos
# One more change for OS X build: make install work a little better.
#
# Revision 1.16  2003/12/11 03:03:51  rpantos
# Clean up mDNSPosix so that it builds on OS X again.
#
# Revision 1.15  2003/12/08 20:47:02  rpantos
# Add support for mDNSResponder on Linux.
#
# Revision 1.14  2003/11/14 20:59:09  cheshire
# Clients can't use AssignDomainName macro because mDNSPlatformMemCopy is defined in mDNSPlatformFunctions.h.
# Best solution is just to combine mDNSClientAPI.h and mDNSPlatformFunctions.h into a single file.
#
# Revision 1.13  2003/08/06 18:20:51  cheshire
# Makefile cleanup
#
# Revision 1.12  2003/08/01 02:20:02  cheshire
# Add mDNSIdentify tool, used to discover what version of mDNSResponder a particular host is running
#
# Revision 1.11  2003/07/14 18:11:54  cheshire
# Fix stricter compiler warnings
#
# Revision 1.10  2003/06/18 05:47:41  cheshire
# Enable stricter warnings on Jaguar and Panther builds
#
# Revision 1.9  2003/06/04 18:34:45  ksekar
# Bug #: <rdar://problem/3218120>: mDNSPosix does not build on Panther that has socklen_t
# Changed build targets "osx10.2" and "osx10.3" to "jaguar" and "panther".
#
# Revision 1.8  2003/06/04 00:23:12  ksekar
# Bug #: <rdar://problem/3218120>: mDNSPosix does not build on Panther that has socklen_t
# Created separate target OS's for 10.2 and 10.3.
#
# Revision 1.7  2003/04/16 02:11:37  cheshire
# Remove unnecessary $(CFLAGS) from linking rules
#
# Revision 1.6  2003/04/04 01:37:14  cheshire
# Added NetMonitor.c
#

# This Makefile builds an mDNSResponder daemon and a libmdns.so shared library 
# for Linux. It also builds several example programs for embedded systems. 
#
# Make with no arguments to build all production targets.
# 'make DEBUG=1' to build debugging targets.
# 'make clean' or 'make clean DEBUG=1' to delete prod/debug objects & targets
# 'sudo make install [DEBUG=1]' to install mdnsd daemon and libmdns.
#
# Notes:
# $@ means "The file name of the target of the rule"
# $< means "The name of the first prerequisite"
# $+ means "The names of all the prerequisites, with spaces between them, exactly as given"
# For more magic automatic variables, see
# <http://www.gnu.org/software/make/manual/html_chapter/make_10.html#SEC111>

#############################################################################

LIBVERS = 1

COREDIR = ../mDNSCore
SHAREDDIR = ../mDNSShared
JDK = /usr/jdk

CC = cc
LD = ld
CP = cp
RM = rm
LN = ln -s -f
CFLAGS_COMMON = -I. -I$(COREDIR) -I$(SHAREDDIR) -W -Wall -DPID_FILE=\"/var/run/mdnsd.pid\"
LDFLAGS = -shared
LDSUFFIX = so
JAVACFLAGS_OS = -fPIC -shared -lmdns

# Set up diverging paths for debug vs. prod builds
DEBUG=0
ifeq ($(DEBUG),1)
CFLAGS_DEBUG = -g -DMDNS_DEBUGMSGS=2 
OBJDIR = objects/debug
BUILDDIR = build/debug
STRIP = echo 
else
CFLAGS_DEBUG = -Os -DMDNS_DEBUGMSGS=0 
OBJDIR = objects/prod
BUILDDIR = build/prod
STRIP = strip -S 
endif

# Configure per-OS peculiarities
ifeq ($(os),solaris)
CFLAGS_OS = -DNOT_HAVE_DAEMON -DNOT_HAVE_SA_LEN -D_XPG4_2 -D__EXTENSIONS__ -DHAVE_BROKEN_RECVIF_NAME -lsocket -lnsl
else
ifeq ($(os),linux)
CFLAGS_OS = -DNOT_HAVE_SA_LEN -DUSES_NETLINK
JAVACFLAGS_OS += -I$(JDK)/include/linux
else
ifeq ($(os),netbsd)
CFLAGS_OS =
LDCONFIG = ldconfig
else
ifeq ($(os),freebsd)
# If not already defined, set LOCALBASE to /usr/local
# FreeBSD requires the startup script to end in ".sh"
LOCALBASE?=/usr/local
INSTBASE=$(LOCALBASE)
STARTUPSCRIPTNAME=mdns.sh
CFLAGS_OS =
LDCONFIG = ldconfig
else
ifeq ($(os),openbsd)
CFLAGS_OS = -DHAVE_BROKEN_RECVDSTADDR
LDCONFIG = ldconfig
else
ifeq ($(os),jaguar)
CFLAGS_OS = -DHAVE_IPV6 -no-cpp-precomp -DNOT_HAVE_SOCKLEN_T
LD = libtool
LDFLAGS = -dynamic -lSystem
LDSUFFIX = dylib
JDK = /System/Library/Frameworks/JavaVM.framework/Home
JAVACFLAGS_OS = -dynamiclib -I/System/Library/Frameworks/JavaVM.framework/Headers -framework JavaVM 
else
ifeq ($(os),panther)
CFLAGS_OS = -DHAVE_IPV6 -no-cpp-precomp
LD = libtool
LDFLAGS = -dynamic -lSystem
LDSUFFIX = dylib
JDK = /System/Library/Frameworks/JavaVM.framework/Home
JAVACFLAGS_OS = -dynamiclib -I/System/Library/Frameworks/JavaVM.framework/Headers -framework JavaVM 
else
$(error ERROR: Must specify target OS on command-line: "make os={jaguar,panther,linux,netbsd,freebsd,openbsd,solaris} [target]") 
endif
endif
endif
endif
endif
endif
endif

# If not otherwise defined, we install into /usr/lib and /usr/include
# and our startup script is called mdns (e.g. /etc/init.d/mdns)
INSTBASE?=/usr
STARTUPSCRIPTNAME?=mdns

ifeq ($(HAVE_IPV6),1)
CFLAGS_OS += -DHAVE_IPV6=1
else
ifeq ($(HAVE_IPV6),0)
CFLAGS_OS += -DHAVE_IPV6=0
endif
endif

# If directory /etc/rc.d/init.d/ exists, then we install into that (old Linux)
ifeq ($(wildcard /etc/rc.d/init.d/), /etc/rc.d/init.d/)
STARTUPSCRIPTDIR = /etc/rc.d/init.d
RUNLEVELSCRIPTSDIR = /etc/rc.d
else
# else if directory /etc/init.d/ exists, then we install into that (new Linux)
ifeq ($(wildcard /etc/init.d/), /etc/init.d/)
STARTUPSCRIPTDIR = /etc/init.d
RUNLEVELSCRIPTSDIR = /etc
else
# else install into /etc/rc.d/ (*BSD)
STARTUPSCRIPTDIR = $(INSTBASE)/etc/rc.d
endif
endif

CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_OS) $(CFLAGS_DEBUG)

#############################################################################

all: setup Daemon libmdns Client Responder ProxyResponder Identify NetMonitor

install: setup InstalledDaemon InstalledLib InstalledStartup

# 'setup' sets up the build directory structure the way we want
setup:
	@if test ! -d objects     ; then mkdir objects     ; fi
	@if test ! -d build       ; then mkdir build       ; fi
	@if test ! -d $(OBJDIR)   ; then mkdir $(OBJDIR)   ; fi
	@if test ! -d $(BUILDDIR) ; then mkdir $(BUILDDIR) ; fi

# clean removes targets and objects
clean:
	if test -d $(OBJDIR)   ; then rm -r $(OBJDIR)   ; fi
	if test -d $(BUILDDIR) ; then rm -r $(BUILDDIR) ; fi

#############################################################################

# daemon target builds the daemon
DAEMONOBJS = $(OBJDIR)/PosixDaemon.c.o $(OBJDIR)/mDNSPosix.c.o $(OBJDIR)/mDNSUNP.c.o $(OBJDIR)/mDNS.c.o \
             $(OBJDIR)/DNSDigest.c.o $(OBJDIR)/uDNS.c.o $(OBJDIR)/DNSCommon.c.o $(OBJDIR)/uds_daemon.c.o \
             $(OBJDIR)/mDNSDebug.c.o $(OBJDIR)/dnssd_ipc.c.o $(OBJDIR)/GenLinkedList.c.o 

Daemon: setup $(BUILDDIR)/mdnsd
	@echo "Responder daemon done"

$(BUILDDIR)/mdnsd: $(DAEMONOBJS)
	$(CC) -o $@ $+
	$(STRIP) $@

# libmdns target builds the client library
libmdns: setup $(BUILDDIR)/libmdns.$(LDSUFFIX)
	@echo "Client library done"

$(BUILDDIR)/libmdns.$(LDSUFFIX): $(OBJDIR)/dnssd_clientlib.c.so.o $(OBJDIR)/dnssd_clientstub.c.so.o $(OBJDIR)/dnssd_ipc.c.so.o
	$(LD) $(LDFLAGS) -o $@ $+
	$(STRIP) $@

#############################################################################

# The Install targets place built stuff in their proper places
InstalledDaemon: $(INSTBASE)/sbin/mdnsd
	@echo $< " installed"

InstalledLib: $(INSTBASE)/lib/libmdns.$(LDSUFFIX).$(LIBVERS) $(INSTBASE)/include/dns_sd.h
	@echo $< " installed"

InstalledStartup: $(STARTUPSCRIPTDIR)/$(STARTUPSCRIPTNAME)
	@echo $< " installed"

$(INSTBASE)/sbin/mdnsd: $(BUILDDIR)/mdnsd
	$(CP) $< $@

$(INSTBASE)/lib/libmdns.$(LDSUFFIX).$(LIBVERS): $(BUILDDIR)/libmdns.$(LDSUFFIX)
	$(CP) $< $@
	$(LN) $@ $(INSTBASE)/lib/libmdns.$(LDSUFFIX)
ifdef LDCONFIG
    # -m means 'merge into existing database', -R means 'rescan directories'
	$(LDCONFIG) -mR
endif

$(INSTBASE)/include/dns_sd.h: $(SHAREDDIR)/dns_sd.h
	$(CP) $< $@

$(STARTUPSCRIPTDIR)/$(STARTUPSCRIPTNAME): mdnsd.sh $(STARTUPSCRIPTDIR)
	$(CP) $< $@
	chmod ugo+x $@
ifdef RUNLEVELSCRIPTSDIR
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc2.d/S52mdns
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc3.d/S52mdns
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc4.d/S52mdns
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc5.d/S52mdns
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc0.d/K16mdns
	$(LN) $@ $(RUNLEVELSCRIPTSDIR)/rc6.d/K16mdns
endif

#############################################################################

# The following targets build Java wrappers for the dns-sd.h API.

JAVAC = $(JDK)/bin/javac
JAVAH = $(JDK)/bin/javah
JAVADOC = $(JDK)/bin/javadoc
JAR = $(JDK)/bin/jar
JAVACFLAGS = $(CFLAGS) $(JAVACFLAGS_OS) -I$(JDK)/include

Java: setup $(BUILDDIR)/dns_sd.jar $(BUILDDIR)/libjdns_sd.$(LDSUFFIX)
	@echo "Java wrappers done"

JAVASRC	= $(SHAREDDIR)/Java
JARCONTENTS =	$(OBJDIR)/com/apple/dnssd/DNSSDService.class \
				$(OBJDIR)/com/apple/dnssd/DNSRecord.class \
				$(OBJDIR)/com/apple/dnssd/DNSSDException.class \
				$(OBJDIR)/com/apple/dnssd/TXTRecord.class \
				$(OBJDIR)/com/apple/dnssd/DNSSDRegistration.class \
				$(OBJDIR)/com/apple/dnssd/BaseListener.class \
				$(OBJDIR)/com/apple/dnssd/BrowseListener.class \
				$(OBJDIR)/com/apple/dnssd/ResolveListener.class \
				$(OBJDIR)/com/apple/dnssd/RegisterListener.class \
				$(OBJDIR)/com/apple/dnssd/QueryListener.class \
				$(OBJDIR)/com/apple/dnssd/DomainListener.class \
				$(OBJDIR)/com/apple/dnssd/DNSSD.class

$(BUILDDIR)/dns_sd.jar: $(JARCONTENTS)
	$(JAR) -cf $@ -C $(OBJDIR) com

$(BUILDDIR)/libjdns_sd.$(LDSUFFIX): $(JAVASRC)/JNISupport.c $(OBJDIR)/DNSSD.java.h
	$(CC) -o $@ $< $(JAVACFLAGS) -I$(OBJDIR)

$(OBJDIR)/com/apple/dnssd/%.class:	$(JAVASRC)/%.java
	$(JAVAC) -d $(OBJDIR) -classpath $(OBJDIR) $<

$(OBJDIR)/DNSSD.java.h: $(OBJDIR)/com/apple/dnssd/DNSSD.class
	$(JAVAH) -force -classpath $(OBJDIR) -o $@ \
		com.apple.dnssd.AppleDNSSD \
		com.apple.dnssd.AppleBrowser \
		com.apple.dnssd.AppleResolver \
		com.apple.dnssd.AppleRegistration \
		com.apple.dnssd.AppleQuery \
		com.apple.dnssd.AppleDomainEnum \
		com.apple.dnssd.AppleService 

#############################################################################

# The following target builds documentation for the Java wrappers.

JavaDoc: setup Java
	$(JAVADOC) $(JAVASRC)/*.java -classpath $(OBJDIR) -d $(BUILDDIR) -public

#############################################################################

# The following targets build embedded example programs
SPECIALOBJ = $(OBJDIR)/mDNSPosix.c.o $(OBJDIR)/mDNSUNP.c.o $(OBJDIR)/mDNSDebug.c.o $(OBJDIR)/GenLinkedList.c.o $(OBJDIR)/DNSDigest.c.o $(OBJDIR)/uDNS.c.o $(OBJDIR)/DNSCommon.c.o
COMMONOBJ  = $(SPECIALOBJ) $(OBJDIR)/mDNS.c.o
APPOBJ     = $(COMMONOBJ) $(OBJDIR)/ExampleClientApp.c.o

Client: setup $(BUILDDIR)/mDNSClientPosix
	@echo "Embedded Standalone Client done"

Responder: setup $(BUILDDIR)/mDNSResponderPosix
	@echo "Embedded Standalone Responder done"

ProxyResponder: setup $(BUILDDIR)/mDNSProxyResponderPosix
	@echo "Embedded Standalone ProxyResponder done"

Identify: setup $(BUILDDIR)/mDNSIdentify
	@echo "Identify done"

NetMonitor: setup $(BUILDDIR)/mDNSNetMonitor
	@echo "NetMonitor done"

$(BUILDDIR)/mDNSClientPosix:         $(APPOBJ)     $(OBJDIR)/Client.c.o
	$(CC) $+ -o $@

$(BUILDDIR)/mDNSResponderPosix:      $(COMMONOBJ)  $(OBJDIR)/Responder.c.o
	$(CC) $+ -o $@

$(BUILDDIR)/mDNSProxyResponderPosix: $(COMMONOBJ)  $(OBJDIR)/ProxyResponder.c.o
	$(CC) $+ -o $@

$(BUILDDIR)/mDNSIdentify:            $(SPECIALOBJ) $(OBJDIR)/Identify.c.o
	$(CC) $+ -o $@

$(BUILDDIR)/mDNSNetMonitor:          $(SPECIALOBJ) $(OBJDIR)/NetMonitor.c.o
	$(CC) $+ -o $@

#############################################################################

# Implicit rules
$(OBJDIR)/%.c.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.o:	$(COREDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.o:	$(SHAREDDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.so.o:	%.c
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

$(OBJDIR)/%.c.so.o:	$(SHAREDDIR)/%.c
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<
