#pragma once
#include "ITokenConsumer.h"
#include "Message.h"

namespace LazyParkingWindowsCPPClientBL
{
	class RequestSerializer
	{
		ITokenConsumer& m_Consumer;
		bool m_IsFirstMessage;
		static const unsigned char TC_CLASSDESC = 0x72;
		static const unsigned char TC_REFERENCE = 0x71;
		static const unsigned char TC_ENDBLOCKDATA = 0x78;
		static const unsigned char TC_NULL = 0x70;
		static const unsigned char TC_RESET = 0x79;
		static const unsigned char TC_OBJECT = 0x73;
		static const unsigned char TC_BLOCKDATA = 0x77;
		static const unsigned char TC_ENUM = 0x7E;

		void writeClassDesc();
		void writeClassData(const LazyParkingWindowsCPPClientBL::Request& i_Request);
		void writeUTF(const string& i_UTF);
		void writeString(const string& i_String);
		void writeClassField(char i_Typecode, const string& i_FieldName, const string& i_ClassName1 = string());
		void writeStringField(const string& i_StringField);

	public:
		RequestSerializer(ITokenConsumer& i_Consumer);
		void WriteRequest(const LazyParkingWindowsCPPClientBL::Request& i_Request);
	};
}