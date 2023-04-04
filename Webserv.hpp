#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <netinet/in.h> //sockaddr_in
# include "utils.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"

// # include "ServerHandler.hpp"

# define PORT 4242

class HTTPRequest;
class HTTPResponse;

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
