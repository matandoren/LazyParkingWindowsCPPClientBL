#include "pch.h"
#include <string>
#include "Exceptions.h"

using namespace LazyParkingWindowsCPPClientBL;

WSAStartupException::WSAStartupException(int i_ErrorCode): runtime_error(std::string("WSAStartup failed: ") + std::to_string(i_ErrorCode))
{}

SocketConnectionException::SocketConnectionException(const std::string& i_Cause): runtime_error(i_Cause)
{}

SocketReadException::SocketReadException(const std::string& i_Cause) : runtime_error(i_Cause)
{}

SocketWriteException::SocketWriteException(const std::string& i_Cause) : runtime_error(i_Cause)
{}