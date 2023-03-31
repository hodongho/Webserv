#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"

class HTTPResponse : public HTTPMessage
{
	private:
		std::string	status_code;
		std::string	status_message;

	public:
		HTTPResponse() {};
		~HTTPResponse() {};
		std::string	makeResponseMessage(void);

		void		setVersion(std::string _version);
		void		setStatusCode(std::string _status_code);
		void		setStatusMessage(std::string _status_message);
		void		setBody(std::string _body);

		void		addHeader(std::string _header_name, std::string _header_value);
};

#endif
