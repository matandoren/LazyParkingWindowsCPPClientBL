#pragma once

namespace LazyParkingWindowsCPPClientBL
{
	class ITokenConsumer
	{
	public:
		virtual int WriteBytes(const void* i_BytesToWrite, int i_NumberOfBytesToWrite) = 0; // returns the number of bytes committed
		virtual bool WriteInt16(unsigned __int16 i_Int16toWrite) = 0;
		virtual bool WriteInt32(unsigned __int32 i_Int32toWrite) = 0;
		virtual bool WriteInt64(unsigned __int64 i_Int64toWrite) = 0;
		virtual int Flush() = 0; // forces buffered writers to commit buffered data to the consumer; returns the number of bytes flushed
		virtual ~ITokenConsumer() = 0;
	};
}