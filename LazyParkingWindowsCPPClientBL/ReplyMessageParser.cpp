#include "pch.h"
#include "ReplyMessageParser.h"
#include "Exceptions.h"

#define TC_RESET 0x79
#define TC_NULL 0x70
#define TC_OBJECT 0x73
#define TC_CLASSDESC 0x72
#define TC_STRING 0x74
#define TC_ENDBLOCKDATA 0x78
#define TC_BLOCKDATA 0x77
#define TC_ENUM 0x7E
#define TC_REFERENCE 0x71
#define TEMP_STRING_SIZE 512
#define MESSAGE_ERROR_EXCEPTION SocketReadException("Message ended ubruptly or wrong message format")
#define ASSERT(PREDICATE) if (!(PREDICATE)) {throw MESSAGE_ERROR_EXCEPTION;}
#define ASSERT_SIZE(NUM_OF_BYTES_TO_READ) ASSERT(NUM_OF_BYTES_TO_READ == numberOfBytesRead)

using namespace LazyParkingWindowsCPPClientBL;

ReplyMessageParser::ReplyMessageParser(ITokenProvider& i_Provider): m_Provider(i_Provider)
{
	m_IsFirstMessage = true;
}

LazyParkingWindowsCPPClientBL::ReplyMessage ReplyMessageParser::GetReply()
{
	int numberOfBytesRead;
	unsigned long long dateFieldTemp;
	unsigned int intFieldTemp;
	unsigned short stringLength;
	unsigned char* tempBytePtr;
	const char* messageStringPtr;
	eReply tempEReply;
	eRequestType tempERequestType;
	ReplyMessage reply;

	if (m_IsFirstMessage) // skip stream magic number and stream version
	{
		m_Provider.ReadBytes(4, numberOfBytesRead);
		ASSERT_SIZE(4)
		m_IsFirstMessage = false;
	}

	m_Provider.ReadBytes(195, numberOfBytesRead); // skip header
	if (numberOfBytesRead == 0) // there is no reply to parse
	{
		throw MESSAGE_ERROR_EXCEPTION;
	}

	ASSERT_SIZE(195)
	// read boolField1
	tempBytePtr = (unsigned char*)m_Provider.ReadBytes(1, numberOfBytesRead);
	ASSERT_SIZE(1)
	reply.SetBoolField1(*tempBytePtr == 1);
	// read boolField2
	tempBytePtr = (unsigned char*)m_Provider.ReadBytes(1, numberOfBytesRead);
	ASSERT_SIZE(1)
	reply.SetBoolField2(*tempBytePtr == 1);
	// read intField
	ASSERT(m_Provider.ReadInt32(intFieldTemp))
	reply.SetIntField((int) intFieldTemp);
	// parse dateField
	tempBytePtr = (unsigned char*)m_Provider.ReadBytes(1, numberOfBytesRead);
	ASSERT_SIZE(1)
	switch (*tempBytePtr)
	{
	case TC_NULL:
		// do nothing since dateField is already NULL_DATE
		break;
	case TC_OBJECT:
		m_Provider.ReadBytes(32, numberOfBytesRead); // skip header
		ASSERT_SIZE(32)
		ASSERT(m_Provider.ReadInt64(dateFieldTemp)) // read dateField
		reply.SetDateField(dateFieldTemp / 1000);
		m_Provider.ReadBytes(1, numberOfBytesRead); // skip footer
		ASSERT_SIZE(1)
		break;
	default:
		throw MESSAGE_ERROR_EXCEPTION;
		break;
	}

	// parse reply field
	m_Provider.ReadBytes(59, numberOfBytesRead); // skip header
	ASSERT_SIZE(59)
	// read reply field
	ASSERT(m_Provider.ReadInt16(stringLength)) // read the length of the following message string
	messageStringPtr = (char*)m_Provider.ReadBytes(stringLength, numberOfBytesRead); // read message string
	ASSERT_SIZE(stringLength)
	ASSERT(LazyParkingWindowsCPPClientBL::eReplyFromString(string(messageStringPtr, stringLength), tempEReply));
	reply.SetReply(tempEReply);
	// parse stringField
	tempBytePtr = (unsigned char*)m_Provider.ReadBytes(1, numberOfBytesRead);
	ASSERT_SIZE(1)
	switch (*tempBytePtr)
	{
	case TC_NULL:
		// do nothing since stringField is already empty string
		break;
	case TC_STRING:
		// read stringField
		ASSERT(m_Provider.ReadInt16(stringLength)) // read the length of the following message string
		messageStringPtr = (char*)m_Provider.ReadBytes(stringLength, numberOfBytesRead); // read message string
		ASSERT_SIZE(stringLength)
		reply.SetStringField(string(messageStringPtr, stringLength));
		break;
	default:
		throw MESSAGE_ERROR_EXCEPTION;
		break;
	}

	// parse type field
	m_Provider.ReadBytes(40, numberOfBytesRead); // skip header
	ASSERT_SIZE(40)
	// read type field
	ASSERT(m_Provider.ReadInt16(stringLength)) // read the length of the following message string
	messageStringPtr = (char*)m_Provider.ReadBytes(stringLength, numberOfBytesRead); // read message string
	ASSERT_SIZE(stringLength)
	ASSERT(LazyParkingWindowsCPPClientBL::eRequestTypeFromString(string(messageStringPtr, stringLength), tempERequestType));
	reply.SetType(tempERequestType);

	return reply;
}
// STREAM_MAGIC: 0xaced
// STREAM_VERSION 0x0005
// TC = Type Code
// TC_RESET = 0x79 'y'
// TC_NULL = 0x70 'p'
// TC_OBJECT = 0x73 's'
// TC_CLASSDESC = 0x72 'r'
// TC_STRING = 0x74 't'
// TC_ENDBLOCKDATA = 0x78 'x'
// TC_BLOCKDATA = 0x77 'w'
// TC_ENUM = 0x7E '~'
// TC_REFERENCE = 0x71 'q'
// ------------------------------------------------
// THE FIRST 4 BYTES OF THE STREAM SHOULD BE SKIPPED -THIS IS THE MAGIC NUMBER + VERSION NUMBER
// THE FIRST 195 BYTES OF EVERY REPLY MESSAGE SHOULD BE SKIPPED - THIS IS THE RESET TOKEN + THE HEADER INFORMATION
// REPLY MESSAGE STRUCTURE:
// - 1 byte representing boolField1, may either be 1 or 0; 
// - 1 byte representing boolField2, may either be 1 or 0;
// - 4 bytes representing intField;
// - 1 byte - either 'TC_NULL' (if dateField is null) or 'TC_OBJECT' (otherwise);
//  if dateField is not null it is represented as follows:
//  1 + 2 + 14 + 8 + 1 + 6 = 32 bytes header information;
//  8 bytes int representing the number of milliseconds since 1/1/1970;
//  1 byte footer;
// - the reply field is represented as follows:
//  4 + 12 + 8 + 1 + 6 + 14 + 8 + 1 + 4 + 1 = 59 bytes header information;
//  2 bytes represeting the number of (Java's modified UTF-8) chars in the following string;
//  x bytes representing a string of (Java's modified UTF-8) chars that is one of the Reply enum string constants ("SUCCESS", "USERNAME_NOT_FOUND", "WRONG_PW", "INSUFFICIENT_PRIVILEDGE", "DATE_EXPIRED" or "ID_NOT_FOUND");
// - 1 byte - either 'TC_NULL' (if stringField is null) or 'TC_STRING' (otherwise);
//  if stringField is not null it is represented as follows:
//  2 bytes represeting the number of (Java's modified UTF-8) chars in the following string;
//  x bytes representing a string of (Java's modified UTF-8) chars;
// - the type field is represented as follows:
//  4 + 18 + 18 = 40 bytes header information;
//  2 bytes represeting the number of (Java's modified UTF-8) chars in the following string;
//  x bytes representing a string of (Java's modified UTF-8) chars that is one of the RequestType enum string constants ("LOGIN", "LOGOUT", "OPEN_GATE", "GET_PARKING_STATUS", "CHANGE_PW", "RESERVE_PARKING_SPOT", "ADD_DRIVER", "REMOVE_DRIVER", "CANCEL_RESERVATION", "CHANGE_EXPIRATION" or "GET_NEXT_EXPIRED");
// ------------------------------------------------