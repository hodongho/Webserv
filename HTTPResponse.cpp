#include "HTTPResponse.hpp"
#include <fstream>

HTTPResponse::HTTPResponse()
:HTTPMessage(), response(""), status_code(""), status_message("")
{};

HTTPResponse::~HTTPResponse()
{};

void	HTTPResponse::makeStatusLine()
{
	this->response +=	this->version + " " +
						this->status_code + " " +
						this->status_message + "\r\n";
}

void	HTTPResponse::makeHeaderField()
{
	std::map<std::string, std::string>::iterator it, end;

	it = header.begin();
	end = header.end();

	for(; it != end; it++)
	{
		this->response += it->first + ": " + it->second + "\r\n";
	}
	this->response += "\r\n";
}

void	HTTPResponse::makeBody()
{
	this->response += this->body;
}

std::string	HTTPResponse::makeResponseMessage()
{
	this->makeStatusLine();
	this->makeHeaderField();
	this->makeBody();

	std::cout << BRW << this->response << WHI << std::endl; //Test

	return (this->response);
}

void	HTTPResponse::setVersion(const std::string& _version)				{ this->version = _version; }
void	HTTPResponse::setStatusCode(const std::string& _status_code)		{ this->status_code = status_code; }
void	HTTPResponse::setStatusMessage(const std::string& _status_message)	{ this->status_message = _status_message; }
void	HTTPResponse::setBody(const std::string& _body) 					{ this->body = _body; }
void	HTTPResponse::addHeader(const std::string& _header_name, const std::string& _header_value) { header[_header_name] = _header_value; }

StatusCode	HTTPResponse::getStatusCode(void) const
{
	if (this->status_code == "200")
		return (OK);
	else if (this->status_code == "301")
		return (REDIR);
	else if (this->status_code == "400")
		return (BADREQ);
	else if (this->status_code == "404")
		return (NOTFOUND);
	else if (this->status_code == "500")
		return (SERVERR);
}

void	HTTPResponse::clear()
{
	this->version.clear();
	this->status_code.clear();
	this->status_message.clear();
	this->header.clear();
	this->body.clear();
	this->response.clear();
}


// response = "HTTP/1.1 200 OK\r\n";
// response += "Content-Type: text/html\r\n";
// response += "Content-Length: 163\r\n";
// response += "Connection: keep-alive\r\n\r\n";
