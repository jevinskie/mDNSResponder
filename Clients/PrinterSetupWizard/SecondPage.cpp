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
Revision 1.10  2005/01/20 19:54:38  shersche
Fix parse error when text record is NULL

Revision 1.9  2005/01/06 08:13:50  shersche
Don't use moreComing flag to determine number of text record, disregard queue name if qtotal isn't defined, don't disregard queue name if "rp" is the only key specified

Revision 1.8  2005/01/04 21:09:14  shersche
Fix problems in parsing text records. Fix problems in remove event handling. Ensure that the same service can't be resolved more than once.

Revision 1.7  2004/12/31 07:25:27  shersche
Tidy up printer management, and fix memory leaks when hitting 'Cancel'

Revision 1.6  2004/12/30 01:24:02  shersche
<rdar://problem/3906182> Remove references to description key
Bug #: 3906182

Revision 1.5  2004/12/30 01:02:47  shersche
<rdar://problem/3734478> Add Printer information box that displays description and location information when printer name is selected
Bug #: 3734478

Revision 1.4  2004/12/29 18:53:38  shersche
<rdar://problem/3725106>
<rdar://problem/3737413> Added support for LPR and IPP protocols as well as support for obtaining multiple text records. Reorganized and simplified codebase.
Bug #: 3725106, 3737413

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
#include "WinServices.h"
#include <winspool.h>

// local variable is initialize but not referenced
#pragma warning(disable:4189)

#define WM_SERVICE_EVENT	( WM_USER + 0x101 )

// CSecondPage dialog

IMPLEMENT_DYNAMIC(CSecondPage, CPropertyPage)
CSecondPage::CSecondPage()
	: CPropertyPage(CSecondPage::IDD),
	m_pdlBrowser( NULL ),
	m_lprBrowser( NULL ),
	m_ippBrowser( NULL ),
	m_selected( NULL )
{
	m_psp.dwFlags &= ~(PSP_HASHELP);
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;

	m_psp.pszHeaderTitle	= MAKEINTRESOURCE(IDS_BROWSE_TITLE);
	m_psp.pszHeaderSubTitle	= MAKEINTRESOURCE(IDS_BROWSE_SUBTITLE);

	m_resolver			=	NULL;
	m_emptyListItem		=	NULL;
	m_initialized		=	false;
	m_waiting			=	false;

	LoadPrinterNames();
}


CSecondPage::~CSecondPage()
{
	StopBrowse();
}


OSStatus
CSecondPage::LoadPrinterNames()
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

	LoadTextAndDisableWindow( text );

	//
	// disable the next button until there's a printer to select
	//
	psheet->SetWizardButtons(PSWIZB_BACK);

	//
	// disable the printer information box
	//
	SetPrinterInformationState( FALSE );

exit:

	return;
}


OSStatus
CSecondPage::StartOperation( DNSServiceRef ref )
{
	OSStatus err;

	err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD(ref), m_hWnd, WM_SERVICE_EVENT, FD_READ|FD_CLOSE);
	require_noerr( err, exit );

	m_serviceRefList.push_back( ref );

exit:

	return err;
}


OSStatus
CSecondPage::StopOperation( DNSServiceRef & ref )
{
	OSStatus err = kNoErr;

	if ( ref )
	{
		m_serviceRefList.remove( ref );

		if ( IsWindow( m_hWnd ) )
		{
			err = WSAAsyncSelect((SOCKET) DNSServiceRefSockFD( ref ), m_hWnd, 0, 0 );
			require_noerr( err, exit );
		}

		DNSServiceRefDeallocate( ref );
		ref = NULL;
	}

exit:

	return err;
}


Printer*
CSecondPage::Lookup(const char * inName)
{
	check( IsWindow( m_hWnd ) );
	check( inName );

	HTREEITEM item = m_browseList.GetChildItem( TVI_ROOT );
	while ( item )
	{
		Printer	*	printer;
		DWORD_PTR	data;

		data = m_browseList.GetItemData( item );
		printer = reinterpret_cast<Printer*>(data);

		if ( printer && ( printer->name == inName ) )
		{
			return printer;
		}

		item = m_browseList.GetNextItem( item, TVGN_NEXT );
	}

	return NULL;
}


OSStatus
CSecondPage::StartBrowse()
{
	OSStatus err;

	//
	// setup the DNS-SD browsing
	//
	err = DNSServiceBrowse( &m_pdlBrowser, 0, 0, kPDLServiceType, NULL, OnBrowse, this );
	require_noerr( err, exit );

	err = StartOperation( m_pdlBrowser );
	require_noerr( err, exit );

	err = DNSServiceBrowse( &m_lprBrowser, 0, 0, kLPRServiceType, NULL, OnBrowse, this );
	require_noerr( err, exit );

	err = StartOperation( m_lprBrowser );
	require_noerr( err, exit );

	err = DNSServiceBrowse( &m_ippBrowser, 0, 0, kIPPServiceType, NULL, OnBrowse, this );
	require_noerr( err, exit );

	err = StartOperation( m_ippBrowser );
	require_noerr( err, exit );

exit:

	return err;
}


OSStatus
CSecondPage::StopBrowse()
{
	OSStatus err;

	err = StopOperation( m_pdlBrowser );
	require_noerr( err, exit );

	err = StopOperation( m_lprBrowser );
	require_noerr( err, exit );

	err = StopOperation( m_ippBrowser );
	require_noerr( err, exit );

	while ( m_printers.size() > 0 )
	{
		Printer * printer = m_printers.front();

		m_printers.pop_front();

		if ( printer->resolving )
		{
			StopResolve( printer );
		}

		delete printer;
	}

exit:

	return err;
}


OSStatus
CSecondPage::StartResolve( Printer * printer )
{
	CPrinterSetupWizardSheet	*	psheet;
	OSStatus						err = kNoErr;
	Services::iterator				it;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	check( printer );

	for ( it = printer->services.begin(); it != printer->services.end(); it++ )
	{
		if ( (*it)->serviceRef == NULL )
		{
			err = StartResolve( *it );
			require_noerr( err, exit );
		}
	}

exit:

	return err;
}


OSStatus
CSecondPage::StartResolve( Service * service )
{
	CPrinterSetupWizardSheet	*	psheet;
	OSStatus						err = kNoErr;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	check( service->serviceRef == NULL );

	//
	// clean out any queues that were collected during a previous
	// resolve
	//

	service->EmptyQueues();

	//
	// now start the new resolve
	//

	err = DNSServiceResolve( &service->serviceRef, 0, 0, service->printer->name.c_str(), service->type.c_str(), service->domain.c_str(), (DNSServiceResolveReply) OnResolve, service );
	require_noerr( err, exit );

	err = StartOperation( service->serviceRef );
	require_noerr( err, exit );

	//
	// If we're not currently resolving, then disable the next button
	// and set the cursor to hourglass
	//

	if ( !service->printer->resolving )
	{
		psheet->SetWizardButtons( PSWIZB_BACK );

		psheet->m_active = psheet->m_wait;
		SetCursor(psheet->m_active);
	}

	service->printer->resolving++;

exit:

	return err;
}


OSStatus
CSecondPage::StopResolve(Printer * printer)
{
	OSStatus err = kNoErr;

	check( printer );

	Services::iterator it;

	for ( it = printer->services.begin(); it != printer->services.end(); it++ )
	{
		if ( (*it)->serviceRef )
		{
			err = StopResolve( *it );
			require_noerr( err, exit );
		}
	}

exit:

	return err;
}


OSStatus
CSecondPage::StopResolve( Service * service )
{
	OSStatus err;

	check( service->serviceRef );

	err = StopOperation( service->serviceRef );
	require_noerr( err, exit );

	service->printer->resolving--;

exit:

	return err;
}


void CSecondPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BROWSE_LIST, m_browseList);
	DDX_Control(pDX, IDC_PRINTER_INFORMATION, m_printerInformation);
	DDX_Control(pDX, IDC_DESCRIPTION_LABEL, m_descriptionLabel);
	DDX_Control(pDX, IDC_DESCRIPTION_FIELD, m_descriptionField);
	DDX_Control(pDX, IDC_LOCATION_LABEL, m_locationLabel);
	DDX_Control(pDX, IDC_LOCATION_FIELD, m_locationField);
}


afx_msg BOOL
CSecondPage::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	DEBUG_UNUSED(pWnd);
	DEBUG_UNUSED(nHitTest);
	DEBUG_UNUSED(message);

	CPrinterSetupWizardSheet * psheet;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	SetCursor(psheet->GetCursor());

exit:

	return TRUE;
}


BOOL
CSecondPage::OnSetActive()
{
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;
	OSStatus						err = kNoErr;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_action( psheet, exit, err = kUnknownErr );

	if ( ( printer = psheet->GetSelectedPrinter() ) != NULL )
	{
		psheet->SetSelectedPrinter( NULL );
		delete printer;
	}

	//
	// initialize the browse list...this will remove everything currently
	// in it, and add the no rendezvous printers item
	//
	InitBrowseList();

	//
	// start browing
	//
	err = StartBrowse();
	require_noerr( err, exit );

exit:

	if ( err != kNoErr )
	{
		if ( err == kDNSServiceErr_Firewall )
		{
			CString text, caption;

			text.LoadString( IDS_FIREWALL );
			caption.LoadString( IDS_FIREWALL_CAPTION );

			MessageBox(text, caption, MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			CPrinterSetupWizardSheet::WizardException exc;
			
			exc.text.LoadString( IDS_NO_MDNSRESPONDER_SERVICE_TEXT );
			exc.caption.LoadString( IDS_ERROR_CAPTION );
			
			throw(exc);
		}
	}

	return CPropertyPage::OnSetActive();
}


BOOL
CSecondPage::OnKillActive()
{
	OSStatus err = kNoErr;

	if ( m_selected )
	{
		CPrinterSetupWizardSheet * psheet;

		psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
		require_quiet( psheet, exit );

		psheet->SetSelectedPrinter( m_selected );
		m_printers.remove( m_selected );
		m_selected = NULL;
	}

	err = StopBrowse();
	require_noerr( err, exit );

exit:

	return CPropertyPage::OnKillActive();
}


void DNSSD_API
CSecondPage::OnBrowse(
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

	CSecondPage	*	self;
	bool			moreComing = (bool) (inFlags & kDNSServiceFlagsMoreComing);

	require_noerr( inErrorCode, exit );
	
	self = reinterpret_cast <CSecondPage*>( inContext );
	require_quiet( self, exit );

	if ( inFlags & kDNSServiceFlagsAdd )
	{
		self->OnAddPrinter( inInterfaceIndex, inName, inType, inDomain, moreComing );
	}
	else
	{
		self->OnRemovePrinter( inName, inType, inDomain, moreComing );
	}

exit:
	
	return;
}


void DNSSD_API
CSecondPage::OnResolve(
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

	CSecondPage	*	self;
	Service		*	service;
	Queue		*	q;
	bool			qtotalDefined = false;
	uint32_t		qpriority = kDefaultPriority;
	CString			qname;
	int				idx;
	OSStatus		err;

	require_noerr( inErrorCode, exit );

	service = reinterpret_cast<Service*>( inContext );
	require_quiet( service, exit);

	check( service->refs != 0 );

	self = service->printer->window;
	require_quiet( self, exit );

	err = self->StopOperation( service->serviceRef );
	require_noerr( err, exit );
	
	//
	// hold on to the hostname...
	//
	err = UTF8StringToStringObject( inHostName, service->hostname );
	require_noerr( err, exit );

	//
	// <rdar://problem/3739200> remove the trailing dot on hostname
	//
	idx = service->hostname.ReverseFind('.');

	if ((idx > 1) && ((service->hostname.GetLength() - 1) == idx))
	{
		service->hostname.Delete(idx, 1);
	}

	//
	// hold on to the port
	//
	service->portNumber = ntohs(inPort);

	//
	// parse the text record.
	//

	err = self->ParseTextRecord( service, inTXTSize, inTXT, qtotalDefined, qname, qpriority );
	require_noerr( err, exit );

	if ( service->qtotal == 1 )
	{	
		//
		// create a new queue
		//
		try
		{
			q = new Queue;
		}
		catch (...)
		{
			q = NULL;
		}

		require_action( q, exit, err = E_OUTOFMEMORY );

		if ( qtotalDefined )
		{
			q->name = qname;
		}

		q->priority = qpriority;
		
		service->queues.push_back( q );

		//
		// we've completely resolved this service
		//

		self->OnResolveService( service );
	}
	else
	{
		//
		// if qtotal is more than 1, then we need to get additional
		// text records.  if not, then this service is considered
		// resolved
		//

		err = DNSServiceQueryRecord(&service->serviceRef, 0, inInterfaceIndex, inFullName, kDNSServiceType_TXT, kDNSServiceClass_IN, OnQuery, (void*) service );
		require_noerr( err, exit );

		err = self->StartOperation( service->serviceRef );
		require_noerr( err, exit );
	}

exit:

	return;
}


void DNSSD_API
CSecondPage::OnQuery(
				DNSServiceRef		inRef, 
				DNSServiceFlags		inFlags, 
				uint32_t			inInterfaceIndex, 
				DNSServiceErrorType inErrorCode,
				const char		*	inFullName, 
				uint16_t			inRRType, 
				uint16_t			inRRClass, 
				uint16_t			inRDLen, 
				const void		*	inRData, 
				uint32_t			inTTL, 
				void			*	inContext)
{
	DEBUG_UNUSED( inTTL );
	DEBUG_UNUSED( inRRClass );
	DEBUG_UNUSED( inRRType );
	DEBUG_UNUSED( inFullName );
	DEBUG_UNUSED( inInterfaceIndex );
	DEBUG_UNUSED( inRef );

	Service		*	service = NULL;
	Queue		*	q;
	CSecondPage	*	self;
	bool			qtotalDefined = false;
	bool			moreComing = (bool) (inFlags & kDNSServiceFlagsMoreComing);
	OSStatus		err = kNoErr;

	require_noerr( inErrorCode, exit );

	service = reinterpret_cast<Service*>( inContext );
	require_quiet( service, exit);

	self = service->printer->window;
	require_quiet( self, exit );

	if ( ( inFlags & kDNSServiceFlagsAdd ) && ( inRDLen > 0 ) && ( inRData != NULL ) )
	{
		const char * inTXT = ( const char * ) inRData;

		//
		// create a new queue
		//
		try
		{
			q = new Queue;
		}
		catch (...)
		{
			q = NULL;
		}

		require_action( q, exit, err = E_OUTOFMEMORY );

		err = service->printer->window->ParseTextRecord( service, inRDLen, inTXT, qtotalDefined, q->name, q->priority );
		require_noerr( err, exit );

		if ( !qtotalDefined )
		{
			q->name = L"";
		}

		//
		// add this queue
		//

		service->queues.push_back( q );

		if ( service->queues.size() == service->qtotal )
		{
			//
			// else if moreComing is not set, then we're going
			// to assume that we're done
			//

			self->StopOperation( service->serviceRef );

			//
			// sort the queues
			//

			service->queues.sort( OrderQueueFunc );

			//
			// we've completely resolved this service
			//

			self->OnResolveService( service );
		}
	}

exit:

	if ( err && service && ( service->serviceRef != NULL ) )
	{
		service->printer->window->StopOperation( service->serviceRef );
	}

	return;
}


BEGIN_MESSAGE_MAP(CSecondPage, CPropertyPage)
	ON_MESSAGE( WM_SERVICE_EVENT, OnServiceEvent )
	ON_NOTIFY(TVN_SELCHANGED, IDC_BROWSE_LIST, OnTvnSelchangedBrowseList)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// Printer::EventHandler implementation
OSStatus
CSecondPage::OnAddPrinter(
				uint32_t		inInterfaceIndex,
				const char *	inName,	
				const char *	inType,	
				const char *	inDomain,
				bool			moreComing)
{
	Printer						*	printer;
	Service						*	service;
	CPrinterSetupWizardSheet	*	psheet;
	DWORD							printerNameCount;
	bool							newPrinter = false;
	OSStatus						err = kNoErr;

	check( IsWindow( m_hWnd ) );

	m_browseList.SetRedraw(FALSE);

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	printer = Lookup( inName );

	if (printer == NULL)
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

		printer->window		=	this;
		printer->name		=	inName;
		
		err = UTF8StringToStringObject(inName, printer->displayName);
		check_noerr( err );
		printer->actualName	=	printer->displayName;
		printer->installed	=	false;
		printer->deflt		=	false;
		printer->resolving	=	0;

		//
		// Compare this name against printers that are already installed
		// to avoid name clashes.  Rename as necessary
		// to come up with a unique name.
		//
		printerNameCount = 2;

		for (;;)
		{
			PrinterNameMap::iterator it;

			it = m_printerNames.find(printer->actualName);

			if (it != m_printerNames.end())
			{
				printer->actualName.Format(L"%s (%d)", printer->displayName, printerNameCount);
			}
			else
			{
				break;
			}

			printerNameCount++;
		}

		newPrinter = true;
	}

	check( printer );

	service = printer->LookupService( inType );

	if ( service != NULL )
	{
		service->refs++;
	}
	else
	{
		try
		{
			service = new Service;
		}
		catch (...)
		{
			service = NULL;
		}

		require_action( service, exit, err = E_OUTOFMEMORY );
		
		service->printer	=	printer;
		service->ifi		=	inInterfaceIndex;
		service->type		=	inType;
		service->domain		=	inDomain;
		service->qtotal		=	1;
		service->refs		=	1;
		service->serviceRef	=	NULL;

		printer->services.push_back( service );

		//
		// if the printer is selected, then we'll want to start a
		// resolve on this guy
		//

		if ( m_selected == printer )
		{
			StartResolve( service );
		}
	}
	
	if ( newPrinter )
	{
		printer->item = m_browseList.InsertItem(printer->displayName);

		m_browseList.SetItemData( printer->item, (DWORD_PTR) printer );

		m_printers.push_back( printer );
		
		m_browseList.SortChildren(TVI_ROOT);
		
		if ( printer->name == m_selectedName )
		{
			m_browseList.SelectItem( printer->item );
		}

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
	}

exit:

	if (!moreComing)
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}

	return err;
}


OSStatus
CSecondPage::OnRemovePrinter(
				const char *	inName,	
				const char *	inType,	
				const char *	inDomain,
				bool			moreComing)
{
	DEBUG_UNUSED( inDomain );
	DEBUG_UNUSED( inType );

	Printer	*	printer;
	OSStatus	err = kNoErr;

	check( IsWindow( m_hWnd ) );

	m_browseList.SetRedraw(FALSE);

	printer = Lookup( inName );

	if ( printer )
	{
		Service * service;

		service = printer->LookupService( inType );

		if ( service && ( --service->refs == 0 ) )
		{
			if ( service->serviceRef != NULL )
			{
				err = StopResolve( service );
				require_noerr( err, exit );
			}

			printer->services.remove( service );

			delete service;
		}

		if ( printer->services.size() == 0 )
		{
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

			m_printers.remove( printer );

			if ( m_selected == printer )
			{
				m_selected		= NULL;
				m_selectedName	= "";
			}

			delete printer;
		}
	}

exit:

	if (!moreComing)
	{
		m_browseList.SetRedraw(TRUE);
		m_browseList.Invalidate();
	}

	return err;
}


void
CSecondPage::OnResolveService( Service * service )
{
	CPrinterSetupWizardSheet * psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );
	
	check( service );

	if ( !--service->printer->resolving )
	{
		//
		// sort the services now.  we want the service that
		// has the highest priority queue to be first in
		// the list.
		//

		service->printer->services.sort( OrderServiceFunc );

		//
		// and set it to selected
		//

		m_selected		= service->printer;
		m_selectedName	= service->printer->name;

		//
		// and update the printer information box
		//
		SetPrinterInformationState( TRUE );

		m_descriptionField.SetWindowText( service->description );
		m_locationField.SetWindowText( service->location );

		psheet->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT );

		//
		// reset the cursor
		//

		psheet->m_active = psheet->m_arrow;
		SetCursor(psheet->m_active);
	}

exit:

	return;
}


LONG
CSecondPage::OnServiceEvent(WPARAM inWParam, LPARAM inLParam)
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
	err = StartResolve( printer );
	require_noerr( err, exit );

	//
	// And clear out the printer information box
	//
	SetPrinterInformationState( FALSE );
	m_descriptionField.SetWindowText(L"");
	m_locationField.SetWindowText(L"");

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


bool
CSecondPage::OrderServiceFunc( const Service * a, const Service * b )
{
	Queue * q1, * q2;

	q1 = (a->queues.size() > 0) ? a->queues.front() : NULL;

	q2 = (b->queues.size() > 0) ? b->queues.front() : NULL;

	if ( !q1 && !q2 )
	{
		return true;
	}
	else if ( q1 && !q2 )
	{
		return true;
	}
	else if ( !q1 && q2 )
	{
		return false;
	}
	else if ( q1->priority < q2->priority )
	{
		return true;
	}
	else if ( q1->priority > q2->priority )
	{
		return false;
	}
	else if ( ( a->type == kPDLServiceType ) || ( ( a->type == kLPRServiceType ) && ( b->type == kIPPServiceType ) ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool
CSecondPage::OrderQueueFunc( const Queue * q1, const Queue * q2 )
{
	return ( q1->priority <= q2->priority ) ? true : false;
}


void
CSecondPage::LoadTextAndDisableWindow( CString & text )
{
	m_emptyListItem = m_browseList.InsertItem( text, 0, 0, NULL, TVI_FIRST );
	m_browseList.SelectItem( NULL );

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

	m_browseList.EnableWindow( FALSE );
}


void
CSecondPage::SetPrinterInformationState( BOOL state )
{
	m_printerInformation.EnableWindow( state );
	m_descriptionLabel.EnableWindow( state );
	m_descriptionField.EnableWindow( state );
	m_locationLabel.EnableWindow( state );
	m_locationField.EnableWindow( state );
}


OSStatus
CSecondPage::ParseTextRecord( Service * service, uint16_t inTXTSize, const char * inTXT, bool & qtotalDefined, CString & qname, uint32_t & qpriority )
{
	bool		rpOnly = true;
	OSStatus	err = kNoErr;
	
	while (inTXTSize)
	{
		char buf[256];

		unsigned char num = *inTXT;
		check( (int) num < inTXTSize );

		if ( num )
		{
			memset(buf, 0, sizeof(buf));
			memcpy(buf, inTXT + 1, num);

			CString elem;

			err = UTF8StringToStringObject( buf, elem );
			require_noerr( err, exit );

			int curPos = 0;

			CString key = elem.Tokenize(L"=", curPos);
			CString val = elem.Tokenize(L"=", curPos);

			key.MakeLower();

			if ( key == L"rp" )
			{
				qname = val;
			}
			else
			{
				rpOnly = false;

				if ((key == L"usb_mfg") || (key == L"usb_manufacturer"))
				{
					service->usb_MFG = val;
				}
				else if ((key == L"usb_mdl") || (key == L"usb_model"))
				{
					service->usb_MDL = val;
				}
				else if (key == L"ty")
				{
					service->description = val;
				}
				else if (key == L"product")
				{
					service->product = val;
				}
				else if (key == L"note")
				{
					service->location = val;
				}
				else if (key == L"qtotal")
				{
					service->qtotal = (unsigned short) _ttoi((LPCTSTR) val);
					qtotalDefined = true;
				}
				else if (key == L"priority")
				{
					qpriority = _ttoi((LPCTSTR) val);
				}
			}
		}

		inTXTSize -= (num + 1);
		inTXT += (num + 1);
	}

exit:

	if ( rpOnly )
	{
		qtotalDefined = true;
	}

	return err;
}


