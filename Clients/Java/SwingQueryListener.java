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

$Log: SwingQueryListener.java,v $
Revision 1.2  2004/04/30 21:53:35  rpantos
Change line endings for CVS.

Revision 1.1  2004/04/30 16:29:35  rpantos
First checked in.

 */


import javax.swing.*;
import com.apple.dnssd.*;


/**	Use this to schedule QueryListener callbacks via SwingUtilities.invokeAndWait(). */

public class SwingQueryListener implements Runnable, QueryListener
{
	/** Create a listener for DNSSD that will call your listener on the Swing/AWT event thread. */
	public	SwingQueryListener( QueryListener listener)
	{ fListener = listener; }

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		fQuery = service;
		fErrorCode = errorCode;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void	queryAnswered( DNSSDService query, int flags, int ifIndex, String fullName, 
								int rrtype, int rrclass, byte[] rdata, int ttl)
	{
		fQuery = query;
		fFlags = flags;
		fIndex = ifIndex;
		fFullName = fullName;
		fType = rrtype;
		fClass = rrclass;
		fData = rdata;
		fTTL = ttl;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void		run()
	{
		if ( fErrorCode != 0)
			fListener.operationFailed( fQuery, fErrorCode);
		else
			fListener.queryAnswered( fQuery, fFlags, fIndex, fFullName, fType, fClass, fData, fTTL);
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

	protected QueryListener		fListener;

	protected DNSSDService		fQuery;
	protected int				fFlags;
	protected int				fIndex;
	protected int				fErrorCode;
	protected String			fFullName;
	protected int				fType;
	protected int				fClass;
	protected byte[]			fData;
	protected int				fTTL;
}

