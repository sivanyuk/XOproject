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
			NewPlayerGame,	//a new game for two players -   only client
			NewServerGame,	//a new game with the server -   only client
			MakeAMove,		//a client send to server information with its move
			GameStatus,		//send iformation about the game - only server to client
			ServerError,	//server inform about an error - only server
		};
	};


}

