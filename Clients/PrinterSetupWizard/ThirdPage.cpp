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
    
$Log: ThirdPage.cpp,v $
Revision 1.10  2004/10/11 22:55:34  shersche
<rdar://problem/3827624> Use the IP port number when deriving the printer port name.
Bug #: 3827624

Revision 1.9  2004/06/27 23:08:00  shersche
code cleanup, make sure EnumPrintDrivers returns non-zero value, ignore comments in inf files

Revision 1.8  2004/06/27 08:06:45  shersche
Parse [Strings] section of inf file

Revision 1.7  2004/06/26 04:00:05  shersche
fix warnings compiling in debug mode
Submitted by: herscher

Revision 1.6  2004/06/26 03:19:57  shersche
clean up warning messages

Submitted by: herscher

Revision 1.5  2004/06/25 05:06:02  shersche
Trim whitespace from key/value pairs when parsing inf files
Submitted by: herscher

Revision 1.4  2004/06/25 02:44:13  shersche
Tweaked code to handle Xerox Phaser printer identification
Submitted by: herscher

Revision 1.3  2004/06/25 02:27:58  shersche
Do a CListCtrl::FindItem() before calling CListCtrl::SetItemState().
Submitted by: herscher

Revision 1.2  2004/06/23 18:09:23  shersche
Normalize tag names when parsing inf files.
Submitted by: herscher

Revision 1.1  2004/06/18 04:36:58  rpantos
First checked in


*/

#include "stdafx.h"
#include "PrinterSetupWizardApp.h"
#include "PrinterSetupWizardSheet.h"
#include "ThirdPage.h"
#include "StdioFileEx.h"
#include <dns_sd.h>
#include <tcpxcv.h>
#include <winspool.h>

// local variable is initialize but not referenced
#pragma warning(disable:4189)


//
// This is the printer description file that is shipped
// with Windows
//
#define kNTPrintFile		L"inf\\ntprint.inf"

//
// These are pre-defined names for Generic manufacturer and model
//
#define kGenericManufacturer	L"Generic"
#define kGenericModel			L"Generic / Text Only"

//
// states for parsing ntprint.inf
//
enum PrinterParsingState
{
	Looking,
	ParsingManufacturers,
	ParsingModels,
	ParsingStrings
};


// CThirdPage dialog

IMPLEMENT_DYNAMIC(CThirdPage, CPropertyPage)
CThirdPage::CThirdPage()
	: CPropertyPage(CThirdPage::IDD),
		m_initialized(false)
{
	m_psp.dwFlags &= ~(PSP_HASHELP);
	m_psp.dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	
	m_psp.pszHeaderTitle = MAKEINTRESOURCE(IDS_INSTALL_TITLE);
	m_psp.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_INSTALL_SUBTITLE);
}


CThirdPage::~CThirdPage()
{
	//
	// clean up all the printer manufacturers
	//
	while (m_manufacturers.size())
	{
		Manufacturers::iterator iter = m_manufacturers.begin();

		while (iter->second->models.size())
		{
			Models::iterator it = iter->second->models.begin();

			Model * model = *it;

			delete model;

			iter->second->models.erase(it);
		}

		delete iter->second;

		m_manufacturers.erase(iter);
	}
}


// ----------------------------------------------------
// SelectMatch
//
// SelectMatch will do all the UI work associated with
// selected a manufacturer and model of printer.  It also
// makes sure the printer object is update with the 
// latest settings
//
// ----------------------------------------------------
void
CThirdPage::SelectMatch(Printer * printer, Manufacturer * manufacturer, Model * model)
{
	LVFINDINFO	info;
	int			nIndex;

	check( printer != NULL );
	check( manufacturer != NULL );
	check( model != NULL );

	Manufacturers manufacturers;
	manufacturers[manufacturer->name] = manufacturer;

	PopulateUI( manufacturers );

	//
	// select the manufacturer
	//
	info.flags	= LVFI_STRING;
	info.psz	= manufacturer->name;

	nIndex = m_manufacturerListCtrl.FindItem(&info);
	
	if (nIndex != -1)
	{
		m_manufacturerListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_manufacturerListCtrl.EnsureVisible(nIndex, FALSE);
	}

	//
	// select the model
	//
	info.flags	= LVFI_STRING;
	info.psz	= model->name;

	nIndex = m_modelListCtrl.FindItem(&info);

	if (nIndex != -1)
	{
		m_modelListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
		m_modelListCtrl.EnsureVisible(nIndex, FALSE);

		m_modelListCtrl.SetFocus();
	}

	CopyPrinterSettings( printer, manufacturer, model );
}


// --------------------------------------------------------
// CopyPrinterSettings
//
// This function makes sure that the printer object has the
// latest settings from the manufacturer and model objects
// --------------------------------------------------------

void
CThirdPage::CopyPrinterSettings( Printer * printer, Manufacturer * manufacturer, Model * model )
{
	printer->manufacturer		=	manufacturer->name;
	printer->model				=	model->name;
	printer->driverInstalled	=	model->driverInstalled;
	printer->infFileName		=	model->infFileName;
	printer->portName.Format(L"IP_%s.%d", static_cast<LPCTSTR>(printer->hostname), printer->portNumber);
}


// ------------------------------------------------------
// LoadPrintDriverDefsFromFile
//
// This function does all the heavy lifting in parsing inf
// files.  It is called to parse both ntprint.inf, and driver
// files that might be shipped on a printer's installation
// disk
//
// The inf file is not totally parsed.  I only want to determine
// the manufacturer and models that are involved. I leave it
// to printui.dll to actually copy the driver files to the
// right places.
//
// I was aiming to parse as little as I could so as not to
// duplicate the parsing code that is contained in Windows.  There
// are no public APIs for parsing inf files.
//
// That part of the inf file that we're interested in has a fairly
// easy format.  Tags are strings that are enclosed in brackets.
// We are only interested in [MANUFACTURERS] and models.
//
// The only potentially opaque thing about this function is the 
// checkForDuplicateModels flag.  The problem here is that ntprint.inf
// doesn't contain duplicate models, and it has hundreds of models
// listed.  You wouldn't check for duplicates there.  But oftentimes,
// loading different windows print driver files contain multiple
// entries for the same printer.  You don't want the UI to display
// the same printer multiple times, so in that case, you would ask
// this function to check for multiple models.

OSStatus
CThirdPage::LoadPrintDriverDefsFromFile(Manufacturers & manufacturers, const CString & filename, bool checkForDuplicateModels )
{
	PrinterParsingState		state		= Looking;
	Manufacturers::iterator iter		= manufacturers.end();
	CStdioFileEx			file;
	CFileException			feError;
	CString					s;
	OSStatus				err;
	BOOL					ok;

	typedef std::map<CString, CString> StringMap;

	StringMap				strings;
 
	ok = file.Open( filename,  CFile::modeRead|CFile::typeText, &feError);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	check ( state == Looking );
	check ( iter == manufacturers.end() );

	//
	// first, parse the file looking for string sections
	//
	while (file.ReadString(s))
	{
		//
		// check for comment
		//
		if (s.Find(';') == 0)
		{
			continue;
		}

		//
		// check for tag
		//
		else if (s.Find('[') == 0)
		{
			//
			// handle any capitalization issues here
			//
			CString tag = s;

			tag.MakeLower();

			if (tag == L"[strings]")
			{
				state = ParsingStrings;
			}
			else
			{
				state = Looking;
			}
		}
		else
		{
			switch (state)
			{
				case ParsingStrings:
				{
					int	curPos = 0;

					if (s.GetLength() > 0)
					{
						CString key = s.Tokenize(L"=",curPos);
						CString val = s.Tokenize(L"=",curPos);

						//
						// get rid of all delimiters
						//
						val.Remove('"');
	
						//
						// and store it
						//
						strings[key] = val;
					}
				}
				break;
			}
		}
	}

	file.Close();

	ok = file.Open( filename,  CFile::modeRead|CFile::typeText, &feError);
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );

	state = Looking;

	check ( iter == manufacturers.end() );

	while (file.ReadString(s))
	{
		//
		// check for comment
		//
		if (s.Find(';') == 0)
		{
			continue;
		}

		//
		// check for tag
		//
		else if (s.Find('[') == 0)
		{
			//
			// handle any capitalization issues here
			//
			CString tag = s;

			tag.MakeLower();

			if (tag == L"[manufacturer]")
			{
				state = ParsingManufacturers;
			}
			else
			{
				// remove the leading and trailing delimiters
				//
				s.Remove('[');
				s.Remove(']');

				// check to see if this is a printer entry
				//
				iter = manufacturers.find(s);

				if (iter != manufacturers.end())
				{
					state = ParsingModels;
				}
				else
				{
					state = Looking;
				}
			}
		}
		//
		// only look at this if the line isn't empty, or
		// if it isn't a comment
		//
		else if ((s.GetLength() > 0) && (s.Find(';') != 0))
		{
			switch (state)
			{
				//
				// if we're parsing manufacturers, then we will parse
				// an entry of the form key=val, where key is a delimited
				// string specifying a manufacturer name, and val is
				// a tag that is used later in the file.  the key is 
				// delimited by either '"' (quotes) or '%' (percent sign).
				//
				// the tag is used further down the file when models are
				// declared.  this allows multiple manufacturers to exist
				// in a single inf file.
				//
				case ParsingManufacturers:
				{
					Manufacturer	*	manufacturer;
					int					curPos = 0;

					CString key = s.Tokenize(L"=",curPos);
					CString val = s.Tokenize(L"=",curPos);

					try
					{
						manufacturer = new Manufacturer;
					}
					catch (...)
					{
						manufacturer = NULL;
					}

					require_action( manufacturer, exit, err = kNoMemoryErr );

					//
					// if it's a variable, look it up
					//
					if (key.Find('%') == 0)
					{
						StringMap::iterator it;

						key.Remove('%');

						it = strings.find(key);

						if (it != strings.end())
						{
							key = it->second;
						}
					}
					else
					{
						key.Remove('"');
					}

					val.TrimLeft();
					val.TrimRight();

					//
					// why is there no consistency in inf files?
					//
					if (val.GetLength() == 0)
					{
						val = key;
					}

					//
					// fix the manufacturer name if necessary
					//
					curPos	=	0;
					val		=	val.Tokenize(L",", curPos);

					manufacturer->name = NormalizeManufacturerName( key );
					manufacturer->tag  = val;

					manufacturers[val] = manufacturer;
				}
				break;

				case ParsingModels:
				{
					check( iter != manufacturers.end() );

					Model	*	model;
					int			curPos = 0;

					CString name		= s.Tokenize(L"=",curPos);
					CString description = s.Tokenize(L"=",curPos);
					
					name.Remove('"');
					name.Trim();
					description.Trim();
					
					//
					// If true, see if we've seen this guy before
					//
					if (checkForDuplicateModels == true)
					{
						if ( MatchModel( iter->second, name ) != NULL )
						{
							continue;
						}
					}

					try
					{
						model = new Model;
					}
					catch (...)
					{
						model = NULL;
					}

					require_action( model, exit, err = kNoMemoryErr );

					model->infFileName		=	filename;
					model->name				=	name;
					model->driverInstalled	=	false;

					iter->second->models.push_back(model);
				}
				break;

				default:
				{
					// pay no attention if we are in any other state
				}
				break;
			}
		}
	}

exit:

	file.Close();

	return (err);
}


// -------------------------------------------------------
// LoadPrintDriverDefs
//
// This function is responsible for loading the print driver
// definitions of all print drivers that have been installed
// on this machine.
// -------------------------------------------------------
OSStatus
CThirdPage::LoadPrintDriverDefs( Manufacturers & manufacturers )
{
	BYTE	*	buffer			=	NULL;
	DWORD		bytesReceived	=	0;
	DWORD		numPrinters		=	0;
	OSStatus	err				=	0;
	BOOL		ok;

	//
	// like a lot of win32 calls, we call this first to get the
	// size of the buffer we need.
	//
	EnumPrinterDrivers(NULL, L"all", 6, NULL, 0, &bytesReceived, &numPrinters);

	if (bytesReceived > 0)
	{
		try
		{
			buffer = new BYTE[bytesReceived];
		}
		catch (...)
		{
			buffer = NULL;
		}
	
		require_action( buffer, exit, err = kNoMemoryErr );
		
		//
		// this call gets the real info
		//
		ok = EnumPrinterDrivers(NULL, L"all", 6, buffer, bytesReceived, &bytesReceived, &numPrinters);
		err = translate_errno( ok, errno_compat(), kUnknownErr );
		require_noerr( err, exit );
	
		DRIVER_INFO_6 * info = (DRIVER_INFO_6*) buffer;
	
		for (DWORD i = 0; i < numPrinters; i++)
		{
			Manufacturer	*	manufacturer;
			Model			*	model;
			CString				name;
	
			//
			// skip over anything that doesn't have a manufacturer field.  This
			// fixes a bug that I noticed that occurred after I installed 
			// ProComm.  This program add a print driver with no manufacturer
			// that screwed up this wizard.
			//
			if (info[i].pszMfgName == NULL)
			{
				continue;
			}
	
			//
			// look for manufacturer
			//
			Manufacturers::iterator iter;
	
			//
			// save the name
			//
			name = NormalizeManufacturerName( info[i].pszMfgName );
	
			iter = manufacturers.find(name);
	
			if (iter != manufacturers.end())
			{
				manufacturer = iter->second;
			}
			else
			{
				try
				{
					manufacturer = new Manufacturer;
				}
				catch (...)
				{
					manufacturer = NULL;
				}
	
				require_action( manufacturer, exit, err = kNoMemoryErr );
	
				manufacturer->name	=	name;
	
				manufacturers[name]	=	manufacturer;
			}
	
			//
			// now look to see if we have already seen this guy.  this could
			// happen if we have already installed printers that are described
			// in ntprint.inf.  the extant drivers will show up in EnumPrinterDrivers
			// but we have already loaded their info
			//
			//
			if ( MatchModel( manufacturer, ConvertToModelName( info[i].pName ) ) == NULL )
			{
				try
				{
					model = new Model;
				}
				catch (...)
				{
					model = NULL;
				}
	
				require_action( model, exit, err = kNoMemoryErr );
	
				model->name				=	info[i].pName;
				model->driverInstalled	=	true;
	
				manufacturer->models.push_back(model);
			}
		}
	}

exit:

	if (buffer != NULL)
	{
		delete [] buffer;
	}

	return err;
}


// ------------------------------------------------------
// ConvertToManufacturerName
//
// This function is responsible for tweaking the 
// name so that subsequent string operations won't fail because
// of capitalizations/different names for the same manufacturer
// (i.e.  Hewlett-Packard/HP/Hewlett Packard)
//
CString
CThirdPage::ConvertToManufacturerName( const CString & name )
{
	//
	// first we're going to convert all the characters to lower
	// case
	//
	CString lower = name;
	lower.MakeLower();

	//
	// now we're going to check to see if the string says "hewlett-packard",
	// because sometimes they refer to themselves as "hewlett-packard", and
	// sometimes they refer to themselves as "hp".
	//
	if ( lower == L"hewlett-packard")
	{
		lower = "hp";
	}

	//
	// tweak for Xerox Phaser, which doesn't announce itself
	// as a xerox
	//
	else if ( lower.Find( L"phaser", 0 ) != -1 )
	{
		lower = "xerox";
	}

	return lower;
}


// ------------------------------------------------------
// ConvertToModelName
//
// This function is responsible for ensuring that subsequent
// string operations don't fail because of differing capitalization
// schemes and the like
// ------------------------------------------------------

CString
CThirdPage::ConvertToModelName( const CString & name )
{
	//
	// convert it to lowercase
	//
	CString lower = name;
	lower.MakeLower();

	return lower;
}


// ------------------------------------------------------
// NormalizeManufacturerName
//
// This function is responsible for tweaking the manufacturer
// name so that there are no aliases for vendors
//
CString
CThirdPage::NormalizeManufacturerName( const CString & name )
{
	CString normalized = name;

	//
	// now we're going to check to see if the string says "hewlett-packard",
	// because sometimes they refer to themselves as "hewlett-packard", and
	// sometimes they refer to themselves as "hp".
	//
	if ( normalized == L"Hewlett-Packard")
	{
		normalized = "HP";
	}

	return normalized;
}


// -------------------------------------------------------
// MatchPrinter
//
// This function is responsible for matching a printer
// to a list of manufacturers and models.  It calls
// MatchManufacturer and MatchModel in turn.
//

OSStatus CThirdPage::MatchPrinter(Manufacturers & manufacturers, Printer * printer)
{
	CString					normalizedProductName;
	Manufacturer		*	manufacturer	=	NULL;
	Model				*	model			=	NULL;
	bool					found			=	false;
	CString					text;
	OSStatus				err				=	kNoErr;

	//
	// first look to see if we have a usb_MFG descriptor
	//
	if (printer->usb_MFG.GetLength() > 0)
	{
		manufacturer = MatchManufacturer( manufacturers, ConvertToManufacturerName ( printer->usb_MFG ) );
	}

	if ( manufacturer == NULL )
	{
		printer->product.Remove('(');
		printer->product.Remove(')');

		manufacturer = MatchManufacturer( manufacturers, ConvertToManufacturerName ( printer->product ) );
	}
	
	//
	// if we found the manufacturer, then start looking for the model
	//
	if ( manufacturer != NULL )
	{
		if (printer->usb_MDL.GetLength() > 0)
		{
			model = MatchModel ( manufacturer, ConvertToModelName ( printer->usb_MDL ) );
		}

		if ( model == NULL )
		{
			printer->product.Remove('(');
			printer->product.Remove(')');

			model = MatchModel ( manufacturer, ConvertToModelName ( printer->product ) );
		}

		if ( model != NULL )
		{
			SelectMatch(printer, manufacturer, model);
			found = true;
		}
	}

	//
	// display a message to the user based on whether we could match
	// this printer
	//
	if (found)
	{
		text.LoadString(IDS_PRINTER_MATCH_GOOD);
	}
	else
	{
		text.LoadString(IDS_PRINTER_MATCH_BAD);

		//
		// if there was any crud in this list from before, get rid of it now
		//
		m_modelListCtrl.DeleteAllItems();
		
		//
		// select the manufacturer if we found one
		//
		if (manufacturer != NULL)
		{
			LVFINDINFO	info;
			int			nIndex;

			//
			// select the manufacturer
			//
			info.flags	= LVFI_STRING;
			info.psz	= manufacturer->name;

			nIndex = m_manufacturerListCtrl.FindItem(&info);
	
			if (nIndex != -1)
			{
				m_manufacturerListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
				m_manufacturerListCtrl.EnsureVisible(nIndex, FALSE);
			}
		}
	}

	m_printerSelectionText.SetWindowText(text);

	return err;
}


// ------------------------------------------------------
// MatchManufacturer
//
// This function is responsible for finding a manufacturer
// object from a string name.  It does a CString::Find, which
// is like strstr, so it doesn't have to do an exact match
//
// If it can't find a match, NULL is returned
// ------------------------------------------------------

Manufacturer*
CThirdPage::MatchManufacturer( Manufacturers & manufacturers, const CString & name)
{
	Manufacturers::iterator iter;

	for (iter = manufacturers.begin(); iter != manufacturers.end(); iter++)
	{
		//
		// we're going to convert all the manufacturer names to lower case,
		// so we match the name passed in.
		//
		CString lower = iter->second->name;
		lower.MakeLower();

		//
		// now try and find the lowered string in the name passed in.
		// 
		if (name.Find(lower) == 0)
		{
			return iter->second;
		}
	}

	return NULL;
}


// -------------------------------------------------------
// MatchModel
//
// This function is responsible for matching a model from
// a name.  It does a CString::Find(), which works like strstr,
// so it doesn't rely on doing an exact string match.
//

Model*
CThirdPage::MatchModel(Manufacturer * manufacturer, const CString & name)
{
	Models::iterator iter;

	iter = manufacturer->models.begin();

	for (iter = manufacturer->models.begin(); iter != manufacturer->models.end(); iter++)
	{
		Model * model = *iter;

		//
		// convert the model name to lower case
		//
		CString lowered = model->name;
		lowered.MakeLower();

		if (lowered.Find( name ) != -1)
		{
			return model;
		}
	}

	return NULL;
}



// -----------------------------------------------------------
// OnInitPage
//
// This function is responsible for doing initialization that
// only occurs once during a run of the wizard
//

OSStatus CThirdPage::OnInitPage()
{
	static const int		bufferSize	= 32768;
	TCHAR					windowsDirectory[bufferSize];
	CString					header;
	CString					ntPrint;
	OSStatus				err;
	BOOL					ok;
	

	//
	// The CTreeCtrl widget automatically sends a selection changed
	// message which initially we want to ignore, because the user
	// hasn't selected anything
	//
	// this flag gets reset in the message handler.  Every subsequent
	// message gets handled.
	//

	//
	// we have to make sure that we only do this once.  Typically, 
	// we would do this in something like OnInitDialog, but we don't
	// have this in Wizards, because the window is a PropertySheet.
	// We're considered fully initialized when we receive the first
	// selection notice
	//
	header.LoadString(IDS_MANUFACTURER_HEADING);
	m_manufacturerListCtrl.InsertColumn(0, header, LVCFMT_LEFT, 138);
	m_manufacturerSelected = NULL;

	header.LoadString(IDS_MODEL_HEADING);
	m_modelListCtrl.InsertColumn(0, header, LVCFMT_LEFT, 247);
	m_modelSelected = NULL;

	//
	// load printers from ntprint.inf
	//
	ok = GetWindowsDirectory( windowsDirectory, bufferSize );
	err = translate_errno( ok, errno_compat(), kUnknownErr );
	require_noerr( err, exit );
 
	ntPrint.Format(L"%s\\%s", windowsDirectory, kNTPrintFile);
	err = LoadPrintDriverDefsFromFile( m_manufacturers, ntPrint, false );
	require_noerr(err, exit);

	//
	// load printer drivers that have been installed on this machine
	//
	err = LoadPrintDriverDefs( m_manufacturers );
	require_noerr(err, exit);

exit:

	return (err);
}


void CThirdPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRINTER_MANUFACTURER, m_manufacturerListCtrl);
	DDX_Control(pDX, IDC_PRINTER_MODEL, m_modelListCtrl);
	DDX_Control(pDX, IDC_PRINTER_NAME, m_printerName);
	DDX_Control(pDX, IDC_DEFAULT_PRINTER, m_defaultPrinterCtrl);
	DDX_Control(pDX, IDC_PRINTER_SELECTION_TEXT, m_printerSelectionText);
}


// ----------------------------------------------------------
// OnSetActive
//
// This function is called by MFC after the window has been
// activated.
//

BOOL
CThirdPage::OnSetActive()
{
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );
   
	if ((m_manufacturerListCtrl.GetFirstSelectedItemPosition() != NULL) &&
	    (m_modelListCtrl.GetFirstSelectedItemPosition() != NULL))
	{
		psheet->SetWizardButtons( PSWIZB_BACK|PSWIZB_NEXT );
	}
	else
	{
		psheet->SetWizardButtons( PSWIZB_BACK );
	}

	printer = psheet->GetSelectedPrinter();
	require_quiet( printer, exit );

	//
	// call OnInitPage once
	//
	if (!m_initialized)
	{
		OnInitPage();
		m_initialized = true;
	}

	//
	// update the UI with the printer name
	//
	m_printerName.SetWindowText(printer->displayName);

	//
	// populate the list controls with the manufacturers and models
	// from ntprint.inf
	//
	PopulateUI( m_manufacturers );

	//
	// and try and match the printer
	//
	MatchPrinter( m_manufacturers, printer );

exit:

	return CPropertyPage::OnSetActive();
}


// -------------------------------------------------------
// PopulateUI
//
// This function is called to populate the list of manufacturers
//
OSStatus
CThirdPage::PopulateUI(Manufacturers & manufacturers)
{
	Manufacturers::iterator iter;
	
	m_manufacturerListCtrl.DeleteAllItems();

	for (iter = manufacturers.begin(); iter != manufacturers.end(); iter++)
	{
		int nIndex;

		Manufacturer * manufacturer = iter->second;

		nIndex = m_manufacturerListCtrl.InsertItem(0, manufacturer->name);

		m_manufacturerListCtrl.SetItemData(nIndex, (DWORD_PTR) manufacturer);
	}

	return 0;
}


BEGIN_MESSAGE_MAP(CThirdPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PRINTER_MANUFACTURER, OnLvnItemchangedManufacturer)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PRINTER_MODEL, OnLvnItemchangedPrinterModel)
	ON_BN_CLICKED(IDC_DEFAULT_PRINTER, OnBnClickedDefaultPrinter)
	ON_BN_CLICKED(IDC_HAVE_DISK, OnBnClickedHaveDisk)
END_MESSAGE_MAP()


// CThirdPage message handlers
void CThirdPage::OnLvnItemchangedManufacturer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION p = m_manufacturerListCtrl.GetFirstSelectedItemPosition();
	int nSelected = m_manufacturerListCtrl.GetNextSelectedItem(p);

	if (nSelected != -1)
	{
		m_manufacturerSelected = (Manufacturer*) m_manufacturerListCtrl.GetItemData(nSelected);

		m_modelListCtrl.SetRedraw(FALSE);
		
		m_modelListCtrl.DeleteAllItems();
		m_modelSelected = NULL;

		Models::iterator iter;

		for (iter = m_manufacturerSelected->models.begin(); iter != m_manufacturerSelected->models.end(); iter++)
		{
			Model * model = *iter;

			int nItem = m_modelListCtrl.InsertItem(0, model->name);

			m_modelListCtrl.SetItemData(nItem, (DWORD_PTR) model);
		}

		m_modelListCtrl.SetRedraw(TRUE);
	}

	*pResult = 0;
}

void CThirdPage::OnLvnItemchangedPrinterModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW					pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	printer = psheet->GetSelectedPrinter();
	require_quiet( printer, exit );

	check ( m_manufacturerSelected );

	POSITION p = m_modelListCtrl.GetFirstSelectedItemPosition();
	int nSelected = m_modelListCtrl.GetNextSelectedItem(p);

	if (nSelected != -1)
	{
		m_modelSelected = (Model*) m_modelListCtrl.GetItemData(nSelected);

		CopyPrinterSettings( printer, m_manufacturerSelected, m_modelSelected );

		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	else
	{
		psheet->SetWizardButtons(PSWIZB_BACK);
	}

exit:

	*pResult = 0;
}


void CThirdPage::OnBnClickedDefaultPrinter()
{
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	printer = psheet->GetSelectedPrinter();
	require_quiet( printer, exit );

	printer->deflt = m_defaultPrinterCtrl.GetState() ? true : false;

exit:

	return;
}

void CThirdPage::OnBnClickedHaveDisk()
{
	CPrinterSetupWizardSheet	*	psheet;
	Printer						*	printer;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, L"Setup Information (*.inf)|*.inf||", this);

	psheet = reinterpret_cast<CPrinterSetupWizardSheet*>(GetParent());
	require_quiet( psheet, exit );

	printer = psheet->GetSelectedPrinter();
	require_quiet( printer, exit );
	
	if ( dlg.DoModal() == IDOK )
	{
		Manufacturers	manufacturers;
		CString			filename = dlg.GetPathName();

		LoadPrintDriverDefsFromFile( manufacturers, filename, true );
   
		PopulateUI( manufacturers );

		MatchPrinter( manufacturers, printer );
	}

exit:

	return;
}
