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
