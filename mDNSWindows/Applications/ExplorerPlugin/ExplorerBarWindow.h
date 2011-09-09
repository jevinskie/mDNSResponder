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
    
$Log: ExplorerBarWindow.h,v $
Revision 1.3  2004/04/15 01:00:05  bradley
Removed support for automatically querying for A/AAAA records when resolving names. Platforms
without .local name resolving support will need to manually query for A/AAAA records as needed.

Revision 1.2  2004/04/08 09:43:43  bradley
Changed callback calling conventions to __stdcall so they can be used with C# delegates.

Revision 1.1  2004/01/30 03:01:56  bradley
Explorer Plugin to browse for Rendezvous-enabled Web and FTP servers from within Internet Explorer.

*/

#ifndef	__EXPLORER_BAR_WINDOW__
#define	__EXPLORER_BAR_WINDOW__

#pragma once

#include	"afxtempl.h"

#include	"DNSSD.h"

//===========================================================================================================================
//	Structures
//===========================================================================================================================

// Forward Declarations

struct	ServiceHandlerEntry;
class	ExplorerBarWindow;

// ServiceInfo

struct	ServiceInfo
{
	CString						displayName;
	char *						name;
	char *						type;
	char *						domain;
	uint32_t					ifi;
	HTREEITEM					item;
	ServiceHandlerEntry *		handler;
	
	ServiceInfo( void )
	{
		item	= NULL;
		type 	= NULL;
		domain	= NULL;
		handler	= NULL;
	}
	
	~ServiceInfo( void )
	{
		if( name )
		{
			free( name );
		}
		if( type )
		{
			free( type );
		}
		if( domain )
		{
			free( domain );
		}
	}
};

typedef CArray < ServiceInfo *, ServiceInfo * >		ServiceInfoArray;

// TextRecord

struct	TextRecord
{
	uint8_t *		mData;
	size_t			mSize;
	
	TextRecord( void )
	{
		mData = NULL;
		mSize = 0;
	}
	
	~TextRecord( void )
	{
		if( mData )
		{
			free( mData );
		}
	}
	
	void	GetData( void *outData, size_t *outSize )
	{
		if( outData )
		{
			*( (void **) outData ) = mData;
		}
		if( outSize )
		{
			*outSize = mSize;
		}
	}
	
	OSStatus	SetData( const void *inData, size_t inSize )
	{
		OSStatus		err;
		uint8_t *		newData;
		
		newData = (uint8_t *) malloc( inSize );
		require_action( newData, exit, err = kNoMemoryErr );
		memcpy( newData, inData, inSize );
		
		if( mData )
		{
			free( mData );
		}
		mData = newData;
		mSize = inSize;
		err  = kNoErr;
		
	exit:
		return( err );
	}
};

// ResolveInfo

struct	ResolveInfo
{
	CString						host;
	uint16_t					port;
	uint32_t					ifi;
	TextRecord					txt;
	ServiceHandlerEntry *		handler;
};

// ServiceHandlerEntry

struct	ServiceHandlerEntry
{
	const char *			type;
	const char *			urlScheme;
	DNSServiceRef			ref;
	ServiceInfoArray		array;
	HTREEITEM				treeItem;
	bool					treeFirst;
	ExplorerBarWindow *		obj;
	bool					needsLogin;
	
	ServiceHandlerEntry( void )
	{
		type		= NULL;
		urlScheme	= NULL;
		ref 		= NULL;
		treeItem	= NULL;
		treeFirst	= true;
		obj			= NULL;
		needsLogin	= false;
	}
	
	~ServiceHandlerEntry( void )
	{
		if( ref )
		{
			DNSServiceRefDeallocate( ref );
		}
		
		int		i;
		int		n;
		
		n = (int) array.GetSize();
		for( i = 0; i < n; ++i )
		{
			delete array[ i ];
		}
	}
};

typedef CArray < ServiceHandlerEntry *, ServiceHandlerEntry * >		ServiceHandlerArray;

//===========================================================================================================================
//	ExplorerBarWindow
//===========================================================================================================================

class	ExplorerBar;	// Forward Declaration

class	ExplorerBarWindow : public CWnd
{
	protected:

		ExplorerBar *			mOwner;
		CTreeCtrl				mTree;
		
		ServiceHandlerArray		mServiceHandlers;
		DNSServiceRef			mResolveServiceRef;
		
	public:
		
		ExplorerBarWindow( void );
		virtual	~ExplorerBarWindow( void );

	protected:
		
		// General
		
		afx_msg int		OnCreate( LPCREATESTRUCT inCreateStruct );
		afx_msg void	OnDestroy( void );
		afx_msg void	OnSize( UINT inType, int inX, int inY );
		afx_msg void	OnDoubleClick( NMHDR *inNMHDR, LRESULT *outResult );
		
		// Browsing
		
		static void CALLBACK_COMPAT
			BrowseCallBack(
				DNSServiceRef 			inRef,
				DNSServiceFlags 		inFlags,
				uint32_t 				inInterfaceIndex,
				DNSServiceErrorType 	inErrorCode,
				const char *			inName,	
				const char *			inType,	
				const char *			inDomain,	
				void *					inContext );
		afx_msg LONG OnServiceAdd( WPARAM inWParam, LPARAM inLParam );
		afx_msg LONG OnServiceRemove( WPARAM inWParam, LPARAM inLParam );
		
		// Resolving
		
		OSStatus	StartResolve( ServiceInfo *inService );
		void		StopResolve( void );

		static void CALLBACK_COMPAT
			ResolveCallBack(
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
		afx_msg LONG OnResolve( WPARAM inWParam, LPARAM inLParam );		
				
		// Accessors
	
	public:
	
		ExplorerBar *	GetOwner( void ) const				{ return( mOwner ); }
		void			SetOwner( ExplorerBar *inOwner )	{ mOwner = inOwner; }
		
		DECLARE_MESSAGE_MAP()
};

#endif	// __EXPLORER_BAR_WINDOW__
