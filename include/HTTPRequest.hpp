#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <sstream>
# include <unistd.h> //path validation
# include "HTTPMessage.hpp"

# include <iomanip>

# define HOST 		"Host"
# define CONNECTION	"Connection"

class HTTPRequest : public HTTPMessage
{
	private:
		MethodType	method;
		std::string	URI;

		int	parseStartLine(std::stringstream& request_stream);
		int	validateStartLine(void);
		int	parseHeaderField(std::stringstream& request_stream);
		int	validateHeaderField(void);

	public:
		HTTPRequest(void);
		virtual ~HTTPRequest(void);

		int											parseRequestMessage(std::string& msg);

		void										saveBody(const std::string& _body);

		//getter
		const std::string&							getVersion(void) const;
		const MethodType&							getMethod(void) const;
		const std::string&							getURI(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::string&							getBody(void) const;
		const std::string							getConnection(void) const;
		const std::string							getServerName(void)	const;
		const std::string							getServerPort(void)	const;
		const std::string							getLocalPath(void) const;
		const std::string							getContentType(void) const;
		ssize_t										getContentLength(void) const;

		void										clear(void);
};

#endif
