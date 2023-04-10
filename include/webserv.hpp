#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <netinet/in.h> //sockaddr_in
# include "utils.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"
// # include "ServerHandler.hpp"

# define PORT 4242
# define PIPE_RD 0
# define PIPE_WR 1

class HTTPRequest;
class HTTPResponse;

enum IdentType
{
	ID_LISTEN_SOCKET,
	ID_CLIENT_SOCKET
};

enum SocketStatus
{
	SOCKSTAT_CLIENT_RECV_HEADER,
	SOCKSTAT_CLIENT_RECV_BODY,
	SOCKSTAT_CLIENT_MAKE_RESPONSE,
	SOCKSTAT_CLIENT_MAKE_CGI_RESPONSE,
	SOCKSTAT_CLIENT_SEND_RESPONSE,
	SOCKSTAT_CLIENT_GET,
	SOCKSTAT_CLIENT_POST,
	SOCKSTAT_CLIENT_DELETE,
};

struct SocketData
{
	int				sock_fd;
	sockaddr_in		addr;
	IdentType		id_type;
};

struct ClientSocketData : public SocketData
{
	sockaddr_in		listen_addr;
	SocketStatus	status;
	HTTPRequest		http_request;
	HTTPResponse	http_response;
	std::string		buf_str;
};

#endif