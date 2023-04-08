#ifndef SERVER_HPP
# define SERVER_HPP
# define HEADER_END_SIZE 4
# define MAX_HEADER_SIZE 3000
# define RECV_BUF_SIZE 50

# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/event.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include "Webserv.hpp"
# include "HTTPMessage.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"
# include "ConfigInfo.hpp"

class ServerHandler {
	private:
		int									kq;
		int									listen_sock_fd;
		std::vector<struct kevent>			change_list;
		struct kevent						event_list[8];
		std::map<int, SocketData*>			sock_list;
		std::map<std::string, std::string>	content_type_table_map;
		ConfigInfo							conf;

		void		changeEvent(const uintptr_t& ident,
								const int16_t& filter,
								const uint16_t& flags,
								const uint32_t& fflags,
								const intptr_t& data,
								void* const & udata);
		void		closeEvent(struct kevent * const & curr_event);
		void		initClientSocketData(struct SocketData* socket, const int& _sock_fd);
		void		clearClientSocketData(struct SocketData* socket);

		// serverReady sub function
		int			serverListen(void);
		void		initListenerData(struct SocketData* listen_sock);

		// serverRun sub function
		void		keventError(const IdentType& event_id_type);
		void		handleListenEvent(void);
		void		handleClientEvent(struct kevent* const & curr_event);
		void		recvHeader(struct kevent* const & curr_event, 
						SocketData* const & client_socket);
		void		recvBody(struct kevent* const & curr_event, 
						SocketData* const & client_socket);
		void		readFileToBody(struct kevent* const & curr_event, 
						SocketData* const & client_socket);
		void		readCgiPipeToBody(struct kevent* const & curr_event, 
						SocketData* const & client_socket);

		void		makeCgiPipeIoEvent(std::string cgi_script_path,
						struct kevent* const & curr_event,
						SocketData* const & client_socket);
		void		makeFileIoEvent(const std::string& stat_code,
						const std::string& file_path,
						struct kevent* const & curr_event,
						SocketData* const & client_socket);
		void		makeAutoIndexResponse(HTTPResponse& res, 
						std::string dir_path);

		void		getMethod(struct kevent* const & curr_event, SocketData* const & client_socket);
		void		postMethod(struct kevent* const & curr_event, SocketData* const & client_socket);
		void		deleteMethod(struct kevent* const & curr_event, SocketData* const & client_socket);
		void		sendResponse(struct kevent* const & curr_event, SocketData* const & client_socket);

		//default error page response generate
		void		setErrorPageResponse(StatusCode err_stat, struct kevent* const & curr_event, SocketData* const & client_socket);
		void		setDefaultBadRequest(HTTPResponse& http_res, const HTTPRequest& http_req);
		void		setDefaultNotFound(HTTPResponse& http_res, const HTTPRequest& http_req);
		void		setDefaultNotAllow(HTTPResponse& http_res, const HTTPRequest& http_req);
		void		setDefaultServerError(HTTPResponse& http_res, const HTTPRequest& http_req);

	public:
		ServerHandler(void);
		virtual ~ServerHandler(void);

		void	serverReady(const char *conf_file);
		void	serverRun(void);
};

#endif
