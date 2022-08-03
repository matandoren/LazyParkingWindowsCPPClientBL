#pragma once
#include <stdexcept>

namespace LazyParkingWindowsCPPClientBL
{
	class WSAStartupException : public std::runtime_error
	{
	public:
		WSAStartupException(int i_ErrorCode);
	};

	class SocketConnectionException : public std::runtime_error
	{
	public:
		SocketConnectionException(const std::string& i_Cause);
	};

	class SocketReadException : public std::runtime_error
	{
	public:
		SocketReadException(const std::string& i_Cause);
	};

	class SocketWriteException : public std::runtime_error
	{
	public:
		SocketWriteException(const std::string& i_Cause);
	};
}