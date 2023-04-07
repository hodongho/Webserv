#include "utils.hpp"

void	throwError(std::string msg)
{
	std::cerr << msg << std::flush;
	throw(funcException());
}

const char* funcException::what() const throw()
{
	return (strerror(errno));
}

std::string	itos(size_t num)
{
	std::string new_string_num(""), old_string_num("");

	for (; num != 0; num /= 10)
	{
		new_string_num = num % 10 + '0';
		new_string_num += old_string_num;
		old_string_num = new_string_num;
	}

	return (new_string_num);
}

static bool	in_str(const std::string& str, const char& ch)
{
	for(size_t idx = 0; idx < str.size(); idx++)
	{
		if (ch == str[idx])
			return (true);
	}
	return (false);
}

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

std::string ft_strtrim(const std::string& str, const std::string& set)
{
	std::string clean_str;
	size_t      start_of_str;
	size_t      end_of_str;

	if (str == "" || set == "")
		return (str);
	start_of_str = str.find_first_not_of(set);
	end_of_str = str.find_last_not_of(set);
	if (start_of_str == std::string::npos || end_of_str == std::string::npos)
		return ("");
	clean_str = str.substr(start_of_str, end_of_str - start_of_str + 1);
	return (clean_str);
}
