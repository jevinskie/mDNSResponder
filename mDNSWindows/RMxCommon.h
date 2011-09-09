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

$Log: RMxCommon.h,v $
Revision 1.1  2004/01/30 02:35:13  bradley
Rendezvous Message Exchange implementation for DNS-SD IPC on Windows.

*/

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		RMxCommon.h
	
	@abstract	Common code between the RMxClient and RMxServer.
	
	@discussion	
	
	This handles establishing and accepting connections, the underying message sending and receiving, portable data 
	packing an unpacking, and shared utility routines.
*/

#ifndef __RMx_COMMON__
#define __RMx_COMMON__

#include	"CommonServices.h"
#include	"DebugServices.h"

#ifdef	__cplusplus
	extern "C" {
#endif

#if 0
#pragma mark == Documentation ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@header		RMxCommon
	
	@abstract	Rendezvous Message Exchange (RMx) protocol.
	
	@discussion
	
	Data Type Notes
	===============
	
	All multi-byte data types are transmitted in network byte order (i.e. big endian).
	
	Data types requiring the use of specific values (e.g. opcodes) are typedef'd to an abstract type. Data types used 
	in this manner can be easily searched to determine the appropriate values for the data type. Modern development 
	environments can jump directly to a type and therefore the appropriate constants, which speeds up development.
	
	Naming Conventions
	==================
	
	All data types and related constants are named to make it easy to associate them. The general scheme is:
	
	Data Types: RMx<type>
	
		<type>	Describes the data type (e.g. "OpCode" is used for operation codes).
		
		For example: RMxOpCode
		
	Constants:	kRMx<type><name>
	
		<type>	Describes the data type (e.g. "OpCode" is used for operation codes in RMx messages).
		<name>	Symbolic name for the constant (e.g. "Browse" for browse messages).
		
		For example: kRMxOpCodeBrowse
	
	These conventions make it easier for users to associate data types with their values and to find places where 
	these data types and constants are used. You can always search for the base portion of symbolic name (e.g. 
	"RMxOpCode" in kRMxOpCodeBrowse) and find all the data types and constants associated with it. Modern 
	development environments also allow you to jump directly to the data type or constant.
	
	Packet Format
	=============
	
	All data exchanged between client and server is encapsulated in packets of a specific format. There is a 
	generic header followed by an optional, opcode-specific data section:

	                        1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 0 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                       signature (RMx1)                        |
	 4 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                            opcode                             |
	 8 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                            flags                              |
	12 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                             xid                               |
	16 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                            status                             |
	20 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                             size                              |
	24 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      data ("size" octets)                     |
	   \                                                               \
	   \                                                               \
	   |                                                               |
	 n +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
	The packet format uses implicit typing, which means the sender and receiver must know the types and ordering of the 
	data based on the opcode of the message. There are standard primitive data types defined that all data structures
	are built on:
	
	Byte 		- 8-bit value. Uses the "b" pack/unpack format specifier.
	Half-Word 	- 16-bit value. Uses the "h" pack/unpack format specifier.
	Word		- 32-bit value. Uses the "w" pack/unpack format specifier.
	String		- UTF-8 string, null-terminated. Uses the "s" pack/unpack format specifier.
	Raw Bytes	- 32-bit length-prefixed block of data. Uses the "n" pack/unpack format specifier.
	
	The String and Raw Byte data types are designed to support directly referencing the raw packet data without 
	copying them into a temporary buffer for efficiency, easy of use, and to reduce the possibility of resource
	leaks. The String data type contains a null terminator in the packet data so it can be treated as a normal 
	C-style string (e.g. strcpy works on it). The Raw Bytes data type can be used via a pointer and size. The data 
	packing and unpacking routines are designed around the packet format to allow for efficient packet handling.
	
	Protocol
	========
	
	The RMx protocol is a TCP-based client/server protocol. The server listens on a well-known TCP port. The client 
	connects to the server port and a session of message exchange begins. The client drives the session by sending
	the initial message describing the action it wants performed by the server. The server performs operations
	requested by the client and asynchronously sends reply messages as specified by the type of request.
	
	The general operation of the RMx server is as follows:
	
		1) Initialize server (set up synchronization support, etc.).
		2) Run server by creating socket(s) for listening and wait until a connection is accepted.
		3) If a connection is accepted:
			3a) Accept session (allocate, link into session list, create session thread, etc.).

	The general operation of the RMx client is as follows:
	
		1) Initialize client (set up synchronization support, etc.).
		2) Start a session to the server.
	
	The general operation of the RMx session thread (used by the client and server) is as follows:
	
		1) Initialize session (set up synchronization support, etc.).
		2) Wait for a message to be received.
		3) If a message is received:
			3a) Process the message.
*/

#if 0
#pragma mark == Versions ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	kRMxCurrentVersion

	@abstract	Current version of the RMx implementation.
*/

#define kRMxCurrentVersion			NumVersionBuild( 1, 0, 0, kVersionStageFinal, 0 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	kRMxOldestClientVersion

	@abstract	Oldest version of the RMx client that works with this server.
*/

#define kRMxOldestClientVersion		NumVersionBuild( 1, 0, 0, kVersionStageFinal, 0 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	kRMxOldestServerVersion

	@abstract	Oldest version of the RMx server that works with this client.
*/

#define kRMxOldestServerVersion		NumVersionBuild( 1, 0, 0, kVersionStageFinal, 0 )

#if 0
#pragma mark == General ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kRMxServerPort

	@abstract	TCP port of DNS-SD server in host byte order.
*/

#define	kRMxServerPort				5354
#define	kRMxServerPortString		"5354"

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kRMxClientTimeout

	@abstract	Default client timeout.
*/

#define	kRMxClientTimeout		( 5 * 1000 )	// 5 seconds

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defined	kDNSRecordIndexDefault

	@abstract	Record index for the default TXT record.
*/

#define	kDNSRecordIndexDefaultTXT		0

//---------------------------------------------------------------------------------------------------------------------------
/*!	@enum		RMxState

	@abstract	State of the RMx engine.
	
	@constant	kRMxStateInvalid	Not in a valid state.
	@constant	kRMxStateStop		Stopped.
	@constant	kRMxStateRun		Running.
*/

typedef enum
{
	kRMxStateInvalid, 
	kRMxStateStop, 
	kRMxStateRun
	
}	RMxState;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@var		gRMxState

	@abstract	Global state of the RMx engine.
*/

extern RMxState		gRMxState;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@var		gRMxState

	@abstract	Global state change event associated with the RMx engine.
*/

extern HANDLE		gRMxStateChangeEvent;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxInitialize

	@abstract	Initializes the RMx engine.
*/

OSStatus	RMxInitialize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxFinalize

	@abstract	Finalizes the RMx engine.
*/

void	RMxFinalize( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxLock

	@abstract	Locks the RMx engine.
*/

void	RMxLock( void );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxUnlock

	@abstract	Unlocks the RMx engine.
*/

void	RMxUnlock( void );

#if 0
#pragma mark == Messages ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxSignature

	@abstract	Signature for RMx messages.
	
	@constant	kRMxSignatureVersion1		Version 1 message.
*/

typedef uint32_t	RMxSignature;

#define	kRMxSignatureVersion1		0x524D7831	// 'RMx1' Version 1 message.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxFlags

	@abstract	Flags for RMx messages.
	
	@constant	kRMxFlagsNone	No flags.
*/

typedef uint32_t	RMxFlags;

#define	kRMxFlagsNone		0

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxOpCode

	@abstract	Operation code for RMx messages.
	
	@constant	kRMxOpCodeInvalid				Invalid message.
	@constant	kRMxOpCodeCheckVersion			DNSServiceCheckVersion message.
	@constant	kRMxOpCodeCopyProperty			DNSServiceCopyProperty message.
	@constant	kRMxOpCodeEnumerateDomains		DNSServiceEnumerateDomains message.
	@constant	kRMxOpCodeRegister				DNSServiceRegister message.
	@constant	kRMxOpCodeAddRecord				DNSServiceAddRecord message.
	@constant	kRMxOpCodeUpdateRecord			DNSServiceUpdateRecord message.
	@constant	kRMxOpCodeRemoveRecord			DNSServiceRemoveRecord message.
	@constant	kRMxOpCodeBrowse				DNSServiceBrowse message.
	@constant	kRMxOpCodeResolve				DNSServiceResolve message.
	@constant	kRMxOpCodeCreateConnection		DNSServiceCreateConnection message.
	@constant	kRMxOpCodeRegisterRecord		DNSServiceRegisterRecord message.
	@constant	kRMxOpCodeQueryRecord			DNSServiceQueryRecord message.
	@constant	kRMxOpCodeReconfirmRecord		DNSServiceReconfirmRecord message.
*/

typedef uint32_t	RMxOpCode;

#define	kRMxOpCodeInvalid				0

#define kRMxOpCodeCheckVersion			10
	// Request:	<w:clientCurrentVersion> <w:clientOldestClientVersion> <w:clientOldestServerVersion> 
	// Reply:	<w:errorCode> <w:serverCurrentVersion> <w:serverOldestClientVersion> <w:serverOldestServerVersion> 

#define kRMxOpCodeCopyProperty			11
	// Request:	<w:propertyCode> 
	// Reply:	<w:errorCode> <w:propertyCode> <n:propertyData>

#define kRMxOpCodeEnumerateDomains		100
	// Request:	<w:flags> <w:interfaceIndex>
	// Reply:	<w:flags> <w:interfaceIndex> <w:errorCode> <s:domain>

#define kRMxOpCodeRegister				200
	// Request:	<w:flags> <w:interfaceIndex> <s:name> <s:type> <s:domain> <s:host> <h:port> <n:txt>
	// Reply:	<w:flags> <w:errorCode> <s:name> <s:type> <s:domain>

#define kRMxOpCodeAddRecord				201
	// Request:	<w:id> <w:flags> <h:rrType> <n:rrData> <w:ttl>
	// Reply:	no reply

#define kRMxOpCodeUpdateRecord			202
	// Request:	<w:id> <w:flags> <n:rrData> <w:ttl>
	// Reply:	no reply

#define kRMxOpCodeRemoveRecord			203
	// Request:	<w:id> <w:flags>
	// Reply:	no reply

#define kRMxOpCodeBrowse				300
	// Request:	<w:flags> <w:interfaceIndex> <s:type> <s:domain>
	// Reply:	<w:flags> <w:interfaceIndex> <w:errorCode> <s:name> <s:type> <s:domain>

#define kRMxOpCodeResolve				400
	// Request:	<w:flags> <w:interfaceIndex> <s:name> <s:type> <s:domain>
	// Reply:	<w:flags> <w:interfaceIndex> <w:errorCode> <s:fullName> <n:addr> <s:hostName> <h:port> <n:txtRecord>

#define kRMxOpCodeCreateConnection		500
	// Request:	no data
	// Reply:	no reply

#define	kRMxOpCodeRegisterRecord		510
	// Request:	<w:flags> <w:interfaceIndex> <s:name> <h:rrType> <h:rrClass> <n:rrData> <w:ttl>
	// Reply:	<w:flags> <w:errorCode> <w:recordID>

#define	kRMxOpCodeQueryRecord			511
	// Request:	<w:flags> <w:interfaceIndex> <s:name> <h:rrType> <h:rrClass>
	// Reply:	<w:flags> <w:interfaceIndex> <w:errorCode> <s:name> <h:rrType> <h:rrClass> <n:rrData> <w:ttl>

#define	kRMxOpCodeReconfirmRecord		512
	// Request:	<w:flags> <w:interfaceIndex> <s:name> <h:rrType> <h:rrClass> <n:rrData>
	// Reply:	no reply

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxMessageCallBack

	@abstract	Callback function for a message.
*/

typedef struct RMxMessage	RMxMessage;

typedef void ( *RMxMessageCallBack )( RMxMessage *inMessage );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxSessionRef

	@abstract	References to a session.
*/

typedef struct	RMxSession *		RMxSessionRef;

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		RMxMessage

	@abstract	RMx message.
	
	@field		signature			Signature identifying message as an RMx message (and for future versioning).
	@field		opcode				Operation code of the message.
	@field		flags				Flags for the message.
	@field		xid					Transaction ID. WARNING: Must be unique within execution environment (e.g. process).
	@field		status				Status of the message (used to return error replies).
	
	@field		sendSize			Size of data to send.
	@field		sendData			Data to send.
	@field		recvSize			Size of received data.
	@field		recvData			Received data.
	
	@field		context				Context for the completion callback (specified when session started).
	@field		session				Session where the message is sent/received.
	
	@field		bufferSize			PRIVATE: Size of entire message buffer or 0 if message not constructed.
	@field		buffer				PRIVATE: Ptr to entire message buffer or NULL if message not constructed.
	@field		storage				PRIVATE: Local storage for small messages.

	@constant	kRMxMessageSynchronous			Meta-message indicating operation should be performed synchronously.
	@constant	kRMxMessageHeaderSize			Size of the header section of an RMx message packet.
	@constant	kRMxMessageInlineBufferSize		Size of the inline message buffer.
	
	@discussion
	
	Meanings of the field descriptions:
	
	PRIVATE - Indicates field is private and should not be touched.
	IN		- Input field that must be set up before passing the message to RMx.
	OUT		- Output field that whose value on input will be ignore and replaced with a new value on completion.
	
	When a message is passed to RMx, the message becomes the ownership of RMx until completion of the message. 
	No fields of the messages should be touched while a message is in use by RMx.
	
	When sending messages asynchronously, the underlying storage for the message must be valid for the duration
	of the message. This means stack-based storage cannot be for asynchronous messages because when the function
	that allocated the stack-based variable returns, the message storage will go out of scope and become invalid.
*/

#define	kRMxMessageHeaderSize			24		// <w:signature> <w:opcode> <w:flags> <w:xid> <w:status> <w:size>
#define	kRMxMessageInlineBufferSize		1024	

check_compile_time( kRMxMessageHeaderSize <= kRMxMessageInlineBufferSize );

struct	RMxMessage
{
	RMxSignature			signature;
	RMxOpCode				opcode;
	RMxFlags				flags;
	uint32_t				xid;
	OSStatus				status;

	uint32_t				sendSize;
	uint8_t *				sendData;
	uint32_t				recvSize;
	uint8_t *				recvData;
	
	void *					context;
	RMxSessionRef			session;
	
	// PRIVATE (internal use only)
	
	uint32_t				bufferSize;
	uint8_t *				buffer;
	uint8_t					storage[ kRMxMessageInlineBufferSize ];
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxMessageInitialize

	@abstract	Initializes an RMx message structure. Must be called before the message structure can be used.
*/

void	RMxMessageInitialize( RMxMessage *inMessage );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxMessageRelease

	@abstract	Releases the resources used by an RMx message.
*/

void	RMxMessageRelease( RMxMessage *inMessage );

#if 0
#pragma mark == Sessions ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@typedef	RMxSessionFlags

	@abstract	Session flags.
	
	@constant	kRMxSessionFlagsNone			No flags.
	@constant	kRMxSessionFlagServer			Session is operating in server mode (replying to requests).
	@constant	kRMxSessionFlagsNoThread		Do not create a thread to process messages (i.e. one-shot sync session).
	@constant	kRMxSessionFlagsThreadDone		Thread is no longer active.
	@constant	kRMxSessionFlagsNoClose			Do not close the session when the thread exits.
*/

typedef uint32_t		RMxSessionFlags;

#define	kRMxSessionFlagsNone			0
#define	kRMxSessionFlagsServer			( 1 << 0 )
#define	kRMxSessionFlagsNoThread		( 1 << 1 )
#define	kRMxSessionFlagsThreadDone		( 1 << 2 )
#define	kRMxSessionFlagsNoClose			( 1 << 3 )

//---------------------------------------------------------------------------------------------------------------------------
/*!	@struct		RMxSession

	@abstract	Data structure for a session.
*/

typedef struct	RMxSession		RMxSession;
struct	RMxSession
{
	RMxSessionRef			next;
	RMxSessionFlags			flags;
	SocketRef				sock;
	HANDLE					sockEvent;
	HANDLE					closeEvent;
	HANDLE					thread;
	unsigned				threadID;
	DWORD					waitCount;
	HANDLE					waitHandles[ 3 ];
	bool					quit;
	
	RMxMessageCallBack		callback;
	RMxMessage				message;
	uint8_t *				messageRecvBuffer;
	uint8_t *				messageRecvBufferPtr;
	int						messageRecvRemaining;
	bool					messageRecvHeaderDone;
};

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxSessionOpen

	@abstract	Opens a session and optionally sends a formatted message.
*/

OSStatus
	RMxSessionOpen( 
		const char *		inServer, 
		RMxSessionFlags		inFlags, 
		SocketRef 			inSock, 
		RMxMessageCallBack 	inCallBack, 
		void *				inContext, 
		RMxSessionRef *		outSession, 
		RMxOpCode 			inMessageOpCode, 
		const char *		inMessageFormat, 
		... );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxSessionClose

	@abstract	Closes a session.
*/

OSStatus	RMxSessionClose( RMxSessionRef inSession, OSStatus inReason );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxSessionSendMessage

	@abstract	Sends a formatted message.
*/

OSStatus	RMxSessionSendMessage( RMxSessionRef inSession, RMxOpCode inOpCode, OSStatus inStatus, const char *inFormat, ... );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxSessionRecvMessage

	@abstract	Synchronously receives a message.
*/

OSStatus	RMxSessionRecvMessage( RMxSessionRef inSession, DWORD inTimeout );

#if 0
#pragma mark == Utilities ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxCheckVersion

	@abstract	Compares client and server version information and returns if they are compatible.
*/

OSStatus
	RMxCheckVersion( 
		uint32_t inClientCurrentVersion, uint32_t inClientOldestClientVersion, uint32_t inClientOldestServerVersion, 
		uint32_t inServerCurrentVersion, uint32_t inServerOldestClientVersion, uint32_t inServerOldestServerVersion );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxPackedSize

	@abstract	Determines the number of bytes required to pack data using the specified format string and arguments.
*/

OSStatus	RMxPackedSize( size_t *outSize, const char *inFormat, ... );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxPackedSizeVAList

	@abstract	Determines the number of bytes required to pack data using the format string and argument list.
*/

OSStatus	RMxPackedSizeVAList( size_t *outSize, const char *inFormat, va_list inArgs );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxPack

	@abstract	Packs data into a buffer using the format string and arguments. 
*/

OSStatus	RMxPack( void *inBuffer, size_t inMaxSize, size_t *outSize, const char *inFormat, ... );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxPackVAList

	@abstract	Packs data into a buffer using the format string and argument list. 
*/

OSStatus	RMxPackVAList( void *inBuffer, size_t inMaxSize, size_t *outSize, const char *inFormat, va_list inArgs );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxUnpack

	@abstract	Unpacks data from a buffer using the format string and arguments.
*/

OSStatus	RMxUnpack( const void *inData, size_t inSize, const char *inFormat, ... );

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	RMxUnpackVAList

	@abstract	Unpacks data from a buffer using the format string and argument list.
*/

OSStatus	RMxUnpackVAList( const void *inData, size_t inSize, const char *inFormat, va_list inArgs );

#ifdef	__cplusplus
	}
#endif

#endif  // __RMx_COMMON__
