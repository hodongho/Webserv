#include "Server.hpp"

int main()
{
	Server kqueueServer;

	std::cout << WHI;

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
