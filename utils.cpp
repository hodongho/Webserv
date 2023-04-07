#include "utils.hpp"

void	throwError(std::string msg)
{
	std::cerr << msg << std::flush;
	throw(funcException());
}

const char* funcException::what() const throw()
{
	return (strerror(errno));
}

std::string	itos(size_t num)
{
	std::string new_string_num(""), old_string_num("");

	for (; num != 0; num /= 10)
	{
		new_string_num = num % 10 + '0';
		new_string_num += old_string_num;
		old_string_num = new_string_num;
	}

	return (new_string_num);
}
