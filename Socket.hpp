#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Webserv.hpp"

class Socket {
	private:
		int 		fd;
		sockaddr_in	addr;

	public:
		Socket() {};
		Socket(int& _fd):fd(_fd) {};
		Socket(const Socket& copy) {};

		virtual ~Socket() {};

		void				setFD(int _fd);
		const int&			getFD() const;
		void				initAddr();
		const sockaddr_in&	getAddr() const;
};

#endif
