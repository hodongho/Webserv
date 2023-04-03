#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <unistd.h> //path validation
# include "HTTPMessage.hpp"
# include "manageStdout.hpp"

# include <iomanip>

# define HOST "Host"
# define USER_AGENT "User-Agent"
# define ACCEPT "Accept"
# define CONNECTION "Connection"

class HTTPRequest : public HTTPMessage
{
	private:
		Method		method;
		std::string	URI;

		int	parseStartLine(std::stringstream& request_stream);
		int	validateStartLine(void);
		int	parseHeaderField(std::stringstream& request_stream);
		int	validateHeaderField(void);

	public:
		HTTPRequest(void);
		virtual ~HTTPRequest(void);

		/**
		 * @brief HTTP Request start-line과 Header field를 파싱하는 함수입니다.
		 * 멤버변수에 값을 넣어준 뒤 start-line과 Header field의 유효성을 검사합니다.
		 *
		 *
		 * @param msg HTTP Request
		 */
		int											parseRequestMessage(std::string& msg);

		void										saveBody(std::string& _body);

		//getter
		const std::string&							getVersion(void) const;
		const Method&								getMethod(void) const;
		const std::string&							getURI(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::string&							getBody(void) const;
		ssize_t 									getContentLength(void);
};

#endif
