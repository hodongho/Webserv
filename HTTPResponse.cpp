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
						this->status_message + "\r\n\r\n";
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
	this->response += "\r\n\r\n";
}

void	HTTPResponse::makeBody()
{
	this->response += this->body + "\r\n\r\n";
}

std::string	HTTPResponse::makeResponseMessage()
{
	this->makeStatusLine();
	this->makeHeaderField();
	this->makeBody();

	// response = "HTTP/1.1 200 OK\r\n";
	// response += "Content-Type: text/html\r\n";
	// response += "Content-Length: 163\r\n";
	// response += "Connection: keep-alive\r\n\r\n";

	// std::ifstream	html("./html/Hello.html");

	// if (html.is_open())
	// {
	// 	std::string	buf;

	// 	while (!html.eof())
	// 	{
	// 		std::getline(html, buf);
	// 		response += buf;
	// 		response += "\n";
	// 	}
	// }

	return (response);
}

void	HTTPResponse::setVersion(std::string _version)				{ this->version = _version; }
void	HTTPResponse::setStatusCode(std::string _status_code)		{ this->status_code = _status_code; }
void	HTTPResponse::setStatusMessage(std::string _status_message)	{ this->status_message = _status_message; }
void	HTTPResponse::setBody(std::string _body) 					{ this->body = _body; }
void	HTTPResponse::addHeader(std::string _header_name, std::string _header_value) { header[_header_name] = _header_value; }
