
#include "xoclient1.h"


void xo_client::EchoServer()
{
	xop::net::message msg;
	std::string s = "This is just a message for echo";
	//message to the server
	msg = s;	// "This is just a message for echo";
	std::cout << "Sent to server :" << s << "\n";

	msg.header.id = xop::net::CustMsgTypes::ServerEcho;
	Send(msg);
}
