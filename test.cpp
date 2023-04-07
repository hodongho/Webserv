#include <iostream>
#include <string>
#include "utils.hpp"

int main()
{
	std::string	test("12345678awwefawefwef01234");

	std::string	size = itos(test.size());

	std::cout << test.size() << std::endl;
	std::cout << size << std::endl;
	return 0;
}
