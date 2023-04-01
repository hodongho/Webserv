// ft_split.cpp
#include <string>
#include <vector>

static bool	in_str(const std::string& str, const char& ch)
{
	// for(std::string::iterator iter = str.begin(); iter != str.end(); iter++)
	for(size_t idx = 0; idx < str.size(); idx++)
	{
		if (ch == str[idx])
			return (true);
	}
	return (false);
}

// char delimiter -> std::string delimiter
std::vector<std::string>    ft_split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string>	word_list;
	size_t						idx;

	idx = 0;
	while (idx < str.size())
	{
		if (in_str(delimiter, str[idx]))
			idx++;
		else
		{
			size_t  begin_of_word;
			size_t  len;

			begin_of_word = idx;
			while (str[idx] && (in_str(delimiter, str[idx]) == false))
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
// 	std::string					delimiter;

// 	origin_str = "hello \tworld\t\n42";
// 	delimiter = " \t";
// 	word_list = ft_split(origin_str, delimiter);
// 	for (auto iter = word_list.begin(); iter != word_list.end(); iter++)
// 		std::cout << "*iter : $" << *iter << "$" << std::endl;
// 	return (0);
// }