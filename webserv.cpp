// #include "ServerHandler.hpp"
#include "ConfigInfo.hpp"

int main(int argc, char *argv[])
{
	// ServerHandler	kqueueServer;
	ConfigInfo		conf;

	try
	{
		if (argc != 2)
		{
			std::cerr << "please input server config file as argument\n\t\te.g) ./webserv server.conf" << std::endl;
			return (1);
		}
			// throwError("please input server config file as argument\n\t\te.g) ./webserv server.conf");
		conf.parseConfig(argv[1]);
		conf.printWebservConfig();
		// kqueueServer.serverReady(argv[1]);
		// kqueueServer.serverRun();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
