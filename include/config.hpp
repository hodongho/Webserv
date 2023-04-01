#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <map>
# include <string>

struct ServerConfig;
struct LocationConfig;

std::vector<ServerConfig>	webserv_config;

struct LocationConfig
{
	std::string					location_path;
	std::vector<std::string>	allow_method;
	bool						autoindex;
	std::map<int, std::string>	redirection_path;
	/* to be added cgi infomation */
};

struct ServerConfig
{
	std::string					server_name;
	std::string					ip;		// char *고려 
	uint16_t					port;	//unsigned short
	std::string					root;
	std::string					index;
	size_t						client_max_body_size;
	std::map<int, std::string>	error_page;
	std::vector<LocationConfig>	locations;
};

#endif