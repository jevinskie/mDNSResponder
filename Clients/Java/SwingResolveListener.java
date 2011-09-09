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

$Log: SwingResolveListener.java,v $
Revision 1.2  2004/06/14 23:46:55  cheshire
Recreate CVS state

Revision 1.1  2004/06/14 23:46:26  cheshire
First checkin

 */


import javax.swing.*;
import com.apple.dnssd.*;


/**	Use this to schedule ResolveListener callbacks via SwingUtilities.invokeAndWait(). */

public class SwingResolveListener implements Runnable, ResolveListener
{
	/** Create a listener for DNSSD that will call your listener on the Swing/AWT event thread. */
	public	SwingResolveListener( ResolveListener listener)
	{ fListener = listener; }

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		fResolver = service;
		fErrorCode = errorCode;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void	serviceResolved( DNSSDService resolver, int flags, int ifIndex, String fullName, 
								String hostName, int port, TXTRecord txtRecord)
	{
		fResolver = resolver;
		fFlags = flags;
		fIndex = ifIndex;
		fFullName = fullName;
		fHostName = hostName;
		fPort = port;
		fTXTRecord = txtRecord;
		this.schedule();
	}

	/** (Clients should not call this method directly.) */
	public void		run()
	{
		if ( fErrorCode != 0)
			fListener.operationFailed( fResolver, fErrorCode);
		else
			fListener.serviceResolved( fResolver, fFlags, fIndex, fFullName, fHostName, fPort, fTXTRecord);
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

	protected ResolveListener	fListener;

	protected DNSSDService		fResolver;
	protected int				fFlags;
	protected int				fIndex;
	protected int				fErrorCode;
	protected String			fFullName;
	protected String			fHostName;
	protected int				fPort;
	protected TXTRecord			fTXTRecord;
}

