#include "../../include/ConfigInfo.hpp"
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

static std::vector<std::string>	ft_split(const std::string& str, const std::string& delimiter);
static std::string 				ft_strtrim(const std::string& str, const std::string& set);

void	ConfigInfo::printContent(const std::string& str, const std::string& str_name, const std::string& color)
{
	std::cout << color << str_name << " : $" << str << "$" << WHI << std::endl;
}

ConfigInfo::ConfigInfo(void)
	: whitespace(" \t\n\v\f\r")
{}


void ConfigInfo::parseConfig(const char *file_name)
{
	std::string		file_content;

	// check file name rule
	if (this->checkFileNameExtension(file_name))
		std::cout << "file name is correct to the config file name rule" << std::endl;
	else
	{
		std::cout << "file name is not correct to the config file name rule!!!" << std::endl;
		exit(1);
	}
	// open and read file
	file_content = this->readFile(file_name);

	// validation
	if (this->validateConfigFile(file_content))
		std::cout << "Config file content is validate" << std::endl;
	else
		std::cout << "Config file content is N.O.T validate" << std::endl;
	
	// ConfigInfo ConfigInfo;
	// parse
	// ParseConfig(); // 결국 이 안으로 들어가야함
	// ConfigInfo.parseConfig(argv[1]);
	//std::cout << ConfigInfo.config << std::endl;
	// ConfigInfo.parseServer();
}

void ConfigInfo::parseServer() {
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

// ServerInfo ConfigInfo::parseServerBlock(size_t it) {
// 	ServerInfo res;
// 	std::string key;
// 	std::string value;

// 	while (it != std::string::npos) {
// 		// aa
// 	}

// 	return (res);
// }

/*
	throw하는 식으로 변경 필요
	throw 이후 exit()
*/
bool	ConfigInfo::checkFileNameExtension(const char *file_name_parms)
{
	std::string	file_name(file_name_parms);
	size_t		pos;

	pos = file_name.rfind(".conf");
	if (pos != std::string::npos && \
		file_name.compare(pos, file_name.size() - pos, ".conf") == 0)
		return (true);
	return (false);
}

std::string ConfigInfo::readFile(std::string file_name)
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
static std::vector<std::string>    ft_split(const std::string& str, const std::string& delimiter)
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

static std::string ft_strtrim(const std::string& str, const std::string& set)
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

/*
	여러번 파일을 읽을것이라서 const불가, 
	reference하면 파일 pos가 맨 밑으로 갈것으로 예상되어서 deep copy되게 놔둠
	- 짝은 맞지만 불필요한 곳에 {}가 있는 경우에 대한 처리 필요.
	- split된 인자의시작이 
*/
bool	ConfigInfo::checkCurlyBrackeyPair(const std::string& file_content)
{
	std::stringstream		file_content_stream(file_content);
	std::string				str;
	std::stack<std::string>	curly_stack;

	while (std::getline(file_content_stream, str))
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;

		clean_str = ft_strtrim(str, this->whitespace);
		// printContent(clean_str, "clean_str", BLU);
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
		for (std::vector<std::string>::iterator iter = word_list.begin(); iter != word_list.end(); iter++)
		{
			if ((*iter)[0] == '#')
				break ;
			// printContent(*iter, "\titer");
			if ((*iter == "{" || *iter == "}"))
			{
				// printContent(*iter, "\t###iter ", GRN);
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
								const std::vector<std::string>::iterator& end_iter,
								bool& server_flag,
								bool& location_flag,
								bool& error_flag)
{
	// printContent(*(begin_iter + 1) , "*(begin_iter + 1) ");
	if (*begin_iter == "server")
	{
		if ((((begin_iter + 1) != end_iter &&*(begin_iter + 1) == "{") && ((begin_iter + 2) == end_iter || (*(begin_iter + 2))[0] == '#')) || \
			(*begin_iter == "{" && (begin_iter + 1 == end_iter || (*(begin_iter + 1))[0] == '#')))
			server_flag = true;
		else if ((begin_iter + 1) == end_iter)
			;
		else
		{
			error_flag = false;
			return (false);
		}
		return (true);
	}
	else if (*begin_iter == "location")
	{
		if (((end_iter - begin_iter) > 2 && ((begin_iter + 2) != end_iter &&*(begin_iter + 2) == "{") && ((begin_iter + 3) == end_iter || (*(begin_iter + 3))[0] == '#')) || \
		(*begin_iter == "{" && (begin_iter + 1 == end_iter || (*(begin_iter + 1))[0] == '#')))
			location_flag = true;
		else
		{
			error_flag = false;
			return (false);
		}
		return (true);
	}
	return (false);
}

bool	findCloseCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
								const std::vector<std::string>::iterator& end_iter,
								bool& error_flag)
{
	if (*begin_iter == "}")
	{
		if (((begin_iter + 1) == end_iter || (*(begin_iter + 1))[0] == '#'))
			return (true);
		error_flag = false;
	}
	return (false);
}

bool	ConfigInfo::findServerBlock(std::vector<std::string>::iterator& iter, \
													const std::vector<std::string>::iterator& src_end_iter, \
													std::vector<std::string>::iterator& begin_iter, \
													std::vector<std::string>::iterator& end_iter)
{
	std::stack<std::string> 			server_stack;
	std::stack<std::string> 			location_stack;
	bool								server_flag = false; // for stack.push() 해도되는지 확인용
	bool								location_flag = false;
	bool								error_flag = true;

	// findServerBlock
	for (;iter != src_end_iter; iter++)
	{
		std::vector<std::string>	word_list;
		std::string					clean_str;

		clean_str = ft_strtrim(*iter, this->whitespace);
		// printContent(clean_str, "clean_str", CYN);
		// 만약 block 짝이 안맞을때 에러케이스 거르기 위함
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
		// for (std::vector<std::string>::iterator tmp_iter = word_list.begin(); tmp_iter != word_list.end(); tmp_iter++)
		// 	printContent(*tmp_iter, "#*tmp_iter in findServerBlock", CYN);
	 	if (findOpenCurlyBracket(word_list.begin(), word_list.end(), server_flag, location_flag, error_flag))
		{
			// std::cout << WHI << std::boolalpha << "server_flag : " << server_flag << WHI << std::endl;
			// std::cout << WHI << std::boolalpha << "location_flag : " << location_flag << WHI << std::endl;
			if (server_flag == true)
			{
				server_flag = false;
				server_stack.push("{");
				begin_iter = iter;
			}
			else if (location_flag == true)
			{
				location_flag = false;
				location_stack.push("{");
			}
			// std::cout << BLU << std::boolalpha << "server_flag : " << server_flag << WHI << std::endl;
			// std::cout << BLU << std::boolalpha << "location_flag : " << location_flag << WHI << std::endl;
		}
		else if (findCloseCurlyBracket(word_list.begin(), word_list.end(), error_flag))
		{
			if (location_stack.empty() == false)
			{
				location_stack.pop();
				location_flag = false;
			}
			// else if (location_stack.empty() && server_stack.empty() == false)
			else if (server_stack.empty() == false)
			{
				server_stack.pop();
				// begin_iter++; // block내부만 가리키게 만듦
				end_iter = iter;
				// for (std::vector<std::string>::iterator iter = begin_iter; iter != end_iter; iter++)
				// 	printContent(*iter, "#*iter about findServerBlock :", CYN);
				// printContent(*begin_iter, "begin_iter");
				// printContent(*end_iter, "end_iter");
				return (true);
				// validateServerBlock(begin_iter, end_iter); // block위치를 알게됨
			}
		}

		if (error_flag == false)
			return (false);
	}
	// if (server_stack.empty() && location_stack.empty())
	// 	return (true);
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
bool	ConfigInfo::checkWhole(const std::string& file_content)
{
	std::string							str;
	std::vector<std::string>			file_content_vector;
	std::vector<std::string>::iterator 	cur_iter;
	std::vector<std::string>::iterator 	begin_iter;
	std::vector<std::string>::iterator 	end_iter;
	// std::stack<std::string> 			server_stack;
	// std::stack<std::string> 			location_stack;
	// bool								server_flag = false; // for stack.push() 해도되는지 확인용
	// bool								location_flag = false;
	
	// prepareValidateKeyList()
	file_content_vector = ft_split(file_content, "\n");
	cur_iter = file_content_vector.begin();
	while (cur_iter != file_content_vector.end())
	{
		std::string	clean_str;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		// printContent(clean_str, "#clean_str in checkWhole()", RED);
		if (clean_str.size() == 0 || clean_str[0] == '#')
		{
			cur_iter++;
			continue ;
		}
		{// 영역 벗어나면 vector 소멸자 호출되게 만들기 위함
			std::vector<std::string>	word_list = ft_split(*cur_iter, this->whitespace);
			if (word_list[0] != "server")
			{
				printContent(*cur_iter, "*cur_iter", GRN);
				return (false);
			}
		}
		// cur_iter = findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter);
		if (findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter) == false)
			return (false);
		for (std::vector<std::string>::iterator iter = begin_iter; iter != end_iter; iter++)
			printContent(*iter, "#*iter about findServerBlock :", BRW);
		// begin_iter, end_iter
		printContent(*begin_iter, "*begin_iter :", RED);
		printContent(*end_iter, "*end_iter :", BLU);
		cur_iter++;
		// printContent(*cur_iter, "##after increament *cur_iter", GRN);
		// std::cout << std::endl;
	}
	return (true);
}

/*
	curly bracket pair
	block이 제대로 되었는지 확인을 추가시킬지 포함 
		- server, locatoin 이후에 {}이 왔는지
		- {}이 아예 안오는 경우
			다른 key에 {가 붙는 경우
		- {}만 들어오는 케이스에 대한 처리
			- "{" "}" 다음에는 아무것도 없거나 주석만 가능
		뜬금없는 위치이지만 {}만 있는 케이스는 놔둔다.
		이 부분까지 checkBracket에서 처리할려면 "{trtlkfldaf}", "234234{trtlkfldaf}23424" 같은 케이스도 확인해야함
		- 한 줄씩 확인할때 유효키인지 정도만 확인하며 value값이 경우는 문자열을 받는 value는 확인하지 않.는다.
			e.g) index "{}"
				- root "{sdf}"
				- location_path "{}"
				이런 경우들은 확인하지 않음.
*/
bool	ConfigInfo::validateConfigFile(const std::string& file_content)
{
	if (checkCurlyBrackeyPair(file_content) == false)
		return (false);
	if (checkWhole(file_content) == false)
		return (false);
	return (true);
}

int main(int argc, char *argv[])
{
	// std::string line;
	ConfigInfo	configPaser;

	if (argc != 2) {
		std::cout << "Usage : string" << std::endl;
		return (1);
	}

	try
	{
		configPaser.parseConfig(argv[1]);
		/* code */
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}