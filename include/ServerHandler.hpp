#ifndef SERVER_HPP
# define SERVER_HPP
# define HEADER_END_SIZE 4
# define MAX_HEADER_SIZE 3000
# define RECV_BUF_SIZE 1024

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
# include "webserv.hpp"
# include "HTTPMessage.hpp"
# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"
# include "ConfigInfo.hpp"

class ServerHandler {
	private:
		int									kq;
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
		void		closeEvent(SocketData * listen_socket);
		void		closeEvent(ClientSocketData * client_socket);
		void		initClientSocketData(struct ClientSocketData* socket, const int& _sock_fd, const sockaddr_in _listen_addr);
		void		clearClientSocketData(struct ClientSocketData* socket);

		// serverReady sub function
		int			serverListen(const ServerConfig& serv_conf);
		void		initListenerData(struct SocketData* listen_sock, const ServerConfig& server_conf);

		// serverRun sub function
		void		keventError(const IdentType& event_id_type);
		void		handleListenEvent(SocketData* const & listen_sock);
		void		handleClientEvent(struct kevent* const & curr_event);
		void		recvHeader(ClientSocketData* const & client_socket);
		void		recvBody(ClientSocketData* const & client_socket);

		void		readFileToBody(struct kevent* const & curr_event,
						ClientSocketData* const & client_socket);
		void		readCgiPipeToBody(struct kevent* const & curr_event,
						ClientSocketData* const & client_socket);

		void		makeCgiPipeIoEvent(std::string cgi_script_path,
						ClientSocketData* const & client_socket);
		void		makeFileIoEvent(const std::string& stat_code,
						const std::string& file_path,
						ClientSocketData* const & client_socket);
						
		void		makeAutoIndexResponse(ClientSocketData* const & client_socket,
						const std::string& dir_path);
		void		makeRedirectResponse(ClientSocketData* const & client_socket,
						const std::string& redir_loc);

		void		getMethod(ClientSocketData* const & client_socket);
		void		postMethod(ClientSocketData* const & client_socket);
		void		deleteMethod(ClientSocketData* const & client_socket);
		void		sendResponse(ClientSocketData* const & client_socket);

		void		setPostBody(ClientSocketData* const & client_socket);
		void		makeCgiPipeResponse(ClientSocketData* const & client_socket);

		// CGI
		void		initCgiVariable(char **&arg, char **&env,
						ClientSocketData* const & socket_data,
						const std::string& cgi_script_path);
		void		initCgiArg(char **&arg, const std::string& cgi_script_path, const unsigned short& port);
		void		initCgiEnv(char **&arg, char **&env, ClientSocketData* const & socket_data);

		//default error page response generate
		void		throwServerError(std::string msg, ClientSocketData* const & client_socket);
		void		setErrorPageResponse(StatusCode err_stat, ClientSocketData* const & client_socket);
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