#include <string>

/*
    -set에 있는 문자가 양끝에 있는 동안은 다듬는다.
    왼쪽, 오른쪽 나뉘어서 해본다.
    동시에 진행가능? 안하는게 나을듯
    왼쪽 시작점과, 오른쪽 끝점 위치를 찾아서 substr을 사용한다.
*/
std::string ft_strtrim(const std::string& str, const std::string& set)
{
    std::string clean_str;
    size_t      start_of_str;
    size_t      end_of_str;

    if (str == "" || set == "")
		return (str);
    start_of_str = str.find_first_not_of(set);
    end_of_str = str.find_last_not_of(set);
    clean_str = str.substr(start_of_str, end_of_str - start_of_str + 1);
    return (clean_str);
}

// #include <iostream>
// int	main()
// {
// 	std::string origin_str;
//     std::string clean_str;
//     std::string set;

// 	origin_str = " \t  hello world   \t    \v";
// 	set = " \t\v";
// 	clean_str = ft_strtrim(origin_str, set);
//     std::cout << "clean_str : $" << clean_str << "$" << std::endl;
// 	return (0);
// }