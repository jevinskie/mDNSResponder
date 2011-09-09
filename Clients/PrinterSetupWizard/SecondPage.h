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
    
$Log: SecondPage.h,v $
Revision 1.6  2005/01/04 21:09:14  shersche
Fix problems in parsing text records. Fix problems in remove event handling. Ensure that the same service can't be resolved more than once.

Revision 1.5  2004/12/31 07:25:27  shersche
Tidy up printer management, and fix memory leaks when hitting 'Cancel'

Revision 1.4  2004/12/30 01:02:46  shersche
<rdar://problem/3734478> Add Printer information box that displays description and location information when printer name is selected
Bug #: 3734478

Revision 1.3  2004/12/29 18:53:38  shersche
<rdar://problem/3725106>
<rdar://problem/3737413> Added support for LPR and IPP protocols as well as support for obtaining multiple text records. Reorganized and simplified codebase.
Bug #: 3725106, 3737413

Revision 1.2  2004/09/13 21:23:42  shersche
<rdar://problem/3796483> Add moreComing argument to OnAddPrinter and OnRemovePrinter callbacks
Bug #: 3796483

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#pragma once

#include "PrinterSetupWizardSheet.h"
#include "CommonServices.h"
#include "UtilTypes.h"
#include "afxcmn.h"
#include "dns_sd.h"
#include "afxwin.h"
#include <map>

using namespace PrinterSetupWizard;

// CSecondPage dialog

class CSecondPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSecondPage)

public:
	CSecondPage();
	virtual ~CSecondPage();

// Dialog Data
	enum { IDD = IDD_SECOND_PAGE };

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

	static void DNSSD_API
	OnQuery(
		DNSServiceRef			inRef, 
		DNSServiceFlags			inFlags, 
		uint32_t				inInterfaceIndex, 
		DNSServiceErrorType		inErrorCode,
		const char			*	inFullName, 
		uint16_t				inRRType, 
		uint16_t				inRRClass, 
		uint16_t				inRDLen, 
		const void			*	inRData, 
		uint32_t				inTTL, 
		void				*	inContext);

protected:

	void		 InitBrowseList();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message);
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

	DECLARE_MESSAGE_MAP()

public:

	HTREEITEM		m_emptyListItem;
	bool			m_selectOkay;
	CTreeCtrl		m_browseList;
	DNSServiceRef	m_resolver;
	bool			m_initialized;
	bool			m_waiting;
	
	LONG			OnServiceEvent(WPARAM inWParam, LPARAM inLParam);
	afx_msg void	OnTvnSelchangedBrowseList(NMHDR *pNMHDR, LRESULT *pResult);

private:

	OSStatus
	LoadPrinterNames();

	Printer*
	Lookup( const char * name );

	OSStatus
	StartOperation( DNSServiceRef ref );

	OSStatus
	StopOperation( DNSServiceRef & ref );

	OSStatus
	StartBrowse();

	OSStatus
	StopBrowse();

	OSStatus
	StartResolve( Printer * printer );

	OSStatus
	StartResolve( Service * service );

	OSStatus
	StopResolve( Printer * printer );

	OSStatus
	StopResolve( Service * service );

	OSStatus
	OnAddPrinter(
			uint32_t		inInterfaceIndex,
			const char *	inName,	
			const char *	inType,	
			const char *	inDomain,
			bool			moreComing);

	OSStatus
	OnRemovePrinter(
			const char *	inName,	
			const char *	inType,	
			const char *	inDomain,
			bool			moreComing);

	void
	OnResolveService( Service * service );

	static bool
	OrderServiceFunc( const Service * a, const Service * b );

	static bool
	OrderQueueFunc( const Queue * q1, const Queue * q2 );

	void
	LoadTextAndDisableWindow( CString & text );
	
	void
	SetPrinterInformationState( BOOL state );

	OSStatus
	ParseTextRecord( Service * service, uint16_t inTXTSize, const char * inTXT, bool & qtotalDefined, CString & qname, uint32_t & qpriority );

	typedef std::map<CString,CString>	PrinterNameMap;
	typedef std::list<DNSServiceRef>	ServiceRefList;
	typedef std::list<Printer*>			Printers;

	
	PrinterNameMap	m_printerNames;
	Printers		m_printers;
	ServiceRefList	m_serviceRefList;
	DNSServiceRef	m_pdlBrowser;
	DNSServiceRef	m_lprBrowser;
	DNSServiceRef	m_ippBrowser;

	Printer		*	m_selected;
	std::string		m_selectedName;

private:

	CStatic m_printerInformation;
	CStatic m_descriptionLabel;
	CStatic m_descriptionField;
	CStatic m_locationLabel;
	CStatic m_locationField;
};
