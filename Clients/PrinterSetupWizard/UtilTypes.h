/*
 * Copyright (c) 1997-2004 Apple Computer, Inc. All rights reserved.
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
    
$Log: UtilTypes.h,v $
Revision 1.4  2004/09/13 21:22:44  shersche
<rdar://problem/3796483> Add moreComing argument to OnAddPrinter and OnRemovePrinter callbacks
Bug #: 3796483

Revision 1.3  2004/06/26 23:27:12  shersche
support for installing multiple printers of the same name

Revision 1.2  2004/06/25 02:25:59  shersche
Remove item field from manufacturer and model structures
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:58  rpantos
First checked in


*/

#pragma once

#include <dns_sd.h>
#include <list>

class CPrinterSetupWizardSheet;

namespace PrinterSetupWizard
{
	struct Printer;
	struct Manufacturer;
	struct Model;
	
	typedef std::list<CString>	TextRecord;
	typedef std::list<Model*>	Models;

	struct Printer
	{
		DNSServiceRef					serviceRef;
		CPrinterSetupWizardSheet	*	window;
		HTREEITEM						item;
		DWORD							refs;

		//
		// these are from the browse reply
		//
		uint32_t						ifi;
		std::string						name;
		CString							displayName;
		CString							actualName;
		std::string						type;
		std::string						domain;

		//
		// these are from the resolve
		//
		CString							hostname;
		unsigned short					portNumber;
		CString							usb_MFG;
		CString							usb_MDL;
		CString							description;
		CString							product;

		//
		// these are derived from the printer matching code
		//
		// if driverInstalled is false, then infFileName should
		// have an absolute path to the printers inf file.  this
		// is used to install the printer from printui.dll
		//
		// if driverInstalled is true, then model is the name
		// of the driver to use in AddPrinter
		// 
		bool							driverInstalled;
		CString							infFileName;
		CString							manufacturer;
		CString							model;
		CString							portName;
		bool							deflt;

		//
		// state
		//
		bool							installed;
	};


	struct Manufacturer
	{
		CString		name;
		CString		tag;
		Models		models;
	};


	struct Model
	{
		bool		driverInstalled;
		CString		infFileName;
		CString		name;
	};


	class EventHandler
	{
	public:

		virtual void
		OnAddPrinter(
				Printer	*	printer,
				bool			moreComing) = 0;

		virtual void
		OnRemovePrinter(
				Printer	*	printer,
				bool			moreComing) = 0;

		virtual void
		OnResolvePrinter(
				Printer * printer) = 0;
	};
}
