#include "ServerHandler.hpp"
#include "ConfigInfo.hpp"

ServerHandler::ServerHandler()
{
	// initialize content_type_table
	this->content_type_table_map[".html"] = "text/html; charset=utf-8";
	this->content_type_table_map[".txt"] = "text/plain; charset=utf-8";
	this->content_type_table_map[".jpeg"] = "image/jpeg";
	this->content_type_table_map[".gif"] = "image/gif";
	this->content_type_table_map[".png"] = "image/png";
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

void ServerHandler::handleListenEvent(SocketData* const & listen_sock)

{
	int					client_sock_fd;
	socklen_t			client_sock_addr_len;
	ClientSocketData*	client_socket;

	client_socket = new ClientSocketData();
	client_sock_addr_len = sizeof(client_socket->addr);
	client_sock_fd = accept(listen_sock->sock_fd, (sockaddr *)&(client_socket->addr), &client_sock_addr_len);
	if (client_sock_fd == -1)
	{
		delete client_socket;
		throwError("accept: ");
	}
	this->initClientSocketData(client_socket, client_sock_fd, listen_sock->addr);
	this->sock_list[client_sock_fd] = client_socket;
	std::cout << "accept port: " << ntohs(listen_sock->addr.sin_port) << std::endl;
	std::cout << "accept new client: " << client_sock_fd << std::endl;

	bool	tmp = true;
	setsockopt(client_sock_fd, SOL_SOCKET, SO_KEEPALIVE, &tmp, sizeof(tmp));

	fcntl(client_sock_fd, F_SETFL, O_NONBLOCK);
	changeEvent(client_sock_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
	changeEvent(client_sock_fd, EVFILT_WRITE, EV_ADD | EV_EOF | EV_DISABLE, 0, NULL, client_socket);
}

void ServerHandler::handleClientEvent(struct kevent * const & curr_event)
{
	ClientSocketData* client_data = static_cast<ClientSocketData*>(curr_event->udata);
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
			if (curr_event->filter == EVFILT_READ)
				this->readFileToBody(curr_event, client_data);
			break;
		case SOCKSTAT_CLIENT_MAKE_CGI_RESPONSE:
			this->readCgiPipeToBody(curr_event, client_data);
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

void ServerHandler::recvHeader(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
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

	if (client_socket->buf_str.size() > MAX_HEADER_SIZE)
	{
		this->setErrorPageResponse(STATCODE_BADREQ, curr_event, client_socket);
		return ;
	}

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
				if (client_socket->http_request.getContentLength() < 0)
					this->setErrorPageResponse(STATCODE_BADREQ, curr_event, client_socket);
				else if (static_cast<size_t>(client_socket->http_request.getContentLength()) == client_socket->buf_str.size())
				{
					this->setPostBody(curr_event, client_socket);
				}
				else
					client_socket->status = SOCKSTAT_CLIENT_RECV_BODY;
				break;
			case METHOD_DELETE :
				client_socket->status = SOCKSTAT_CLIENT_DELETE;
				changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
				changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
				break;
			case METHOD_NONE :
				this->setErrorPageResponse(STATCODE_BADREQ, curr_event, client_socket);
				break;
		}
	}
}

void ServerHandler::recvBody(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
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

	if (client_socket->buf_str.size() > conf.getMaxBodySize(ntohs(client_socket->listen_addr.sin_port)))
		this->setErrorPageResponse(STATCODE_BADREQ, curr_event, client_socket);
	if (static_cast<size_t>(client_socket->http_request.getContentLength()) <= client_socket->buf_str.size())
	{
		// client_socket->status = SOCKSTAT_CLIENT_POST;
		// client_socket->http_request.saveBody(client_socket->buf_str);
		// client_socket->buf_str.clear();
		// changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
		// changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
		this->setPostBody(curr_event, client_socket);
	}
}

void ServerHandler::readFileToBody(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	char		buf[RECV_BUF_SIZE];
	ssize_t		ret;

	ret = 1;
	while (ret)
	{
		ret = read(curr_event->ident, buf, RECV_BUF_SIZE - 1);
		buf[ret] = 0;
		client_socket->buf_str.append(buf, ret);
	}
	if (ret < 0)
		throwError("read respond body: ");
	close(curr_event->ident);
	client_socket->http_response.setBody(client_socket->buf_str);
	client_socket->http_response.setBasicField(client_socket->http_request);
	client_socket->buf_str.clear();
	client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
	changeEvent(client_socket->sock_fd, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
}

void ServerHandler::readCgiPipeToBody(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	if (curr_event->filter == EVFILT_WRITE)
	{
		ssize_t		wr_size;

		std::cout << RED << "EVFILT_WRITE: " << curr_event->ident << WHI << std::endl;
		wr_size = write(curr_event->ident, client_socket->buf_str.c_str(), client_socket->buf_str.size());
		if (wr_size <= 0)
		{
			if (wr_size < 0)
				throwError("write to cgi pipe");
			close(curr_event->ident);
		}
		client_socket->buf_str.erase(0, wr_size);
	}
	else if (curr_event->filter == EVFILT_READ)
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
			this->makeCgiPipeResponse(client_socket);
			client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
			changeEvent(client_socket->sock_fd, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
			return ;
		}
		client_socket->buf_str.append(buf, ret);
	}
}

void ServerHandler::makeCgiPipeIoEvent(std::string cgi_script_path,
			struct kevent* const & curr_event,
			ClientSocketData* const & client_socket)
{
	pid_t pid;
	int pipe_fd_result[2];
	int pipe_fd_input[2];

	if (pipe(pipe_fd_result))
		throwError("pipe: ");
	if (pipe(pipe_fd_input))
		throwError("pipe: ");
	pid = fork();
	if (pid == 0)
	{
		char **arg;
		char **env;
		this->initCgiVariable(arg, env, client_socket, cgi_script_path);
		std::cout << PUP << std::endl;
		if (close(pipe_fd_result[PIPE_RD]) == -1)
		{
			delete[] arg;
			delete[] env;
			exit(-1);
		}
		if (close(pipe_fd_input[PIPE_WR]) == -1)
		{
			delete[] arg;
			delete[] env;
			exit(-1);
		}
		if (dup2(pipe_fd_result[PIPE_WR], STDOUT_FILENO) == -1)
		{
			delete[] arg;
			delete[] env;
			exit(-1);
		}
		if (dup2(pipe_fd_input[PIPE_RD], STDIN_FILENO) == -1)
		{
			delete[] arg;
			delete[] env;
			exit(-1);
		}
		if (execve(arg[0], arg, env) == -1) // CGI use
		{
			delete[] arg;
			delete[] env;
			exit(-1);
		}
	}
	else
	{
		close(pipe_fd_result[PIPE_WR]);
		close(pipe_fd_input[PIPE_RD]);
		fcntl(pipe_fd_result[PIPE_RD], F_SETFL, O_NONBLOCK);
		fcntl(pipe_fd_input[PIPE_WR], F_SETFL, O_NONBLOCK);
		client_socket->status = SOCKSTAT_CLIENT_MAKE_CGI_RESPONSE;
		client_socket->buf_str = client_socket->http_request.getBody();
		changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
		changeEvent(pipe_fd_input[PIPE_WR], EVFILT_WRITE, EV_ADD | EV_EOF, 0, NULL, client_socket);
		changeEvent(pipe_fd_result[PIPE_RD], EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
		changeEvent(pid, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, NULL, client_socket);
		std::cout << BLU << "Make Pipe Event(input): " << pipe_fd_input[PIPE_WR] << std::endl;
		std::cout << BLU << "Make Pipe Event(result): " << pipe_fd_result[PIPE_RD] <<  WHI << std::endl;
	}
}

void ServerHandler::makeFileIoEvent(const std::string& stat_code,
			const std::string& file_path,
			struct kevent* const & curr_event,
			ClientSocketData* const & client_socket)
{
	int 											file_fd;
	std::string 									file_ext;
	std::map<std::string, std::string>::iterator	content_type_iter;

	file_ext = getExtension(file_path);
	client_socket->http_response.setStatusCode(stat_code);
	content_type_iter = this->content_type_table_map.find(file_ext);
	if (content_type_iter != content_type_table_map.end())
		client_socket->http_response.addHeader("Content-Type", content_type_iter->second);
	else
		client_socket->http_response.addHeader("Content-Type", "application/octet-stream"); // temp
	file_fd = open(file_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (file_fd == -1)
	{
		setDefaultServerError(client_socket->http_response, client_socket->http_request);
		client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
		return ;
	}
	client_socket->status = SOCKSTAT_CLIENT_MAKE_RESPONSE;
	changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
	changeEvent(file_fd, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, client_socket);
}

void ServerHandler::makeAutoIndexResponse(ClientSocketData* const & client_socket, std::string dir_path)
{
	DIR* dir_ptr;
	dirent* dirent_ptr;
	std::string page_body;

	dir_ptr = opendir(dir_path.c_str());
	if (dir_ptr == NULL)
		throwError("opendir: ");
	page_body = "\r\n";
	page_body += "<!DOCTYPE html>\r\n";
	page_body += "<html>\r\n";
	page_body += "<head>\r\n";
	page_body += "    <title>Index of " + dir_path + "</title>\r\n";
	page_body += "</head>\r\n";
	page_body += "<body>\r\n";
	page_body += "    <h1>Index of " + dir_path + "</h1>\r\n";
	page_body += "    <hr>\r\n";
	page_body += "    <ul>\r\n";
	while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
        if (dirent_ptr->d_name[0] == '.') {
            continue;
		}
		page_body += "        <li><a href=/";
		page_body += dirent_ptr->d_name;
		page_body += ">";
		page_body += dirent_ptr->d_name;
		page_body += "</a></li>\r\n";
	}
	page_body += "    </ul>\r\n";
	page_body += "    <hr>\r\n";
	page_body += "</body>\r\n";
	page_body += "</html>\r\n";
	client_socket->http_response.setStatusCode("200");
	client_socket->http_response.setBody(page_body);
	client_socket->http_response.addHeader("Content-Type", "text/html; charset=utf-8");
	client_socket->http_response.setBasicField(client_socket->http_request);
	client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
}

void ServerHandler::makeRedirectResponse(ClientSocketData* const & client_socket,
									const std::string& redir_loc)
{
	client_socket->http_response.setStatusCode("301");
	client_socket->http_response.addHeader("Location", redir_loc);
	client_socket->http_response.setBasicField(client_socket->http_request);
	client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
}

void ServerHandler::getMethod(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	std::string		file_path;
	enum PathState	path_stat;

	if (!this->conf.isAllowedMethod(client_socket->http_request.getURI(), ntohs(client_socket->listen_addr.sin_port), METHOD_GET))
	{
		this->setErrorPageResponse(STATCODE_NOTALLOW, curr_event, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), htons(client_socket->listen_addr.sin_port), file_path);
	switch (path_stat)
	{
	case PATH_NOTFOUND:
		this->setErrorPageResponse(STATCODE_NOTFOUND, curr_event, client_socket);
		break;
	case PATH_VALID:
		this->makeFileIoEvent("200", file_path, curr_event, client_socket);
		break;
	case PATH_AUTOINDEX:
		this->makeAutoIndexResponse(client_socket, file_path);
		break;
	case PATH_REDIRECT:
		this->makeRedirectResponse(client_socket, file_path);
		break;
	case PATH_CGI:
		this->setErrorPageResponse(STATCODE_NOTALLOW, curr_event, client_socket);
		break;
	}
}

void ServerHandler::postMethod(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	std::string		file_path;
	enum PathState	path_stat;

	if (!this->conf.isAllowedMethod(client_socket->http_request.getURI(), ntohs(client_socket->listen_addr.sin_port), METHOD_POST))
	{
		this->setErrorPageResponse(STATCODE_NOTALLOW, curr_event, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), htons(client_socket->listen_addr.sin_port), file_path);
	switch (path_stat)
	{
	case PATH_NOTFOUND:
		this->setErrorPageResponse(STATCODE_NOTFOUND, curr_event, client_socket);
		break;
	case PATH_VALID:
		this->makeFileIoEvent("200", file_path, curr_event, client_socket);
		break;
	case PATH_AUTOINDEX:
		this->makeAutoIndexResponse(client_socket, file_path);
		client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
		break;
	case PATH_REDIRECT:
		this->makeRedirectResponse(client_socket, file_path);
		break;
	case PATH_CGI:
		this->makeCgiPipeIoEvent(file_path, curr_event, client_socket);
		break;
	}
}

void ServerHandler::deleteMethod(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	std::string		file_path;
	enum PathState	path_stat;

	if (!this->conf.isAllowedMethod(client_socket->http_request.getURI(), ntohs(client_socket->listen_addr.sin_port), METHOD_DELETE))
	{
		this->setErrorPageResponse(STATCODE_NOTALLOW, curr_event, client_socket);
		return ;
	}
	path_stat = this->conf.convUriToPath(client_socket->http_request.getURI(), htons(client_socket->listen_addr.sin_port), file_path);
	if (path_stat == PATH_NOTFOUND)
	{
		this->setErrorPageResponse(STATCODE_NOTFOUND, curr_event, client_socket);
		return ;
	}
	else
	{
		if (unlink(file_path.c_str()))
		{
			if (errno == EISDIR)
			{
				client_socket->http_response.setStatusCode("403");
				client_socket->http_response.setBody("\r\ndelete rejected. (directory delete)");
				client_socket->http_response.addHeader("Content-Type", "text/plain");
				client_socket->http_response.setBasicField(client_socket->http_request);
				client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
			}
			else
				throwError("file unlink: ");
		}
		else
		{
			client_socket->http_response.setStatusCode("200");
			client_socket->http_response.setBody("\r\ndelete sucess!");
			client_socket->http_response.addHeader("Content-Type", "text/plain");
			client_socket->http_response.setBasicField(client_socket->http_request);
			client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
		}
	}
}

void ServerHandler::sendResponse(struct kevent * const & curr_event, ClientSocketData* const & client_socket)
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
		if (client_socket->http_request.getConnection() == "keep-alive")
		{
			this->clearClientSocketData(client_socket);
			changeEvent(curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, client_socket);
			changeEvent(curr_event->ident, EVFILT_READ, EV_ENABLE, 0, NULL, client_socket);
		}
		else
			this->closeEvent(curr_event);
	}
}

void ServerHandler::initListenerData(struct SocketData* listen_sock, const ServerConfig& server_conf)
{
	listen_sock->id_type = ID_LISTEN_SOCKET;

	memset(&listen_sock->addr, 0, sizeof(listen_sock->addr));
	listen_sock->addr.sin_family = AF_INET;
	inet_pton(AF_INET, server_conf.getHost().c_str(), &listen_sock->addr.sin_addr);
	listen_sock->addr.sin_port = htons(server_conf.getPort());
}

int ServerHandler::serverListen(const ServerConfig& serv_conf)
{
	SocketData* serverSocket = new SocketData();
	serverSocket->sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket->sock_fd == -1)
		throwError("socket: ");

	if (fcntl(serverSocket->sock_fd, F_SETFL, O_NONBLOCK) == -1)
		throwError("fcntl: ");

	int	bf = 1;

	if (setsockopt(serverSocket->sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&bf, (int)sizeof(bf)) == -1)
		throwError("setsockopt: ");

	this->initListenerData(serverSocket, serv_conf);
	this->sock_list[serverSocket->sock_fd] = serverSocket;

	if (bind(serverSocket->sock_fd, (const sockaddr *)&serverSocket->addr, sizeof(sockaddr_in)) == -1)
		throwError("bind: ");

	if (listen(serverSocket->sock_fd, 5) == -1)
		throwError("listen: ");

	changeEvent(serverSocket->sock_fd, EVFILT_READ, EV_ADD, 0, NULL, serverSocket);
	std::cout << "Server SocketData Created!" << std::endl;

	return (0);
}

void ServerHandler::serverReady(const char *conf_file)
{
	std::vector<ServerConfig>::const_iterator serv_conf_iter;

	this->conf.parseConfig(conf_file);
	this->conf.printWebservConfig(); //for test
	this->kq = kqueue();

	if (this->kq == -1)
		throwError("kqueue: ");
	for (serv_conf_iter = this->conf.getWebservConfig().begin(); serv_conf_iter < this->conf.getWebservConfig().end(); serv_conf_iter++)
		this->serverListen(*serv_conf_iter);
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
			if (curr_event->filter == EVFILT_PROC)
			{
				int status;
				if (waitpid(static_cast<pid_t>(curr_event->ident), &status, WNOHANG) == -1 || WEXITSTATUS(status) == -1)
					throwError("cgi proc error: ");
				continue;
			}
			if (curr_event->flags & EV_ERROR)
				keventError(sock_type->id_type);
			switch (sock_type->id_type)
			{
				case ID_LISTEN_SOCKET :
					handleListenEvent(sock_type);
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
	SocketData* sock_data = static_cast<SocketData *>(curr_event->udata);
	close(curr_event->ident);
	if (sock_data->id_type == ID_LISTEN_SOCKET)
		delete (sock_data);
	else
		delete static_cast<ClientSocketData *>(sock_data);
	this->sock_list.erase(curr_event->ident);
}

void ServerHandler::initClientSocketData(struct ClientSocketData* client_socket, const int& _sock_fd, const sockaddr_in _listen_addr)
{
	client_socket->sock_fd = _sock_fd;
	client_socket->listen_addr = _listen_addr;
	client_socket->id_type = ID_CLIENT_SOCKET;
	client_socket->status = SOCKSTAT_CLIENT_RECV_HEADER;
	client_socket->buf_str = "";
}

void ServerHandler::clearClientSocketData(struct ClientSocketData* client_socket)
{
	client_socket->id_type = ID_CLIENT_SOCKET;
	client_socket->status = SOCKSTAT_CLIENT_RECV_HEADER;
	client_socket->http_request.clear();
	client_socket->http_response.clear();
	client_socket->buf_str.clear();
}

void	ServerHandler::initCgiArg(char **&arg, const std::string& cgi_script_path, const unsigned short& port)
{
	std::cout << "ext: " << getExtension(cgi_script_path) << " port: " << port << std::endl;
	std::string cgi_program_path = this->conf.getCgiProgramPath(getExtension(cgi_script_path), port);

	arg = new char *[3];
	arg[0] = strdup(cgi_program_path.c_str());
	arg[1] = strdup(cgi_script_path.c_str());
	arg[2] = NULL;
}

void	ServerHandler::initCgiEnv(char **&arg, char **&env,  ClientSocketData* const & socket_data)
{
	std::map<std::string, std::string>	env_map;
	char								remote_addr[15];

	env_map["REQUEST_METHOD"]		= "POST";
	env_map["SERVER_PROTOCOL"]		= "HTTP/1.1";
	env_map["GATEWAY_INTERFACE"]	= "CGI/1.1";
	env_map["REMOTE_ADDR"]			= inet_ntop(AF_INET, &(socket_data->addr), remote_addr, socket_data->addr.sin_len);
	env_map["REMOTE_HOST"]			= itos(ntohs(socket_data->listen_addr.sin_port));
	env_map["SCRIPT_NAME"]			= socket_data->http_request.getURI();
	env_map["SERVER_NAME"]			= socket_data->http_request.getServerName();
	env_map["SERVER_PORT"]			= socket_data->http_request.getServerPort();
	env_map["SERVER_SOFTWARE"]		= "42 Web Server";
	env_map["SCRIPT_FILENAME"]		= arg[1];
	env_map["REDIRECT_STATUS"]		= "200";
	env_map["CONTENT_LENGTH"]		= itos(socket_data->http_request.getBody().size());
	env_map["CONTENT_TYPE"]			= socket_data->http_request.getContentType();

	int i = 0;
	env = new char *[14];
	for (std::map<std::string, std::string>::iterator it = env_map.begin(); it != env_map.end(); it++)
	{
		env[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	env[i] = NULL;
}

void	ServerHandler::initCgiVariable(char **&arg, char **&env,
			ClientSocketData* const & socket_data,
			const std::string& cgi_script_path)
{
	this->initCgiArg(arg, cgi_script_path, ntohs(socket_data->listen_addr.sin_port));
	this->initCgiEnv(arg, env, socket_data);
}

void ServerHandler::setErrorPageResponse(StatusCode err_stat, struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	std::string	err_file_path;

	if (this->conf.getErrorPage(err_stat, ntohs(client_socket->listen_addr.sin_port), err_file_path) == -1)
	{
		switch (err_stat)
		{
		case STATCODE_BADREQ:
			setDefaultBadRequest(client_socket->http_response, client_socket->http_request);
			break;
		case STATCODE_NOTFOUND:
			setDefaultNotFound(client_socket->http_response, client_socket->http_request);
			break;
		case STATCODE_NOTALLOW:
			setDefaultNotAllow(client_socket->http_response, client_socket->http_request);
			break;
		case STATCODE_SERVERR:
			setDefaultServerError(client_socket->http_response, client_socket->http_request);
			break;
		default:
			break;
		}
		client_socket->status = SOCKSTAT_CLIENT_SEND_RESPONSE;
	}
	else
	{
		switch (err_stat)
		{
		case STATCODE_BADREQ:
			this->makeFileIoEvent("400", err_file_path, curr_event, client_socket);
			break;
		case STATCODE_NOTFOUND:
			this->makeFileIoEvent("404", err_file_path, curr_event, client_socket);
			break;
		case STATCODE_NOTALLOW:
			this->makeFileIoEvent("405", err_file_path, curr_event, client_socket);
			break;
		case STATCODE_SERVERR:
			this->makeFileIoEvent("500", err_file_path, curr_event, client_socket);
			break;
		default:
			break;
		}
	}
}

void ServerHandler::setDefaultBadRequest(HTTPResponse& http_res, const HTTPRequest& http_req)
{
	std::string body;

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "\t<title>Bad Request</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "<h1>400 Bad Request</h1>\r\n";
	body += "\t<hr>\r\n";
	body += "\t<p>Bad Request message accepted</p>\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	http_res.setStatusCode("400");
	http_res.setBody(body);
	http_res.addHeader("Content-Type", "text/html");
	http_res.setBasicField(http_req);
}

void ServerHandler::setDefaultNotFound(HTTPResponse& http_res, const HTTPRequest& http_req)
{
	std::string body;

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "    <title>Not Found</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "    <h1>404 Not Found</h1>\r\n";
	body += "	<hr>\r\n";
	body += "	<p>Not found requested file</p>\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	http_res.setStatusCode("404");
	http_res.setBody(body);
	http_res.addHeader("Content-Type", "text/html");
	http_res.setBasicField(http_req);
}

void ServerHandler::setDefaultNotAllow(HTTPResponse& http_res, const HTTPRequest& http_req)
{
	std::string body;

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "    <title>Not Allowed Method</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "    <h1>405 Not Allowed Method</h1>\r\n";
	body += "	<hr>\r\n";
	body += "	<p>Not Allowed Method requested.</p>\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	http_res.setStatusCode("405");
	http_res.setBody(body);
	http_res.addHeader("Content-Type", "text/html");
	http_res.setBasicField(http_req);
}

void ServerHandler::setDefaultServerError(HTTPResponse& http_res, const HTTPRequest& http_req)
{
	std::string body;

	body = "<!DOCTYPE html>\r\n";
	body += "<html>\r\n";
	body += "<head>\r\n";
	body += "    <title>Server error</title>\r\n";
	body += "</head>\r\n";
	body += "<body>\r\n";
	body += "    <h1>500 Server error</h1>\r\n";
	body += "	<hr>\r\n";
	body += "	<p>Something went wrong with the server.</p>\r\n";
	body += "</body>\r\n";
	body += "</html>\r\n";

	http_res.setStatusCode("500");
	http_res.setBody(body);
	http_res.addHeader("Content-Type", "text/html");
	http_res.setBasicField(http_req);
}

void	ServerHandler::setPostBody(struct kevent* const & curr_event, ClientSocketData* const & client_socket)
{
	client_socket->status = SOCKSTAT_CLIENT_POST;
	client_socket->http_request.saveBody(client_socket->buf_str);
	client_socket->buf_str.clear();
	changeEvent(curr_event->ident, EVFILT_READ, EV_DISABLE, 0, NULL, client_socket);
	changeEvent(curr_event->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, client_socket);
}

void	ServerHandler::makeCgiPipeResponse(ClientSocketData* const & client_socket)
{
	std::string	header_part, body_part;
	size_t		rnrn_pos = 0;

	rnrn_pos = client_socket->buf_str.find("\r\n\r\n");

	header_part = client_socket->buf_str.substr(0, rnrn_pos + 2);
	body_part = client_socket->buf_str.substr(rnrn_pos + 4);

	std::stringstream	header_stream(header_part);

	std::string header_name, header_value, end, white_space = " \t";

	while(true)
	{
		std::getline(header_stream, header_name, ':');
		std::getline(header_stream, header_value, '\r');
		std::getline(header_stream, end, '\n');
		if (header_stream.eof())
			break ;

		if (header_value.find_first_not_of(white_space) != 0)
			header_value.erase(0, 1);

		client_socket->http_response.addHeader(header_name, header_value);
	}
	client_socket->http_response.setStatusCode("200");
	client_socket->http_response.setBody(body_part);
	client_socket->http_response.setBasicField(client_socket->http_request);
	client_socket->buf_str.clear();
}
