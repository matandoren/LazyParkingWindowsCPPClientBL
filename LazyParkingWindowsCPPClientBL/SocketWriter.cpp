#include "pch.h"
#include <string>
#include "SocketWriter.h"
#include "Exceptions.h"

using namespace LazyParkingWindowsCPPClientBL;

SocketWriter::SocketWriter(SOCKET i_ConnectedSocket, bool i_DontBuffer, int i_SendBufferLength) : k_SendBufferLength(i_SendBufferLength), k_SendImmediately(i_DontBuffer)
{
	m_Socket = i_ConnectedSocket;
	m_DataSize = 0;
	m_SendBuffer = new char[k_SendBufferLength];
	m_IsConnectionOpen = true;
}

SocketWriter::~SocketWriter()
{
	Flush();
	delete[] m_SendBuffer;
}

int SocketWriter::WriteBytes(const void* i_BytesToWrite, int i_NumberOfBytesToWrite)
{
	int numberOfBytesCommitedToBufferDuringThisIteration = 0, numberOfBytesRemainingToCommit = i_NumberOfBytesToWrite;

	while (m_IsConnectionOpen && numberOfBytesRemainingToCommit > 0)
	{
		// commit to the buffer as much data as possible
		numberOfBytesCommitedToBufferDuringThisIteration = (numberOfBytesRemainingToCommit < k_SendBufferLength - m_DataSize) ? numberOfBytesRemainingToCommit : (k_SendBufferLength - m_DataSize);
		memcpy(m_SendBuffer + m_DataSize, i_BytesToWrite, numberOfBytesCommitedToBufferDuringThisIteration);
		m_DataSize += numberOfBytesCommitedToBufferDuringThisIteration;
		numberOfBytesRemainingToCommit -= numberOfBytesCommitedToBufferDuringThisIteration;

		if (m_DataSize == k_SendBufferLength) // the buffer is full - flush the entire buffer
		{
			Flush();
		}
	}

	if (k_SendImmediately)
	{
		Flush();
	}
	
	return (m_IsConnectionOpen ? i_NumberOfBytesToWrite : 0);
}

bool SocketWriter::WriteInt16(unsigned __int16 i_Int16toWrite)
{
	i_Int16toWrite = htons(i_Int16toWrite);
	return WriteBytes(&i_Int16toWrite, 2) == 2;
}

bool SocketWriter::WriteInt32(unsigned __int32 i_Int32toWrite)
{
	i_Int32toWrite = htonl(i_Int32toWrite);
	return WriteBytes(&i_Int32toWrite, 4) == 4;
}

bool SocketWriter::WriteInt64(unsigned __int64 i_Int64toWrite)
{
	i_Int64toWrite = htonll(i_Int64toWrite);
	return WriteBytes(&i_Int64toWrite, 8) == 8;
}

int SocketWriter::Flush()
{
	int numberOfBytesWrittenBySend, numberOfBytesFlushed = 0;
	// Send until the data in the buffer has been written or the peer closes the connection
	while (m_IsConnectionOpen && m_DataSize > 0)
	{
		numberOfBytesWrittenBySend = send(m_Socket, m_SendBuffer, m_DataSize, 0);
		if (numberOfBytesWrittenBySend == 0)
		{
			m_IsConnectionOpen = false;
		}
		else if (numberOfBytesWrittenBySend < 0)
		{
			// throw exception
			throw SocketWriteException(std::string("send() error: ") + std::to_string(WSAGetLastError()));
		}

		m_DataSize -= numberOfBytesWrittenBySend;
		numberOfBytesFlushed += numberOfBytesWrittenBySend;
	}

	return numberOfBytesFlushed;
}