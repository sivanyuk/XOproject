#pragma once
namespace xop
{
	namespace net
	{

		enum class CustMsgTypes : uint16_t
		{
			ServerAccept,
			//ServerDeny,
			ServerPing,
			ServerEcho,
		};
	};
}

