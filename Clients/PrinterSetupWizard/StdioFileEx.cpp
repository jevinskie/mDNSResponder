/* -*- Mode: C; tab-width: 4 -*-
 *
 * Copyright (c) 1997-2004 Apple Computer, Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

    Change History (most recent first):
    
$Log: StdioFileEx.cpp,v $
Revision 1.3  2006/08/14 23:24:09  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.2  2004/06/26 03:19:57  shersche
clean up warning messages

Submitted by: herscher

Revision 1.1  2004/06/18 04:36:58  rpantos
First checked in


*/

#include "stdafx.h"
#include "StdioFileEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define	nUNICODE_BOM					0xFEFF		// Unicode "byte order mark" which goes at start of file
#define	sNEWLINE						_T("\r\n")	// New line characters
#define	sDEFAULT_UNICODE_FILLER_CHAR	"#"			// Filler char used when no conversion from Unicode to local code page is possible

// local variable is initialize but not referenced
#pragma warning(disable:4189)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStdioFileEx::CStdioFileEx(): CStdioFile()
{
	m_bIsUnicodeText = false;
}

CStdioFileEx::CStdioFileEx(LPCTSTR lpszFileName,UINT nOpenFlags)
	:CStdioFile(lpszFileName, ProcessFlags(lpszFileName, nOpenFlags))
{
}


BOOL CStdioFileEx::Open(LPCTSTR lpszFileName,UINT nOpenFlags,CFileException* pError /*=NULL*/)
{
	// Process any Unicode stuff
	ProcessFlags(lpszFileName, nOpenFlags);

	return CStdioFile::Open(lpszFileName, nOpenFlags, pError);
}

BOOL CStdioFileEx::ReadString(CString& rString)
{
	const int	nMAX_LINE_CHARS = 4096;
	BOOL			bReadData;
	LPTSTR		lpsz;
	int			nLen = 0; //, nMultiByteBufferLength = 0, nChars = 0;
	CString		sTemp;
	wchar_t*		pszUnicodeString = NULL;
	char	*		pszMultiByteString= NULL;

	// If at position 0, discard byte-order mark before reading
	if (!m_pStream || (GetPosition() == 0 && m_bIsUnicodeText))
	{
		wchar_t	cDummy;
		Read(&cDummy, sizeof(wchar_t));
	}

	// If compiled for Unicode
#ifdef _UNICODE
	// Do standard stuff -- both ANSI and Unicode cases seem to work OK
	bReadData = CStdioFile::ReadString(rString);
#else

	if (!m_bIsUnicodeText)
	{
		// Do standard stuff -- read ANSI in ANSI
		bReadData = CStdioFile::ReadString(rString);
	}
	else
	{
		pszUnicodeString	= new wchar_t[nMAX_LINE_CHARS]; 
		pszMultiByteString= new char[nMAX_LINE_CHARS];  

		// Read as Unicode, convert to ANSI; fixed by Dennis Jeryd 6/8/03
		bReadData = (NULL != fgetws(pszUnicodeString, nMAX_LINE_CHARS, m_pStream));
		if (GetMultiByteStringFromUnicodeString(pszUnicodeString, pszMultiByteString, nMAX_LINE_CHARS))
		{
			rString = (CString)pszMultiByteString;
		}

		if (pszUnicodeString)
		{
			delete pszUnicodeString;
		}

		if (pszMultiByteString)
		{
			delete pszMultiByteString;
		}
	}
#endif

	// Then remove end-of-line character if in Unicode text mode
	if (bReadData)
	{
		// Copied from FileTxt.cpp but adapted to Unicode and then adapted for end-of-line being just '\r'. 
		nLen = rString.GetLength();
		if (nLen > 1 && rString.Mid(nLen-2) == sNEWLINE)
		{
			rString.GetBufferSetLength(nLen-2);
		}
		else
		{
			lpsz = rString.GetBuffer(0);
			if (nLen != 0 && (lpsz[nLen-1] == _T('\r') || lpsz[nLen-1] == _T('\n')))
			{
				rString.GetBufferSetLength(nLen-1);
			}
		}
	}

	return bReadData;
}


UINT CStdioFileEx::ProcessFlags(const CString& sFilePath, UINT& nOpenFlags)
{
	m_bIsUnicodeText = false;

	// If reading in text mode and not creating... ; fixed by Dennis Jeryd 6/8/03
	if (nOpenFlags & CFile::typeText && !(nOpenFlags & CFile::modeCreate) && !(nOpenFlags & CFile::modeWrite ))
	{
		m_bIsUnicodeText = IsFileUnicode(sFilePath);

		// If it's Unicode, switch to binary mode
		if (m_bIsUnicodeText)
		{
			nOpenFlags ^= CFile::typeText;
			nOpenFlags |= CFile::typeBinary;
		}
	}

	m_nFlags = nOpenFlags;

	return nOpenFlags;
}

// ------------------------------------------------------
//
//	CStdioFileEx::IsFileUnicode()
//
// ------------------------------------------------------
// Returns:    bool
// Parameters: const CString& sFilePath
//
// Purpose:		Determines whether a file is Unicode by reading the first character and detecting
//					whether it's the Unicode byte marker.
// Notes:		None.
// Exceptions:	None.
//
/*static*/ bool CStdioFileEx::IsFileUnicode(const CString& sFilePath)
{
	CFile				file;
	bool				bIsUnicode = false;
	wchar_t			cFirstChar;
	CFileException	exFile;

	// Open file in binary mode and read first character
	if (file.Open(sFilePath, CFile::typeBinary | CFile::modeRead, &exFile))
	{
		// If byte is Unicode byte-order marker, let's say it's Unicode
		if (file.Read(&cFirstChar, sizeof(wchar_t)) > 0 && cFirstChar == (wchar_t)nUNICODE_BOM)
		{
			bIsUnicode = true;
		}

		file.Close();
	}
	else
	{
		// Handle error here if you like
	}

	return bIsUnicode;
}


ULONGLONG CStdioFileEx::GetCharCount()
{
	int				nCharSize;
	ULONGLONG		nByteCount;
	ULONGLONG	    nCharCount = 0;

	if (m_pStream)
	{
		// Get size of chars in file
		nCharSize = m_bIsUnicodeText ? sizeof(wchar_t): sizeof(char);

		// If Unicode, remove byte order mark from count
		nByteCount = GetLength();
		
		if (m_bIsUnicodeText)
		{
			nByteCount = nByteCount - sizeof(wchar_t);
		}

		// Calc chars
		nCharCount = (nByteCount / nCharSize);
	}

	return nCharCount;
}

// ------------------------------------------------------------
//
//	CStdioFileEx::GetUnicodeStringFromMultiByteString()
//
// ------------------------------------------------------------
// Returns:    bool
// Parameters: char *		szMultiByteString		(IN)	Multi-byte input string
//					wchar_t*		szUnicodeString		(OUT)	Unicode outputstring
//					short			nUnicodeBufferSize	(IN)	Size of Unicode output buffer
//					UINT			nCodePage				(IN)	Code page used to perform conversion
//																		Default = -1 (Get local code page).
//
// Purpose:		Gets a Unicode string from a MultiByte string.
// Notes:		None.
// Exceptions:	None.
//
bool CStdioFileEx::GetUnicodeStringFromMultiByteString(
										char	*	szMultiByteString,
										wchar_t	*	szUnicodeString,
										short		nUnicodeBufferSize,
										UINT		nCodePage)
{
	bool		bOK = true;
	int		nReturn = 0;
	CString	sErrorMsg;
		
	if (szUnicodeString && szMultiByteString)
	{
		// If no code page specified, take default for system
		if (nCodePage == -1)
		{
			nCodePage = GetACP();
		}

		try 
		{
			nReturn = MultiByteToWideChar(nCodePage,MB_PRECOMPOSED,szMultiByteString,-1,szUnicodeString,nUnicodeBufferSize);

			if (nReturn == 0)
			{
				bOK = false;
			}
		}
		catch(...)
		{
			bOK = false;
		}
	}
	else
	{
		bOK = false;
	}

	ASSERT(bOK);
	return bOK;
}

// -----------------------------------------------------------
//
//	CStdioFileEx::GetMultiByteStringFromUnicodeString()
//
// -----------------------------------------------------------
// Returns:    BOOL
// Parameters: wchar_t *	szUnicodeString			(IN)	Unicode input string
//					char*			szMultiByteString			(OUT)	Multibyte output string
//					short			nMultiByteBufferSize		(IN)	Multibyte buffer size
//					UINT			nCodePage					(IN)	Code page used to perform conversion
//																			Default = -1 (Get local code page).
//
// Purpose:		Gets a MultiByte string from a Unicode string
// Notes:		None.
// Exceptions:	None.
//
BOOL CStdioFileEx::GetMultiByteStringFromUnicodeString(
									wchar_t	*	szUnicodeString,
									char	*	szMultiByteString, 
									short		nMultiByteBufferSize,
									UINT		nCodePage)
{
	BOOL			bUsedDefChar	= FALSE;
	BOOL			bGotIt = FALSE;

	if (szUnicodeString && szMultiByteString) 
	{
		// If no code page specified, take default for system
		if (nCodePage == -1)
		{
			nCodePage = GetACP();
		}

		try 
		{
			bGotIt = WideCharToMultiByte(nCodePage, WC_COMPOSITECHECK | WC_SEPCHARS,
							szUnicodeString,-1, szMultiByteString, nMultiByteBufferSize, sDEFAULT_UNICODE_FILLER_CHAR, &bUsedDefChar);
		}
		catch(...) 
		{
			TRACE(_T("Controlled exception in WideCharToMultiByte!\n"));
		}
	} 

	return bGotIt;
}
