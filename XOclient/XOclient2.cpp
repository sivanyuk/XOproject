
#include "Xoclient1.h"
#include "Xoclient2.h"

namespace xop
{
	void xo_client::client_status_msg(net::message&  msg)	//process game status
	{
		game_info gameInf;	//information to player about the shar_game1
		msg >> gameInf;
		
		client_status = gameInf.game_status;

		if (client_status == ClientStatus::Wait_for_opponent_connection)
		{
			id = gameInf.playerID;		//diring this command write the information about the connection and game number
			id_game = gameInf.gameID;

		}
			//check if there is starting a game
		else if (client_status == ClientStatus::StartedThis || client_status ==ClientStatus::StartedOpponent)
		{
			id = gameInf.playerID;		//diring this command write the information about the connection and game number
			id_game = gameInf.gameID;

			//whose turn?
			if (client_status == ClientStatus::StartedThis) turn = TurnClient::YourMove;	
			else turn = TurnClient::YourMove;		
		}
		char* text_msg = (char*)showMessage(client_status);

		std::cout << "Player [" << id << "], Game ID = " << id_game;
		if (text_msg == nullptr) std::cout << ", Message < " << uint32_t(client_status) << ">\n";
		else std::cout << ", " << text_msg << "\n";

	}

	void xo_client::client_error_msg(net::message& msg)	//information abouth the error
	{
		err_info err_inf;
		//copy dada from game inf stuct

		msg >> err_inf;	//get the message abot the 

		uint32_t gameID = err_inf.gameID ;
		uint32_t playerID = err_inf.playerID ;
		ServErr srv_err = err_inf.err;

		//fing error message
		//int adr = (int)srv_err;

		//char* text_msg = (char*)showMessage(srvErrInfo, srv_err, srvErrInfoSize);
		char* text_msg = (char*)showMessage(srv_err);



		std::cout << "Error! Player [" << playerID << "], Game ID = " << gameID;
		if (text_msg == nullptr) std::cout << ", Message < " << uint32_t(srv_err) << ">\n";
		else std::cout << ", " << text_msg << "\n";

		msg << err_inf;	//send to messge information about the error 

	}
};