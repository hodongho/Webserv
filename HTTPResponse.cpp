#include "HTTPResponse.hpp"
#include <fstream>
#include <stdlib.h>
#include "utils.hpp"

HTTPResponse::HTTPResponse()
:HTTPMessage(), response(""), status_code(""), status_message("")
{};

HTTPResponse::~HTTPResponse()
{};

void HTTPResponse::makeStatusLine()
{
	this->response +=	this->version + " " +
						this->status_code + " " +
						this->status_message + "\r\n";
}

void HTTPResponse::makeHeaderField()
{
	std::map<std::string, std::string>::iterator it, end;

	it = header.begin();
	end = header.end();

	for(; it != end; it++)
	{
		this->response += it->first + ": " + it->second + "\r\n";
	}
}

void HTTPResponse::makeBody()
{
	this->response += this->body;
}

std::string HTTPResponse::makeResponseMessage()
{
	this->makeStatusLine();
	this->makeHeaderField();
	this->makeBody();

	std::cout << BRW << this->response << WHI << std::endl; //Test

	return (this->response);
}

void	HTTPResponse::setVersion(const std::string& _version)										{ this->version = _version; }
void	HTTPResponse::setStatusMessage(const std::string& _status_message)							{ this->status_message = _status_message; }
void	HTTPResponse::setBody(const std::string& _body) 											{ this->body = _body; }
void	HTTPResponse::addHeader(const std::string& _header_name, const std::string& _header_value)	{ header[_header_name] = _header_value; }

StatusCode HTTPResponse::getStatusCode(void) const
{
	if (this->status_code == "200")
		return (STATCODE_OK);
	else if (this->status_code == "301")
		return (STATCODE_REDIR);
	else if (this->status_code == "400")
		return (STATCODE_BADREQ);
	else if (this->status_code == "404")
		return (STATCODE_NOTFOUND);
	else if (this->status_code == "405")
		return (STATCODE_NOTALLOW);
	else if (this->status_code == "500")
		return (STATCODE_SERVERR);
	else
		return (static_cast<enum StatusCode>(-1));
}

void HTTPResponse::setStatusCode(const StatusCode& _status_code)
{
	switch (_status_code)
	{
	case STATCODE_OK:
		this->status_code = "200";
		break;

	case STATCODE_REDIR:
		this->status_code = "301";
		break;

	case STATCODE_BADREQ:
		this->status_code = "400";
		break;

	case STATCODE_NOTFOUND:
		this->status_code = "404";
		break;

	case STATCODE_SERVERR:
		this->status_code = "500";
		break;

	default:
		break;
	}
}

void HTTPResponse::clear()
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
