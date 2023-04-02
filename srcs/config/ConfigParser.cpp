#include "../../include/ConfigParser.hpp"
#include "../../include/configStructure.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

// #include "config.hpp"

/*
	- vaildate validation
		- file name rule *.conf
	- vaildate file content
		- culry bracket pair
		- each line
			- key *{value} 
			- key가 유효한지 확인
			- 유효하더라도 중복이되는지 확인
			- 없어도 되는지 있게된다면 어떻게 있어야하는지 확인
			- 각 항목마다 rule이 다름 
				- server와 "{" 가 존재가능
				// - server밑에 {}블럭으로 되어있을수도 있다.
				- host는 옆에 IP가 있어야함
					- IPv4형식에 맞아야함 
				- port
					- 0 ~ 65525 범위
						- 예약된 port 신경쓰지 않음
				- index
					- 단 하나의 문자열 값
				- root
					- 단 하나의 문자열 값
				- client_max_body_size
					- unsigned int 범위
				- location block!!
					- 아래와 같이 빈줄로 떨어져 있어도 허용됨, 구현하기 나름
						location

						{}
					- location path
						- 단 하나의 문자열 값
						- 절대 경로만 허용
							- 별도의 ~, = 같은 옵션 쓰지 않음!!
					- allow_method
						- 허용되는 메소드
							- GET,POST,DELTE중에 있는지 확인필요
							- 중복 허용안됨
					- autoindex
						- 단 하나의 문자열 값
						- on, off인지 확인
					- root
						단 하나의 문자열
					- index
						단 하나의 문자열
					- redirection
						return으로 시작되는 형식
						split으로 return될 vector<std::string>에는 3개 항목만이 있다.
						location ~ / {
							return 301 https://primarydomain/en$request_uri;
						}
}
*/

void	printContent(const std::string& str, const std::string& str_name)
{
	std::cout << str_name << " : $" << str << "$" << std::endl;
}

void ConfigParser::parseConfig(const char *file)
{
    //prev
	// std::ifstream ifs(file);
	// if (ifs.fail())
	// {
	// 	exit(1);
	// }
	// std::string str;
	// int count = 0;
	// webserv_config.clear();

	// while (!ifs.eof())
	// {
	// 	if (ifs.bad()) exit(1);
	// 	std::getline(ifs, str);
	// 	str = trim(str);
	// 	if (str[0] == '#' || str.length() <= 0)
	// 		continue;
	// 	 size_t pos = str.find_first_of(";{}");
	// 	 if (pos == std::string::npos)
	// 	 	exit(1);
	// 	 str = str.substr(0, pos + 1);
	// 	webserv_config.append(str + '\n');
	// }
	// for (int i = 0; i < webserv_config.size(); i++) {
	// 	if (config[i] == '{')
	// 		count++;
	// 	if (config[i] == '}')
	// 		count--;
	// }
	// if (count) exit(1);
}

void ConfigParser::parseServer() {
	size_t start = 0;
	size_t end = 0;
	std::string tmp;

    // prev
	// while (end != std::string::npos) {
	// 	start = config.find_first_not_of(whitespace, end);
	// 	end = config.find_first_of(" \t\n\r\f\v{", start);
	// 	tmp = config.substr(start, end - start);
	// 	std::cout << tmp << std::endl;
	// 	if (tmp != "server") exit(1);
	// 	server_block.push_back(parseServerBlock(end));
	// }
	
}

// ServerInfo ConfigParser::parseServerBlock(size_t it) {
// 	ServerInfo res;
// 	std::string key;
// 	std::string value;

// 	while (it != std::string::npos) {
// 		// aa
// 	}

// 	return (res);
// }


static bool	checkFileName(const char *file_name_parms)
{
	std::string	file_name(file_name_parms);
	size_t		pos;

	pos = file_name.rfind(".conf");
	if (pos != std::string::npos && \
		file_name.compare(pos, file_name.size() - pos, ".conf") == 0)
		return (true);
	return (false);
}

std::string readFile(std::string file_name)
{
    std::string     s;
    std::ifstream   ifs(file_name);

    if (ifs.is_open())
    {
        ifs.seekg(0, std::ios::end);
        int size = ifs.tellg();
        s.resize(size);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&s[0], size);
    }
    else
    {
        // error_handling("Fail to open file", 1);
        exit(1);
    }
    return (s);
}

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

std::string ft_strtrim(const std::string& str, const std::string& set)
{
    std::string clean_str;
    size_t      start_of_str;
    size_t      end_of_str;

	// std::cout << "### in ft_strtrim ###" << std::endl;
	// printContent(str, "str");
	// printContent(set, "set");
	if (str == "" || set == "")
		return (str);
    start_of_str = str.find_first_not_of(set);
    end_of_str = str.find_last_not_of(set);
    clean_str = str.substr(start_of_str, end_of_str - start_of_str + 1);
    return (clean_str);
}

/*
	여러번 파일을 읽을것이라서 const불가, 
	reference하면 파일 pos가 맨 밑으로 갈것으로 예상되어서 deep copy되게 놔둠
	- {{
	- {{}} case 해결안됨
		- 별도의 방법 필요
	- split된 인자의 시작이 
*/
bool	checkCurlyBrackeyPair(const std::string& file_content)
{
	std::stringstream		file_content_stream(file_content);
	std::string				str;
	std::stack<std::string>	curly_stack;

	while (std::getline(file_content_stream, str))
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;
		const std::string&			whitespace = " \n\t\v\r";

		clean_str = ft_strtrim(str, whitespace);
		// printContent(clean_str, "clean_str");
		word_list = ft_split(clean_str, whitespace);
		for (std::vector<std::string>::iterator iter = word_list.begin(); iter != word_list.end(); iter++)
		{
			printContent(*iter, "\titer");
			if ((*iter == "{" || *iter == "}"))
			{
				printContent(*iter, "\t###iter ");
				if (*iter == "{")
					curly_stack.push(*iter);
				else if (!curly_stack.empty())
					curly_stack.pop();
				else
					return (false);
			}
		}
	}
	return (curly_stack.empty());
}

// std::map<std::string, >	prepareValidateServerKeyList(void)
// {

// }


// findBlock()

// prepareValidateLocationKeyList();


/*
case
- server {
- server { #4242 sdf 
- server
  { # comment dsf erw tdfg
- serverr test {
*/
bool	findOpenCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
								const std::vector<std::string>::iterator& end_iter)
{
	// printContent(*(begin_iter + 1) , "*(begin_iter + 1) ");
	if ((*begin_iter == "server" && ((begin_iter + 1) != end_iter &&*(begin_iter + 1) == "{") && ((begin_iter + 2) == end_iter || (*(begin_iter + 2))[0] == '#')) || \
		(*begin_iter == "{" && (begin_iter + 1 == end_iter || (*(begin_iter + 1))[0] == '#')))
		return (true);
	return (false);
}

/*
	- #주석이후로는 제외
	- key value 짝이 맞아야함
		- value가 list로 나올 수도 있다.
		- 들어온 key가 유효한지, 중복이 된것은 없는지 확인해야한다.
			- 소문자만 허용!!
			- host, port index, server_name, root, client_max_body_size
		- value역시 해당 key에 허용하는 형식인지 확인해야한다
	- vector 요소의 가장 처음 것은 key이거나 {, 주석이다
	- 혹은 비어있거나
	- {, } 이후에 무언가 있다면 바로 다음 것이 주석 이어야한다.
	- 주석이나 {,} 기호가 없다면 ";"으로 끝나야한다
	- server, location block 에서 확인하는 validation이 다르다.

	block을 찾는다.
	server,location이 나오면 그 다음에 나오는 word_list에서 값은 '{' 이어야한다.
	그렇지 않다면 error
	begin_iter는 server, location 이후에 나오는 '{'부터이다.
	end_iter는 '}'

	
*/
bool	checkWhole(const std::string& file_content)
{
	const std::string&					whitespace = " \n\t\v\r";
	std::string							str;
	std::vector<std::string>			file_content_vector;
	std::vector<std::string>::iterator 	iter;
	std::vector<std::string>::iterator 	begin_iter;
	std::stack<std::string> 			server_stack;
	std::stack<std::string> 			location_stack;
	bool								server_flag = false; // for stack.push() 해도되는지 확인용
	bool								location_flag = false;
	
	// prepareValidateKeyList()
	file_content_vector = ft_split(file_content, "\n");
	iter = file_content_vector.begin();
	// while (iter != file_content_vector.end())
	for (;iter != file_content_vector.end(); iter++)
	{
		std::vector<std::string>	word_list;
		std::string					clean_str;

		clean_str = ft_strtrim(*iter, whitespace);
		// 만약 block 짝이 안맞을때 에러케이스 거르기 위함
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, whitespace);
		if (*(word_list.begin()) == "server")
			server_flag = true;
		if (*(word_list.begin()) == "location")
			location_flag = true;
		// '}' 뒤에는 없거나 있어도 주석이어야함
		if (*(word_list.begin()) == "}" && \
			((word_list.begin() + 1) == word_list.end() || *(word_list.begin() + 1) == "#"))
		{
			if (location_stack.empty() && server_stack.empty() == false)
			{
				std::vector<std::string>::iterator 	end_iter;

				server_stack.pop();
				end_iter = iter;
				printContent(*begin_iter, "begin_iter");
				printContent(*end_iter, "end_iter");
				// validateServerBlock(begin_iter, end_iter); // block위치를 알게됨
			}
			if (location_stack.empty() == false)
				location_stack.pop();
		}
		for (std::vector<std::string>::iterator word = word_list.begin(); \
				word != word_list.end(); \
				word++)
		{
			if (server_flag == true)
			{
				if (findOpenCurlyBracket(word_list.begin(), word_list.end()))
				{
					server_flag = false;
					server_stack.push("{");
					begin_iter = iter;
				}
			}
			if (location_flag == true)
			{
				if (findOpenCurlyBracket(word_list.begin(), word_list.end()))
				{
					location_flag = false;
					location_stack.push("{");
				}
			}
			// printContent(*word, "word");
		}
		// printContent(*iter, "iter");
		// printContent(*begin_iter, "begin_iter");
		// iter++;
	}
	if (server_stack.empty() && location_stack.empty())
		return (true);
	return (false);
}

bool	validateConfigFile(const std::string& file_content)
{
	if (checkCurlyBrackeyPair(file_content) == false)
		return (false);
	// if (checkWhole(file_content) == false)
	// 	return (false);
	return (true);
}

int main(int argc, char *argv[])
{
	// std::string line;

	if (argc != 2) {
		std::cout << "Usage : string" << std::endl;
		return (1);
	}

	// check file name rule
	if (checkFileName(argv[1]))
		std::cout << "file name is correct to the config file name rule" << std::endl;
	else
	{
		std::cout << "file name is not correct to the config file name rule!!!" << std::endl;
		exit(1);
	}
	// open and read file
	std::string		file_content;
	file_content = readFile(argv[1]);

	// validation
	if (validateConfigFile(file_content))
		std::cout << "Config file content is validate" << std::endl;
	else
		std::cout << "Config file content is N.O.T validate" << std::endl;

	// ConfigParser configParser;
	// parse
	// ParseConfig(); // 결국 이 안으로 들어가야함
	// configParser.parseConfig(argv[1]);
	//std::cout << configParser.config << std::endl;
	// configParser.parseServer();
	return (0);
}