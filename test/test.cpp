#include <iostream>
#include <string>

int main()
{
	std::string string_str = "";
	std::string char_str = "";
	std::string str("asd\0qwe\0zxc", 11);
	std::string test = "";

	test += str + " " + str;
	std::cout << "test: " << test << std::endl;

	std::cout << str << " " << std::endl;
	std::cout << str.size() << std::endl;

	std::cout << "string append ret: " << string_str.append(str, 5, str.size()) << std::endl;
	std::cout << "char append ret: " << char_str.append(str.c_str(), str.size()) << std::endl;

	std::cout << "std::string test: " << string_str.size() << std::endl;
	for(int i = 0; i < string_str.size(); i++)
	{
		std::cout << string_str[i] << " ";
	}
	std::cout << std::endl;

	std::cout << "c_str test: " << char_str.size() << std::endl;
	for(int i = 0; i < char_str.size(); i++)
	{
		std::cout << char_str[i] << " ";
	}
	std::cout << std::endl;

    return 0;
}
