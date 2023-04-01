// ft_split.cpp
#include <string>
#include <vector>

std::vector<std::string>    ft_split(const std::string& str, char delimiter)
{
	std::vector<std::string>	word_list;
	size_t						idx;

	idx = 0;
	while (idx < str.size())
	{
		if (str[idx] == delimiter)
			idx++;
		else
		{
			size_t  begin_of_word;
			size_t  len;

			begin_of_word = idx;
			while (str[idx] && str[idx] != delimiter)
				idx++;
			len = idx - begin_of_word;
			word_list.push_back(str.substr(begin_of_word, len));
		}
	}
	return (word_list);
}

// #include <iostream>

// int	main()
// {
// 	std::string					origin_str;
// 	std::vector<std::string>	word_list;
// 	char						delimiter;

// 	origin_str = "hello world";
// 	delimiter = ' ';
// 	word_list = ft_split(origin_str, delimiter);
// 	for (auto iter = word_list.begin(); iter != word_list.end(); iter++)
// 	{
// 		std::cout << "*iter : " << *iter << "$" << std::endl;
// 	}
// 	return (0);
// }