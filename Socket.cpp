#include "Socket.hpp"

void	Socket::setFD(int _fd)
{
	fd = _fd;
}

const int&	Socket::getFD() const
{
	return (fd);
}

void	Socket::initAddr()
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family	= AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
}

const sockaddr_in&	Socket::getAddr() const
{
	getsockname(fd, (sockaddr *)&addr, 0);
	return (addr);
}
