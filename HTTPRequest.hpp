#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <sstream>
# include <iostream>
# include <unistd.h> //path validation
# include "HTTPMessage.hpp"

# include <iomanip>

# define HOST "Host"
# define USER_AGENT "User-Agent"
# define ACCEPT "Accept"
# define CONNECTION "Connection"

# define WHI "\e[1;37m"
# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define BRW "\e[0;33m"
# define BLU "\e[0;34m"
# define PUP "\e[0;35m"
# define CYN "\e[0;36m"
class HTTPRequest : public HTTPMessage
{
	private:
		std::string	method;
		std::string	URI;

		void	parseStartLine(std::stringstream& request_stream);
		void	validateStartLine(void);
		void	parseHeaderField(std::stringstream& request_stream);
		void	validateHeaderField(void);

	public:
		HTTPRequest();
		~HTTPRequest();

		/**
		 * @brief HTTP Request start-line과 Header field를 파싱하는 함수입니다.
		 * 멤버변수에 값을 넣어준 뒤 start-line과 Header field의 유효성을 검사합니다.
		 *
		 *
		 * @param msg HTTP Request
		 */
		void										parseRequestMessage(std::string& msg);

		void										saveBody(std::string& _body);

		//getter
		const std::string&							getVersion(void) const;
		const std::string&							getMethod(void) const;
		const std::string&							getURI(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::string&							getBody(void) const;
		ssize_t 									getContentLength(void);
};

#endif
