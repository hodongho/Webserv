#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <errno.h>

void	throwError(std::string msg);

class funcException : public std::exception
{
	public:
		const char* what() const throw();
};

std::string	getExtension(const std::string& file_path);

#endif
