/*
	MMO Client/Server Framework using ASIO
	"Happy Birthday Mrs Javidx9!" - javidx9

	Videos:
	Part #1: https://youtu.be/2hNdkYInj4g
	Part #2: https://youtu.be/UbjxGvrDrbw

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2020 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019, 2020

*/

#include <iostream>
#include <olc_net.h>
#include <chrono>
#include <thread>
#include "..\SCRC\scrc.h"	//s& crypto librarry

//enum class CustomMsgTypes : uint16_t
//{
//	ServerAccept,
//	ServerDeny,
//	ServerPing,
//	MessageAll,
//	ServerMessage,
//	SendSKey,	//to send secret Keys
//};


class xo_server : public olc::net::server_interface
{
public:
	xo_server(uint16_t nPort) : olc::net::server_interface(nPort)
	{

	};

protected:

	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection> client)
	{
		olc::net::message msg;
		msg.header.id = olc::net::CustMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection> client, olc::net::message & msg)
	{

		std::cout << (std::string)"Message from a client1 \n";

		switch (msg.header.id)
		{
		case olc::net::CustMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case olc::net::CustMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: Message All\n";

			// Construct a new message and send it to all clients
			olc::net::message msg;
			msg.header.id = olc::net:: CustMsgTypes::ServerMessage;
			msg << client->GetID();
			MessageAllClients(msg, client);

		}
		break;
		}
	}
};

//-----------
//class xo_server : public olc::net::server_interface		// <olc::net::CustMsgTypes /*CustomMsgTypes*/>
//{
//public:
//	//xo_server(uint16_t nPort) : olc::net::server_interface<olc::net::CustMsgTypes /*CustomMsgTypes*/>(nPort)
//	xo_server(uint16_t nPort) : olc::net::server_interface(nPort)
//	{
//
//	}
//
//protected:
//
//
//
//	//virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<olc::net::CustMsgTypes /*CustomMsgTypes*/>> client)
//	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection> client)
//	{
//		//olc::net::message<olc::net::CustMsgTypes /*CustomMsgTypes*/> msg;
//		//msg.header.id = olc::net::CustMsgTypes /*CustomMsgTypes*/::ServerAccept;
//		olc::net::message msg;
//		msg.header.id = olc::net::CustMsgTypes /*CustomMsgTypes*/::ServerAccept;
//
//		client->Send(msg);
//		return true;
//	}
//
//	// Called when a client appears to have disconnected
//	//virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<olc::net::CustMsgTypes /*CustomMsgTypes*/>> client)
//	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection> client)
//	{
//		std::cout << "Removing client [" << client->GetID() << "]\n";
//	}
//
//	// Called when a message arrives
//	//virtual void OnMessage(std::shared_ptr<olc::net::connection<olc::net::CustMsgTypes /*CustomMsgTypes*/>> client, olc::net::message/*<olc::net::CustMsgTypes /*CustomMsgTypes>*/& msg)
//	virtual void OnMessage(std::shared_ptr<olc::net::connection> client, olc::net::message/*<olc::net::CustMsgTypes /*CustomMsgTypes>*/& msg)
//	{
//
//		std::cout << (std::string)"Message from a client1 \n";
//
//		switch (msg.header.id)
//		{
//		case olc::net::CustMsgTypes /*CustomMsgTypes*/::ServerPing:
//		{
//			std::cout << "[" << client->GetID() << "]: Server Ping\n";
//
//			// Simply bounce message back to client
//			client->Send(msg);
//		}
//		break;
//
//		case olc::net::CustMsgTypes /*CustomMsgTypes*/::MessageAll:
//		{
//			std::cout << "[" << client->GetID() << "]: Message All\n";
//
//			// Construct a new message and send it to all clients
//			//olc::net::message<olc::net::CustMsgTypes /*CustomMsgTypes*/> msg;
//			//olc::net::message<olc::net::CustMsgTypes /*CustomMsgTypes*/> msg;
//			msg.header.id = olc::net::CustMsgTypes::ServerMessage;
//			msg.header.id = olc::net::CustMsgTypes /*CustomMsgTypes*/::ServerMessage;
//			msg << client->GetID();
//			MessageAllClients(msg, client);
//
//		}
//		break;
//		default:
//		{
//			std::cout << "Another message from client, message size is " << std::hex << std::showbase << msg.header.size << "\n";
//		}
//		}
//	}
//};

//-------

//
//

volatile int count = 0;

void ShowTime()
{
	int time = 0;
	while (1)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "Time from start = " << time++ << "\n";
		count++;
	}
}

int main()
{
	xo_server server(60000); 
	server.Start();

	std::thread thread1 = std::thread(ShowTime);


	while (1)
	{
		std::cout << "Time from start = " << count << "\n";
		server.Update(-1, true);
	}
	
	if (thread1.joinable()) thread1.join();

	return 0;
}