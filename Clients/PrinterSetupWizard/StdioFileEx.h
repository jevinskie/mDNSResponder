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
    
$Log: StdioFileEx.h,v $
Revision 1.2  2006/08/14 23:24:09  cheshire
Re-licensed mDNSResponder daemon source code under Apache License, Version 2.0

Revision 1.1  2004/06/18 04:36:58  rpantos
First checked in


*/

#pragma once


class CStdioFileEx: public CStdioFile
{
public:

	CStdioFileEx();
	CStdioFileEx( LPCTSTR lpszFileName, UINT nOpenFlags );

	virtual BOOL	Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL );
	virtual BOOL	ReadString( CString& rString );
	ULONGLONG		GetCharCount();

	// static utility functions

	// --------------------------------------------------------------
	//
	//	CStdioFileEx::GetUnicodeStringFromMultiByteString()
	//
	// --------------------------------------------------------------
	// Returns:    bool
	// Parameters: char *		szMultiByteString		(IN)	Multi-byte input string
	//					wchar_t*		szUnicodeString		(OUT)	Unicode output string
	//					short			nUnicodeBufferSize	(IN)	Size of Unicode output buffer
	//					UINT			nCodePage				(IN)	Code page used to perform conversion
	//																		Default = -1 (Get local code page).
	//
	// Purpose:		Gets a Unicode string from a MultiByte string.
	// Notes:		None.
	// Exceptions:	None.
	//
	static bool
	GetUnicodeStringFromMultiByteString(
							char	*	szMultiByteString,
							wchar_t	*	szUnicodeString,
							short		nUnicodeBufferSize,
							UINT		nCodePage=-1);

	// --------------------------------------------------------------
	//
	//	CStdioFileEx::GetMultiByteStringFromUnicodeString()
	//
	// --------------------------------------------------------------
	// Returns:    BOOL
	// Parameters: wchar_t *	szUnicodeString			(IN)	Unicode input string
	//					char*			szMultiByteString			(OUT)	Multibyte output string
	//					short			nMultiByteBufferSize		(IN)	Multibyte buffer size
	//					UINT			nCodePage					(IN)	Code page used to perform conversion
	//																			Default = -1 (Get local code page).
	//
	// Purpose:		Gets a MultiByte string from a Unicode string.
	// Notes:		.
	// Exceptions:	None.
	//
	static BOOL
	GetMultiByteStringFromUnicodeString(
							wchar_t	*	szUnicodeString,
							char	*	szMultiByteString,
							short		nMultiByteBufferSize,
							UINT		nCodePage=-1);

	static bool CStdioFileEx::IsFileUnicode(
							const CString& sFilePath);

protected:

	UINT		ProcessFlags(const CString& sFilePath, UINT& nOpenFlags);

	bool		m_bIsUnicodeText;
	UINT		m_nFlags;
};
