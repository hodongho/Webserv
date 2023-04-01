#include <vector>
#include <map>
#include <string>

struct Server;
struct Location;

std::vector<Server>  webserv_config;

struct Location
{
	std::string                location_path;
  vector<std::string>        allow_method;
  bool                       autoindex;
  std::map<int, std::string> redirection_path;
  /* to be added cgi infomation */
};

struct Server
{
	std::string           server_name;
  std::string           ip; // char *고려 
	unsigned __int16      port; //unsigned short
	std::string           root;
	std::string           index;
	size_t                client_max_body_size;
  std::string           error_page;
  std::vector<location> locations;
};

