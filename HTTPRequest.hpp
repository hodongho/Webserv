#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <sstream>
# include <iostream>
# include "HTTPMessage.hpp"

# define WHI "\e[1;37m"
# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define BLU "\e[0;34m"

class HTTPRequest : public HTTPMessage
{
	private:
		std::string	method;
		std::string	URI;

	public:
		HTTPRequest() {};
		~HTTPRequest() {};

		void										parseRequestMessage(std::string msg);

		const std::string&							getVersion(void) const;
		const std::string&							getMethod(void) const;
		const std::string&							getURI(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::string&							getBody(void) const;
};

#endif
