#include "pch.h"
#include "RequestSerializer.h"

using namespace LazyParkingWindowsCPPClientBL;

void RequestSerializer::writeClassDesc()
{
	m_Consumer.WriteBytes(&TC_CLASSDESC, 1);
	writeUTF("common.RequestMessage"); // write className
	m_Consumer.WriteInt64(0x1); // write serialVersionUID
	// write fields:
	unsigned char classDescFlags = 0x2;
	m_Consumer.WriteBytes(&classDescFlags, 1);
	m_Consumer.WriteInt16(5); // fields count
	writeClassField('I', "intField");
	writeClassField('L', "dateField", "Ljava/util/Date;");
	writeClassField('L', "stringField1", "Ljava/lang/String;");
	writeClassField('L', "stringField2");
	m_Consumer.WriteBytes(&TC_REFERENCE, 1); // write back-reference for stringField2's className1 (since it's the same as stringField1's)
	m_Consumer.WriteInt32(0x007E0002);
	writeClassField('L', "type", "Lcommon/RequestType;");
	// write classAnnotation (there isn't any)
	m_Consumer.WriteBytes(&TC_ENDBLOCKDATA, 1);
	// write superClassDesc (there isn't any)
	m_Consumer.WriteBytes(&TC_NULL, 1);
}

void RequestSerializer::writeClassData(const LazyParkingWindowsCPPClientBL::Request& i_Request)
{
	unsigned char classDescFlags = 0x3;
	// write intField
	m_Consumer.WriteInt32(i_Request.GetIntField());
	// write dateField
	time_t dateField = i_Request.GetDateField();
	if (dateField == NULL_DATE)
	{
		m_Consumer.WriteBytes(&TC_NULL, 1);
	}
	else
	{
		dateField *= 1000;
		m_Consumer.WriteBytes(&TC_OBJECT, 1);
		m_Consumer.WriteBytes(&TC_CLASSDESC, 1);
		writeUTF("java.util.Date");
		m_Consumer.WriteInt64(0x686A81014B597419); // write serialVersionUID
		m_Consumer.WriteBytes(&classDescFlags, 1);
		m_Consumer.WriteInt16(0x0); // write fields (there are no fields)
		// write classAnnotation and superClassDesc (there aren't any)
		m_Consumer.WriteBytes(&TC_ENDBLOCKDATA, 1);
		m_Consumer.WriteBytes(&TC_NULL, 1);
		// write objectAnnotation
		m_Consumer.WriteBytes(&TC_BLOCKDATA, 1);
		unsigned char blockDataSize = 0x8;
		m_Consumer.WriteBytes(&blockDataSize, 1);
		m_Consumer.WriteInt64(dateField);
		m_Consumer.WriteBytes(&TC_ENDBLOCKDATA, 1);
	}

	writeStringField(i_Request.GetStringField1());
	writeStringField(i_Request.GetStringField2());
	// write type
	m_Consumer.WriteBytes(&TC_ENUM, 1);
	m_Consumer.WriteBytes(&TC_CLASSDESC, 1);
	writeUTF("common.RequestType");
	m_Consumer.WriteInt64(0x0); // write serialVersionUID
	classDescFlags = 0x12;
	m_Consumer.WriteBytes(&classDescFlags, 1);
	m_Consumer.WriteInt16(0x0); // write fields (there are no fields)
	m_Consumer.WriteBytes(&TC_ENDBLOCKDATA, 1); // write classAnnotation (there is none)
	// write superClassDesc
	m_Consumer.WriteBytes(&TC_CLASSDESC, 1);
	writeUTF("java.lang.Enum");
	m_Consumer.WriteInt64(0x0); // write serialVersionUID
	m_Consumer.WriteBytes(&classDescFlags, 1);
	m_Consumer.WriteInt16(0x0); // write fields (there are no fields)
	// write classAnnotation and superClassDesc (there aren't any)
	m_Consumer.WriteBytes(&TC_ENDBLOCKDATA, 1);
	m_Consumer.WriteBytes(&TC_NULL, 1);
	string typeString;
	stringFromERequestType(i_Request.GetType(), typeString);
	writeString(typeString);
}

void RequestSerializer::writeUTF(const string& i_UTF)
{
	size_t length = i_UTF.length();

	if (length <= 0xFFFF)
	{
		m_Consumer.WriteInt16((unsigned short)length);
	}
	else
	{
		m_Consumer.WriteInt64(length);
	}

	m_Consumer.WriteBytes(i_UTF.data(), length);
}

void RequestSerializer::writeString(const string& i_String)
{
	size_t length = i_String.length();
	unsigned char stringCode = length <= 0xFFFF ? 0x74 : 0x7C;

	m_Consumer.WriteBytes(&stringCode, 1);
	writeUTF(i_String);
}

void RequestSerializer::writeClassField(char i_Typecode, const string& i_FieldName, const string& i_ClassName1)
{
	m_Consumer.WriteBytes(&i_Typecode, 1);
	writeUTF(i_FieldName);
	if (!i_ClassName1.empty())
	{
		writeString(i_ClassName1);
	}
}

void RequestSerializer::writeStringField(const string& i_StringField)
{
	if (i_StringField.empty())
	{
		m_Consumer.WriteBytes(&TC_NULL, 1);
	}
	else
	{
		writeString(i_StringField);
	}
}

RequestSerializer::RequestSerializer(ITokenConsumer& i_Consumer) : m_Consumer(i_Consumer)
{
	m_IsFirstMessage = true;
}

void RequestSerializer::WriteRequest(const LazyParkingWindowsCPPClientBL::Request& i_Request)
{
	if (m_IsFirstMessage) // write stream magic number and stream version
	{
		m_Consumer.WriteInt16(0xaced); // STREAM_MAGIC
		m_Consumer.WriteInt16(5); // STREAM_VERSION
		m_IsFirstMessage = false;
	}

	m_Consumer.WriteBytes(&TC_RESET, 1); // reset the stream
	// write new object
	m_Consumer.WriteBytes(&TC_OBJECT, 1);
	writeClassDesc();
	writeClassData(i_Request);
	m_Consumer.Flush();
}


//--------------------------------------------------------------------
// newObject: TC_OBJECT classDesc newHandle classdata[]  // data for each class
/* classdata:
		nowrclass                 // SC_SERIALIZABLE & classDescFlag &&
								  // !(SC_WRITE_METHOD & classDescFlags)
		wrclass objectAnnotation  // SC_SERIALIZABLE & classDescFlag &&
								  // SC_WRITE_METHOD & classDescFlags
		externalContents          // SC_EXTERNALIZABLE & classDescFlag &&
								  // !(SC_BLOCKDATA  & classDescFlags
		objectAnnotation          // SC_EXTERNALIZABLE & classDescFlag&&
								  // SC_BLOCKDATA & classDescFlags

	nowrclass:
		values                    // fields in order of class descriptor
	wrclass:
		nowrclass
	objectAnnotation:
		endBlockData
		contents endBlockData     // contents written by writeObject
								  // or writeExternal PROTOCOL_VERSION_2.
*/
/* newClassDesc:
		TC_CLASSDESC className serialVersionUID newHandle classDescInfo
		TC_PROXYCLASSDESC newHandle proxyClassDescInfo
*/
// className: (utf) // 2-byte size of the following UTF-8 string followed by the UTF-8 string
// serialVersionUID: (long)
// classDescInfo: classDescFlags fields classAnnotation superClassDesc
// fields: (short) < count > fieldDesc[count]
/* fieldDesc:
		primitiveDesc
		objectDesc
*/
// primitiveDesc: prim_typecode fieldName
// objectDesc: obj_typecode fieldName className1
/* prim_typecode:
		`B'	// byte
		`C'	// char
		`D'	// double
		`F'	// float
		`I'	// integer
		`J'	// long
		`S'	// short
		`Z'	// boolean
*/
/* obj_typecode:
		`[`	// array
		`L'	// object
*/
// fieldName: (utf)
// className1: (String)object             // String containing the field's type in field descriptor format
/*classAnnotation:
		endBlockData
		contents endBlockData      // contents written by annotateClass
*/
// superClassDesc: classDesc
// nullReference: TC_NULL
// newEnum: TC_ENUM classDesc newHandle enumConstantName
// TC_ENUM = 0x7E;