#pragma once
#include <iostream>
namespace xop
{

	//Errors defined by server
	enum class ServErr : uint16_t
	{
		noError = 1,	//
		CantCreateNewGame,
		NoGameID,
		PlayerHasGame,
		PlayerWaitsAnotherPlayer,
		NotYourTurn,	//not your turn
	};

	struct XY
	{
		uint8_t x;
		uint8_t y;
	};


	union position
	{
		uint16_t posXY;
		XY xy;
	};

	struct Move 
	{
		uint32_t gameID;
		uint32_t playerID;
		position pos;	//new position
		uint16_t nMove;	//number of move
		ServErr err;

	};



	//information abput error
	struct err_info
	{
		uint32_t gameID;
		uint32_t playerID;
		ServErr err;	//tur fo normal 
	};

	//whose turn is it to go
	enum class Turn : uint16_t
	{
		player1 = 0,
		player2,
		//wait_for_start,		//waiting for connect with another player and start the game 
		//wait_for_other_move,	//wait for move of other player
		//your_move,	//move of the current player
	};

	enum class TurnClient : uint16_t
	{
		YourMove = 0,
		OpponentMove,
	};

	enum class ClientStatus : uint16_t
	{
		StartedThis = 0,	//the game is going on, the first move was for this player, 
		StartedOpponent,	//the game is going on, the first move was for opposite player, 
		NoGame,
		Wait_for_opponent_connection,
		FinishedGame,	//game is finished

	};

	enum class GameStatus : uint16_t
	{
		Player1_First_move = 0,	//
		Player2_First_move,
		Your_move,	//move of the current player
		Opponent_move,	//wait for move of other player
		No_game,
		Wait_for_player2_connection,
		Game_on,
		Game_finished,
		No_such_game,	//no such game
	};

	struct GameInfo
	{
		GameStatus game_status;
		const char* Msg;	//error messgae
	};



	struct ServErrInfo
	{
		ServErr key;
		const char* Msg;	//error messgae
	};

	//information to the player about the game
	struct game_info
	{
		uint32_t gameID;
		uint32_t playerID;
		ClientStatus game_status;		//game status
	};

	extern const GameInfo game_status[];

	extern ServErrInfo srvErrInfo[];

	extern const size_t srvErrInfoSize;

	struct ClienInfo
	{
		ClientStatus key;
		const char* Msg;	//error messgae
	};


	extern const ClienInfo client_info[];
	

	//const size_t clientInfoSize = sizeof(client_info) / sizeof(client_info[0]);

	//show message about client
	const char* showMessage(ClientStatus key);

	//show message about error
	const char* showMessage(ServErr key);

};

