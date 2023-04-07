#include "HTTPResponse.hpp"
#include <fstream>
#include <stdlib.h>
#include "utils.hpp"

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
	this->setVersion("HTTP/1.1");
	this->setStatusCode(OK);
	this->setStatusMessage("OK");

	this->addHeader(CONTENT_TYPE, "text/html; charset=utf-8");
	this->addHeader(CONTENT_LENGTH, "163");
	this->addHeader(CONNECTION, "keep-alive");

	std::ifstream	html("./html/Hello.html");
	std::string		body;

	if (html.is_open())
	{
		std::string buf;

		while (!html.eof())
		{
			std::getline(html, buf);
			body += buf;
			body += "\n";
		}
	}

	this->setBody(body);

	this->makeStatusLine();
	this->makeHeaderField();
	this->makeBody();

	std::cout << BRW << this->response << WHI << std::endl; //Test

	return (response);
}

void	HTTPResponse::setVersion(std::string _version)				{ this->version = _version; }
void	HTTPResponse::setStatusMessage(std::string _status_message)	{ this->status_message = _status_message; }
void	HTTPResponse::setBody(std::string _body) 					{ this->body = _body; }
void	HTTPResponse::addHeader(const std::string& _header_name, const std::string& _header_value) { header[_header_name] = _header_value; }

void	HTTPResponse::setStatusCode(StatusCode _status_code)
{
	switch (_status_code)
	{
	case OK:
		this->status_code = "200";
		break;

	case REDIR:
		this->status_code = "301";
		break;

	case BADREQ:
		this->status_code = "400";
		break;

	case NOTFOUND:
		this->status_code = "404";
		break;

	case SERVERR:
		this->status_code = "500";
		break;

	default:
		break;
	}
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

const std::string	HTTPResponse::getBodySize()
{
	std::string size = itos(body.size());
	return (size);
}

void	HTTPResponse::setBasicField(const HTTPRequest& http_request)
{
	this->addHeader(CONTENT_LENGTH, this->getBodySize());
	this->addHeader(CONNECTION, http_request.getConnection());
}
