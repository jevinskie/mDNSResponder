/*
 * Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
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

$Log: SwingBrowseListener.java,v $
Revision 1.2  2004/04/30 21:53:35  rpantos
Change line endings for CVS.

Revision 1.1  2004/04/30 16:29:35  rpantos
First checked in.

 */


import javax.swing.*;
import com.apple.dnssd.*;


/**	Use this to schedule BrowseListener callbacks via SwingUtilities.invokeAndWait(). */

public class SwingBrowseListener implements Runnable, BrowseListener
{
	/** Create a listener for DNSSD that will call your listener on the Swing/AWT event thread. */
	public	SwingBrowseListener( BrowseListener listener)
	{ fListener = listener; fErrorCode = 0; }

	/** (Clients should not call this method directly.) */
	public void	operationFailed( DNSSDService service, int errorCode)
	{
		fBrowser = service;
		fErrorCode = errorCode;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void	serviceFound( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)

	{
		fBrowser = browser;
		fIsAdd = true;
		fFlags = flags;
		fIndex = ifIndex;
		fService = serviceName;
		fRegType = regType;
		fDomain = domain;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void	serviceLost( DNSSDService browser, int flags, int ifIndex,
							String serviceName, String regType, String domain)
	{
		fBrowser = browser;
		fIsAdd = false;
		fFlags = flags;
		fIndex = ifIndex;
		fService = serviceName;
		fRegType = regType;
		fDomain = domain;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void		run()
	{
		if ( fErrorCode != 0)
			fListener.operationFailed( fBrowser, fErrorCode);
		else if ( fIsAdd)
			fListener.serviceFound( fBrowser, fFlags, fIndex, fService, fRegType, fDomain);
		else
			fListener.serviceLost( fBrowser, fFlags, fIndex, fService, fRegType, fDomain);
	}

	protected void	schedule()
	{		
		try {
			SwingUtilities.invokeAndWait( this);
		}
		catch ( Exception e)
		{
			e.printStackTrace();
		}
	}

	protected BrowseListener	fListener;

	protected boolean			fIsAdd;
	protected DNSSDService		fBrowser;
	protected int				fFlags;
	protected int				fIndex;
	protected int				fErrorCode;
	protected String			fService;
	protected String			fRegType;
	protected String			fDomain;
}

