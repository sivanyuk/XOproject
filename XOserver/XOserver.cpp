
#include "XOserver.h"


// Just show a time from start with 100s period to show the server is alive
void ShowTime()
{
	int time = 0;
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100000));
		std::cout << "Time from start = " << ++time * 100 << " s.\n";
	}
}


//Server itself 
int main()
{
	xop::net::СonnectionInfo conn_info;
	xo_server server(conn_info.port());
	server.Start();

	std::thread thread1 = std::thread(ShowTime);


	//the main cycle
	while (1)
	{
		server.Update(-1, true);
	}


	if (thread1.joinable()) thread1.join();

	return 0;
}