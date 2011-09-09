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
    
$Log: PrinterSetupWizardSheet.h,v $
Revision 1.4  2004/07/13 21:24:23  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.3  2004/06/28 00:51:47  shersche
Move call to EnumPrinters out of browse callback into standalone function

Revision 1.2  2004/06/24 20:12:07  shersche
Clean up source code
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#pragma once


#include "firstpage.h"
#include "secondpage.h"
#include "thirdpage.h"
#include "fourthpage.h"
#include "UtilTypes.h"
#include "dns_sd.h"
#include <stdexcept>
#include <map>

using namespace PrinterSetupWizard;

// CPrinterSetupWizardSheet

class CPrinterSetupWizardSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPrinterSetupWizardSheet)

public:

	struct WizardException
	{
		CString	text;
		CString	caption;
	};

public:

	CPrinterSetupWizardSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CPrinterSetupWizardSheet();
	
	int
	InstallEventHandler(EventHandler * handler);

	int
	RemoveEventHandler(EventHandler * handler);

	OSStatus
	SetSelectedPrinter(Printer * printer);

	Printer*
	GetSelectedPrinter();

	OSStatus
	LoadPrinterDriver(const CString & filename);

	HCURSOR
	GetCursor();

	//
	// handles socket events for DNSService operations
	//
	virtual LONG
	OnServiceEvent(WPARAM inWParam, LPARAM inLParam);

	//
	// handles end of process event
	//
	virtual LONG
	OnProcessEvent(WPARAM inWParam, LPARAM inLParam);

	virtual BOOL
	OnCommand(WPARAM wParam, LPARAM lParam);

	virtual BOOL
	OnInitDialog();

	virtual BOOL
	OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);

	virtual void
	OnContextMenu(CWnd * pWnd, CPoint pos);

	afx_msg void
	OnOK();

protected:
	DECLARE_MESSAGE_MAP()
	CFirstPage		m_pgFirst;
	CSecondPage		m_pgSecond;
	CThirdPage		m_pgThird;
	CFourthPage		m_pgFourth;

	static void DNSSD_API
	OnBrowse(
		DNSServiceRef 			inRef,
		DNSServiceFlags 		inFlags,
		uint32_t 				inInterfaceIndex,
		DNSServiceErrorType 	inErrorCode,
		const char *			inName,	
		const char *			inType,	
		const char *			inDomain,	
		void *					inContext );

	static void DNSSD_API
	OnResolve(
		DNSServiceRef			inRef,
		DNSServiceFlags			inFlags,
		uint32_t				inInterfaceIndex,
		DNSServiceErrorType		inErrorCode,
		const char *			inFullName,	
		const char *			inHostName, 
		uint16_t 				inPort,
		uint16_t 				inTXTSize,
		const char *			inTXT,
		void *					inContext );

	void Init(void);

private:

	OSStatus
	LoadPrinterNames();

	OSStatus
	StartResolve(Printer * printer);

	OSStatus
	StopResolve(Printer * printer);

	Printer*
	LookUp(const char * inName);

	OSStatus
	InstallPrinter(Printer * printer);

	static unsigned WINAPI
	InstallPrinterThread( LPVOID inParam );


	typedef std::list<Printer*>			PrinterList;
	typedef std::list<EventHandler*>	EventHandlerList;
	typedef std::list<DNSServiceRef>	ServiceRefList;
	typedef std::map<CString,CString>	PrinterNameMap;

	Printer	*	m_selectedPrinter;
	
	PrinterNameMap			m_printerNames;
	PrinterList				m_printerList;
	EventHandlerList		m_eventHandlerList;
	ServiceRefList			m_serviceRefList;

	bool					m_processFinished;

	HCURSOR					m_active;
	HCURSOR					m_arrow;
	HCURSOR					m_wait;

	DNSServiceRef			m_pdlBrowser;
};


inline Printer*
CPrinterSetupWizardSheet::GetSelectedPrinter()
{	
	return m_selectedPrinter;
}


inline HCURSOR
CPrinterSetupWizardSheet::GetCursor()
{
	return m_active;
}
