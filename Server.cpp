#include "Server.hpp"

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
			{
				if (currEvent->ident == serverSock.getFD())
					throwError("sever socket: ");
				else
				{
					close(currEvent->ident);
					clientData.erase(currEvent->ident);
					throwError("client socket: ");
				}
			}
			else if (currEvent->filter == EVFILT_READ)
			{
				if (currEvent->ident == serverSock.getFD())
				{
					int clientSock;

					clientSock = accept(serverSock.getFD(), NULL, NULL);
					if (clientSock == -1)
						throwError("accept: ");
					std::cout << "accept new client: " << clientSock << std::endl;

					bool	tmp = true;
					// setsockopt(clientSock, SOL_SOCKET, SO_KEEPALIVE, &tmp, sizeof(tmp));
					fcntl(clientSock, F_SETFL, O_NONBLOCK);

					ChangeEvent(clientSock, EVFILT_READ, EV_ADD | EV_EOF, 0, NULL, NULL);
					ChangeEvent(clientSock, EVFILT_WRITE, EV_ADD | EV_EOF | EV_DISABLE, 0, NULL, NULL);
					clientData[clientSock] = "";
				}
				else if (clientData.find(currEvent->ident) != clientData.end())
				{
					char	buf[50];
					int		ret = recv(currEvent->ident, buf, sizeof(buf), 0);

					if (ret <= 0)
					{
						if (ret < 0)
							throwError("recv: ");
						std::cout << "EOF" << std::endl;
						close(currEvent->ident);
						clientData.erase(currEvent->ident);
					}
					else if (strstr(buf, "\r\n\r\n") != NULL)
					{
						buf[ret] = 0;

						clientData[currEvent->ident] = "HTTP/1.1 200 OK\r\n";
						clientData[currEvent->ident] += "Content-Type: image/jpeg\r\n";
						clientData[currEvent->ident] += "Content-Length: 40000\r\n";
						clientData[currEvent->ident] += "Connection: keep-alive\r\n\r\n";

						std::ifstream	html("./img/JPEG.jpeg");

						if (html.is_open())
						{
							std::string	buf;

							while (!html.eof())
							{
								std::getline(html, buf);
								clientData[currEvent->ident] += buf;
								clientData[currEvent->ident] += "\n";
							}
							clientData[currEvent->ident] += "\r\n\r\n";
						}
						ChangeEvent(currEvent->ident, EVFILT_WRITE, EV_ENABLE, 0, NULL, NULL);
						std::cout	<< "received request from "
									<< currEvent->ident << ": \n\n"
									<< GRN << buf << BLK
									<< std::endl;
						std::cout << "i'm here" << std::endl;
					}
					else
					{
						std::cout	<< "received request from "
									<< currEvent->ident << ": \n\n"
									<< GRN << buf << BLK
									<< std::endl;
					}
				}
			}
			else if (currEvent->filter == EVFILT_WRITE)
			{

				std::map<int, std::string>::iterator it = clientData.find(currEvent->ident);
				if (it != clientData.end())
				{
					std::cout << "Write Event fd: " << currEvent->ident << std::endl;
					std::cout << "New Events: " << newEvents << std::endl;
					if (clientData[currEvent->ident] != "")
					{
						int ret;
						ret = send(currEvent->ident, clientData[currEvent->ident].c_str(),
						clientData[currEvent->ident].size(), 0);
						if (ret == -1)
						{
							close(currEvent->ident);
							clientData.erase(currEvent->ident);
							throwError("send: ");
						}
						else
						{
							// std::cout << clientData[currEvent->ident] << std::endl;
							clientData[currEvent->ident].clear();
							ChangeEvent(currEvent->ident, EVFILT_WRITE, EV_DISABLE, 0, NULL, NULL);
							// close(currEvent->ident);
						}
					}
				}
			}
		}
	}
}

void	Server::ChangeEvent(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void* udata)
{
	struct kevent	newEvent;

	EV_SET(&newEvent, ident, filter, flags, fflags, data, udata);
	changeList.push_back(newEvent);
}
