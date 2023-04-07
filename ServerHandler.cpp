#include "ServerHandler.hpp"

ServerHandler::ServerHandler()
{
	// initialize content_type_table
	this->content_type_table_map[".html"] = "text/html";
	this->content_type_table_map[".txt"] = "text/plain";
	this->content_type_table_map[".jpeg"] = "image/jpeg";
	this->content_type_table_map[".gif"] = "image/gif";
};

ServerHandler::~ServerHandler()
{
	for (std::map<int, SocketData*>::iterator it = this->sock_list.begin(); it != this->sock_list.end(); it++)
	{
		close(it->first);
		delete (it->second);
	}
	this->sock_list.clear();
};


void ServerHandler::keventError(const IdentType& event_id_type)
{
	if (event_id_type == ID_LISTEN_SOCKET)
		throwError("sever socket: ");
	else if (event_id_type == ID_CLIENT_SOCKET)
		throwError("client socket: ");
}

void ServerHandler::handleListenEvent(void)
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
	this->initClientSocketData(client_socket, client_sock_fd);
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
		case SOCKSTAT_CLIENT_RECV_HEADER:
			if (curr_event->filter == EVFILT_READ)
				this->recvHeader(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_RECV_BODY:
			if (curr_event->filter == EVFILT_READ)
				this->recvBody(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_GET:
			if (curr_event->filter == EVFILT_WRITE)
				this->getMethod(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_POST:
			if (curr_event->filter == EVFILT_WRITE)
				this->postMethod(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_DELETE:
			if (curr_event->filter == EVFILT_WRITE)
				this->deleteMethod(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_MAKE_RESPONSE:
			if (curr_event->filter == EVFILT_WRITE)
				this->readLocal(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_SEND_RESPONSE:
			if (curr_event->filter == EVFILT_WRITE)
				this->sendResponse(curr_event, client_data);
			break;
		default:
			break;
	}
}

static void	printRecvData(const int& fd, const std::string& data, const ssize_t& recv_size)
{
	std::cout << "received request from "
			<< fd << ": \n\n"
			<< "received data size: "
			<< recv_size << "\n\n"
			<< GRN << data << WHI
			<< std::endl;
}


void ServerHandler::recvHeader(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	size_t		header_end_pos;
	ssize_t		ret;

	ret = recv(curr_event->ident, buf, RECV_BUF_SIZE - 1, 0);
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("recv header: ");
		close(curr_event->ident);
		std::cout << "socket closed successfully." << std::endl;
		sock_list.erase(curr_event->ident);
	}
	buf[ret] = 0;
	printRecvData(curr_event->ident, buf, ret); // for test
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
			case METHOD_GET :
				client_socket->status = SOCKSTAT_CLIENT_GET;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
			case METHOD_POST :
				// if (client_socket->http_request.getContentLength() < 0)
				// {
				// 	client_socket->status = SOCKSTAT_CLIENT_RECV_ERROR;
				//	client_socket->http_response.setStatusCode("400");
				// 	changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				// 	changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				// }
				client_socket->status = SOCKSTAT_CLIENT_RECV_BODY;
				break;
			case METHOD_DELETE :
				client_socket->status = SOCKSTAT_CLIENT_DELETE;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
			case METHOD_NONE :
				client_socket->http_response.setStatusCode("400");
				client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
				int fd = open(this->conf.getErrorPage(), O_RDONLY | O_NONBLOCK);
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(fd, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
		}
	}
}

void ServerHandler::recvBody(struct kevent* const & curr_event, SocketData* const & client_socket)
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
		client_socket->status = SOCKSTAT_CLIENT_POST;
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
		client_socket->http_response.setBody(client_socket->buf_str);
		client_socket->http_response.setBasicField();
		client_socket->buf_str.clear();
		client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
		changeEvent(client_socket->sock_fd, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
		return ;
	}
	client_socket->buf_str.append(buf, ret);
}

void ServerHandler::makeCgiPipeIoEvent(std::string cgi_file_path,
			struct kevent* const & curr_event,
			SocketData* const & client_socket)
{
	pid_t pid = fork();
	int pipe_fd_result[2];
	int pipe_fd_input[2];
	if (pipe(pipe_fd_result))
		throwError("pipe: ");
	if (pipe(pipe_fd_input))
		throwError("pipe: ");
	if (pid == 0)
	{
		close(pipe_fd_result[PIPE_RD]);
		close(pipe_fd_input[PIPE_RD]);
		dup2(pipe_fd_result[PIPE_WR], STDOUT_FILENO);
		dup2(pipe_fd_input[PIPE_RD], STDIN_FILENO);
		execve(cgi_file_path.c_str(), NULL, NULL); // CGI use
	}
	else
	{
		close(pipe_fd_result[PIPE_WR]);
		close(pipe_fd_input[PIPE_RD]);
		fcntl(pipe_fd_result[PIPE_RD], F_SETFL, O_NONBLOCK);
		fcntl(pipe_fd_input[PIPE_RD], F_SETFL, O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(pipe_fd_result[PIPE_RD], EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		changeEvent(pipe_fd_input[PIPE_WR], EVFILT_WRITE, EV_ADD | EV_EOF, 0, NULL, client_socket);
		changeEvent(pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, NULL, client_socket);
	}
}

void ServerHandler::makeFileIoEvent(enum PathState path_stat,
			std::string file_path,
			struct kevent* const & curr_event,
			SocketData* const & client_socket)
{
	int file_fd;

	switch (path_status)
	{
	case PATH_NOTFOUND:
		client_socket->http_response.setStatusCode("404");
		file_fd = open(this->conf.getErrorPage(STATCODE_NOTFOUND), O_RDONLY | O_NONBLOCK);
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		changeEvent(client_socket->sock_fd, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		break;
	case PATH_VALID:
		client_socket->http_response.setStatusCode("200");
		file_fd = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		break;
	case PATH_AUTOINDEX:
		client_socket->http_response.setStatusCode("200");
		this->makeAutoIndexResponse(client_socket->http_response, file_path);
		client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
		return ;
	case default:
		throwError("invalid path_status: ");
		break;
	}
}

void ServerHandler::makeAutoIndexResponse(HTTPResponse& res, std::string dir_path)
{
	DIR* dir_ptr;
	dirent* dirent_ptr;
	std::string page_body;

	dir_ptr = opendir(dir_path.c_str());
	if (dir_ptr == NULL)
		throwError("opendir: ");
	
	/**
	 * @brief write Autoindex page html page to std::string page_body;
	 * while(dirent_ptr = readdir(dir_ptr)!= NULL)
	 */
	res.setBody(page_body);
}

void ServerHandler::getMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	int				file_fd;
	std::string		file_path;
	enum PathState	path_stat;

	if (this->conf.isAllowedMethod(client_socket->http_request.getURI(), client_socket->addr.sin_port, GET) == -1)
	{
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		client_socket->http_response.setStatusCode("405");
		file_fd = open(this->conf.getErrorPage(STATCODE_NOTALLOW), O_RDONLY | O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), file_path);
	makeFileIoEvent(path_stat, file_path, curr_event, client_socket);
}

void ServerHandler::postMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	int				file_fd;
	std::string		file_path;
	enum PathState	path_stat;

	if (this->conf.isAllowedMethod(client_socket->http_request.getURI(), client_socket->addr.sin_port, POST) == 0)
	{
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		client_socket->http_response.setStatusCode("405");
		file_fd = open(this->conf.getErrorPage(STATCODE_NOTALLOW), O_RDONLY | O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), file_path);
	if (this->conf.isCgiRequest(client_socket->http_request.getURI()))
		this->makeCgiPipeIoEvent(file_path, curr_event, client_socket);
	else
		this->makeFileIoEvent(path_stat, file_path, curr_event, client_socket);
}

void ServerHandler::deleteMethod(struct kevent* const & curr_event, SocketData* const & client_socket)
{
	int				file_fd;
	std::string		file_path;
	enum PathState	path_stat;
	
	if (this->conf.isAllowedMethod(client_socket->http_request.getURI(), client_socket->addr.sin_port, DELETE) == 0)
	{
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		client_socket->http_response.setStatusCode("405");
		file_fd = open(this->conf.getErrorPage(STATCODE_NOTALLOW), O_RDONLY | O_NONBLOCK);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), file_path);
	if (path_stat == PATH_NOTFOUND)
	{
		client_socket->http_response.setStatusCode("404");
		file_fd = open(this->conf.getErrorPage(STATCODE_NOTFOUND), O_RDONLY | O_NONBLOCK);
		client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		changeEvent(client_socket->sock_fd, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
	}
	else
	{
		if (unlink(file_path.c_str()))
		{
			if (errno == EISDIR)
			{
				client_socket->http_response.setStatusCode("405");
				client_socket->http_response.setBody("delete rejected. (directory delete)");
				client_socket->http_response.setBasicField();
				client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
			}
			else
				throwError("file unlink: ");
		}
		else
		{
			client_socket->http_response.setStatusCode("200");
			client_socket->http_response.setBody("delete sucess!");
			client_socket->http_response.setBasicField();
			client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
		}
	}
}

void ServerHandler::sendResponse(struct kevent * const & curr_event, SocketData* const & client_socket)
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
		this->clearClientSocketData(client_socket);
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(curr_event->ident, EVFILT_READ, EV_ENABLE, 0, NULL, client_socket);
		// end of response.
		// keep-alive
	}
}

void ServerHandler::initListenerData(struct SocketData* listen_sock)
{
	listen_sock->id_type = ID_LISTEN_SOCKET;
	listen_sock->status = SOCKSTAT_SERVER_LISTEN;

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

void ServerHandler::serverRun()
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

		for (int loop = 0; loop < event_count; loop++)
		{
			curr_event = &this->event_list[loop];
			sock_type = static_cast<SocketData *>(curr_event->udata);
			if (fstat(curr_event->ident, &stat_buf) == -1)
				continue;
			if (curr_event->flags & EV_ERROR)
				keventError(sock_type->id_type);
			switch (sock_type->id_type)
			{
				case ID_LISTEN_SOCKET :
					handleListenEvent();
					break;
				case ID_CLIENT_SOCKET :
					handleClientEvent(curr_event);
					break;
				default:
					break;
			}
		}
	}
}

void ServerHandler::changeEvent(const uintptr_t& ident,
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

void ServerHandler::closeEvent(struct kevent * const & curr_event)
{
	IdentType event_id_type = static_cast<SocketData *>(curr_event->udata)->id_type;

	close(curr_event->ident);
	delete static_cast<SocketData *>(curr_event->udata);
	this->sock_list.erase(curr_event->ident);
}

void ServerHandler::initClientSocketData(struct SocketData* client_socket, const int& _sock_fd)
{
	client_socket->sock_fd = _sock_fd;
	client_socket->id_type = ID_CLIENT_SOCKET;
	client_socket->status = SOCKSTAT_CLIENT_RECV_HEADER;
	client_socket->buf_str = "";
}

void ServerHandler::clearClientSocketData(struct SocketData* client_socket)
{
	client_socket->id_type = ID_CLIENT_SOCKET;
	client_socket->status = SOCKSTAT_CLIENT_RECV_HEADER;
	client_socket->http_request.clear();
	client_socket->http_response.clear();
	client_socket->buf_str.clear();
}