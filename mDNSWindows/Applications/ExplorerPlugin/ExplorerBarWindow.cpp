/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
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
    
$Log: ExplorerBarWindow.cpp,v $
Revision 1.5  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.4  2004/04/09 21:03:15  bradley
Changed port numbers to use network byte order for consistency with other platforms.

Revision 1.3  2004/04/08 09:43:43  bradley
Changed callback calling conventions to __stdcall so they can be used with C# delegates.

Revision 1.2  2004/02/21 04:36:19  bradley
Enable dot local name lookups now that the NSP is being installed.

Revision 1.1  2004/01/30 03:01:56  bradley
Explorer Plugin to browse for Rendezvous-enabled Web and FTP servers from within Internet Explorer.

*/

#include	"StdAfx.h"

#include	"CommonServices.h"
#include	"DebugServices.h"
#include	"DNSSD.h"

#include	"ExplorerBar.h"
#include	"LoginDialog.h"
#include	"Resource.h"

#include	"ExplorerBarWindow.h"

// MFC Debugging

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0
#pragma mark == Constants ==
#endif

//===========================================================================================================================
//	Constants
//===========================================================================================================================

// Control IDs

#define	IDC_EXPLORER_TREE				1234

// Private Messages

#define	WM_PRIVATE_SERVICE_ADD			( WM_USER + 0x100 )
#define	WM_PRIVATE_SERVICE_REMOVE		( WM_USER + 0x101 )
#define	WM_PRIVATE_RESOLVE				( WM_USER + 0x102 )

// TXT records

#define	kTXTRecordKeyPath				"path="
#define	kTXTRecordKeyPathSize			sizeof_string( kTXTRecordKeyPath )

#if 0
#pragma mark == Prototypes ==
#endif

//===========================================================================================================================
//	Prototypes
//===========================================================================================================================

DEBUG_LOCAL int			FindServiceArrayIndex( const ServiceInfoArray &inArray, const ServiceInfo &inService, int &outIndex );
DEBUG_LOCAL OSStatus	UTF8StringToStringObject( const char *inUTF8, CString &inObject );

#if 0
#pragma mark == Message Map ==
#endif

//===========================================================================================================================
//	Message Map
//===========================================================================================================================

BEGIN_MESSAGE_MAP( ExplorerBarWindow, CWnd )
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY( NM_DBLCLK, IDC_EXPLORER_TREE, OnDoubleClick )
	ON_MESSAGE( WM_PRIVATE_SERVICE_ADD, OnServiceAdd )
	ON_MESSAGE( WM_PRIVATE_SERVICE_REMOVE, OnServiceRemove )
	ON_MESSAGE( WM_PRIVATE_RESOLVE, OnResolve )
END_MESSAGE_MAP()

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	ExplorerBarWindow
//===========================================================================================================================

ExplorerBarWindow::ExplorerBarWindow( void )
{
	mOwner				= NULL;
	mResolveServiceRef	= NULL;
}

//===========================================================================================================================
//	~ExplorerBarWindow
//===========================================================================================================================

ExplorerBarWindow::~ExplorerBarWindow( void )
{
	//
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	OnCreate
//===========================================================================================================================

int	ExplorerBarWindow::OnCreate( LPCREATESTRUCT inCreateStruct )
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	
	OSStatus		err;
	CRect			rect;
	CString			s;
	
	err = CWnd::OnCreate( inCreateStruct );
	require_noerr( err, exit );
	
	GetClientRect( rect );
	mTree.Create( WS_TABSTOP | WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES, rect, this, 
		IDC_EXPLORER_TREE );
	
	err = DNSServiceInitialize( kDNSServiceInitializeFlagsNone, 0 );
	if( err != kNoErr )
	{
		// Cannot talk to the mDNSResponder service. Show the error message and exit (with kNoErr so they can see it).
		
		s.LoadString( IDS_RENDEZVOUS_NOT_AVAILABLE );
		mTree.InsertItem( s, 0, 0, TVI_ROOT, TVI_LAST );
		err = kNoErr;
		goto exit;
	}
	
	ServiceHandlerEntry *		e;
	
	// Web Site Handler
	
	e = new ServiceHandlerEntry;
	check( e );
	e->type				= "_http._tcp";
	e->urlScheme		= "http://";
	e->ref				= NULL;
	e->treeItem			= NULL;
	e->treeFirst		= true;
	e->obj				= this;
	e->needsLogin		= false;
	mServiceHandlers.Add( e );
	
	s.LoadString( IDS_WEB_SITES );
	e->treeItem = mTree.InsertItem( s, 0, 0 );
	mTree.Expand( e->treeItem, TVE_EXPAND );
	
	err = DNSServiceBrowse( &e->ref, 0, 0, e->type, NULL, BrowseCallBack, e );
	require_noerr( err, exit );

	// FTP Site Handler
	
	e = new ServiceHandlerEntry;
	check( e );
	e->type				= "_ftp._tcp";
	e->urlScheme		= "ftp://";
	e->ref				= NULL;
	e->treeItem			= NULL;
	e->treeFirst		= true;
	e->obj				= this;
	e->needsLogin		= true;
	mServiceHandlers.Add( e );
	
	s.LoadString( IDS_FTP_SITES );
	e->treeItem = mTree.InsertItem( s, 0, 0 );
	mTree.Expand( e->treeItem, TVE_EXPAND );
	
	err = DNSServiceBrowse( &e->ref, 0, 0, e->type, NULL, BrowseCallBack, e );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	OnDestroy
//===========================================================================================================================

void	ExplorerBarWindow::OnDestroy( void ) 
{
	// Stop any resolves that may still be pending (shouldn't be any).
	
	StopResolve();
	
	// Clean up the service handlers.
	
	int		i;
	int		n;
	
	n = (int) mServiceHandlers.GetSize();
	for( i = 0; i < n; ++i )
	{
		delete mServiceHandlers[ i ];
	}
	
	DNSServiceFinalize();
	CWnd::OnDestroy();
}

//===========================================================================================================================
//	OnSize
//===========================================================================================================================

void	ExplorerBarWindow::OnSize( UINT inType, int inX, int inY ) 
{
	CWnd::OnSize( inType, inX, inY );
	mTree.MoveWindow( 0, 0, inX, inY );
}

//===========================================================================================================================
//	OnDoubleClick
//===========================================================================================================================

void	ExplorerBarWindow::OnDoubleClick( NMHDR *inNMHDR, LRESULT *outResult )
{
	HTREEITEM			item;
	ServiceInfo *		service;
	OSStatus			err;
	
	DEBUG_UNUSED( inNMHDR );
	
	item = mTree.GetSelectedItem();
	require( item, exit );
	
	service = reinterpret_cast < ServiceInfo * > ( mTree.GetItemData( item ) );
	require_quiet( service, exit );
	
	err = StartResolve( service );
	require_noerr( err, exit );

exit:
	*outResult = 0;
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	BrowseCallBack
//===========================================================================================================================

void CALLBACK_COMPAT
	ExplorerBarWindow::BrowseCallBack(
		DNSServiceRef 			inRef,
		DNSServiceFlags 		inFlags,
		uint32_t 				inInterfaceIndex,
		DNSServiceErrorType 	inErrorCode,
		const char *			inName,	
		const char *			inType,	
		const char *			inDomain,	
		void *					inContext )
{
	ServiceHandlerEntry *		obj;
	ServiceInfo *				service;
	OSStatus					err;
	
	DEBUG_UNUSED( inRef );
	
	service = NULL;
	
	require_noerr( inErrorCode, exit );
	obj = reinterpret_cast < ServiceHandlerEntry * > ( inContext );
	check( obj );
	check( obj->obj );
	
	try
	{
		// Post a message to the main thread so it can handle it since MFC is not thread safe.
		
		service = new ServiceInfo;
		require_action( service, exit, err = kNoMemoryErr );
		
		err = UTF8StringToStringObject( inName, service->displayName );
		check_noerr( err );
		
		service->name = strdup( inName );
		require_action( service->name, exit, err = kNoMemoryErr );
		
		service->type = strdup( inType );
		require_action( service->type, exit, err = kNoMemoryErr );
		
		service->domain = strdup( inDomain );
		require_action( service->domain, exit, err = kNoMemoryErr );
		
		service->ifi 		= inInterfaceIndex;
		service->handler	= obj;
		
		DWORD		message;
		BOOL		ok;
		
		message = ( inFlags & kDNSServiceFlagsAdd ) ? WM_PRIVATE_SERVICE_ADD : WM_PRIVATE_SERVICE_REMOVE;
		ok = ::PostMessage( obj->obj->GetSafeHwnd(), message, 0, (LPARAM) service );
		check( ok );
		if( ok )
		{
			service = NULL;
		}
	}
	catch( ... )
	{
		dlog( kDebugLevelError, "BrowseCallBack: exception thrown\n" );
	}
	
exit:
	if( service )
	{
		delete service;
	}
}

//===========================================================================================================================
//	OnServiceAdd
//===========================================================================================================================

LONG	ExplorerBarWindow::OnServiceAdd( WPARAM inWParam, LPARAM inLParam )
{
	ServiceInfo *				service;
	ServiceHandlerEntry *		handler;
	int							cmp;
	int							index;
	
	DEBUG_UNUSED( inWParam );
	
	service = reinterpret_cast < ServiceInfo * > ( inLParam );
	check( service );
	handler = service->handler; 
	check( handler );
	
	cmp = FindServiceArrayIndex( handler->array, *service, index );
	if( cmp == 0 )
	{
		// Found a match so update the item. The index is index + 1 so subtract 1.
		
		index -= 1;
		check( index < handler->array.GetSize() );
		
		service->item = handler->array[ index ]->item;
		delete handler->array[ index ];
		handler->array[ index ] = service;
		mTree.SetItemText( service->item, service->displayName );
		mTree.SetItemData( service->item, (DWORD_PTR) service );
	}
	else
	{
		HTREEITEM		afterItem;
		
		// Insert the new item in sorted order.
		
		afterItem = ( index > 0 ) ? handler->array[ index - 1 ]->item : TVI_FIRST;
		handler->array.InsertAt( index, service );
		service->item = mTree.InsertItem( service->displayName, handler->treeItem, afterItem );
		mTree.SetItemData( service->item, (DWORD_PTR) service );
		
		// Make sure the item is visible if this is the first time a service was added.
	
		if( handler->treeFirst )
		{
			handler->treeFirst = false;
			mTree.EnsureVisible( service->item );
		}
	}
	return( 0 );
}

//===========================================================================================================================
//	OnServiceRemove
//===========================================================================================================================

LONG	ExplorerBarWindow::OnServiceRemove( WPARAM inWParam, LPARAM inLParam )
{
	ServiceInfo *				service;
	ServiceHandlerEntry *		handler;
	int							cmp;
	int							index;
	
	DEBUG_UNUSED( inWParam );
	
	service = reinterpret_cast < ServiceInfo * > ( inLParam );
	check( service );
	handler = service->handler; 
	check( handler );
	
	// Search to see if we know about this service instance. If so, remove it from the list.
	
	cmp = FindServiceArrayIndex( handler->array, *service, index );
	check( cmp == 0 );
	if( cmp == 0 )
	{
		// Found a match remove the item. The index is index + 1 so subtract 1.
		
		index -= 1;
		check( index < handler->array.GetSize() );
		
		mTree.DeleteItem( handler->array[ index ]->item );
		delete handler->array[ index ];
		handler->array.RemoveAt( index );
	}
	delete service;
	return( 0 );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	StartResolve
//===========================================================================================================================

OSStatus	ExplorerBarWindow::StartResolve( ServiceInfo *inService )
{
	OSStatus		err;
	
	check( inService );
	
	// Stop any current resolve that may be in progress.
	
	StopResolve();
	
	// Resolve the service.
	
	err = DNSServiceResolve( &mResolveServiceRef, kDNSServiceFlagsNone, inService->ifi, 
		inService->name, inService->type, inService->domain, ResolveCallBack, inService->handler );
	require_noerr( err, exit );
	
exit:
	return( err );
}

//===========================================================================================================================
//	StopResolve
//===========================================================================================================================

void	ExplorerBarWindow::StopResolve( void )
{
	if( mResolveServiceRef )
	{
		DNSServiceRefDeallocate( mResolveServiceRef );
		mResolveServiceRef = NULL;
	}
}

//===========================================================================================================================
//	ResolveCallBack
//===========================================================================================================================

void CALLBACK_COMPAT
	ExplorerBarWindow::ResolveCallBack(
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
	ExplorerBarWindow *			obj;
	ServiceHandlerEntry *		handler;
	OSStatus					err;
	
	DEBUG_UNUSED( inRef );
	DEBUG_UNUSED( inFlags );
	DEBUG_UNUSED( inErrorCode );
	DEBUG_UNUSED( inFullName );
	
	require_noerr( inErrorCode, exit );
	handler = (ServiceHandlerEntry *) inContext;
	check( handler );
	obj = handler->obj;
	check( obj );
	
	try
	{
		ResolveInfo *		resolve;
		BOOL				ok;
		
		dlog( kDebugLevelNotice, "resolved %s on ifi %d to %s\n", inFullName, inInterfaceIndex, inHostName );
		
		// Stop resolving after the first good result.
		
		obj->StopResolve();
		
		// Post a message to the main thread so it can handle it since MFC is not thread safe.
		
		resolve = new ResolveInfo;
		require_action( resolve, exit, err = kNoMemoryErr );
		
		UTF8StringToStringObject( inHostName, resolve->host );
		resolve->port		= ntohs( inPort );
		resolve->ifi		= inInterfaceIndex;
		resolve->handler	= handler;
		
		err = resolve->txt.SetData( inTXT, inTXTSize );
		check_noerr( err );
		
		ok = ::PostMessage( obj->GetSafeHwnd(), WM_PRIVATE_RESOLVE, 0, (LPARAM) resolve );
		check( ok );
		if( !ok )
		{
			delete resolve;
		}
	}
	catch( ... )
	{
		dlog( kDebugLevelError, "ResolveCallBack: exception thrown\n" );
	}

exit:
	return;
}

//===========================================================================================================================
//	OnResolve
//===========================================================================================================================

LONG	ExplorerBarWindow::OnResolve( WPARAM inWParam, LPARAM inLParam )
{
	ResolveInfo *		resolve;
	CString				url;
	uint8_t *			path;
	size_t				pathSize;
	char *				pathPrefix;
	CString				username;
	CString				password;
	
	DEBUG_UNUSED( inWParam );
	
	resolve = reinterpret_cast < ResolveInfo * > ( inLParam );
	check( resolve );
		
	// Get login info if needed.
	
	if( resolve->handler->needsLogin )
	{
		LoginDialog		dialog;
		
		if( !dialog.GetLogin( username, password ) )
		{
			goto exit;
		}
	}
	
	// If the HTTP TXT record is a "path=" entry, use it as the resource path. Otherwise, use "/".
	
	pathPrefix = "";
	if( strcmp( resolve->handler->type, "_http._tcp" ) == 0 )
	{
		resolve->txt.GetData( &path, &pathSize );
		if( pathSize > 0 )
		{
			pathSize = *path++;
		}
		if( ( pathSize > kTXTRecordKeyPathSize ) && ( memicmp( path, kTXTRecordKeyPath, kTXTRecordKeyPathSize ) == 0 )  )
		{
			path 		+= kTXTRecordKeyPathSize;
			pathSize	-= kTXTRecordKeyPathSize;
		}
		else if( pathSize == 0 )
		{
			path 	 	= (uint8_t *) "/";
			pathSize	= 1;
		}
		if( *path != '/' )
		{
			pathPrefix = "/";
		}
	}
	else
	{
		path			= (uint8_t *) "";
		pathSize		= 1;
	}

	// Build the URL in the following format:
	//
	// <urlScheme>[<username>[:<password>]@]<name/ip>[<path>]

	url.AppendFormat( TEXT( "%S" ), resolve->handler->urlScheme );					// URL Scheme
	if( username.GetLength() > 0 )
	{
		url.AppendFormat( TEXT( "%s" ), username );									// Username
		if( password.GetLength() > 0 )
		{
			url.AppendFormat( TEXT( ":%s" ), password );							// Password
		}
		url.AppendFormat( TEXT( "@" ) );
	}
	
	url += resolve->host;															// Host
	url.AppendFormat( TEXT( ":%d" ), resolve->port );								// :Port
	url.AppendFormat( TEXT( "%S" ), pathPrefix );									// Path Prefix ("/" or empty).
	url.AppendFormat( TEXT( "%.*S" ), (int) pathSize, (char *) path );				// Path (possibly empty).
	
	// Tell Internet Explorer to go to the URL.
	
	check( mOwner );
	mOwner->GoToURL( url );

exit:
	delete resolve;
	return( 0 );
}

#if 0
#pragma mark -
#endif

//===========================================================================================================================
//	FindServiceArrayIndex
//===========================================================================================================================

DEBUG_LOCAL int	FindServiceArrayIndex( const ServiceInfoArray &inArray, const ServiceInfo &inService, int &outIndex )
{
	int		result;
	int		lo;
	int		hi;
	int		mid;
	
	result 	= -1;
	mid		= 0;
	lo 		= 0;
	hi 		= (int)( inArray.GetSize() - 1 );
	while( lo <= hi )
	{
		mid = ( lo + hi ) / 2;
		result = inService.displayName.CompareNoCase( inArray[ mid ]->displayName );
		if( result == 0 )
		{
			result = ( (int) inService.ifi ) - ( (int) inArray[ mid ]->ifi );
		}
		if( result == 0 )
		{
			break;
		}
		else if( result < 0 )
		{
			hi = mid - 1;
		}
		else
		{
			lo = mid + 1;
		}
	}
	if( result == 0 )
	{
		mid += 1;	// Bump index so new item is inserted after matching item.
	}
	else if( result > 0 )
	{
		mid += 1;
	}
	outIndex = mid;
	return( result );
}

//===========================================================================================================================
//	UTF8StringToStringObject
//===========================================================================================================================

DEBUG_LOCAL OSStatus	UTF8StringToStringObject( const char *inUTF8, CString &inObject )
{
	OSStatus		err;
	int				n;
	BSTR			unicode;
	
	unicode = NULL;
	
	n = MultiByteToWideChar( CP_UTF8, 0, inUTF8, -1, NULL, 0 );
	if( n > 0 )
	{
		unicode = (BSTR) malloc( (size_t)( n * sizeof( wchar_t ) ) );
		if( !unicode )
		{
			err = ERROR_INSUFFICIENT_BUFFER;
			goto exit;
		}

		n = MultiByteToWideChar( CP_UTF8, 0, inUTF8, -1, unicode, n );
		try
		{
			inObject = unicode;
		}
		catch( ... )
		{
			err = ERROR_NO_UNICODE_TRANSLATION;
			goto exit;
		}
	}
	else
	{
		inObject = "";
	}
	err = ERROR_SUCCESS;
	
exit:
	if( unicode )
	{
		free( unicode );
	}
	return( err );
}
