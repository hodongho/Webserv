#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <map>
# include <string>
# include <iostream>
# include "manageStdout.hpp"

enum MethodType
{
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	METHOD_NONE
};

enum StatusCode
{
	STATCODE_OK,
	STATCODE_REDIR,
	STATCODE_BADREQ,
	STATCODE_FORBIDDEN,
	STATCODE_NOTFOUND,
	STATCODE_NOTALLOW,
	STATCODE_SERVERR
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
