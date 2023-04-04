#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <map>
# include <string>
# include <iostream>
# include "manageStdout.hpp"

enum MethodType
{
	GET,
	POST,
	DELETE,
	NONE
};

enum StatusCode
{
	OK,
	REDIR,
	BADREQ,
	NOTFOUND,
	SERVERR,
};

class HTTPMessage
{
	protected:
		std::string							version;
		std::map<std::string, std::string>	header;
		std::string							body;

	public:
		HTTPMessage();
		virtual ~HTTPMessage();
};

#endif
