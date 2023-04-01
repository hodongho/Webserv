#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "utils.hpp"
# include "ServerHandler.hpp"
# include "SocketData.hpp"

# define PORT 4242

# define BLK "\e[0;30m"
# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define CYN "\e[0;34m"

enum IdentType
{
	LISTEN_SOCKET,
	CLIENT_SOCKET,
	PIPE,
	PROCESS
};

enum SocketStatus
{
	SERVER_LISTEN,
	CLIENT_RECV_ERROR,
	CLIENT_RECV_HEADER,
	CLIENT_RECV_BODY,
	CLIENT_SEND_RESPONSE,
	CLIENT_WAIT_CGI,
	CLIENT_READY_CGI
};

struct EventData
{
	IdentType	id_type;
};

struct SocketData : public EventData
{
	sockaddr_in		addr;
	SocketStatus	status;
	HTTPRequest		http_request;
	HTTPResponse	http_response;
	std::string		header_str;
	std::string		body_str;
	ssize_t			body_size;
};

#endif
