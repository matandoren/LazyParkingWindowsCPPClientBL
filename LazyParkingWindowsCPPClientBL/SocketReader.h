#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include "ITokenProvider.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LENGTH 512

namespace LazyParkingWindowsCPPClientBL
{
    class SocketReader : public ITokenProvider
    {
        SOCKET m_Socket;
        char* m_ReceiveBuffer;
        const int k_ReceiveBufferLength;
        int m_DataStartIndex;
        int m_DataSize;
        bool m_IsConnectionOpen;

    public:
        SocketReader(SOCKET i_ConnectedSocket, int i_ReceiveBufferLength = DEFAULT_BUFFER_LENGTH);
        ~SocketReader() override;
        const void* ReadBytes(int i_NumberOfBytesToRead, int& o_NumberOfBytesRead) override;
        bool ReadInt16(unsigned __int16& o_ReadInt16) override;
        bool ReadInt32(unsigned __int32& o_ReadInt32) override;
        bool ReadInt64(unsigned __int64& o_ReadInt64) override;
    };
}