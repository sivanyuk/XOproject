#include <iostream>
#include "XOmessages.h"

namespace xop
{



	////whose turn is it to go
	//enum class Turn : uint16_t
	//{
	//	player1 = 0,
	//	player2,
	//	//wait_for_start,		//waiting for connect with another player and start the game 
	//	//wait_for_other_move,	//wait for move of other player
	//	//your_move,	//move of the current player
	//};

	//enum class TurnClient : uint16_t
	//{
	//	YourMove = 0,
	//	OpponentMove,
	//};

	//enum class ClientStatus
	//{
	//	StartedThis = 0,	//the game is going on, the first move was for this player, 
	//	StartedOpponent,	//the game is going on, the first move was for opposite player, 
	//	NoGame,
	//	PrepareToGame,	//preparation for the game
	//	Wait_for_opponent_connection,
	//	FinishedGame,	//game is finished

	//};

	//enum class GameStatus : uint16_t
	//{
	//	Player1_First_move = 0,	//
	//	Player2_First_move,
	//	Your_move,	//move of the current player
	//	Opponent_move,	//wait for move of other player
	//	No_game,
	//	Wait_for_player_connection,
	//	Game_on,
	//	Game_finished,
	//	No_such_game,	//no such game
	//};

	//struct GameInfo
	//{
	//	GameStatus game_status;
	//	const char* Msg;	//error messgae
	//};


	////Errors defined by server
	//enum class ServErr : uint16_t
	//{
	//	CantCreateNewGame = 1,
	//	NoGameID,
	//	PlayerHasGame,
	//	PlayerWaitsAnotherPlayer,
	//};

	//struct ServErrInfo
	//{
	//	ServErr key;
	//	const char* Msg;	//error messgae
	//};

	//enum class ClientStatus : uint16_t
	//{
	//	StartedThis = 0,	//the game is going on, the first move was for this player, 
	//	StartedOpponent,	//the game is going on, the first move was for opposite player, 
	//	NoGame,
	//	Wait_for_opponent_connection,
	//	FinishedGame,	//game is finished


	//struct ClienInfo
	//{
	//	ClientStatus key;
	//	const char* Msg;	//error messgae
	//};


	const ClienInfo client_info[] =
	{
		ClientStatus:: StartedThis, "Your move is first",
		ClientStatus::StartedOpponent, "Opponent starts to move",
		ClientStatus::Wait_for_opponent_connection, "Please wait for your opponent connection",
	};

	//const size_t clientInfoSize = sizeof(client_info) / sizeof(client_info[0]);

	//show message about status or error
	const char* showMessage(ClientStatus key)
	{
		size_t size = sizeof(client_info) / sizeof(client_info[0]);
		for (size_t i = 0; i < size; ++i)
		{
			if (client_info[i].key == key) return client_info[i].Msg;	//exit if keys are equal

		}
		return (const char*)nullptr;
	}



	//information for the client
	const GameInfo game_status[] =
	{
		GameStatus::Wait_for_player2_connection, "Waiting for an opponent connection",
		GameStatus::Your_move , "Your turn",
		GameStatus::Opponent_move, "Opponent's turn",

	};

	
	ServErrInfo srvErrInfo[] =
	{
		ServErr::CantCreateNewGame, "Can't create new game",
		ServErr::NoGameID , "There isn't the game with such ID",
		ServErr::PlayerHasGame, "The player aleardy has a game",
		ServErr::PlayerWaitsAnotherPlayer, "The player waits for another player conection",
		ServErr::NotYourTurn, "Not your turn",
	};


	//show message about status or error
	const char* showMessage(ServErr key)
	{
		size_t size = sizeof(srvErrInfo) / sizeof(srvErrInfo[0]);
		for (size_t i = 0; i < size; ++i)
		{
			if (srvErrInfo[i].key == key) return srvErrInfo[i].Msg;	//exit if keys are equal

		}
		return (const char*)nullptr;
	}
};