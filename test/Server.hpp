#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"

class Server {
	private:
		Socket						serverSock;
		std::vector<struct kevent>	changeList;
		struct kevent				eventList[8];
		std::map<int, std::string>	clientData;

		void		keventError(struct kevent * const & currEvent);
		void		addClient(void);
		void		recvClient(struct kevent * const & currEvent);
		void		keventRead(struct kevent * const & currEvent);
		std::string	makeResponse(void);
		void		keventWrite(struct kevent * const & currEvent);
	public:
		Server();
		virtual ~Server();

		void	CreateListenSock();
		void	Run();
		void	ChangeEvent(uintptr_t ident,
							int16_t filter,
							uint16_t flags,
							uint32_t fflags,
							intptr_t data,
							void* udata);
};

#endif

