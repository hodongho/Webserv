#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"
# include "HTTPRequest.hpp"

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

		void				makeStatusLine(void);
		void				makeHeaderField(void);
		void				makeBody(void);

		const std::string	getBodySize(void);

	public:
		HTTPResponse(void);
		virtual ~HTTPResponse(void);

		std::string	makeResponseMessage(void);

		void		setVersion(const std::string& _version);
		void		setStatusCode(const std::string& _status_code);
		void		setStatusMessage(const std::string& _status_message);
		void		setBody(const std::string& _body);

		void		addHeader(const std::string& _header_name, const std::string& _header_value);

		StatusCode	getStatusCode(void) const;

    void		    clear(void);

		void		setBasicField(const HTTPRequest& http_request);
};

#endif
