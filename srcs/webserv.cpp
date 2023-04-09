#include "ServerHandler.hpp"
#include "ConfigInfo.hpp"

int main(int argc, char *argv[])
{
	ServerHandler kqueueServer;

	try
	{
		if (argc != 2)
			throwError("please input server config file as argument\n\t\te.g) ./webserv server.conf");
		kqueueServer.serverReady(argv[1]);
		kqueueServer.serverRun();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
