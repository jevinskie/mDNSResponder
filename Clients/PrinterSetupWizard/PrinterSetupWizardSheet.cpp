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

#define WM_SERVICE_EVENT				( WM_USER + 0x100 )
#define WM_PROCESS_EVENT				( WM_USER + 0x101 )

// Service Types

#define	kPDLDataStreamServiceType		"_pdl-datastream._tcp"
#define kLPRServiceType					"_printer._tcp"
#define kIPPServiceType					"_ipp._tcp"


// CPrinterSetupWizardSheet

IMPLEMENT_DYNAMIC(CPrinterSetupWizardSheet, CPropertySheet)
CPrinterSetupWizardSheet::CPrinterSetupWizardSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_selectedPrinter(NULL)
{
	m_arrow		=	LoadCursor(0, IDC_ARROW);
	m_wait		=	LoadCursor(0, IDC_APPSTARTING);
	m_active	=	m_arrow;
	
	Init();
}


CPrinterSetupWizardSheet::~CPrinterSetupWizardSheet()
{
	//
	// rdar://problem/3701837 memory leaks
	//
	// Clean up the ServiceRef and printer list on exit
	//
	if (m_pdlBrowser != NULL)
	{
		DNSServiceRefDeallocate(m_pdlBrowser);
		m_pdlBrowser = NULL;
	}

	while (m_printerList.size() > 0)
	{
		Printer * printer = m_printerList.front();

		m_printerList.pop_front();

		delete printer;
	}

	if (m_selectedPrinter != NULL)
	{
		delete m_selectedPrinter;
	}
}


// ------------------------------------------------------
// InstallEventHandler
//
// Installs an event handler for DNSService events.  
//	
int
CPrinterSetupWizardSheet::InstallEventHandler(EventHandler * handler)
{
	PrinterList::iterator iter;
	
	m_eventHandlerList.push_back(handler);

	iter = m_printerList.begin();

	while (iter != m_printerList.end())
	{
		Printer * printer = *iter++;

		handler->OnAddPrinter(printer, iter != m_printerList.end());
	}
	
	return kNoErr;
}


// ------------------------------------------------------
// RemoveEventHandler
//
// Removes an event handler for DNSService events.  
//	
int
CPrinterSetupWizardSheet::RemoveEventHandler(EventHandler * handler)
{
	m_eventHandlerList.remove(handler);

	return kNoErr;
}



// ------------------------------------------------------
// SetSelectedPrinter
//
// Manages setting a printer as the printer to install.  Stops
// any pending resolves.  
//	
OSStatus
CPrinterSetupWizardSheet::SetSelectedPrinter(Printer * printer)
{
	OSStatus err;

	//
	// we only want one resolve going on at a time, so we check
	// state of the m_selectedPrinter
	//
	if (m_selectedPrinter != NULL)
	{
		//
		// if we're currently resolving, then stop the resolve
		//
		if (m_selectedPrinter->serviceRef)
		{
			err = StopResolve(m_selectedPrinter);
			require_noerr(err, exit);
		}

		delete m_selectedPrinter;
		m_selectedPrinter = NULL;
	}

	check( m_selectedPrinter == NULL );

	try
	{
		m_selectedPrinter = new Printer;
	}
	catch (...)
	{
		m_selectedPrinter = NULL;
	}

	require_action( m_selectedPrinter, exit, err = E_OUTOFMEMORY );

	m_selectedPrinter->window		=	printer->window;
	m_selectedPrinter->serviceRef	=	NULL;
	m_selectedPrinter->item			=	NULL;
	m_selectedPrinter->ifi			=	printer->ifi;
	m_selectedPrinter->name			=	printer->name;
	m_selectedPrinter->displayName	=	printer->displayName;
	m_selectedPrinter->actualName	=	printer->actualName;
	m_selectedPrinter->type			=	printer->type;
	m_selectedPrinter->domain		=	printer->domain;
	m_selectedPrinter->installed	=	printer->installed;
	m_selectedPrinter->deflt		=	printer->deflt;
	m_selectedPrinter->refs			=	1;
			
	err = StartResolve(m_selectedPrinter);
	require_noerr(err, exit);

exit:

	return err;
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
	PRINTER_DEFAULTS	printerDefaults =	{ NULL,  NULL, SERVER_ACCESS_ADMINISTER };
	DWORD				dwStatus;
	DWORD				cbInputData		=	100;
	PBYTE				pOutputData		=	NULL;
	DWORD				cbOutputNeeded	=	0;
	PORT_DATA_1			portData;
	HANDLE				hXcv			=	NULL;
	HANDLE				hPrinter		=	NULL;
	BOOL				ok;
	OSStatus			err;

	check(printer != NULL);
	check(printer->installed == false);

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
    	
	portData.dwPortNumber	=	printer->portNumber;
	portData.dwVersion		=	1;
    	
	portData.dwProtocol	= PROTOCOL_RAWTCP_TYPE;
	portData.cbSize		= sizeof PORT_DATA_1;
	portData.dwReserved	= 0L;
    	
	wcscpy(portData.sztQueue, printer->hostname);
	wcscpy(portData.sztIPAddress, printer->hostname); 
	wcscpy(portData.sztHostAddress, printer->hostname);
		
	ok = XcvData(hXcv, L"AddPort", (PBYTE) &portData, sizeof(PORT_DATA_1), pOutputData, cbInputData,  &cbOutputNeeded, &dwStatus);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );
	
	if (printer->driverInstalled)
	{
		PRINTER_INFO_2 pInfo;
	
		ZeroMemory(&pInfo, sizeof(pInfo));
			
		pInfo.pPrinterName			=	printer->actualName.GetBuffer();
		pInfo.pServerName			=	NULL;
		pInfo.pShareName			=	NULL;
		pInfo.pPortName				=	printer->portName.GetBuffer();
		pInfo.pDriverName			=	printer->model.GetBuffer();
		pInfo.pComment				=	printer->model.GetBuffer();
		pInfo.pLocation				=	L"";
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
	}
	else
	{
		DWORD		dwResult;
		HANDLE		hThread;
		unsigned	threadID;
			
	
		m_processFinished = false;
	
		//
		// create the thread
		//
		hThread = (HANDLE) _beginthreadex_compat( NULL, 0, InstallPrinterThread, printer, 0, &threadID );
		err = translate_errno( hThread, (OSStatus) GetLastError(), kUnknownErr );
		require_noerr( err, exit );
			
		//
		// go modal
		//
		while (!m_processFinished)
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
	}

	printer->installed = true;

	//
	// if the user specified a default printer, set it
	//
	if (printer->deflt)
	{
		ok = SetDefaultPrinter(printer->actualName);
		err = translate_errno( ok, errno_compat(), err = kUnknownErr );
		require_noerr( err, exit );
	}

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


BEGIN_MESSAGE_MAP(CPrinterSetupWizardSheet, CPropertySheet)
ON_MESSAGE( WM_SERVICE_EVENT, OnServiceEvent )
ON_MESSAGE( WM_PROCESS_EVENT, OnProcessEvent )
ON_WM_SETCURSOR()
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
// Initializes this Dialog object.  We start the browse here,
// so that printers show up instantly when the user clicks
// the next button.  
//	
BOOL CPrinterSetupWizardSheet::OnInitDialog()
{
	OSStatus err;

	CPropertySheet::OnInitDialog();
	
	//
	// setup the DNS-SD browsing
	//
	err = DNSServiceBrowse( &m_pdlBrowser, 0, 0, kPDLDataStreamServiceType, NULL, OnBrowse, this );
	require_noerr( err, exit );

	m_serviceRefList.push_back(m_pdlBrowser);

	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(m_pdlBrowser), m_hWnd, WM_SERVICE_EVENT, FD_READ|FD_CLOSE);
	require_noerr( err, exit );

	LoadPrinterNames();

exit:

	if (err != kNoErr)
	{
		WizardException exc;

		exc.text.LoadString(IDS_NO_MDNSRESPONDER_SERVICE_TEXT);
		exc.caption.LoadString(IDS_NO_MDNSRESPONDER_SERVICE_CAPTION);

		throw(exc);
	}

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

	m_psh.hInstance = AfxGetInstanceHandle();

	SetWizardMode();
}


LONG
CPrinterSetupWizardSheet::OnServiceEvent(WPARAM inWParam, LPARAM inLParam)
{
	if (WSAGETSELECTERROR(inLParam) && !(HIWORD(inLParam)))
    {
		dlog( kDebugLevelError, "OnServiceEvent: window error\n" );
    }
    else
    {
		SOCKET sock = (SOCKET) inWParam;

		// iterate thru list
		ServiceRefList::iterator begin = m_serviceRefList.begin();
		ServiceRefList::iterator end   = m_serviceRefList.end();

		while (begin != end)
		{
			DNSServiceRef ref = *begin++;

			check(ref != NULL);

			if ((SOCKET) DNSServiceRefSockFD(ref) == sock)
			{
				DNSServiceProcessResult(ref);
				break;
			}
		}
	}

	return ( 0 );
}


LONG
CPrinterSetupWizardSheet::OnProcessEvent(WPARAM inWParam, LPARAM inLParam)
{
	DEBUG_UNUSED(inWParam);
	DEBUG_UNUSED(inLParam);

	m_processFinished = true;

	return 0;
}


void DNSSD_API
CPrinterSetupWizardSheet::OnBrowse(
								DNSServiceRef 			inRef,
								DNSServiceFlags 		inFlags,
								uint32_t 				inInterfaceIndex,
								DNSServiceErrorType 	inErrorCode,
								const char *			inName,	
								const char *			inType,	
								const char *			inDomain,	
								void *					inContext )
{
	DEBUG_UNUSED(inRef);

	CPrinterSetupWizardSheet	*	self;
	Printer						*	printer;
	EventHandlerList::iterator		it;
	DWORD							printerNameCount;
	bool							moreComing = (bool) (inFlags & kDNSServiceFlagsMoreComing);

	require_noerr( inErrorCode, exit );
	
	self = reinterpret_cast <CPrinterSetupWizardSheet*>( inContext );
	require_quiet( self, exit );

	printer = self->LookUp(inName);

	if (inFlags & kDNSServiceFlagsAdd)
	{
		OSStatus err;

		if (printer != NULL)
		{
			printer->refs++;
		}
		else
		{
			try
			{
				printer = new Printer;
			}
			catch (...)
			{
				printer = NULL;
			}

			require_action( printer, exit, err = E_OUTOFMEMORY );

			printer->window		=	self;
			printer->ifi		=	inInterfaceIndex;
			printer->name		=	inName;
			err = UTF8StringToStringObject(inName, printer->displayName);
			check_noerr( err );
			printer->actualName	=	printer->displayName;

			//
			// Compare this name against printers that are already installed
			// to avoid name clashes.  Rename as necessary
			// to come up with a unique name.
			//
			printerNameCount = 2;

			for (;;)
			{
				PrinterNameMap::iterator it;

				it = self->m_printerNames.find(printer->actualName);

				if (it != self->m_printerNames.end())
				{
					printer->actualName.Format(L"%s (%d)", printer->displayName, printerNameCount);
				}
				else
				{
					break;
				}

				printerNameCount++;
			}

			printer->type		=	inType;
			printer->domain		=	inDomain;
			printer->installed	=	false;
			printer->deflt		=	false;
			printer->refs		=	1;
			
			self->m_printerList.push_back( printer );

			//
			// now invoke event handlers for AddPrinter event
			//
			for (it = self->m_eventHandlerList.begin(); it != self->m_eventHandlerList.end(); it++)
			{
				EventHandler * handler = *it;

				handler->OnAddPrinter(printer, moreComing);
			}
		}
	}
	else
	{
		if ((printer != NULL) && (--printer->refs == 0))
		{
			//
			// now invoke event handlers for RemovePrinter event
			//
			for (it = self->m_eventHandlerList.begin(); it != self->m_eventHandlerList.end(); it++)
			{
				EventHandler * handler = *it;

				handler->OnRemovePrinter(printer, moreComing);
			}

			self->m_printerList.remove(printer);

			//
			// check to see if we've selected this printer
			//
			if (self->m_selectedPrinter == printer)
			{
				//
				// this guy is being removed while we're resolving it...so let's 
				// stop the resolve
				//
				if (printer->serviceRef != NULL)
				{
					self->StopResolve(printer);
				}

				self->m_selectedPrinter = NULL;
			}

			delete printer;
		}
	}

exit:

	return;
}


void DNSSD_API
CPrinterSetupWizardSheet::OnResolve(
								DNSServiceRef			inRef,
								DNSServiceFlags			inFlags,
								uint32_t				inInterfaceIndex,
								DNSServiceErrorType		inErrorCode,
								const char *			inFullName,	
								const char *			inHostName, 
								uint16_t 				inPort,
								uint16_t 				inTXTSize,
								const char *			inTXT,
								void *					inContext )
{
	DEBUG_UNUSED(inFullName);
	DEBUG_UNUSED(inInterfaceIndex);
	DEBUG_UNUSED(inFlags);
	DEBUG_UNUSED(inRef);

	Printer						*	printer;
	CPrinterSetupWizardSheet	*	self;
	EventHandlerList::iterator		it1;
	EventHandlerList::iterator		it2;
	int								idx;
	OSStatus						err;

	require_noerr( inErrorCode, exit );

	printer = reinterpret_cast<Printer*>( inContext );
	require_quiet( printer, exit);

	self = printer->window;
	require_quiet( self, exit );

	err = self->StopResolve(printer);
	require_noerr(err, exit);
	
	//
	// hold on to the hostname...
	//
	err = UTF8StringToStringObject( inHostName, printer->hostname );
	require_noerr( err, exit );

	//
	// <rdar://problem/3739200> remove the trailing dot on hostname
	//
	idx = printer->hostname.ReverseFind('.');

	if ((idx > 1) && ((printer->hostname.GetLength() - 1) == idx))
	{
		printer->hostname.Delete(idx, 1);
	}

	//
	// hold on to the port
	//
	printer->portNumber = ntohs(inPort);

	//
	// parse the text record.  we create a stringlist of text record
	// entries that can be interrogated later
	//
	while (inTXTSize)
	{
		char buf[256];

		unsigned char num = *inTXT;
		check( (int) num < inTXTSize );

		memset(buf, 0, sizeof(buf));
		memcpy(buf, inTXT + 1, num);
		
		inTXTSize -= (num + 1);
		inTXT += (num + 1);

		CString elem;

		err = UTF8StringToStringObject( buf, elem );
		require_noerr( err, exit );

		int curPos = 0;

		CString key = elem.Tokenize(L"=", curPos);
		CString val = elem.Tokenize(L"=", curPos);

		key.MakeLower();

		if ((key == L"usb_mfg") || (key == L"usb_manufacturer"))
		{
			printer->usb_MFG = val;
		}
		else if ((key == L"usb_mdl") || (key == L"usb_model"))
		{
			printer->usb_MDL = val;
		}
		else if (key == L"description")
		{
			printer->description = val;
		}
		else if (key == L"product")
		{
			printer->product = val;
		}
	}

	//
	// now invoke event handlers for Resolve event
	//
	it1 = self->m_eventHandlerList.begin();
	it2 = self->m_eventHandlerList.end();

	while (it1 != it2)
	{
		EventHandler * handler = *it1++;

		handler->OnResolvePrinter(printer);
	}

exit:

	return;
}


OSStatus
CPrinterSetupWizardSheet::LoadPrinterNames()
{
	PBYTE		buffer	=	NULL;
	OSStatus	err		= 0;

	//
	// rdar://problem/3701926 - Printer can't be installed twice
	//
	// First thing we want to do is make sure the printer isn't already installed.
	// If the printer name is found, we'll try and rename it until we
	// find a unique name
	//
	DWORD dwNeeded = 0, dwNumPrinters = 0;

	BOOL ok = EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, NULL, 0, &dwNeeded, &dwNumPrinters);
	err = translate_errno( ok, errno_compat(), kUnknownErr );

	if ((err == ERROR_INSUFFICIENT_BUFFER) && (dwNeeded > 0))
	{
		try
		{
			buffer = new unsigned char[dwNeeded];
		}
		catch (...)
		{
			buffer = NULL;
		}
	
		require_action( buffer, exit, kNoMemoryErr );
		ok = EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 4, buffer, dwNeeded, &dwNeeded, &dwNumPrinters);
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );

		for (DWORD index = 0; index < dwNumPrinters; index++)
		{
			PRINTER_INFO_4 * lppi4 = (PRINTER_INFO_4*) (buffer + index * sizeof(PRINTER_INFO_4));

			m_printerNames[lppi4->pPrinterName] = lppi4->pPrinterName;
		}
	}

exit:

	if (buffer != NULL)
	{
		delete [] buffer;
	}

	return err;
}


OSStatus
CPrinterSetupWizardSheet::StartResolve(Printer * printer)
{
	OSStatus err;

	check( printer );

	err = DNSServiceResolve( &printer->serviceRef, 0, 0, printer->name.c_str(), printer->type.c_str(), printer->domain.c_str(), (DNSServiceResolveReply) OnResolve, printer );
	require_noerr( err, exit);

	m_serviceRefList.push_back(printer->serviceRef);

	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(printer->serviceRef), m_hWnd, WM_SERVICE_EVENT, FD_READ|FD_CLOSE);
	require_noerr( err, exit );

	//
	// set the cursor to arrow+hourglass
	//
	m_active = m_wait;
	SetCursor(m_active);

exit:

	return err;
}


OSStatus
CPrinterSetupWizardSheet::StopResolve(Printer * printer)
{
	OSStatus err;

	check( printer );
	check( printer->serviceRef );

	m_serviceRefList.remove( printer->serviceRef );

	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(printer->serviceRef), m_hWnd, 0, 0);
	check(err == 0);	

	DNSServiceRefDeallocate( printer->serviceRef );

	printer->serviceRef = NULL;
	
	//
	// set the cursor back to normal
	//
	m_active = m_arrow;
	SetCursor(m_active);

	return kNoErr;
}


Printer*
CPrinterSetupWizardSheet::LookUp(const char * inName)
{
	PrinterList::iterator it1 = m_printerList.begin();
	PrinterList::iterator it2 = m_printerList.end();

	while (it1 != it2)
	{
		Printer * printer = *it1++;

		if (printer->name == inName)
		{
			return printer;
		}
	}

	return NULL;
}


unsigned WINAPI
CPrinterSetupWizardSheet::InstallPrinterThread( LPVOID inParam )
{
	check( inParam );
		
	Printer			*	printer = (Printer*) inParam;
	CString				actualName;
	CString				command;
	DWORD				exitCode = 0;
	DWORD				dwResult;
	OSStatus			err;
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	BOOL				ok;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	//
	// <rdar://problem/3764873> Escape '\', '@', '"' characters which seem to cause problems for printui
	//

	actualName = printer->actualName;

	actualName.Replace(L"\\", L"\\\\");
	actualName.Replace(L"@", L"\\@");
	actualName.Replace(L"\"", L"\\\"");
	
	command.Format(L"rundll32.exe printui.dll,PrintUIEntry /if /b \"%s\" /f \"%s\" /r \"%s\" /m \"%s\"", (LPCTSTR) actualName, (LPCTSTR) printer->infFileName, (LPCTSTR) printer->portName, (LPCTSTR) printer->model);

	ok = CreateProcess(NULL, command.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	dwResult = WaitForSingleObject( pi.hProcess, INFINITE );
	translate_errno( dwResult == WAIT_OBJECT_0, errno_compat(), err = kUnknownErr );
	require_noerr( err, exit );

	ok = GetExitCodeProcess( pi.hProcess, &exitCode );
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	//
	// Close process and thread handles. 
	//
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

exit:

	//
	// alert the main thread
	//
	printer->window->PostMessage( WM_PROCESS_EVENT, err, exitCode );

	return 0;
}
