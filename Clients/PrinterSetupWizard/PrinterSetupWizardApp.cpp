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
    
$Log: PrinterSetupWizardApp.cpp,v $
Revision 1.3  2004/07/13 21:24:23  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.2  2004/06/24 20:12:08  shersche
Clean up source code
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "DebugServices.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPrinterSetupWizardApp

BEGIN_MESSAGE_MAP(CPrinterSetupWizardApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPrinterSetupWizardApp construction

CPrinterSetupWizardApp::CPrinterSetupWizardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPrinterSetupWizardApp object

CPrinterSetupWizardApp theApp;


// CPrinterSetupWizardApp initialization

BOOL CPrinterSetupWizardApp::InitInstance()
{
	//
	// initialize the debugging framework
	//
	debug_initialize( kDebugOutputTypeWindowsDebugger, "PrinterSetupWizard", NULL );
	debug_set_property( kDebugPropertyTagPrintLevel, kDebugLevelTrace );


	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CPrinterSetupWizardSheet dlg(IDS_CAPTION);

	m_pMainWnd = &dlg;

	try
	{
		INT_PTR nResponse = dlg.DoModal();
	
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}
	catch (CPrinterSetupWizardSheet::WizardException & exc)
	{
		MessageBox(NULL, exc.text, exc.caption, MB_OK|MB_ICONEXCLAMATION);
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
