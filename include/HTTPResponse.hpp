#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"

# define DATE "Date"
# define SERVER "Server"
# define CONTENT_LENGTH "Content-length"
# define CONTENT_TYPE "Content-type"
# define CONNECTION "Connection"

class HTTPResponse : public HTTPMessage
{
	private:
		std::string response;
		std::string	status_code;
		std::string	status_message;

		void		makeStatusLine(void);
		void		makeHeaderField(void);
		void		makeBody(void);

	public:
		HTTPResponse(void);
		virtual ~HTTPResponse(void);

		std::string	makeResponseMessage(void);

		void		setVersion(std::string _version);
		void		setStatusCode(StatusCode _status_code);
		void		setStatusMessage(std::string _status_message);
		void		setBody(std::string _body);

		void		addHeader(std::string _header_name, std::string _header_value);

		void		clear();
};

#endif
