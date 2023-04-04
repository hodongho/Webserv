#include "ServerHandler.hpp"

ServerHandler::ServerHandler() {};

ServerHandler::~ServerHandler()
{
	for (std::map<int, EventData*>::iterator it = this->fd_list.begin(); it != this->fd_list.end(); it++)
	{
		close(it->first);
		delete (it->second);
	}
	this->fd_list.clear();
};


void	ServerHandler::keventError(const IdentType& event_id_type)
{
	if (event_id_type == LISTEN_SOCKET)
		throwError("sever socket: ");
	else if (event_id_type == CLIENT_SOCKET)
		throwError("client socket: ");
}

void	ServerHandler::handleListenEvent(void)
{
	int			client_sock_fd;
	socklen_t	client_sock_addr_len;
	SocketData* client_socket;

	client_socket = new SocketData();
	client_sock_addr_len = sizeof(client_socket->addr);
	client_sock_fd = accept(this->listen_sock_fd, (sockaddr *)&(client_socket->addr), &client_sock_addr_len);
	if (client_sock_fd == -1)
	{
		delete client_socket;
		throwError("accept: ");
	}
	client_socket->id_type = CLIENT_SOCKET;
	client_socket->status = CLIENT_RECV_HEADER;
	client_socket->header_str = "";
	this->fd_list[client_sock_fd] = client_socket;

	std::cout << "accept new client: " << client_sock_fd << std::endl;

	bool	tmp = true;
	setsockopt(client_sock_fd, SOL_SOCKET, SO_KEEPALIVE, &tmp, sizeof(tmp));

	fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);
	changeEvent(client_sock_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
	changeEvent(client_sock_fd, EVFILT_WRITE, EV_ADD | EV_EOF | EV_DISABLE, 0, NULL, client_socket);
}

void ServerHandler::handleClientEvent(struct kevent * const & curr_event)
{
	SocketData* client_data = static_cast<SocketData*>(curr_event->udata);
	switch (client_data->status)
	{
		case CLIENT_RECV_ERROR:
			// if (curr_event->filter == EVFILT_WRITE)
			// 	this->makeErrorResponse(curr_event, client_data);
			break;
		case CLIENT_RECV_HEADER:
			if (curr_event->filter == EVFILT_READ)
				this->recvHeader(curr_event, client_data);
			break;
		case CLIENT_RECV_BODY:
			if (curr_event->filter == EVFILT_READ)
				this->recvBody(curr_event, client_data);
			break;
		case CLIENT_GET:
			if (curr_event->filter == EVFILT_WRITE)
				this->getMethod(curr_event, client_data);
			break;
		case CLIENT_POST:
			if (curr_event->filter == EVFILT_WRITE)
				this->postMethod(curr_event, client_data);
			break;
		case CLIENT_DELETE:
			if (curr_event->filter == EVFILT_WRITE)
				this->deleteMethod(curr_event, client_data);
			break;
		case CLIENT_SEND_RESPONSE:
			if (curr_event->filter == EVFILT_WRITE)
				this->sendResponse(curr_event, client_data);
			break;
		default:
			break;
	}
}

/*
 void ServerHandler::makeErrorResponse(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	switch (client_socket->http_response.getStatusCode())
	{
	case REDIR:
		this->makeRedirResponse(curr_event, client_socket);
		break;
	case BADREQ:
		this->makeBadReqResponse(curr_event, client_socket);
		break;
	case NOTFOUND:
		this->makeNotFoundResponse(curr_event, client_socket);
		break;
	case SERVERR:
		this->makeServErrResponse(curr_event, client_socket);
		break;
	default:
		break;
	}
}
 */

/*
static void	printRecvData(const int& fd, const std::string& data, const ssize_t& recv_size)
{
	std::cout << "received request from "
			<< fd << ": \n\n"
			<< "received data size: "
			<< recv_size << "\n\n"
			<< GRN << data << BLK
			<< std::endl;
}
*/

void	ServerHandler::recvHeader(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	size_t		header_end_pos;
	ssize_t		ret;

	ret = recv(curr_event->ident, buf, RECV_BUF_SIZE - 1, 0);
	buf[ret] = 0;
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("recv header: ");
		close(curr_event->ident);
		std::cout << "socket closed successfully." << std::endl;
		fd_list.erase(curr_event->ident);
	}
	client_socket->header_str.append(buf, ret);
	header_end_pos = client_socket->header_str.rfind("\r\n\r\n");

	/*
	if (client_socket->header_str.size() > conf->getMaxHeaderSize())
	{
		client_socket->status = CLIENT_RECV_ERROR;
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
 	*/

	if (header_end_pos != std::string::npos)
	{
		client_socket->body_str.append(client_socket->header_str.substr(header_end_pos + HEADER_END_SIZE));
		client_socket->header_str.erase(header_end_pos + HEADER_END_SIZE);
		client_socket->http_request.parseRequestMessage(client_socket->header_str);
		MethodType method = client_socket->http_request.getMethod();
		switch (method)
		{
			case GET :
				client_socket->status = CLIENT_GET;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
			case POST :
				// client_socket->body_size = client_socket->http_request.getContentLength();
				// if (client_socket->body_size < 0)
				// {
				// 	client_socket->status = CLIENT_RECV_ERROR;
				// 	changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				// 	changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				// }
				client_socket->status = CLIENT_RECV_BODY;
				break;
			case DELETE :
				client_socket->status = CLIENT_DELETE;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
			case NONE :
				client_socket->http_response.setStatusCode("400");
				client_socket->status = CLIENT_RECV_ERROR;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
		}
	}
}

void	ServerHandler::recvBody(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	ssize_t		ret;

	ret = recv(curr_event->ident, buf, RECV_BUF_SIZE - 1, 0);
	buf[ret] = 0;
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("recv body: ");
		close(curr_event->ident);
		std::cout << "socket closed successfully." << std::endl;
		fd_list.erase(curr_event->ident);
		return ;
	}
	client_socket->body_str.append(buf, ret);

	/*
	if (client_socket->header_str.size() > conf->getMaxBodySize() )
	{
		client_socket->status = CLIENT_RECV_ERROR;
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
 	*/

	if (static_cast<size_t>(client_socket->body_size) <= client_socket->body_str.size())
	{
		client_socket->status = CLIENT_POST;
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
}

void		getMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	client_socket->http_response.setVersion("HTTP/1.1");
	client_socket->http_response.setStatusCode("200");
	client_socket->http_response.setStatusMessage("OK");

	client_socket->http_response.addHeader(CONTENT_TYPE, "text/html; charset=utf-8");
	client_socket->http_response.addHeader(CONTENT_LENGTH, "163");
	client_socket->http_response.addHeader(CONNECTION, "keep-alive");

	std::string		file_path = 
	std::ifstream	file();
	std::string		body;

	if (file.is_open())
	{
		std::string buf;

		while (!file.eof())
		{
			std::getline(file, buf);
			body += buf;
			body += "\n";
		}
	}

	this->setBody(body);
}

void		postMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{

}

void		deleteMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{

}


void	ServerHandler::sendResponse(struct kevent * const & curr_event, SocketData* const & client_socket)
{
	ssize_t ret;
	std::string res;

	res = client_socket->http_response.makeResponseMessage();
	ret = send(curr_event->ident, res.c_str(), res.size(), 0);
	if (ret == -1)
	{
		closeEvent(curr_event);
		throwError("send: ");
	}
	else
	{
		client_socket->http_response.clear();
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		// end of response.
	}
}

void ServerHandler::initListenerData(struct SocketData* listen_sock)
{
	listen_sock->id_type = LISTEN_SOCKET;
	listen_sock->status = SERVER_LISTEN;

	memset(&listen_sock->addr, 0, sizeof(listen_sock->addr));
	listen_sock->addr.sin_family	= AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &listen_sock->addr.sin_addr);
	listen_sock->addr.sin_port = htons(PORT);
}

int ServerHandler::serverListen(void)
{
	this->listen_sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->listen_sock_fd == -1)
		throwError("socket: ");

	if (fcntl(this->listen_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		throwError("fcntl: ");

	int	bf = 1;

	if (setsockopt(this->listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&bf, (int)sizeof(bf)) == -1)
		throwError("setsockopt: ");

	SocketData* serverSocket = new SocketData();
	this->initListenerData(serverSocket);
	this->fd_list[this->listen_sock_fd] = serverSocket;

	if (bind(this->listen_sock_fd, (const sockaddr *)&serverSocket->addr, sizeof(sockaddr_in)) == -1)
		throwError("bind: ");

	if (listen(this->listen_sock_fd, 5) == -1)
		throwError("listen: ");

	changeEvent(this->listen_sock_fd, EVFILT_READ, EV_ADD, 0, NULL, serverSocket);
	std::cout << "Server SocketData Created!" << std::endl;

	return (0);
}


void ServerHandler::serverReady(void)
{
	this->kq = kqueue();

	if (this->kq == -1)
		throwError("kqueue: ");

	this->serverListen();
}

void	ServerHandler::serverRun()
{
	int				event_count;
	struct kevent*	curr_event;
	IdentType		event_id_type;
	struct stat		stat_buf;

	while (1)
	{
		event_count = kevent(this->kq,
							&this->change_list[0],
							this->change_list.size(),
							this->event_list,
							8, NULL);
		if (event_count == -1)
			throwError("kevent: ");

		this->change_list.clear();

		for (int i = 0; i < event_count; i++)
		{
			curr_event = &this->event_list[i];
			event_id_type = static_cast<EventData *>(curr_event->udata)->id_type;
			if (fstat(curr_event->ident, &stat_buf) == -1)
				continue;
			if (curr_event->flags & EV_ERROR)
				keventError(event_id_type);

			switch (event_id_type)
			{
				case LISTEN_SOCKET :
					handleListenEvent();
					break;
				case CLIENT_SOCKET :
					handleClientEvent(curr_event);
					break;
				default:
					break;
			}
		}
	}
}

void	ServerHandler::changeEvent(const uintptr_t& ident,
								const int16_t& filter,
								const uint16_t& flags,
								const uint32_t& fflags,
								const intptr_t& data,
								void* const & udata)
{
	struct kevent	newEvent;

	EV_SET(&newEvent, ident, filter, flags, fflags, data, udata);
	this->change_list.push_back(newEvent);
}

void	ServerHandler::closeEvent(struct kevent * const & curr_event)
{
	IdentType event_id_type = static_cast<EventData *>(curr_event->udata)->id_type;

	close(curr_event->ident);
	if (event_id_type == CLIENT_SOCKET || event_id_type == LISTEN_SOCKET)
		delete static_cast<SocketData *>(curr_event->udata);
	this->fd_list.erase(curr_event->ident);
}
