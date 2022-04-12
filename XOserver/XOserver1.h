#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <queue>

#include <xop_net.h>
#include "XOmessages.h"


#include "..\scrc\scrc_crypto.h"	//s& crypto 

namespace xop
{
	//board board_size is 21x21
	constexpr size_t board_size = 21;
	


	union board
	{
		uint8_t line[board_size * board_size];	//linear board_size
		uint8_t sq[board_size][board_size];		//square board_size
	};




	struct game_
	{
		//unique game_ ID
		uint32_t sesID = 0;	

		//ID of players 0 - Player no connected; (-1) - computer 
		uint32_t p1ID = 0;	//
		uint32_t p2ID = 0;

		std::shared_ptr <xop::net::connection> player1;
		std::shared_ptr <xop::net::connection> player2;
		//std::shared_ptr<board> 
		board game_board;	//game board

		GameStatus game_status = GameStatus::No_game;		//game isn't started yet
		Turn turn = Turn::player1;
		uint16_t n_move = 0;	//number of move
		bool f_play_with_server = false;	//game with server

		//add time for game_ inactivity
		game_()	//std::shared_ptr<xop::net::connection>& player) 
		{
			std::memset(&game_board, 0, sizeof(board::line));	//clear the board = std::make_shared<board>();
			player1 = nullptr;	// player;
			player2 = nullptr;
		};
	};


class xo_server : public xop::net::server_interface
{
private:
	//unorderd map with data about all game_ in the game
	std::unordered_map <uint32_t, std::shared_ptr<game_>>  games;
		
	//queue of games with waiting player. It should be only 1 elemet in this queue maximum
	std::deque<uint32_t> wait2_game;		//session waiting for the second player
		
	//mutex for game_'s map blocking 
	std::mutex mut_games;

	//	prepare information about an error
	void prepErrMsg(xop::net::message& msg, const uint32_t gameID, const uint32_t playerID, const ServErr srv_err);

	//check if the plaeyr already has a game
	//result - game status about the game
	GameStatus chkPlayerGame(const std::shared_ptr<xop::net::connection>& client, uint32_t& gameID);


public:
	xo_server(uint16_t nPort);	// : xop::net::server_interface(nPort);
protected:

		//start a game for two players
		void start2Pgame(std::shared_ptr<xop::net::connection> client);

		void clientsMove(std::shared_ptr<xop::net::connection> client, xop::net::message& msg);

		virtual bool OnClientConnect(std::shared_ptr<xop::net::connection> client);

		// Called when a client appears to have disconnected
		virtual void OnClientDisconnect(std::shared_ptr<xop::net::connection> client);

		// Called when a message arrives
		virtual void OnMessage(std::shared_ptr<xop::net::connection> client, xop::net::message& msg);
	};

};

