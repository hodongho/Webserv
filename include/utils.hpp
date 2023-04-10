#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <errno.h>


void		throwError(std::string msg);

class funcException : public std::exception
{
	public:
		const char* what() const throw();
};

std::string					getExtension(const std::string& file_path);
std::vector<std::string>    ft_split(const std::string& str, const std::string& delimiter);
std::string 				ft_strtrim(const std::string& str, const std::string& set);
std::string					itos(size_t num);
#endif
