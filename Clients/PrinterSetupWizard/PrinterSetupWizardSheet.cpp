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
    
$Log: PrinterSetupWizardSheet.cpp,v $
Revision 1.22  2005/01/25 18:49:43  shersche
Get icon resources from resource DLL

Revision 1.21  2005/01/10 01:09:32  shersche
Use the "note" key to populate pLocation field when setting up printer

Revision 1.20  2005/01/03 19:05:01  shersche
Store pointer to instance of wizard sheet so that print driver install thread sends a window message to the correct window

Revision 1.19  2004/12/31 07:23:53  shersche
Don't modify the button setting in SetSelectedPrinter()

Revision 1.18  2004/12/29 18:53:38  shersche
<rdar://problem/3725106>
<rdar://problem/3737413> Added support for LPR and IPP protocols as well as support for obtaining multiple text records. Reorganized and simplified codebase.
Bug #: 3725106, 3737413

Revision 1.17  2004/10/12 18:02:53  shersche
<rdar://problem/3764873> Escape '/', '@', '"' characters in printui command.
Bug #: 3764873

Revision 1.16  2004/09/13 21:27:22  shersche
<rdar://problem/3796483> Pass the moreComing flag to OnAddPrinter and OnRemovePrinter callbacks
Bug #: 3796483

Revision 1.15  2004/09/11 05:59:06  shersche
<rdar://problem/3785766> Fix code that generates unique printer names based on currently installed printers
Bug #: 3785766

Revision 1.14  2004/09/02 01:57:58  cheshire
<rdar://problem/3783611> Fix incorrect testing of MoreComing flag

Revision 1.13  2004/07/26 21:06:29  shersche
<rdar://problem/3739200> Removing trailing '.' in hostname
Bug #: 3739200

Revision 1.12  2004/07/13 21:24:23  rpantos
Fix for <rdar://problem/3701120>.

Revision 1.11  2004/06/28 00:51:47  shersche
Move call to EnumPrinters out of browse callback into standalone function

Revision 1.10  2004/06/27 23:06:47  shersche
code cleanup, make sure EnumPrinters returns non-zero value

Revision 1.9  2004/06/27 15:49:31  shersche
clean up some cruft in the printer browsing code

Revision 1.8  2004/06/27 08:04:51  shersche
copy selected printer to prevent printer being deleted out from under

Revision 1.7  2004/06/26 23:27:12  shersche
support for installing multiple printers of the same name

Revision 1.6  2004/06/26 21:22:39  shersche
handle spaces in file names

Revision 1.5  2004/06/26 03:19:57  shersche
clean up warning messages

Submitted by: herscher

Revision 1.4  2004/06/25 02:26:52  shersche
Normalize key fields in text record entries
Submitted by: herscher

Revision 1.3  2004/06/24 20:12:07  shersche
Clean up source code
Submitted by: herscher

Revision 1.2  2004/06/23 17:58:21  shersche
<rdar://problem/3701837> eliminated memory leaks on exit
<rdar://problem/3701926> installation of a printer that is already installed results in a no-op
Bug #: 3701837, 3701926
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:57  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "CommonServices.h"
#include "DebugServices.h"
#include "WinServices.h"
#include "About.h"
#include <winspool.h>
#include <tcpxcv.h>
#include <string>

// unreachable code
#pragma warning(disable:4702)


#if( !TARGET_OS_WINDOWS_CE )
#	include	<mswsock.h>
#	include	<process.h>
#endif

// Private Messages

#define WM_PROCESS_EVENT	( WM_USER + 0x100 )


// CPrinterSetupWizardSheet
CPrinterSetupWizardSheet * CPrinterSetupWizardSheet::m_self;

IMPLEMENT_DYNAMIC(CPrinterSetupWizardSheet, CPropertySheet)
CPrinterSetupWizardSheet::CPrinterSetupWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_selectedPrinter(NULL),
	m_driverThreadExitCode( 0 ),
	m_driverThreadFinished( false )
{
	m_arrow		=	LoadCursor(0, IDC_ARROW);
	m_wait		=	LoadCursor(0, IDC_APPSTARTING);
	m_active	=	m_arrow;
	m_self		=	this;
	
	Init();
}


CPrinterSetupWizardSheet::~CPrinterSetupWizardSheet()
{
	if ( m_selectedPrinter != NULL )
	{
		delete m_selectedPrinter;
		m_selectedPrinter = NULL;
	}

	m_self = NULL;
}


// ------------------------------------------------------
// SetSelectedPrinter
//
// Manages setting a printer as the printer to install.  Stops
// any pending resolves.  
//	
void
CPrinterSetupWizardSheet::SetSelectedPrinter(Printer * printer)
{
	check( !printer || ( printer != m_selectedPrinter ) );

	m_selectedPrinter = printer;
}


// ------------------------------------------------------
// InstallPrinter
//
// Installs a printer with Windows.
//
// NOTE: this works one of two ways, depending on whether
// there are drivers already installed for this printer.
// If there are, then we can just create a port with XcvData,
// and then call AddPrinter.  If not, we use the printui.dll
// to install the printer. Actually installing drivers that
// are not currently installed is painful, and it's much
// easier and less error prone to just let printui.dll do
// the hard work for us.
//	

OSStatus
CPrinterSetupWizardSheet::InstallPrinter(Printer * printer)
{
	Service	*	service;
	BOOL		ok;
	OSStatus	err;

	service = printer->services.front();
	check( service );

	//
	// if the driver isn't installed, then install it
	//
	if ( !printer->driverInstalled )
	{
		DWORD		dwResult;
		HANDLE		hThread;
		unsigned	threadID;

		m_driverThreadFinished = false;
	
		//
		// create the thread
		//
		hThread = (HANDLE) _beginthreadex_compat( NULL, 0, InstallDriverThread, printer, 0, &threadID );
		err = translate_errno( hThread, (OSStatus) GetLastError(), kUnknownErr );
		require_noerr( err, exit );
			
		//
		// go modal
		//
		while (!m_driverThreadFinished)
		{
			MSG msg;
	
			GetMessage( &msg, m_hWnd, 0, 0 );
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
		//
		// Wait until child process exits.
		//
		dwResult = WaitForSingleObject( hThread, INFINITE );
		err = translate_errno( dwResult == WAIT_OBJECT_0, errno_compat(), err = kUnknownErr );
		require_noerr( err, exit );

		//
		// check the return value of thread
		//
		require_noerr( m_driverThreadExitCode, exit );

		//
		// now we know that the driver was successfully installed
		//
		printer->driverInstalled = true;
	}

	if ( service->type == kPDLServiceType )
	{
		err = InstallPrinterPDLAndLPR( printer, service, PROTOCOL_RAWTCP_TYPE );
		require_noerr( err, exit );
	}
	else if ( service->type == kLPRServiceType )
	{
		err = InstallPrinterPDLAndLPR( printer, service, PROTOCOL_LPR_TYPE );
		require_noerr( err, exit );
	}
	else if ( service->type == kIPPServiceType )
	{
		err = InstallPrinterIPP( printer, service );
		require_noerr( err, exit );
	}
	else
	{
		err = kUnknownErr;
		require_noerr( err, exit );
	}

	printer->installed = true;

	//
	// if the user specified a default printer, set it
	//
	if (printer->deflt)
	{
		ok = SetDefaultPrinter( printer->actualName );
		err = translate_errno( ok, errno_compat(), err = kUnknownErr );
		require_noerr( err, exit );
	}

exit:

	return err;
}


OSStatus
CPrinterSetupWizardSheet::InstallPrinterPDLAndLPR(Printer * printer, Service * service, DWORD protocol )
{
	PRINTER_DEFAULTS	printerDefaults =	{ NULL,  NULL, SERVER_ACCESS_ADMINISTER };
	DWORD				dwStatus;
	DWORD				cbInputData		=	100;
	PBYTE				pOutputData		=	NULL;
	DWORD				cbOutputNeeded	=	0;
	PORT_DATA_1			portData;
	PRINTER_INFO_2		pInfo;
	HANDLE				hXcv			=	NULL;
	HANDLE				hPrinter		=	NULL;
	Queue			*	q;
	BOOL				ok;
	OSStatus			err;

	check(printer != NULL);
	check(printer->installed == false);

	q = service->queues.front();
	check( q );

	ok = OpenPrinter(L",XcvMonitor Standard TCP/IP Port", &hXcv, &printerDefaults);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );
	
	//
	// BUGBUG: MSDN said this is not required, but my experience shows it is required
	//
	try
	{
		pOutputData = new BYTE[cbInputData];
	}
	catch (...)
	{
		pOutputData = NULL;
	}

	require_action( pOutputData, exit, err = kNoMemoryErr );
	
	//
	// setup the port
	//
	ZeroMemory(&portData, sizeof(PORT_DATA_1));
	wcscpy(portData.sztPortName, printer->portName);
    	
	portData.dwPortNumber	=	service->portNumber;
	portData.dwVersion		=	1;
    	
	portData.dwProtocol	= protocol;
	portData.cbSize		= sizeof PORT_DATA_1;
	portData.dwReserved	= 0L;
    	
	wcscpy(portData.sztQueue, q->name);
	wcscpy(portData.sztIPAddress, service->hostname); 
	wcscpy(portData.sztHostAddress, service->hostname);
		
	ok = XcvData(hXcv, L"AddPort", (PBYTE) &portData, sizeof(PORT_DATA_1), pOutputData, cbInputData,  &cbOutputNeeded, &dwStatus);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	//
	// add the printer
	//
	ZeroMemory(&pInfo, sizeof(pInfo));
		
	pInfo.pPrinterName			=	printer->actualName.GetBuffer();
	pInfo.pServerName			=	NULL;
	pInfo.pShareName			=	NULL;
	pInfo.pPortName				=	printer->portName.GetBuffer();
	pInfo.pDriverName			=	printer->model.GetBuffer();
	pInfo.pComment				=	printer->model.GetBuffer();
	pInfo.pLocation				=	service->location.GetBuffer();
	pInfo.pDevMode				=	NULL;
	pInfo.pDevMode				=	NULL;
	pInfo.pSepFile				=	L"";
	pInfo.pPrintProcessor		=	L"winprint";
	pInfo.pDatatype				=	L"RAW";
	pInfo.pParameters			=	L"";
	pInfo.pSecurityDescriptor	=	NULL;
	pInfo.Attributes			=	PRINTER_ATTRIBUTE_QUEUED;
	pInfo.Priority				=	0;
	pInfo.DefaultPriority		=	0;
	pInfo.StartTime				=	0;
	pInfo.UntilTime				=	0;

	hPrinter = AddPrinter(NULL, 2, (LPBYTE) &pInfo);
	err = translate_errno( hPrinter, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

exit:

	if (hPrinter != NULL)
	{
		ClosePrinter(hPrinter);
	}

	if (hXcv != NULL)
	{
		ClosePrinter(hXcv);
	}

	if (pOutputData != NULL)
	{
		delete [] pOutputData;
	}

	return err;
}


OSStatus
CPrinterSetupWizardSheet::InstallPrinterIPP(Printer * printer, Service * service)
{
	DEBUG_UNUSED( service );

	HANDLE			hPrinter = NULL;
	PRINTER_INFO_2	pInfo;
	OSStatus		err;
	
	//
	// add the printer
	//
	ZeroMemory(&pInfo, sizeof(PRINTER_INFO_2));
	
	pInfo.pPrinterName		= printer->actualName.GetBuffer();
	pInfo.pPortName			= printer->portName.GetBuffer();
	pInfo.pDriverName		= printer->model.GetBuffer();
	pInfo.pPrintProcessor	= L"winprint";
	pInfo.pLocation			= service->location.GetBuffer();
	pInfo.Attributes		= PRINTER_ATTRIBUTE_NETWORK | PRINTER_ATTRIBUTE_LOCAL;
	
	hPrinter = AddPrinter(NULL, 2, (LPBYTE)&pInfo);
	err = translate_errno( hPrinter, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

exit:

	if ( hPrinter != NULL )
	{
		ClosePrinter(hPrinter);
	}

	return err;
}


BEGIN_MESSAGE_MAP(CPrinterSetupWizardSheet, CPropertySheet)
ON_MESSAGE( WM_PROCESS_EVENT, OnProcessEvent )
ON_WM_SETCURSOR()
ON_WM_TIMER()
END_MESSAGE_MAP()


// ------------------------------------------------------
// OnCommand
//
// Traps when the user hits Finish  
//	
BOOL CPrinterSetupWizardSheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//
	// Check if this is OK
	//
	if (wParam == ID_WIZFINISH)              // If OK is hit...
	{
		OnOK();
	}
 
	return CPropertySheet::OnCommand(wParam, lParam);
}


// ------------------------------------------------------
// OnInitDialog
//
// Initializes this Dialog object.
//	
BOOL CPrinterSetupWizardSheet::OnInitDialog()
{
	CPropertySheet::OnInitDialog();

	return TRUE;
}


// ------------------------------------------------------
// OnSetCursor
//
// This is called when Windows wants to know what cursor
// to display.  So we tell it.  
//	
BOOL
CPrinterSetupWizardSheet::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	DEBUG_UNUSED(pWnd);
	DEBUG_UNUSED(nHitTest);
	DEBUG_UNUSED(message);

	SetCursor(m_active);
	return TRUE;
}


// ------------------------------------------------------
// OnContextMenu
//
// This is not fully implemented yet.  
//	

void
CPrinterSetupWizardSheet::OnContextMenu(CWnd * pWnd, CPoint pos)
{
	DEBUG_UNUSED(pWnd);
	DEBUG_UNUSED(pos);

	CAbout dlg;

	dlg.DoModal();
}


// ------------------------------------------------------
// OnOK
//
// This is called when the user hits the "Finish" button  
//	
void
CPrinterSetupWizardSheet::OnOK()
{
	check ( m_selectedPrinter != NULL );

	SetWizardButtons( PSWIZB_DISABLEDFINISH );

	if ( InstallPrinter( m_selectedPrinter ) != kNoErr )
	{
		CString caption;
		CString message;

		caption.LoadString(IDS_INSTALL_ERROR_CAPTION);
		message.LoadString(IDS_INSTALL_ERROR_MESSAGE);

		MessageBox(message, caption, MB_OK|MB_ICONEXCLAMATION);
	}
}


// CPrinterSetupWizardSheet message handlers

void CPrinterSetupWizardSheet::Init(void)
{
	AddPage(&m_pgFirst);
	AddPage(&m_pgSecond);
	AddPage(&m_pgThird);
	AddPage(&m_pgFourth);

	m_psh.dwFlags &= (~PSH_HASHELP);

	m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	m_psh.pszbmWatermark = MAKEINTRESOURCE(IDB_WATERMARK);
	m_psh.pszbmHeader = MAKEINTRESOURCE(IDB_BANNER_ICON);

	m_psh.hInstance = GetNonLocalizedResources();

	SetWizardMode();
}


LONG
CPrinterSetupWizardSheet::OnProcessEvent(WPARAM inWParam, LPARAM inLParam)
{
	DEBUG_UNUSED(inLParam);

	m_driverThreadExitCode	=	(DWORD) inWParam;
	m_driverThreadFinished	=	true;

	return 0;
}


unsigned WINAPI
CPrinterSetupWizardSheet::InstallDriverThread( LPVOID inParam )
{	
	Printer			*	printer = (Printer*) inParam;
	DWORD				exitCode = 0;
	DWORD				dwResult;
	OSStatus			err;
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	BOOL				ok;

	check( printer );
	check( m_self );

	//
	// because we're calling endthreadex(), C++ objects won't be cleaned up
	// correctly.  we'll nest the CString 'command' inside a block so
	// that it's destructor will be invoked.
	//
	{
		CString command;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		command.Format(L"rundll32.exe printui.dll,PrintUIEntry /ia /m \"%s\" /f \"%s\"", (LPCTSTR) printer->model, (LPCTSTR) printer->infFileName );

		ok = CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );

		dwResult = WaitForSingleObject( pi.hProcess, INFINITE );
		translate_errno( dwResult == WAIT_OBJECT_0, errno_compat(), err = kUnknownErr );
		require_noerr( err, exit );

		ok = GetExitCodeProcess( pi.hProcess, &exitCode );
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );
	}

exit:

	//
	// Close process and thread handles. 
	//
	if ( pi.hProcess )
	{
		CloseHandle( pi.hProcess );
	}

	if ( pi.hThread )
	{
		CloseHandle( pi.hThread );
	}

	//
	// alert the main thread
	//
	m_self->PostMessage( WM_PROCESS_EVENT, err, exitCode );

	_endthreadex_compat( 0 );

	return 0;
}
