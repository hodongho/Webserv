#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <errno.h>

void		throwError(std::string msg);
std::string	itos(size_t num);


class funcException : public std::exception
{
	public:
		const char* what() const throw();
};

#endif
