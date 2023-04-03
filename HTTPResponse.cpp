#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
:response(""), status_code(""), status_message("")
{
	version = "";
	body = "";
};

HTTPResponse::~HTTPResponse()
{};

void	HTTPResponse::makeStatusLine()
{
	this->response.append(this->version.c_str(), this->version.size());
	this->response.append(" ", 1);
	this->response.append(this->status_code.c_str(), this->status_code.size());
	this->response.append(" ", 1);
	this->response.append(this->status_message.c_str(), this->status_message.size());
	this->response.append("\r\n\r\n", 4);
}

void	HTTPResponse::makeHeaderField()
{
	std::map<std::string, std::string>::iterator it, end;

	it = header.begin();
	end = header.end();

	for(; it != end; it++)
	{
		this->response.append(it->first.c_str(), it->first.size());
		this->response.append(": ", 2);
		this->response.append(it->second.c_str(), it->second.size());
		this->response.append("\r\n", 2);
	}
	this->response.append("\r\n\r\n", 4);
}

void	HTTPResponse::makeBody()
{
	this->response.append(this->body.c_str(), this->body.size());
	this->response.append("\r\n\r\n", 4);
}

std::string	HTTPResponse::makeResponseMessage()
{
	this->makeStatusLine();
	this->makeHeaderField();
	this->makeBody();

	return (response);
}
