#include "utils.hpp"

void throwError(std::string msg)
{
	std::cerr << msg << std::flush;
	throw(funcException());
}

const char* funcException::what() const throw()
{
	return (strerror(errno));
}

std::string	getExtension(const std::string& file_path)
{
	return (file_path.substr(file_path.rfind(".")));
}
