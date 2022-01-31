
#pragma once


#include <iostream>
#include <vector>
#include <array>
#include <string>

#include <xop_net.h>
#include "..\scrc\scrc_crypto.h"	//s& crypto 



// Forward declare
class xo_client : public xop::net::client_interface<xop::net::CustMsgTypes>
{
public:
	void EchoServer();
};

