#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse()
{};

HTTPResponse::~HTTPResponse()
{};

void	HTTPResponse::makeStatusLine()
{
	this->response =	this->version + " " +
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
		this->response +=	it->first + ": " +
							it->second + "\r\n";
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

	return (response);
}
