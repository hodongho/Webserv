#include "ServerHandler.hpp"

int main()
{
	ServerHandler kqueueServer;

	try
	{
		kqueueServer.serverReady();
		kqueueServer.serverRun();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
