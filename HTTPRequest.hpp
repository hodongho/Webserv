#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "HTTPMessage.hpp"

class HTTPRequest : public HTTPMessage
{
	private:
		std::string	method;
		std::string	URI;

	public:
		void										parseRequestMessage(std::string msg);

		const std::string&							getVersion(void) const;
		const std::string&							getMethod(void) const;
		const std::string&							getURI(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::string&							getBody(void) const;
};

#endif
