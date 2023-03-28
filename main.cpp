#include "Server.hpp"

int main()
{
	Server kqueueServer;

	try
	{
		kqueueServer.CreateListenSock();
		kqueueServer.Run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
