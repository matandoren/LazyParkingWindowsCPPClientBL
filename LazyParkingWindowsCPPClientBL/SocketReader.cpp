#include "pch.h"
#include <string>
#include "SocketReader.h"
#include "Exceptions.h"

using namespace LazyParkingWindowsCPPClientBL;

SocketReader::SocketReader(SOCKET i_ConnectedSocket, int i_ReceiveBufferLength): k_ReceiveBufferLength(i_ReceiveBufferLength)
{
	m_Socket = i_ConnectedSocket;
	m_DataStartIndex = 0;
	m_DataSize = 0;
	m_ReceiveBuffer = new char[k_ReceiveBufferLength];
	m_IsConnectionOpen = true;
}

SocketReader::~SocketReader()
{
	delete[] m_ReceiveBuffer;
}

const void* SocketReader::ReadBytes(int i_NumberOfBytesToRead, int& o_NumberOfBytesRead)
{
	const void* returnAddress;
	int numberOfBytesReadByRecv, minNumberOfBytesToBeReadByRecv;

	if (i_NumberOfBytesToRead > k_ReceiveBufferLength) // the requested data is larger than the buffer
	{
		i_NumberOfBytesToRead = k_ReceiveBufferLength;
	}

	if (m_DataSize < i_NumberOfBytesToRead) // there is a need to read from the socket
	{
		if (m_IsConnectionOpen && i_NumberOfBytesToRead + m_DataStartIndex > k_ReceiveBufferLength) // there is not enough free space in the buffer
		{
			// move the data to the beginning of the buffer
			memmove(m_ReceiveBuffer, m_ReceiveBuffer + m_DataStartIndex, m_DataSize);
			m_DataStartIndex = 0;
		}

		minNumberOfBytesToBeReadByRecv = i_NumberOfBytesToRead - m_DataSize;
		// Receive until the minimal number of bytes has been read or the peer closes the connection
		while (m_IsConnectionOpen && minNumberOfBytesToBeReadByRecv > 0)
		{
			numberOfBytesReadByRecv = recv(m_Socket, m_ReceiveBuffer + m_DataStartIndex + m_DataSize, k_ReceiveBufferLength - (m_DataStartIndex + m_DataSize), 0);
			if (numberOfBytesReadByRecv == 0)
			{
				m_IsConnectionOpen = false;
			}
			else if (numberOfBytesReadByRecv < 0)
			{
				// throw exception
				throw SocketReadException(std::string("recv() error: ") + std::to_string(WSAGetLastError()));
			}

			minNumberOfBytesToBeReadByRecv -= numberOfBytesReadByRecv;
			m_DataSize += numberOfBytesReadByRecv;
		}
	}

	o_NumberOfBytesRead = m_IsConnectionOpen ? i_NumberOfBytesToRead : m_DataSize;
	returnAddress = o_NumberOfBytesRead > 0 ? m_ReceiveBuffer + m_DataStartIndex : NULL;
	m_DataSize -= o_NumberOfBytesRead;
	m_DataStartIndex += o_NumberOfBytesRead;

	return returnAddress;
}

bool SocketReader::ReadInt16(unsigned __int16& o_ReadInt16)
{
	int numberOfBytesRead;
	unsigned __int16* rawDataPtr = (unsigned __int16*) ReadBytes(2, numberOfBytesRead);

	if (numberOfBytesRead == 2)
	{
		o_ReadInt16 = ntohs(*rawDataPtr);
		return true;
	}

	return false;
}

bool SocketReader::ReadInt32(unsigned __int32& o_ReadInt32)
{
	int numberOfBytesRead;
	unsigned __int32* rawDataPtr = (unsigned __int32*) ReadBytes(4, numberOfBytesRead);

	if (numberOfBytesRead == 4)
	{
		o_ReadInt32 = ntohl(*rawDataPtr);
		return true;
	}

	return false;
}

bool SocketReader::ReadInt64(unsigned __int64& o_ReadInt64)
{
	int numberOfBytesRead;
	unsigned __int64* rawDataPtr = (unsigned __int64*) ReadBytes(8, numberOfBytesRead);

	if (numberOfBytesRead == 8)
	{
		o_ReadInt64 = ntohll(*rawDataPtr);
		return true;
	}

	return false;
}