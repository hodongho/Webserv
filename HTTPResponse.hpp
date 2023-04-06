#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"

# define DATE "Date"
# define SERVER "Server"
# define CONTENT_LENGTH "Content-length"
# define CONTENT_TYPE "Content-type"
# define CONNECTION "Connection"

# define ERR_BADREQ_PAGE_FILE_PATH "./error_page/400.html"
# define ERR_NOTFOUND_PAGE_FILE_PATH "./error_page/404.html"
# define ERR_NOTALLOW_PAGE_FILE_PATH "./error_page/405.html"
# define ERR_SERVERR_PAGE_FILE_PATH "./error_page/500.html"

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

		void		setVersion(const std::string& _version);
		void		setStatusCode(const std::string& _status_code);
		void		setStatusMessage(const std::string& _status_message);
		void		setBody(const std::string& _body);

		void		addHeader(const std::string& _header_name, const std::string& _header_value);

		StatusCode	getStatusCode(void) const;
		void		clear();

		void makeOkResponse(void);
		void makeNocontentResponse(void);
		void makeRedirectResponse(const std::string& location);
		void makeBadrequestResponse(void);
		void makeNotFoundResponse(void);
		void makeNotAllowResponse(void);
		void makeServerErrorResponse(void);
};

#endif
