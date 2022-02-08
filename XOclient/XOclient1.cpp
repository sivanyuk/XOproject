
#include "xoclient1.h"

namespace xop
{
	//send an echo message to the serber
	void xo_client::EchoServer()
	{
		xop::net::message msg;
		std::string s = "This is just a message for echo";
		//message to the server
		msg = s;	// "This is just a message for echo";
		std::cout << "Sent to server :" << s << "\n";

		msg.header.id = xop::net::CustMsgTypes::ServerEcho;
		Send(msg);
	}


	//A new Game with another playes
	void xo_client::NewGameWithPlayer()
	{
		xop::net::message msg;

		std::cout << "Attempt to start a new game with another player\n";

		msg.header.id = xop::net::CustMsgTypes::NewPlayerGame;
		this->Send(msg);

	}

	//a new game with a computer (server)
	void xo_client::NewGameWithServer()
	{
		xop::net::message msg;

		std::cout << "Start a new game with server player\n";

		msg.header.id = xop::net::CustMsgTypes::NewServerGame;
		Send(msg);
	}


	void xo_client::MakeAMove(uint8_t x, uint8_t y)	//inform the server about the client move
	{
		xop::net::message msg;

		std::cout << "Make a Move \n";
		Move move;
		move.gameID = id_game;
		move.playerID = id;
		//move.err	//- no matter
		move.nMove = ++n_move;
		move.pos.xy.x = x;
		move.pos.xy.y = y;
		msg.header.id = xop::net::CustMsgTypes::MakeAMove;
		msg << move;
		Send(msg);
	}


};
