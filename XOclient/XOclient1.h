
#pragma once


#include <iostream>
#include <vector>
#include <array>
#include <string>

#include <xop_net.h>
#include "..\scrc\scrc_crypto.h"	//s& crypto 
#include "..\XOserver\XOmessages.h"


namespace xop
{


	// Forward declare
	class xo_client : public xop::net::client_interface<xop::net::CustMsgTypes>
	{

	private:
		ClientStatus client_status = ClientStatus::NoGame;

		uint32_t id = 0;		//id of the clernt //thes parameters are in connection, but here it is more conviniet to use it 

		uint32_t id_game = 0;	//id of client's game	should be cleared after finishing 
								//game with clerance of game info in the games map

		uint16_t n_move = 0;	//number of move

		TurnClient turn;	//tur of client

		//bool game_started = false;

	public:
		void client_status_msg(net::message& msg);	//process game status message

		void client_error_msg(net::message& msg);	//information abouth the error

		void EchoServer();
		//A new Game with another playes
		void NewGameWithPlayer();
		//a new game with a computer (server)
		void NewGameWithServer();

		void MakeAMove(uint8_t x, uint8_t y);	//inform the server about the client move

	};

};