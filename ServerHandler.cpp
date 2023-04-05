#include "ServerHandler.hpp"

ServerHandler::ServerHandler() {};

ServerHandler::~ServerHandler()
{
	for (std::map<int, SocketData*>::iterator it = this->sock_list.begin(); it != this->sock_list.end(); it++)
	{
		close(it->first);
		delete (it->second);
	}
	this->sock_list.clear();
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
	client_socket->buf_str = "";
	this->sock_list[client_sock_fd] = client_socket;

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
		case CLIENT_READ_LOCAL:
			if (curr_event->filter == EVFILT_WRITE)
				this->readLocal(curr_event, client_data);
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


static void	printRecvData(const int& fd, const std::string& data, const ssize_t& recv_size)
{
	std::cout << "received request from "
			<< fd << ": \n\n"
			<< "received data size: "
			<< recv_size << "\n\n"
			<< GRN << data << WHI
			<< std::endl;
}


void	ServerHandler::recvHeader(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	size_t		header_end_pos;
	ssize_t		ret;

	ret = recv(curr_event->ident, buf, RECV_BUF_SIZE - 1, 0);
	buf[ret] = 0;
	printRecvData(curr_event->ident, buf, ret);
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("recv header: ");
		close(curr_event->ident);
		std::cout << "socket closed successfully." << std::endl;
		sock_list.erase(curr_event->ident);
	}
	client_socket->buf_str.append(buf, ret);
	header_end_pos = client_socket->buf_str.rfind("\r\n\r\n");

	/*
	if (client_socket->buf_str.size() > conf->getMaxHeaderSize())
	{
		client_socket->status = CLIENT_RECV_ERROR;
		client_socket->http_response.setStatusCode("400");
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
 	*/

	if (header_end_pos != std::string::npos)
	{
		std::string temp;
		temp = client_socket->buf_str.substr(header_end_pos + HEADER_END_SIZE);
		client_socket->buf_str.erase(header_end_pos + HEADER_END_SIZE);
		client_socket->http_request.parseRequestMessage(client_socket->buf_str);
		client_socket->buf_str = temp;
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
				//	client_socket->http_response.setStatusCode("400");
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
		sock_list.erase(curr_event->ident);
		return ;
	}
	client_socket->buf_str.append(buf, ret);

	/*
	if (client_socket->header_str.size() > conf->getMaxBodySize() )
	{
		client_socket->status = CLIENT_RECV_ERROR;
		client_socket->http_response.setStatusCode("400");
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
 	*/

	if (client_socket->http_request.getContentLength() <= client_socket->buf_str.size())
	{
		client_socket->status = CLIENT_POST;
		client_socket->http_request.saveBody(client_socket->buf_str);
		client_socket->buf_str.clear();
		changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
	}
}

void ServerHandler::readLocal(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	ssize_t		ret;

	ret = read(curr_event->ident, buf, RECV_BUF_SIZE - 1);
	buf[ret] = 0;
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("read respond body: ");
		close(curr_event->ident);
		client_socket->status = CLIENT_SEND_RESPONSE;
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
		return ;
	}
	client_socket->buf_str.append(buf, ret);
}


static void	setTestResponse(HTTPResponse& res) // test response make
{
	res.setVersion("HTTP/1.1");
	res.setStatusCode("200");
	res.setStatusMessage("OK");

	res.addHeader(CONTENT_TYPE, "text/html; charset=utf-8");
	res.addHeader(CONNECTION, "keep-alive");

	std::string		file_path = "./html/example.html";
	std::ifstream	file(file_path);
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
	res.addHeader(CONTENT_LENGTH, std::to_string(body.size()));
	res.setBody(body);
}

void		ServerHandler::getMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	std::string file_path = this->conf.convUriToPath(client_socket->http_request.getURI(), file_path);
	struct stat file_stat;
	if (stat(file_path.c_str(), &file_stat) == -1)
	{
		client_socket->status = CLIENT_RECV_ERROR;
		client_socket->http_response.setStatusCode("404");
		return ;
	}
	if (this->conf.isCgiRequest(client_socket->http_request.getURI()))
	{
		pid_t pid = fork();
		int pipe_fd[2];
		if (pipe(pipe_fd))
			throwError("pipe: ");
		if (pid == 0)
		{
			close(pipe_fd[PIPE_RD]);
			dup2(pipe_fd[PIPE_WR], STDOUT_FILENO);
			execve(this->conf.convUriToPath(client_socket->http_request.getURI()), NULL, NULL); // CGI use
		}
		else
		{
			close(pipe_fd[PIPE_WR]);
			fcntl(pipe_fd[PIPE_RD], F_SETFL, O_NONBLOCK);
			changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
			changeEvent(pipe_fd[PIPE_RD], EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
			changeEvent(pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, NULL, client_socket);
		}
	}
	else
	{
		int fd = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
	}
	client_socket->status = CLIENT_READ_LOCAL;
	// setTestResponse(client_socket->http_response);
}

void		ServerHandler::postMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	std::string file_path = this->conf.convUriToPath(client_socket->http_request.getURI(), file_path);
	struct stat file_stat;
	if (stat(file_path.c_str(), &file_stat) == -1)
	{
		client_socket->status = CLIENT_RECV_ERROR;
		client_socket->http_response.setStatusCode("404");
		return ;
	}
	if (this->conf.isCgiRequest(client_socket->http_request.getURI()))
	{
		pid_t pid = fork();
		int pipe_fd[2];
		if (pipe(pipe_fd))
			throwError("pipe: ");
		if (pid == 0)
		{
			close(pipe_fd[PIPE_RD]);
			dup2(pipe_fd[PIPE_WR], STDOUT_FILENO);
			execve(this->conf.convUriToPath(client_socket->http_request.getURI()), NULL, NULL); // CGI use
		}
		else
		{
			close(pipe_fd[PIPE_WR]);
			fcntl(pipe_fd[PIPE_RD], F_SETFL, O_NONBLOCK);
			changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
			changeEvent(pipe_fd[PIPE_RD], EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
			changeEvent(pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, NULL, client_socket);
		}
	}
	else
	{
		int fd = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
	}
	client_socket->status = CLIENT_READ_LOCAL;
}

void		ServerHandler::deleteMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	std::string file_path = this->conf.convUriToPath(client_socket->http_request.getURI());
	struct stat file_stat;
	if (stat(file_path.c_str(), &file_stat) == -1)
	{
		client_socket->status = CLIENT_RECV_ERROR;
		client_socket->http_response.setStatusCode("404");
		return ;
	}
	client_socket->status = CLIENT_SEND_RESPONSE;
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
		client_socket->header_str.clear();
		client_socket->body_str.clear();
		client_socket->status = CLIENT_RECV_HEADER;
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_READ, EV_ENABLE, 0, NULL, client_socket);
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
	this->sock_list[this->listen_sock_fd] = serverSocket;

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
	SocketData*		sock_type;
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
			sock_type = static_cast<SocketData *>(curr_event->udata);
			if (fstat(curr_event->ident, &stat_buf) == -1)
				continue;
			if (curr_event->flags & EV_ERROR)
				keventError(sock_type->id_type);
			switch (sock_type->id_type)
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
	IdentType event_id_type = static_cast<SocketData *>(curr_event->udata)->id_type;

	close(curr_event->ident);
	if (event_id_type == CLIENT_SOCKET || event_id_type == LISTEN_SOCKET)
		delete static_cast<SocketData *>(curr_event->udata);
	this->sock_list.erase(curr_event->ident);
}
