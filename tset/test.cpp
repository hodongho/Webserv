#include <iostream>
#include <string>

int main()
{
	std::string str1 = "1234567890";
	str1 += "asd\0qwe\0zxc";

	for(int i = 0; i < str1.size(); i++)
	{
		std::cout << str1[i] << " ";
	}
	std::cout << std::endl;

    return 0;
}
