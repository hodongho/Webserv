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
	std::string new_num(""), old_num("");

	while (num != 0)
	{
		old_num = new_num;
		new_num = num % 10 + '0';
		new_num += old_num;
		num /= 10;
	}

	return (new_num);
}
