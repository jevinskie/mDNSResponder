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

class CSecondPage : public CPropertyPage, public EventHandler
{
	DECLARE_DYNAMIC(CSecondPage)

public:
	CSecondPage();
	virtual ~CSecondPage();

// Dialog Data
	enum { IDD = IDD_SECOND_PAGE };

	virtual void
	OnAddPrinter(
			Printer	*	printer,
			bool			moreComing);

	virtual void
	OnRemovePrinter(
			Printer	*	printer,
			bool			moreComing);

	virtual void
	OnResolvePrinter(
			Printer * printer);

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
	
	afx_msg void OnTvnSelchangedBrowseList(NMHDR *pNMHDR, LRESULT *pResult);
};
