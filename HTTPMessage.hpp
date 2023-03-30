#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <map>
# include <string>

class HTTPMessage
{
	protected:
		std::string							version;
		std::map<std::string, std::string>	header;
		std::string							body;
};


#endif
