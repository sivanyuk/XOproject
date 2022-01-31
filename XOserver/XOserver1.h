#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>

#include <xop_net.h>
#include "..\scrc\scrc_crypto.h"	//s& crypto 


class xo_server : public xop::net::server_interface
{
public:
	xo_server(uint16_t nPort);	// : xop::net::server_interface(nPort);
protected:

	virtual bool OnClientConnect(std::shared_ptr<xop::net::connection> client);

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<xop::net::connection> client);

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<xop::net::connection> client, xop::net::message& msg);
};

