
#include "XOserver1.h"


	xo_server::xo_server(uint16_t nPort) : xop::net::server_interface(nPort)
	{
	};

	bool xo_server::OnClientConnect(std::shared_ptr<xop::net::connection> client)
	{
		xop::net::message msg;
		msg.header.id = xop::net::CustMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	void xo_server::OnClientDisconnect(std::shared_ptr<xop::net::connection> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	void xo_server::OnMessage(std::shared_ptr<xop::net::connection> client, xop::net::message& msg)
	{

		std::cout << (std::string)"Message from a client [" << std::to_string(client->GetID()) << "] \n";

		switch (msg.header.id)
		{
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