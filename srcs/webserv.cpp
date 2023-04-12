#include "ServerHandler.hpp"
#include "ConfigInfo.hpp"


void	testConvUriToPath(ConfigInfo& conf)
{
	std::string	uri_abs_file_path;
	std::string	request_uri;
	// - 127.0.0.1:4242/index.php
	// "http://localhost:4242/index.html/test/j324j32kl4/";   /srcs/HTTPMessage.cpp
	request_uri = "localhost:4242/index.html/test/j324j32kl4/";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/index.html/test/j324j32kl4/";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/srcs/HTTPResponse.cpp";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "localhost:4242//index.html";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/test/index.html";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/var/form.html";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/var/";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/var";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/srcs";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/cgi/cgi.php";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/cgi/cgi.py";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/srcs/ConfigInfo.cpp";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "/srcs/mine";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	request_uri = "should be error/srcs/mine";
	conf.convUriToPath(request_uri, 5525, uri_abs_file_path);
	// std::string	uri = "localhost:4242/index.html";
	// conf.printFilePathofURI(URI);
	// conf.convUriToPath();
}

int main(int argc, char *argv[])
{
	ServerHandler	kqueueServer;
	ConfigInfo		conf;

	try
	{
		if (argc != 2)
		{
			std::cerr << "please input server config file as argument\n\t\te.g) ./webserv server.conf" << std::endl;
			return (1);
		}
		kqueueServer.serverReady(argv[1]);
		kqueueServer.serverRun();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
