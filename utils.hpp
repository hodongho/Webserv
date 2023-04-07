#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <errno.h>

void		throwError(std::string msg);
/**
 * @brief
 * Int to String으로 std::string 특성을 이용한 함수입니다.
 * 새로운 나머지에 이전까지 더해뒀던 나머지를 더하면서 뒤에서부터 문자열을 완성하는 함수입니다.
 */
std::string	itos(size_t num);


class funcException : public std::exception
{
	public:
		const char* what() const throw();
};

#endif
