#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include "ITokenConsumer.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LENGTH 512

namespace LazyParkingWindowsCPPClientBL
{
    class SocketWriter : public ITokenConsumer
    {
        SOCKET m_Socket;
        char* m_SendBuffer;
        const int k_SendBufferLength;
        int m_DataSize;
        bool m_IsConnectionOpen;
        const bool k_SendImmediately;

    public:
        SocketWriter(SOCKET i_ConnectedSocket, bool i_SendImmediately = false, int i_SendBufferLength = DEFAULT_BUFFER_LENGTH);
        ~SocketWriter() override;
        virtual int WriteBytes(const void* i_BytesToWrite, int i_NumberOfBytesToWrite) override;
        virtual bool WriteInt16(unsigned __int16 i_Int16toWrite) override;
        virtual bool WriteInt32(unsigned __int32 i_Int32ToWrite) override;
        virtual bool WriteInt64(unsigned __int64 i_Int64ToWrite) override;
        virtual int Flush() override;
    };
}