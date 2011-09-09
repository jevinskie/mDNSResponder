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

$Log: SimpleChat.java,v $
Revision 1.2  2004/04/30 21:53:35  rpantos
Change line endings for CVS.

Revision 1.1  2004/04/30 16:29:35  rpantos
First checked in.

	SimpleChat is a simple peer-to-peer chat program that demonstrates 
	DNSSD registration, browsing, resolving and record-querying.

	To do:
	- remove TXTRecord tests
	- remove diagnostic printf's
	- implement better coloring algorithm
 */


import java.awt.*;
import java.awt.event.*;
import java.text.*;
import java.net.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;

import com.apple.dnssd.*;


class	SimpleChat implements	ResolveListener, RegisterListener, QueryListener, 
								ActionListener, ItemListener, Runnable
{
	Document			textDoc;			// Holds all the chat text
	JTextField			inputField;			// Holds a pending chat response
	String				ourName;			// name used to identify this user in chat
	DNSSDService		browser;			// object that actively browses for other chat clients
	DNSSDService		resolver;			// object that resolves other chat clients
	DNSSDRegistration	registration;		// object that maintains our connection advertisement
	JComboBox			targetPicker;		// Indicates who we're talking to
	TargetListModel		targetList;			// and its list model
	JButton				sendButton;			// Will send text in inputField to target
	InetAddress			buddyAddr;			// and address
	int					buddyPort;			// and port
	DatagramPacket		dataPacket;			// Inbound data packet
	DatagramSocket		outSocket;			// Outbound data socket
	SimpleAttributeSet	textAttribs;

	static final String	kChatExampleRegType = "_p2pchat._udp";
	static final String	kWireCharSet = "ISO-8859-1";

	public		SimpleChat() throws Exception
	{
		JFrame frame = new JFrame("SimpleChat");
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {System.exit(0);}
		});

    	ourName = System.getProperty( "user.name");
		targetList = new TargetListModel();
		textAttribs = new SimpleAttributeSet();
		DatagramSocket inSocket = new DatagramSocket();
		dataPacket = new DatagramPacket( new byte[ 4096], 4096);
		outSocket = new DatagramSocket();

		this.setupSubPanes( frame.getContentPane(), frame.getRootPane());
		frame.pack();
		frame.setVisible(true);
		inputField.requestFocusInWindow();

		browser = DNSSD.browse( 0, 0, kChatExampleRegType, "", new SwingBrowseListener( targetList));

	TXTRecord	tRec = new TXTRecord();
	tRec.set( "name", "roger");
	tRec.set( "color", "blue");

		registration = DNSSD.register( 0, 0, ourName, kChatExampleRegType, "", "", inSocket.getLocalPort(), tRec, this);

		new ListenerThread( this, inSocket, dataPacket).start();
	}

	protected void	setupSubPanes( Container parent, JRootPane rootPane)
	{	
		parent.setLayout( new BoxLayout( parent, BoxLayout.Y_AXIS));

		JPanel textRow = new JPanel();
		textRow.setLayout( new BoxLayout( textRow, BoxLayout.X_AXIS));
		textRow.add( Box.createRigidArea( new Dimension( 16, 0)));
		JEditorPane textPane = new JEditorPane( "text/html", "<BR>");
		textPane.setPreferredSize( new Dimension( 400, 300));
		textPane.setEditable( false);
		JScrollPane textScroller = new JScrollPane( textPane);
		textRow.add( textScroller);
		textRow.add( Box.createRigidArea( new Dimension( 16, 0)));
		textDoc = textPane.getDocument();

		JPanel addressRow = new JPanel();
		addressRow.setLayout( new BoxLayout( addressRow, BoxLayout.X_AXIS));
		targetPicker = new JComboBox( targetList);
		targetPicker.addItemListener( this);
		addressRow.add( Box.createRigidArea( new Dimension( 16, 0)));
		addressRow.add( new JLabel( "Talk to: "));
		addressRow.add( targetPicker);
		addressRow.add( Box.createHorizontalGlue());

		JPanel buttonRow = new JPanel();
		buttonRow.setLayout( new BoxLayout( buttonRow, BoxLayout.X_AXIS));
		buttonRow.add( Box.createRigidArea( new Dimension( 16, 0)));
		inputField = new JTextField();
		// prevent inputField from hijacking <Enter> key
		inputField.getKeymap().removeKeyStrokeBinding( KeyStroke.getKeyStroke( KeyEvent.VK_ENTER, 0));
		buttonRow.add( inputField);
		sendButton = new JButton( "Send");
		buttonRow.add( Box.createRigidArea( new Dimension( 8, 0)));
		buttonRow.add( sendButton);
		buttonRow.add( Box.createRigidArea( new Dimension( 16, 0)));
		rootPane.setDefaultButton( sendButton);
		sendButton.addActionListener( this);
		sendButton.setEnabled( false);

		parent.add( Box.createRigidArea( new Dimension( 0, 16)));
		parent.add( textRow);
		parent.add( Box.createRigidArea( new Dimension( 0, 8)));
		parent.add( addressRow);
		parent.add( Box.createRigidArea( new Dimension( 0, 8)));
		parent.add( buttonRow);
		parent.add( Box.createRigidArea( new Dimension( 0, 16)));
	}

	public void	serviceRegistered( DNSSDRegistration registration, int flags, 
									String serviceName, String regType, String domain)
	{
		ourName = serviceName;		// might have been renamed on collision
System.out.println( "ServiceRegisterCallback() invoked as " + serviceName);
	}

	public void	operationFailed( DNSSDService service, int errorCode)
	{
		System.out.println( "Service reported error " + String.valueOf( errorCode));
	}

	public void	serviceResolved( DNSSDService resolver, int flags, int ifIndex, String fullName, 
										String hostName, int port, TXTRecord txtRecord)
	{
String a;
for ( int i=0; null != ( a = txtRecord.getKey( i)); i++)
	System.out.println( "attr/val " + String.valueOf( i) + ": " + a + "," + txtRecord.getValueAsString( i));

		buddyPort = port;
		try {
			// Start a record query to obtain IP address from hostname
			DNSSD.queryRecord( 0, ifIndex, hostName, 1 /* ns_t_a */, 1 /* ns_c_in */, 
								new SwingQueryListener( this));
		}
		catch ( Exception e) { terminateWithException( e); }
		resolver.stop();
	}

	public void	queryAnswered( DNSSDService query, int flags, int ifIndex, String fullName, 
									int rrtype, int rrclass, byte[] rdata, int ttl)
	{
		try {
			buddyAddr = InetAddress.getByAddress( rdata);
		}
		catch ( Exception e) { terminateWithException( e); }
		sendButton.setEnabled( true);
	}

	public void		actionPerformed( ActionEvent e) 	// invoked when Send button is hit
	{
		try
		{
			String	sendString = ourName + ": " + inputField.getText();
			byte[] sendData = sendString.getBytes( kWireCharSet);
			outSocket.send( new DatagramPacket( sendData, sendData.length, buddyAddr, buddyPort));
			StyleConstants.setForeground( textAttribs, Color.black);
			textDoc.insertString( textDoc.getLength(), inputField.getText() + "\n", textAttribs);
			inputField.setText( "");
		}
		catch ( Exception exception) { terminateWithException( exception); }
	}

	public void		itemStateChanged( ItemEvent e) 	// invoked when Target selection changes
	{
		sendButton.setEnabled( false);
		if ( e.getStateChange() == ItemEvent.SELECTED)
		{
			try {
				TargetListElem	sel = (TargetListElem) targetList.getSelectedItem();
				resolver = DNSSD.resolve( 0, sel.fInt, sel.fServiceName, sel.fType, sel.fDomain, this);
			}
			catch ( Exception exception) { terminateWithException( exception); }
		}
	}

	public void		run()		// invoked on event thread when inbound packet arrives
	{
		try
		{
			String	inMessage = new String( dataPacket.getData(), 0, dataPacket.getLength(), kWireCharSet);
			StyleConstants.setForeground( textAttribs, this.getColorFor( dataPacket.getData(), dataPacket.getLength()));
			textDoc.insertString( textDoc.getLength(), inMessage + "\n", textAttribs);
		}
		catch ( Exception e) { terminateWithException( e); }
	}

	protected Color		getColorFor( byte[] chars, int length)
	// Produce a mapping from a string to a color, suitable for text display
	{
		int		rgb = 0;
		for ( int i=0; i < length && chars[i] != ':'; i++)
			rgb = rgb ^ ( (int) chars[i] << (i%3+2) * 8);
		return new Color( rgb & 0x007F7FFF);	// mask off high bits so it is a dark color
		
//		for ( int i=0; i < length && chars[i] != ':'; i++)
		
	}

	protected static void	terminateWithException( Exception e)
	{
		e.printStackTrace();
		System.exit( -1);
	}

    public static void main(String s[]) 
    {
    	try {
			new SimpleChat();
		}
		catch ( Exception e) { terminateWithException( e); }
    }
}



class	TargetListElem
{
	public	TargetListElem( String serviceName, String domain, String type, int ifIndex)
	{ fServiceName = serviceName; fDomain = domain; fType = type; fInt = ifIndex; }
	
	public String	toString() { return fServiceName; }
	
	public String	fServiceName, fDomain, fType;
	public int		fInt;
}

class		TargetListModel extends DefaultComboBoxModel implements BrowseListener
{
	/* The Browser invokes this callback when a service is discovered. */
	public void	serviceFound( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)
	{
		TargetListElem		match = this.findMatching( serviceName);	// probably doesn't handle near-duplicates well.

		if ( match == null)
			this.addElement( new TargetListElem( serviceName, domain, regType, ifIndex));
	}

	/* The Browser invokes this callback when a service disappears. */
	public void	serviceLost( DNSSDService browser, int flags, int ifIndex, 
							String serviceName, String regType, String domain)
	{
		TargetListElem		match = this.findMatching( serviceName);	// probably doesn't handle near-duplicates well.

		if ( match != null)
			this.removeElement( match);
	}

	/* The Browser invokes this callback when a service disappears. */
	public void	operationFailed( DNSSDService service, int errorCode)
	{
		System.out.println( "Service reported error " + String.valueOf( errorCode));
	}

	protected TargetListElem	findMatching( String match)
	{
		for ( int i = 0; i < this.getSize(); i++)
			if ( match.equals( this.getElementAt( i).toString()))
				return (TargetListElem) this.getElementAt( i);
		return null;
	}

}


// A ListenerThread runs its owner when datagram packet p appears on socket s.
class	ListenerThread extends Thread
{
	public			ListenerThread( Runnable owner, DatagramSocket s, DatagramPacket p) 
	{ fOwner = owner; fSocket = s; fPacket = p; }

	public void		run()
	{		
		while ( true )
		{
			try 
			{
				fSocket.receive( fPacket);
				SwingUtilities.invokeAndWait( fOwner);	// process data on main thread
			}
			catch( Exception e)
			{
				break;	// terminate thread
			}
		}
	}

	protected Runnable			fOwner;
	protected DatagramSocket	fSocket;
	protected DatagramPacket	fPacket;
}



