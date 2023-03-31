#include "HTTPRequest.hpp"

void	HTTPRequest::parseRequestMessage(std::string msg)
{
	std::stringstream	string_stream(msg);

	std::cout << GRN << msg << WHI;

	string_stream >> this->method >> this->URI >> this->version;

	if (this->method != "GET" || this->version != "HTTP/1.1")
	{
		std::cerr << RED << "400 Bad Request!" << WHI << std::endl;
	}

	std::cout	<< BLU
				<< "method: '" << this->method
				<< "' URI: '" << this->URI
				<< "' version: '" << this->version << WHI << std::endl;
}
