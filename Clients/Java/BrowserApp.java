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

$Log: BrowserApp.java,v $
Revision 1.3  2004/05/26 01:41:58  cheshire
Pass proper flags to DNSSD.enumerateDomains

Revision 1.2  2004/04/30 21:53:34  rpantos
Change line endings for CVS.

Revision 1.1  2004/04/30 16:29:35  rpantos
First checked in.

	BrowserApp demonstrates how to use DNSSD to browse for and resolve services.

	To do:
	- display resolved TXTRecord
 */


import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;
import javax.swing.*;
import javax.swing.event.*;

import com.apple.dnssd.*;


class	BrowserApp implements ListSelectionListener, ResolveListener
{
	static BrowserApp	app;
	JFrame				frame;
	DomainListModel		domainList;
	BrowserListModel	servicesList, serviceList;
	JList				domainPane, servicesPane, servicePane;
	DNSSDService		servicesBrowser, serviceBrowser, domainBrowser;
	JLabel				hostLabel, portLabel;

	public		BrowserApp()
	{
		frame = new JFrame("DNS-SD Service Browser");
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {System.exit(0);}
		});

		domainList = new DomainListModel();
		servicesList = new ServicesBrowserListModel();
		serviceList = new BrowserListModel();

		try {
			domainBrowser = DNSSD.enumerateDomains( DNSSD.BROWSE_DOMAINS, 0, domainList);

			servicesBrowser = DNSSD.browse( 0, 0, "_services._mdns._udp.", "", servicesList);
			serviceBrowser = null;
		}
		catch ( Exception ex) { terminateWithException( ex); }

		this.setupSubPanes( frame.getContentPane());
		frame.pack();
		frame.setVisible(true);
	}

	protected void	setupSubPanes( Container parent)
	{
		parent.setLayout( new BoxLayout( parent, BoxLayout.Y_AXIS));

		JPanel browserRow = new JPanel();
		browserRow.setLayout( new BoxLayout( browserRow, BoxLayout.X_AXIS));
		domainPane = new JList( domainList);
		domainPane.addListSelectionListener( this);
		JScrollPane domainScroller = new JScrollPane( domainPane, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		browserRow.add( domainScroller);
		servicesPane = new JList( servicesList);
		servicesPane.addListSelectionListener( this);
		JScrollPane servicesScroller = new JScrollPane( servicesPane, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		browserRow.add( servicesScroller);
		servicePane = new JList( serviceList);
		servicePane.addListSelectionListener( this);
		JScrollPane serviceScroller = new JScrollPane( servicePane, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		browserRow.add( serviceScroller);

/*
		JPanel buttonRow = new JPanel();
		buttonRow.setLayout( new BoxLayout( buttonRow, BoxLayout.X_AXIS));
		buttonRow.add( Box.createHorizontalGlue());
		JButton connectButton = new JButton( "Don't Connect");
		buttonRow.add( connectButton);
		buttonRow.add( Box.createRigidArea( new Dimension( 16, 0)));
*/

		JPanel labelRow = new JPanel();
		labelRow.setLayout( new BoxLayout( labelRow, BoxLayout.X_AXIS));
		labelRow.add( new JLabel( "  Host: "));
		hostLabel = new JLabel();
		labelRow.add( hostLabel);
		labelRow.add( Box.createRigidArea( new Dimension( 32, 0)));
		labelRow.add( new JLabel( "Port: "));
		portLabel = new JLabel();
		labelRow.add( portLabel);
		labelRow.add( Box.createHorizontalGlue());

		parent.add( browserRow);
		parent.add( Box.createRigidArea( new Dimension( 0, 8)));
		parent.add( labelRow);
//		parent.add( buttonRow);
		parent.add( Box.createRigidArea( new Dimension( 0, 16)));
	}

	public void valueChanged( ListSelectionEvent e)
	{
		try {
			if ( e.getSource() == domainPane && !e.getValueIsAdjusting())
			{
				int		newSel = domainPane.getSelectedIndex();
				if ( -1 != newSel)
				{
					if ( serviceBrowser != null)
						serviceBrowser.stop();
					serviceList.removeAllElements();
					servicesBrowser = DNSSD.browse( 0, 0, "_services._mdns._udp.", "", servicesList);
				}
			}
			else if ( e.getSource() == servicesPane && !e.getValueIsAdjusting())
			{
				int		newSel = servicesPane.getSelectedIndex();
				if ( serviceBrowser != null)
					serviceBrowser.stop();
				serviceList.removeAllElements();
				if ( -1 != newSel)
					serviceBrowser = DNSSD.browse( 0, 0, servicesList.getNthRegType( newSel), "", serviceList);
			}
			else if ( e.getSource() == servicePane && !e.getValueIsAdjusting())
			{
				int		newSel = servicePane.getSelectedIndex();

				hostLabel.setText( "");
				portLabel.setText( "");

				if ( -1 != newSel)
				{
					DNSSD.resolve( 0, serviceList.getNthInterface( newSel), 
										serviceList.getNthServiceName( newSel), 
										serviceList.getNthRegType( newSel), 
										serviceList.getNthDomain( newSel), 
										new SwingResolveListener( this));
				}
			}
		}
		catch ( Exception ex) { terminateWithException( ex); }
	}

	public void	serviceResolved( DNSSDService resolver, int flags, int ifIndex, String fullName, 
								String hostName, int port, TXTRecord txtRecord)
	{
		hostLabel.setText( hostName);
		portLabel.setText( String.valueOf( port));
	}

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		// handle failure here
	}

	protected static void	terminateWithException( Exception e)
	{
		e.printStackTrace();
		System.exit( -1);
	}

    public static void main(String s[]) 
    {
		app = new BrowserApp();
    }
}


class	BrowserListModel extends DefaultListModel implements BrowseListener, Runnable
{
	public		BrowserListModel()
	{
		addCache = new Vector();
		removeCache = new Vector();
	}

	/* The Browser invokes this callback when a service is discovered. */
	public void	serviceFound( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)
	{
		addCache.add( new BrowserListElem( serviceName, domain, regType, ifIndex));
		if ( ( flags & DNSSD.MORE_COMING) == 0)
			this.scheduleOnEventThread();
	}

	public void	serviceLost( DNSSDService browser, int flags, int ifIndex,
							String serviceName, String regType, String domain)
	{
		removeCache.add( serviceName);
		if ( ( flags & DNSSD.MORE_COMING) == 0)
			this.scheduleOnEventThread();
	}

	public void	run()
	{
		while ( removeCache.size() > 0)
		{
			String	serviceName = (String) removeCache.remove( removeCache.size() - 1);
			int		matchInd = this.findMatching( serviceName);	// probably doesn't handle near-duplicates well.
			if ( matchInd != -1)
				this.removeElementAt( matchInd);
		}
		while ( addCache.size() > 0)
		{
			BrowserListElem	elem = (BrowserListElem) addCache.remove( addCache.size() - 1);
			if ( -1 == this.findMatching( elem.fServiceName))	// probably doesn't handle near-duplicates well.
				this.addInSortOrder( elem);
		}
	}

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		// handle failure here
	}

	/* The list contains BrowserListElem's */
	class	BrowserListElem
	{
		public	BrowserListElem( String serviceName, String domain, String type, int ifIndex)
		{ fServiceName = serviceName; fDomain = domain; fType = type; fInt = ifIndex; }
		
		public String	toString() { return fServiceName; }
		
		public String	fServiceName, fDomain, fType;
		public int		fInt;
	}

	public String	getNthServiceName( int n)
	{
		BrowserListElem sel = (BrowserListElem) this.get( n);
		return sel.fServiceName;
	}

	public String	getNthRegType( int n)
	{
		BrowserListElem sel = (BrowserListElem) this.get( n);
		return sel.fType;
	}

	public String	getNthDomain( int n)
	{
		BrowserListElem sel = (BrowserListElem) this.get( n);
		return sel.fDomain;
	}

	public int		getNthInterface( int n)
	{
		BrowserListElem sel = (BrowserListElem) this.get( n);
		return sel.fInt;
	}

	protected void	addInSortOrder( Object obj)
	{
		int	i;
		for ( i = 0; i < this.size(); i++)
			if ( sCollator.compare( obj.toString(), this.getElementAt( i).toString()) < 0)
				break;
		this.add( i, obj);
	}

	protected int	findMatching( String match)
	{
		for ( int i = 0; i < this.size(); i++)
			if ( match.equals( this.getElementAt( i).toString()))
				return i;
		return -1;
	}

	protected void	scheduleOnEventThread()
	{
		try {
			SwingUtilities.invokeAndWait( this);
		}
		catch ( Exception e)
		{
			e.printStackTrace();
		}
	}		

	protected Vector	removeCache;	// list of serviceNames to remove
	protected Vector	addCache;		// list of BrowserListElem's to add

	protected static Collator	sCollator;

	static	// Initialize our static variables
	{
		sCollator = Collator.getInstance();
		sCollator.setStrength( Collator.PRIMARY);
	}
}


class	ServicesBrowserListModel extends BrowserListModel
{
	/* The Browser invokes this callback when a service is discovered. */
	public void	serviceFound( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)
	// Overridden to stuff serviceName into regType and make serviceName human-readable.
	{
		regType = serviceName + ( regType.startsWith( "_udp.") ? "._udp." : "._tcp.");
		super.serviceFound( browser, flags, ifIndex, this.mapTypeToName( serviceName), regType, domain);
	}

	public void	serviceLost( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)
	// Overridden to make serviceName human-readable.
	{
		super.serviceLost( browser, flags, ifIndex, this.mapTypeToName( serviceName), regType, domain);
	}

	protected String	mapTypeToName( String type)
	// Convert a registration type into a human-readable string. Returns original string on no-match.
	{
		final String[]	namedServices = {
			"_afpovertcp",	"Apple File Sharing",
			"_http",		"World Wide Web servers",
			"_daap",		"Digital Audio Access",
			"_apple-sasl",	"Apple Password Servers",
			"_distcc",		"Distributed Compiler nodes",
			"_finger",		"Finger servers",
			"_ichat",		"iChat clients",
			"_presence",	"iChat AV clients",
			"_ssh",			"SSH servers",
			"_telnet",		"Telnet servers",
			"_workstation",	"Macintosh Manager clients",
			"_bootps",		"BootP servers",
			"_xserveraid",	"XServe RAID devices",
			"_eppc",		"Remote AppleEvents",
			"_ftp",			"FTP services",
			"_tftp",		"TFTP services"
		};

		for ( int i = 0; i < namedServices.length; i+=2)
			if ( namedServices[i].equals( type))
				return namedServices[i + 1];
		return type;
	}
}


class	DomainListModel extends DefaultListModel implements DomainListener
{
	/* Called when a domain is discovered. */
	public void	domainFound( DNSSDService domainEnum, int flags, int ifIndex, String domain)
	{
		if ( !this.contains( domain))
			this.addElement( domain);
	}
	
	public void	domainLost( DNSSDService domainEnum, int flags, int ifIndex, String domain)
	{
		if ( this.contains( domain))
			this.removeElement( domain);
	}

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		// handle failure here
	}
}

