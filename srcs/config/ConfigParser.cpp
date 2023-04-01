#include "ConfigParser.hpp"
#include "configStructure.hpp"
#include <fstream>
#include <string>
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
				// - server밑에 {}블럭으로 되어있을수도 있따.
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

int main(int argc, char *argv[])
{
	// std::string line;

	// if (argc > 2) {
	//	std::cout << "Usage : string" << std::endl;
	//	return (1);
	//}
	// line = std::string(argv[1]);
	// ft_split(line, white_space);
	// std::string str = "   \n\thi!  \r\n ";
	// std::cout << trim(str) << std::endl;

	ConfigParser configParser;
	configParser.parseConfig(argv[1]);
	//std::cout << configParser.config << std::endl;

	configParser.parseServer();


	return (0);
}