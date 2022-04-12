#include "XOserver1.h"
#include <mutex>



namespace xop
{

	void xo_server::clientsMove(std::shared_ptr<xop::net::connection> client, xop::net::message& msg)
	{

		uint32_t gameID = 0;
		uint32_t clientID = client->GetID();

		Move move = { 0 };
		msg >> move;
		uint32_t clientID1 = move.playerID;
		std::cout << "Game ID = " << gameID << ". The player[" << clientID << "] sent their move\n";

	}
};