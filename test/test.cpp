#include <iostream>
#include <string>

int main()
{
	std::string string_str = "";
	std::string char_str = "";
	std::string str("asd\0qwe\0zxc", 11);

	string_str.append(str , str.size());
	char_str.append(str.c_str(), str.size());

	std::cout << "std::string test" << std::endl;
	for(int i = 0; i < string_str.size(); i++)
	{
		std::cout << string_str[i] << " ";
	}

	std::cout << "c_str test" << std::endl;
	for(int i = 0; i < char_str.size(); i++)
	{
		std::cout << char_str[i] << " ";
	}
	std::cout << std::endl;

    return 0;
}
