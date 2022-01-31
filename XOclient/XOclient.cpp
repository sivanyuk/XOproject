
#include "xoclient.h"

int main()
{
	xop::net::СonnectionInfo conn_info;	//
	xo_client client;
	std::array<bool, 10> new_key = {};
	std::array<bool, 10> old_key = {};

	do
	{
		std::cout << "Waiting a connection\n";
		client.Connect(conn_info.host(), conn_info.port());
		
	} while (!client.IsConnected());

	std::cout << "Press: \n1 - Server echo \n0 - Exit the programm \n";


	bool fExit = false;
	while (!fExit)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));	// 2000ms)
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			new_key[0] = GetAsyncKeyState('0') & 0x8000;
			new_key[1] = GetAsyncKeyState('1') & 0x8000;
		}

		if (new_key[0] && !old_key[0]) fExit = true;
		if (new_key[1] && !old_key[1]) client.EchoServer();

		for (int i = 0; i < 3; i++) old_key[i] = new_key[i];

		if (client.IsConnected())
		{
			if (!client.Incoming().empty())
			{


				auto msg = client.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case xop::net::CustMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted the Connection\n";
				}
				break;

				case xop::net::CustMsgTypes ::ServerEcho:
				{

					// Server has responded to a ping request	
					std::string server_reply = msg.to_string();
					std::cout << "Server replied: " << server_reply << "\n";
				}
				break;
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