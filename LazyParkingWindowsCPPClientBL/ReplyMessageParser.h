#pragma once
#include "ITokenProvider.h"
#include "Message.h"

namespace LazyParkingWindowsCPPClientBL
{
	class ReplyMessageParser
	{
		ITokenProvider& m_Provider;
		bool m_IsFirstMessage;

	public:
		ReplyMessageParser(ITokenProvider& i_Provider);
		ReplyMessage GetReply();
	};
}