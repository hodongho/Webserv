#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <arpa/inet.h>
# include "Webserv.hpp"

class Socket {
	private:
		int 		fd;
		sockaddr_in	addr;

	public:
		Socket() {};
		Socket(int& _fd):fd(_fd) { getsockname(fd, (sockaddr *)&addr, 0); };
		Socket(const Socket& copy) {};

		virtual ~Socket() {};

		void				initServerAddr();
		void				setFD(int _fd);
		const int&			getFD() const;
		const sockaddr_in&	getAddr() const;
		const int&			getPort() const;
};

#endif
