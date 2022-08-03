#pragma once

namespace LazyParkingWindowsCPPClientBL
{
	class ITokenProvider
	{
	public:
		virtual const void* ReadBytes(int i_NumberOfBytesToRead, int& o_NumberOfBytesRead) = 0;
		virtual bool ReadInt16(unsigned __int16& o_ReadInt16) = 0;
		virtual bool ReadInt32(unsigned __int32& o_ReadInt32) = 0;
		virtual bool ReadInt64(unsigned __int64& o_ReadInt64) = 0;
		virtual ~ITokenProvider() = 0;
	};
}