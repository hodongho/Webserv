#include "Server.hpp"

Server::Server()
{};

Server::~Server()
{
	close(serverSock.getFD());
};

void	Server::keventError(struct kevent * const & currEvent)
{
	if (currEvent->ident == (uintptr_t)serverSock.getFD())
		throwError("sever socket: ");
	else
	{
		close(currEvent->ident);
		clientData.erase(currEvent->ident);
		throwError("client socket: ");
	}
}
void	Server::addClient(void)
{
	int clientSock;

	clientSock = accept(serverSock.getFD(), NULL, NULL);
	if (clientSock == -1)
		throwError("accept: ");
	std::cout << "accept new client: " << clientSock << std::endl;

	// bool	tmp = true;
	// setsockopt(clientSock, SOL_SOCKET, SO_KEEPALIVE, &tmp, sizeof(tmp));
	fcntl(clientSock, F_SETFL, O_NONBLOCK);

	ChangeEvent(clientSock, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, NULL);
	ChangeEvent(clientSock, EVFILT_WRITE, EV_ADD | EV_EOF | EV_DISABLE, 0, NULL, NULL);
	clientData[clientSock] = "";
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

void	Server::recvClient(struct kevent * const & currEvent)
{
	char	buf[1024];
	ssize_t	ret = recv(currEvent->ident, buf, sizeof(buf), 0);

	buf[ret] = 0;
	clientData[currEvent->ident] += buf;
	printRecvData(currEvent->ident, buf, ret);
	if (ret <= 0)
	{
		if (ret < 0)
			throwError("recv: ");
		close(currEvent->ident);
		std::cout << "socket closed successfully." << std::endl;
		clientData.erase(currEvent->ident);
	}
	else if (clientData[currEvent->ident].rfind("\r\n\r\n") != std::string::npos)
	{
		ChangeEvent(currEvent->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, NULL);
	}
}

void	Server::keventRead(struct kevent * const & currEvent)
{
	if (currEvent->ident == (uintptr_t)serverSock.getFD())
		addClient();
	else if (clientData.find(currEvent->ident) != clientData.end())
		recvClient(currEvent);
}

std::string	Server::makeResponse(void)
{
	std::string	res_msg;

	// res_msg = "HTTP/1.1 200 OK\r\n";
	// res_msg += "Content-Type: text/html\r\n";
	// res_msg += "Content-Length: 163\r\n";
	// res_msg += "Connection: keep-alive\r\n\r\n";

	// std::ifstream	html("./html/Hello.html");

	// if (html.is_open())
	// {
	// 	std::string	buf;

	// 	while (!html.eof())
	// 	{
	// 		std::getline(html, buf);
	// 		res_msg += buf;
	// 		res_msg += "\n";
	// 	}
	// }
	res_msg = "HTTP/1.1 307 AAA\n\
Vary: Origin\n\
Access-Control-Allow-Credentials: true\n\
Location: https://www.google.com\n\
Date: Sun, 02 Apr 2023 11:31:20 GMT\n\
Connection: keep-alive\n\
Keep-Alive: timeout=5\n\
Content-Length: 21\r\n\r\n\
123456789012345678901\r\n\r\n";
	return (res_msg);
}

void	Server::keventWrite(struct kevent * const & currEvent)
{
	std::map<int, std::string>::iterator it = clientData.find(currEvent->ident);

	if (it != clientData.end())
	{
		ssize_t ret;
		HTTPRequest	request;
		std::string res;

		request.parseRequestMessage(clientData[currEvent->ident]);
		res = makeResponse();
		ret = send(currEvent->ident, res.c_str(), res.size(), 0);
		if (ret == -1)
		{
			close(currEvent->ident);
			clientData.erase(currEvent->ident);
			throwError("send: ");
		}
		else
		{
			clientData[currEvent->ident].clear();
			ChangeEvent(currEvent->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, NULL);
		}
	}
}

void	Server::CreateListenSock()
{
	sockaddr_in	addr;
	int			bf = 1;

	serverSock.setFD(socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));

	if (serverSock.getFD() == -1)
		throwError("socket: ");

	if (fcntl(serverSock.getFD(), F_SETFL, O_NONBLOCK) == -1)
		throwError("fcntl: ");

	if (setsockopt(serverSock.getFD(), SOL_SOCKET, SO_REUSEADDR, (char *)&bf, (int)sizeof(bf)) == -1)
		throwError("setsockopt: ");

	// memset(&addr, 0, sizeof(addr));
	// addr.sin_family	= AF_INET;
	// addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// addr.sin_port = htons(PORT);

	serverSock.initServerAddr();

	if (bind(serverSock.getFD(), (const sockaddr *)&serverSock.getAddr(), sizeof(addr)) == -1)
		throwError("bind: ");

	if (listen(serverSock.getFD(), 5) == -1)
		throwError("listen: ");

	std::cout << "Server Socket Created!" << std::endl;
}

void	Server::Run()
{
	int		newEvents;
	struct	kevent *currEvent;
	int		queue = kqueue();

	if (queue == -1)
		throwError("kqueue: ");

	ChangeEvent(serverSock.getFD(), EVFILT_READ, EV_ADD, 0, NULL, NULL);

	while (1)
	{
		// std::cout << "Waiting Event" << std::endl;
		newEvents = kevent(queue, &changeList[0], changeList.size(), eventList, 8, NULL);
		if (newEvents == -1)
			throwError("kevent: ");

		changeList.clear();

		for (int i = 0; i < newEvents; i++)
		{
			currEvent = &eventList[i];

			if (currEvent->flags & EV_ERROR)
				keventError(currEvent);
			else if (currEvent->filter == EVFILT_READ)
				keventRead(currEvent);
			else if (currEvent->filter == EVFILT_WRITE)
				keventWrite(currEvent);
		}
	}
}

void	Server::ChangeEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
{
	struct kevent	newEvent;

	EV_SET(&newEvent, ident, filter, flags, fflags, data, udata);
	changeList.push_back(newEvent);
}
