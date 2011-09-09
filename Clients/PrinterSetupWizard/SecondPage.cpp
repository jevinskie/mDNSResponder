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
    
$Log: SecondPage.cpp,v $
Revision 1.3  2004/09/13 21:26:15  shersche
<rdar://problem/3796483> Use the moreComing flag to determine whether drawing should take place in OnAddPrinter and OnRemovePrinter callbacks
Bug #: 3796483

Revision 1.2  2004/06/26 03:19:57  shersche
clean up warning messages

Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "SecondPage.h"
#include "DebugServices.h"

// local variable is initialize but not referenced
#pragma warning(disable:4189)


// CSecondPage dialog

IMPLEMENT_DYNAMIC(CSecondPage, CPropertyPage)
CSecondPage::CSecondPage()
	: CPropertyPage(CSecondPage::IDD)
{
	m_psp.dwFlags &= ~(PSP_HASHELP);
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;

	m_psp.pszHeaderTitle	= MAKEINTRESOURCE(IDS_BROWSE_TITLE);
	m_psp.pszHeaderSubTitle	= MAKEINTRESOURCE(IDS_BROWSE_SUBTITLE);

	m_resolver			=	NULL;
	m_emptyListItem		=	NULL;
	m_initialized		=	false;
	m_waiting			=	false;
}

CSecondPage::~CSecondPage()
{
}


void
CSecondPage::InitBrowseList()
{
	CPrinterSetupWizardSheet	*	psheet;
	CString							text;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	//
	// load the no rendezvous printers message until something shows up in the browse list
	//
	text.LoadString(IDS_NO_RENDEZVOUS_PRINTERS);

	m_emptyListItem = m_browseList.InsertItem( text, 0, 0, NULL, TVI_FIRST );

	//
	// this will remove everything else in the list...we might be navigating
	// back to this window, and the browse list might have changed since
	// we last displayed it.
	//
	if ( m_emptyListItem )
	{
		HTREEITEM item = m_browseList.GetNextVisibleItem( m_emptyListItem );
  
		while ( item )
		{
			m_browseList.DeleteItem( item );
			item = m_browseList.GetNextVisibleItem( m_emptyListItem );
		}
	}

	//
	// disable the next button until there's a printer to select
	//
	psheet->SetWizardButtons(PSWIZB_BACK);

	//
	// disable the window until there's a printer to select
	//
	m_browseList.EnableWindow( FALSE );

exit:

	return;
}


void CSecondPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BROWSE_LIST, m_browseList);
}


afx_msg BOOL
CSecondPage::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	DEBUG_UNUSED(pWnd);
	DEBUG_UNUSED(nHitTest);
	DEBUG_UNUSED(message);

	CPrinterSetupWizardSheet	*	psheet;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	SetCursor(psheet->GetCursor());

exit:

	return TRUE;
}


BOOL
CSecondPage::OnSetActive()
{
	CString							noPrinters;
	CPrinterSetupWizardSheet	*	psheet;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );
  
	//
	// initialize the browse list...this will remove everything currently
	// in it, and add the no rendezvous printers item
	//
	InitBrowseList();

	//
	// this will invoke OnAddPrinter for all the printers that we have
	// browsed
	//
	psheet->InstallEventHandler(this);

exit:

	return CPropertyPage::OnSetActive();
}


BOOL
CSecondPage::OnKillActive()
{
	CPrinterSetupWizardSheet * psheet;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	//
	// we don't want our event handlers called when we don't have
	// anywhere to put the data
	//
	psheet->RemoveEventHandler(this);

exit:

	return CPropertyPage::OnKillActive();
}


BEGIN_MESSAGE_MAP(CSecondPage, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BROWSE_LIST, OnTvnSelchangedBrowseList)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// Printer::EventHandler implementation
void
CSecondPage::OnAddPrinter(
						Printer	*	printer,
						bool			moreComing)
{
	check( IsWindow( m_hWnd ) );

	m_browseList.SetRedraw(FALSE);

	printer->item = m_browseList.InsertItem(printer->displayName);

	m_browseList.SetItemData( printer->item, (DWORD_PTR) printer );
	
	m_browseList.SortChildren(TVI_ROOT);

	//
	// if the searching item is still in the list
	// get rid of it
	//
	// note that order is important here.  Insert the printer
	// item before removing the placeholder so we always have
	// an item in the list to avoid experiencing the bug
	// in Microsoft's implementation of CTreeCtrl
	//
	if (m_emptyListItem != NULL)
	{
		m_browseList.DeleteItem(m_emptyListItem);
		m_emptyListItem = NULL;
		m_browseList.EnableWindow(TRUE);
	}

	if (!moreComing)
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}
}


void
CSecondPage::OnRemovePrinter(
						Printer	*	printer,
						bool			moreComing)
{
	check( IsWindow( m_hWnd ) );

	m_browseList.SetRedraw(FALSE);

	//
	// check to make sure if we're the only item in the control...i.e.
	// the list size is 1.
	//
	if (m_browseList.GetCount() > 1)
	{
		//
		// if we're not the only thing in the list, then
		// simply remove it from the list
		//
		m_browseList.DeleteItem( printer->item );
	}
	else
	{
		//
		// if we're the only thing in the list, then redisplay
		// it with the no rendezvous printers message
		//
		InitBrowseList();
	}

	if (!moreComing)
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}
}


void
CSecondPage::OnResolvePrinter(
						Printer * printer)
{
	DEBUG_UNUSED(printer);

	check( IsWindow( m_hWnd ) );

	CPrinterSetupWizardSheet * psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );
   
	//
	// setup the sheet to enable the next button if we've successfully
	// resolved
	//
	psheet->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT );

exit:

	return;
}


void CSecondPage::OnTvnSelchangedBrowseList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW					pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CPrinterSetupWizardSheet	*	psheet;
	int								err = 0;

	HTREEITEM item = m_browseList.GetSelectedItem();
	require_quiet( item, exit );

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_action( psheet, exit, err = kUnknownErr );	

	Printer * printer;

	printer = reinterpret_cast<Printer*>(m_browseList.GetItemData( item ) );
	require_quiet( printer, exit );

	//
	// this call will trigger a resolve.  When the resolve is complete,
	// our OnResolve will be called.
	//
	err = psheet->SetSelectedPrinter(printer);
	require_noerr( err, exit );

	//
	// setup the sheet to disable the next button until we've successfully
	// resolved this printer
	//
	psheet->SetWizardButtons( PSWIZB_BACK );

exit:

	if (err != 0)
	{
		CString text;
		CString caption;

		text.LoadString(IDS_ERROR_SELECTING_PRINTER_TEXT);
		caption.LoadString(IDS_ERROR_SELECTING_PRINTER_CAPTION);

		MessageBox(text, caption, MB_OK|MB_ICONEXCLAMATION);
	}

	*pResult = 0;
}
