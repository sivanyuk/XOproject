
#include "XOserver1.h"
#include <mutex>



//check if the player play connected to another game


namespace xop
{

	//	prepare information about an error
	void xo_server::prepErrMsg(xop::net::message& msg, const uint32_t gameID, const uint32_t playerID, const ServErr srv_err)
	{
		msg.header.id = xop::net::CustMsgTypes::ServerError;	//server error
		err_info err_inf;
		//copy dada from game inf stuct
		err_inf.gameID = gameID;
		err_inf.playerID = playerID;
		err_inf.err = srv_err;
		
		//fing error message
		//int adr = (int)srv_err;

		char* text_msg = (char*)showMessage(srv_err);

		std::cout << "Error! Player [" << playerID << "], Game ID = " << gameID;
		if (text_msg == nullptr) std::cout << ", Message < " << uint32_t(srv_err) << ">\n";
		else std::cout << ", " << text_msg << "\n";

		msg << err_inf;	//send to messge information about the error 

	}


	xo_server::xo_server(uint16_t nPort): xop::net::server_interface(nPort)
	{
	};

	bool xo_server::OnClientConnect(std::shared_ptr<xop::net::connection> client)
	{
		xop::net::message msg;
		msg.header.id = xop::net::CustMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	//check if the plaeyr already has a game
	//result true - if plaeyr already has an active game
	GameStatus xo_server::chkPlayerGame(const std::shared_ptr<xop::net::connection>& client, uint32_t & gameID)
	{
		gameID = client->GetGameID();
		if (!gameID || games.empty())
		{
			return GameStatus::No_such_game;	//
		}
		auto map_game_element = games.find(gameID);
		if (map_game_element == games.end())
		{
			return GameStatus::No_such_game;	// The game with such number is absent
		}
		else
		{
			//get shared pointer of the shar_game1 from the map
			std::shared_ptr<game_>& searched_game = map_game_element->second;
			GameStatus game_status = searched_game->game_status;
			if (game_status != GameStatus::Game_finished && game_status != GameStatus::Wait_for_player2_connection)
			{
				std::cout << "Player [" << client->GetID() << "] already has active game \n";
				return GameStatus::Game_on;	
			}
			if (game_status == GameStatus::Game_finished)
			{
				//erase the game from map
				games.erase(gameID);
				std::cout << "Player [" << client->GetID() << "] had a finished game. Its \"gameID\"  had data \n";
				return GameStatus::No_such_game;	//here isn't such directory

			}
			return game_status;	// it should be Wait_for_opponent_connection - when try the player to play with himself
		}

	}

	//start a game for 2 plyaers
	void xo_server::start2Pgame(std::shared_ptr<xop::net::connection> client)
	{
		net::message msg;
		uint32_t gameID = 0;
		uint32_t clientID = client ->GetID();
		game_info gameInf;	//information to player about the shar_game1

		{
			//start scope for scoped lock
			//take the mutex
			std::scoped_lock(mut_games);

			//check the status of the player
			GameStatus game_stat = chkPlayerGame(client, gameID);
			if (game_stat == GameStatus::Wait_for_player2_connection)
			{
				prepErrMsg(msg, gameID, clientID, ServErr::PlayerWaitsAnotherPlayer);
				client->Send(msg);
				return;
			}
			else if (game_stat == GameStatus::Game_on)
			{
				prepErrMsg(msg, gameID, clientID, ServErr::PlayerHasGame);
				client->Send(msg);
				return;
			}

			msg.header.id = net::CustMsgTypes::GameStatus;	//the reply with the same type of command

			if (wait2_game.empty())
			{


				int i = 10;
				if (!games.empty())
				{
					while (i-- > 0)
					{
						//range of shar_game1 id 1000 to 0xfffffffe
						gameID = scrc::rand32(1000UL);		//get random nunber
						if (games.find(gameID)!= games.end() )	//try to find if game is absent in th map
						{
							break;
						}
					}

					//the free ssesion namber wasn't fing
					//inform client about the error
					if (!i)
					{
						//gameID is the last gotten number
						prepErrMsg(msg, gameID, clientID, ServErr::CantCreateNewGame);
						client->Send(msg);
						return;
					}
				}
				else
				{
					//here wasn't games
					gameID = scrc::rand32(1000UL);		//get random nunber
				}

				std::cout << "The new game ID = " << gameID << ". Player1 connected \n";

				wait2_game.push_back(gameID);	
				
				//shared pointer to the game
				std::shared_ptr<game_> shar_game1 = std::make_shared<game_>();
				//game_ shar_game1;	// client);

				//write information about session
				std::shared_ptr<game_> ses;	 //std::make_shared<game_>();
				shar_game1->player1 = client;
				shar_game1->sesID = gameID;	//a duplicate of the session ID in the session structure itself
				shar_game1->p1ID = client->GetID();
				shar_game1->game_status = GameStatus::Wait_for_player2_connection;
				shar_game1->n_move = 0;	//counter of move
				
				client->SetGameID(gameID);
				
				games.insert_or_assign(gameID, shar_game1);

				//prepare shar_game1 info tor the client
				gameInf.gameID = gameID;	//inform to he client abput wrong session start
				gameInf.playerID = client->GetID();
				gameInf.game_status = ClientStatus::Wait_for_opponent_connection;	//wait for the second player
				msg << gameInf;

			}
			else
			{
				//player 2 is connecting to the shar_game1

				gameID = wait2_game.front();	//get the number of the shar_game1 from  the queue
				wait2_game.pop_front();

				//check if the session exist (it's a simonyme for the shar_game1)
				auto map_game_element = games.find(gameID);
				if (map_game_element == games.end())
				{
					//error - shar_game1 is absent
					//there isn't a game with required ID
					prepErrMsg(msg, gameID, clientID, ServErr::NoGameID);
					client->Send(msg);
					return;
				}
				else
				{
					
					//get shared pointer of the shar_game1 from the map
					std::shared_ptr<game_>& current_game= map_game_element->second;

					//add information about player 2
					current_game->p2ID = client ->GetID();
					current_game->player2 = client;
					client->SetGameID(gameID);
					//GameStatus  
					
					GameStatus game_stat = (GameStatus)scrc::rand32((uint32_t)GameStatus::Player1_First_move, (uint32_t)GameStatus::Player2_First_move);
					current_game->game_status = game_stat;

					current_game-> turn = (Turn)(uint32_t)game_stat;

					//check the value in Tur and Game status 
					static_assert((uint32_t)Turn::player1 == (uint32_t)GameStatus::Player1_First_move, "Turn::player1 == GameStatus::Player1_First_move");
					static_assert((uint32_t)Turn::player2 == (uint32_t)GameStatus::Player2_First_move, "Turn::player2 == GameStatus::Player2_First_move");

					//shar_game1 is started
					std::cout << "Player1 connected to the game with ID = " << gameID << "\n";

					//whose move next??
					
					gameInf.gameID = gameID;	//inform to he client abput wrong session start
					gameInf.playerID = client->GetID();

					if (game_stat == GameStatus::Player2_First_move)
					{
						gameInf.game_status = ClientStatus::StartedThis;
						msg << gameInf;
						client->Send(msg);	//send a message to the player 
						
						//send message to client 1 about its status
						msg.clear();
						gameInf.playerID = current_game->p1ID;
						gameInf.game_status = ClientStatus::StartedOpponent;
						msg << gameInf;
						current_game->player1->Send(msg);
						return;

					}
					else
					{
						
						gameInf.game_status = ClientStatus ::StartedOpponent;
						msg << gameInf;
						client->Send(msg);	//send a message to the player 

						msg.clear();
						gameInf.playerID = current_game->p1ID;
						gameInf.game_status = ClientStatus ::StartedThis;
						msg << gameInf;
						current_game->player1->Send(msg);
						return;

					}
				}
			}
		}

		//send message to the client
		client->Send(msg);
	}

	// Called when a client appears to have disconnected
	void xo_server::OnClientDisconnect(std::shared_ptr<xop::net::connection> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";

	}

	// Called when a message arrives
	void xo_server::OnMessage(std::shared_ptr<xop::net::connection> client, xop::net::message& msg)
	{

		std::cout << (std::string)"A message <" << (uint32_t)msg.header.id << "> from a client[" << std::to_string(client->GetID()) << "] \n";

		switch (msg.header.id)
		{
		case xop::net::CustMsgTypes::NewPlayerGame:
		{
			std::cout << "The player [" << client->GetID() << "] want to play with another player  " << "\n";
			start2Pgame(client);
			break;

		}
		case xop::net::CustMsgTypes::MakeAMove:
		{
			std::cout << "The player [" << client->GetID() << "] make its move " << "\n";
			clientsMove(client, msg);
			break;

		}


		case xop::net::CustMsgTypes::ServerEcho:
		{

			std::string s = msg.to_string();
			std::cout << "[" << client->GetID() << "] send the message: " << s << "\n";

			// Construct a new message and send it to the client
			xop::net::message msgOut;
			msgOut.header.id = msg.header.id;
			msg.header.id = xop::net::CustMsgTypes::ServerEcho;
			s = "Hello [" + std::to_string(client->GetID()) + "]. Your message is: " + s;

			msgOut = s;
			client->Send(msgOut);
			break;

		}

		};
	};
};
