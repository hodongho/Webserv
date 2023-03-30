#include "Socket.hpp"

void	Socket::setFD(int _fd)
{
	getsockname(fd, (sockaddr *)&addr, 0);
	fd = _fd;
}

const int&	Socket::getFD() const
{
	return (fd);
}

void	Socket::initServerAddr()
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family	= AF_INET;
	// addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	addr.sin_port = htons(PORT);
}

const sockaddr_in&	Socket::getAddr() const
{
	return (addr);
}
