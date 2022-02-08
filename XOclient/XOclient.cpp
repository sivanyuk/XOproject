
#include "xoclient.h"

int main()
{
	xop::net::СonnectionInfo conn_info;	//
	xop::xo_client client;
	std::array<bool, 10> new_key = {};
	std::array<bool, 10> old_key = {};

	do
	{
		std::cout << "Waiting a connection\n";
		client.Connect(conn_info.host(), conn_info.port());
		
	} while (!client.IsConnected());

	std::cout << "   Press: \n 1 - Server echo \n 2 - Start a new game with another player\n" <<
		/*" 3 - Start a new game with the server\n*/" 4 - Make a move\n 0 - Exit the programm \n";


	bool fExit = false;
	while (!fExit)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));	// 2000ms)
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			for (uint8_t i8 = 0; i8 < new_key.size(); ++i8)
			{
				new_key[i8] = GetAsyncKeyState('0'+ i8) & 0x8000;	//scan 0-9 keys

			}
		}

		if (new_key[0] && !old_key[0]) fExit = true;
		if (new_key[1] && !old_key[1]) client.EchoServer();
		if (new_key[2] && !old_key[2]) client.NewGameWithPlayer();
		if (new_key[4] && !old_key[4]) client.MakeAMove(11, 12);	//Client's move
		//if (new_key[3] && !old_key[3]) client.NewGameWithServer();


		for (int i = 0; i < new_key.size(); i++) old_key[i] = new_key[i];

		if (client.IsConnected())
		{
			if (!client.Incoming().empty())
			{

				xop::net::message msg = client.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case xop::net::CustMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted the Connection\n";
				}
				break;

				case xop::net::CustMsgTypes::ServerEcho:
				{

					// Server has responded to a ping request	
					std::string server_reply = msg.to_string();
					std::cout << "Server replied: " << server_reply << "\n";
					break;
				}
				case xop::net::CustMsgTypes::GameStatus:
				{
					client.client_status_msg(msg);	//process game status
					break;
				}
				case xop::net::CustMsgTypes::ServerError:
				{
					client.client_error_msg(msg);	//process error message
					break;
				}
				default:
				{
					break;
				}
				}
			}
		}
		else
		{
			std::cout << "Server Down\n";
			fExit = true;
		}

	}

	//countdown 
	int cou = 5;

	//wait for 5 sec before close the client
	//to show information before this colsole application will be closed

	do
	{
		std::cout << "The program will finished in " << cou << " sec.\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	} while (--cou);
	std::cout << "Bye!\n";
	return 0;
}