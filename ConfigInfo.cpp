#include "ConfigInfo.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <limits>
#include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
#include <sys/stat.h>
// #include <unistd.h>

ConfigInfo::ConfigInfo(void)
    : whitespace(" \t\n\v\f\r")
{}

ConfigInfo::~ConfigInfo()
{}

/*
    config 중복여부 확인필요
    host:port
*/
void ConfigInfo::parseConfig(const char *file_name)
{
	std::string		file_content;

	// check file name rule .conf
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
	{
		std::cout << "Config file content is N.O.T validate" << std::endl;
		exit(1);
	}
	
	if (this->parse(file_content))
		std::cout << "Config parse is OK" << std::endl;
	else
	{
		std::cout << "Config parsing something is wrong" << std::endl;
		exit(1);
	}
}

const std::vector<ServerConfig>& ConfigInfo::getWebservConfig(void) const
{
    return (this->server_config_vector);
}

bool	ConfigInfo::getServerConfig(const unsigned short &port, ServerConfig& server_config) const
{
	std::vector<ServerConfig>::const_iterator	server_iter;

	server_iter = this->server_config_vector.begin();
	for (;server_iter != this->server_config_vector.end(); server_iter++)
	{
		if (server_iter->getPort() == port)
		{
			server_config = *server_iter;
			return (true);
		}
	}
	return (false);
}

// 찾고자하는 path가 config에 있다면 파라미터에 location_config으로 설정된 값을 가져옴
bool	ConfigInfo::getLocationConfig(const unsigned short &port, const std::string &find_path, LocationConfig& location_config) const
{
	ServerConfig	server_config;

	if (getServerConfig(port, server_config) && \
		server_config.getLocationBlock(find_path, location_config))
		return(true);
	return (false);
}

void ConfigInfo::printWebservConfig(void)
{
	std::vector<ServerConfig>::iterator	iter = this->server_config_vector.begin();
	
	for (;iter != this->server_config_vector.end(); iter++)
	{
		iter->printServerConfingContent();
		std::cout << std::endl;
	}
}

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
	std::ifstream   ifs(file_name.c_str());

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

/*
	여러번 파일을 읽을것이라서 const불가, 
	reference하면 파일 pos가 맨 밑으로 갈것으로 예상되어서 deep copy되게 놔둠
	- 짝은 맞지만 불필요한 곳에 {}가 있는 경우에 대한 처리 필요.
	- split된 인자의시작이 
*/
bool	ConfigInfo::checkCurlyBracketPair(const std::string& file_content)
{
	std::stringstream		file_content_stream(file_content);
	std::string				str;
	std::stack<std::string>	curly_stack;

	while (std::getline(file_content_stream, str))
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;

		clean_str = ft_strtrim(str, this->whitespace);
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
		for (std::vector<std::string>::iterator word_iter = word_list.begin(); word_iter != word_list.end(); word_iter++)
		{
			std::string	word;
			
			word = *word_iter;
			if ((word)[0] == '#')
				break ;
			if ((word == "{" || word == "}"))
			{
				if (word_iter + 1 != word_list.end() && (word_iter + 1)->at(0) != '#') // '{', '}' ,다음 단어가 있다면 "#"로 시작해야함
					return (false);
				if (word == "{")
					curly_stack.push(word);
				else if (!curly_stack.empty())
					curly_stack.pop();
				else
					return (false);
			}// '{' '}'로 시작하는데 길이가 1보다 크고, #이 ';' 바로 다음에 없으면 error처리
			else if ((word.at(0) == '{' || word.at(0) == '}') &&  
				(word.size() > 1 && word.at(1) != '#'))
				return (false);
		}
	}
	return (curly_stack.empty());
}

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

bool	ConfigInfo::findLocationBlock(std::vector<std::string>::iterator& iter, \
													const std::vector<std::string>::iterator& src_end_iter, \
													std::vector<std::string>::iterator& begin_iter, \
													std::vector<std::string>::iterator& end_iter)
{
	std::stack<std::string> 			location_stack;
	bool								server_flag = false; // 여기서 쓰지는 않지만 같이 써야하므로 통일성을 위해 놔둠
	bool								location_flag = false;
	bool								error_flag = true;

	for (;iter != src_end_iter; iter++)
	{
		std::vector<std::string>	word_list;
		std::string					clean_str;

		clean_str = ft_strtrim(*iter, this->whitespace);
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
	 	if (findOpenCurlyBracket(word_list.begin(), word_list.end(), server_flag, location_flag, error_flag))
		{
			if (location_flag == true)
			{
				location_flag = false;
				location_stack.push("{");
				begin_iter = iter;
			}
		}
		else if (findCloseCurlyBracket(word_list.begin(), word_list.end(), error_flag))
		{
			if (location_stack.empty() == false)
			{
				location_stack.pop();
				end_iter = iter;
				return (true);
			}
		}
		if (server_flag || error_flag == false)
			return (false);
	}
	return (false);
}

/*
	ValidateFieldInfo 내부에count가 1인지 확인
	count를 증가시키기 전에 확인할것이므로 중복 필드가 아니면 
	count가 0이어야 정상이다.
*/
bool	ConfigInfo::checkDuplicateConfigField(const ValidateFieldInfo& validate_field_info)
{
	if (validate_field_info.getValidateFieldType() == NESSARY_UNIQUE || \
		validate_field_info.getValidateFieldType() == OPTION_UNIQUE)
	{
		if (validate_field_info.getCount() > 0)
			return (false);
	}
	return (true);
}

/*
	# 주석은 상관없다
	이미 이전에 확인한 상태이므로 무조건 ;이 #x
*/
std::string	removeAfterSemicolon(const std::string& origin_value)
{
	std::string	clean_value(origin_value);
	size_t		end_of_str;

	end_of_str = origin_value.find(';');
	if (end_of_str != std::string::npos)
		clean_value = origin_value.substr(0, end_of_str);
	return (clean_value);
}

/*
IPv4
[123].[123].[123].[123];
- ;이 있다면 제거
	; or # 이전까지만 value에 담아둠
- 각 '.'과 '.'사이는 최대 3자리 숫자
- '.'의 개수는 3개
*/
bool ConfigInfo::checkHostConfigField(std::string field_value)
{
	std::string	ip_class;
	size_t		dot_count;

	field_value = removeAfterSemicolon(field_value);
	dot_count = 0;
	for (size_t	idx = 0; idx < field_value.length(); idx++)
	{
		if (field_value[idx] == '.')
		{
			if (checkIpClass(ip_class) == false)
				return (false);
			ip_class = "";
			dot_count++;
			continue ;
		}	
		ip_class += field_value[idx];
	}
	if (checkIpClass(ip_class) == false)
		return (false);
	if (dot_count != 3)
		return (false);
    return (true);
}

bool ConfigInfo::checkIpClass(const std::string& ip_class)
{
	std::stringstream	ip_class_strstream;
	int					ip_class_numeric;

	ip_class_strstream << std::atoi(ip_class.c_str());
	ip_class_strstream >> ip_class_numeric;
	if (ip_class_strstream.str() != ip_class)
		return (false);
	else if (ip_class_numeric > 255 || ip_class_numeric < 0)
		return (false);
    return (true);
}

/*
port 
0 ~ 65526
*/
bool ConfigInfo::checkPortConfigField(std::string port)
{
	std::stringstream	port_strstream;
	int					port_numeric;

	port = removeAfterSemicolon(port);
	port_strstream << std::atoi(port.c_str());
	port_strstream >> port_numeric;
	if (port_strstream.str() != port)
		return (false);
	else if (port_numeric > std::numeric_limits<unsigned short>::max() || port_numeric < 0)
		return (false);
    return (true);
}

bool ConfigInfo::checkClientMaxBodySizeConfigField(std::string client_body_size)
{
	std::stringstream	client_body_size_strstream;
	int					client_body_size_numeric;

	client_body_size = removeAfterSemicolon(client_body_size);
	client_body_size_strstream << std::atoi(client_body_size.c_str());
	client_body_size_strstream >> client_body_size_numeric;
	if (client_body_size_strstream.str() != client_body_size)
		return (false);
	else if (client_body_size_numeric > std::numeric_limits<int>::max() || client_body_size_numeric < 0)
		return (false);
    return (true);
}

/*
Error page code
400~599
*/
bool ConfigInfo::checkErrorPageStatusCodde(const std::string& status_code)
{
	std::stringstream	status_code_strstream;
	int					status_code_numeric;

	status_code_strstream << std::atoi(status_code.c_str());
	status_code_strstream >> status_code_numeric;
	if (status_code_strstream.str() != status_code)
		return (false);
	else if (status_code_numeric > 599 || status_code_numeric < 400)
		return (false);
    return (true);
}

/*
	allow_method GET;
	allow_method GET ;
	allow_method GET POST;
	allow_method DELETE GET POST;
*/
bool ConfigInfo::checkAllowMethodConfigField(std::string allow_method)
{
	std::string							value;
	size_t 								semicolon_pos;
	size_t								comment_pos;
	std::vector<std::string>			allow_method_vector;
	std::vector<std::string>::iterator	allow_method_value_iter;

	semicolon_pos = allow_method.find(';');
	if (semicolon_pos == std::string::npos)
		return (false);
	allow_method = removeAfterSemicolon(allow_method);
	comment_pos = allow_method.find('#');
	if (comment_pos != std::string::npos)
	{
		if (semicolon_pos > comment_pos)
			return (false);
		else if (semicolon_pos + 1 != comment_pos)
			return (false);
	}

	allow_method = ft_strtrim(allow_method, this->whitespace);
	allow_method_vector = ft_split(allow_method, this->whitespace);
	allow_method_value_iter = allow_method_vector.begin() + 1;
	if (allow_method_value_iter == allow_method_vector.end()) // allow_method test ;
		return (false);
	for (std::vector<std::string>::iterator iter = allow_method_value_iter; iter != allow_method_vector.end(); iter++)
	{
		if (*iter != "GET" && \
			*iter != "POST" && \
			*iter != "DELETE")
			return (false);
	}
    return (true);
}

/*
    필수항목에서 빠진 것은 없는지 확인
    필수항목 포함하여 중복된 것은 없는지 확인 이미 확인하기에 필요없을 듯
    NESSARY_UNIQUE,
    NESSARY_MULTI,
	OPTION_UNIQUE,
*/
bool ConfigInfo::checkNessaryOrUniqueField(std::map<std::string, ConfigInfo::ValidateFieldInfo> validate_server_field_map)
{
	std::map<std::string, ValidateFieldInfo>::iterator	map_iter;

	for (map_iter = validate_server_field_map.begin(); map_iter != validate_server_field_map.end(); map_iter++)
	{
		if (map_iter->second.getValidateFieldType() == NESSARY_UNIQUE)
		{
			if (map_iter->second.getCount() != 1) // 중복 및 누락여부 확인 가능 
				return (false);
		}
		if (map_iter->second.getValidateFieldType() == NESSARY_MULTI)
		{
			if (map_iter->second.getCount() == 0) // 누락여부 확인 가능 
				return (false);
		}
		if (map_iter->second.getValidateFieldType() == OPTION_UNIQUE)
		{
			if (map_iter->second.getCount() > 1) // 없어도 되지만 중복이며 안됨
				return (false);
		}
	}
    return (true);
}

/*
    error_page 403 404 400 500 /50x.html
    형식이 제대로 맞는지도 확인필요
    ;이 있는가?
    마지막 단어에 있어야한다.
	;다음에 #이나 문자가 있는 경우 문제됨
*/
bool ConfigInfo::checkErrorPageConfigField(std::string error_page)
{
	std::string							value;
	size_t 								semicolon_pos;
	size_t								comment_pos;
	std::vector<std::string>			error_page_vector;
	std::vector<std::string>::iterator	error_page_end_value_iter;
	std::vector<std::string>::iterator	error_page_value_iter;

	semicolon_pos = error_page.find(';');
	if (semicolon_pos == std::string::npos)
		return (false);
	error_page = removeAfterSemicolon(error_page);
	comment_pos = error_page.find('#');
	if (comment_pos != std::string::npos)
	{
		if (semicolon_pos > comment_pos)
			return (false);
		else if (semicolon_pos + 1 != comment_pos)
			return (false);
	}
	error_page = ft_strtrim(error_page, this->whitespace);
	error_page_vector = ft_split(error_page, this->whitespace);
	error_page_value_iter = error_page_vector.begin() + 1;
	error_page_end_value_iter = error_page_vector.end() - 1;
	if (error_page_value_iter == error_page_vector.end() ||  // error_page;
		error_page_end_value_iter == error_page_vector.begin() || // error_page test;
		error_page_value_iter == error_page_end_value_iter) // error_page test ;
		return (false);

	for (std::vector<std::string>::iterator iter = error_page_value_iter; iter != error_page_end_value_iter; iter++)
	{
		if (checkErrorPageStatusCodde(*iter) == false)
			return (false);
	}
    return (true);
}

/*
- key value;
- key value;#;#;#;; #ㅇㄹㅁㄴㅇ라ㅣㅓㅁㅇ니러
- key value ;
- key value; #
- key value ; #
2개 이상
key는 이미 validate한지 확인한 이후
*/
bool	ConfigInfo::checkCommonConfigLineForm(std::vector<std::string> word_list)
{
	std::vector<std::string>::iterator	value_iter;
	std::vector<std::string>::iterator	semicolon_iter;
	std::string							value;
	size_t 								semicolon_pos;
	size_t								comment_pos;

	value_iter = word_list.begin() + 1;
	if (value_iter == word_list.end())
		return (false);

	value = *value_iter;
	if (value.size() == 0 || value[0] == '#')
		return (false);
	// value; ,value ;#
	// value;#sdfjklasdjklfj 이런 케이스는 음
	// ';'찾은 경우
	// ';'가 마지막에 있거나 ';'바로 다음이 '#'이어야 한다.
	semicolon_pos = value.find(';');
	// std::cout << "semicolon_pos : " << semicolon_pos << std::endl;
	comment_pos = value.find('#');
	// std::cout << BRW << "comment_pos : " << comment_pos << WHI << std::endl;
	// value안에 ';'이 있는 경우 "value;"
	// value안에 주석이 있었다면 ;이 주석 왼쪽에 있어야한다.
		// 없다면 value 다음 문자를 확인해야한다.
	// 주석이 없다면 ;이 "value;" 마지막에 있거나 "value" ";"처럼 value 바로 다음 단어에 첫 글자로 와야한다.

	// "value#;" ";"
	// "value#;" "#;"
	if (comment_pos != std::string::npos)
	{
		// 이미 주석처리 된것이므로 뒤에 단어에 ';'있든 없든 상관없어짐
		if (semicolon_pos > comment_pos)
			return (false);
		else if (semicolon_pos + 1 != comment_pos)
			return (false);
	}
	else
	{
		// "value"에 주석이 없으면서 "value"에 ';'로 없는 경우
		if (semicolon_pos == std::string::npos) // 다음 단어에 ';'이 처음에 있는지 확인
		{
			std::vector<std::string>::iterator	next_iter;
			size_t								next_word_semicolon_pos;

			next_iter = value_iter + 1;
			next_word_semicolon_pos = next_iter->find(';');

			// ';'이 다음단어 처음에 나타나는 경우
			// ';'다음은 없거나 '#'이 ';' 바로 뒤에 나와야함
			// ";#"허용
			if (next_word_semicolon_pos == 0)
			{
				if (next_iter->size() > 1 && next_iter->at(1) != '#')
					return (false);
			}
			else // 다음단어 처음에 ';'가 나타나지 않으면 error
				return (false);
		}
		else //주석이 없다면 ';'이 value 마지막에 나와야함
		{
			if (semicolon_pos != (value.size() - 1))
				return (false);
			else if (value_iter + 1 != word_list.end())
			{
				if ((value_iter + 1)->at(0) != '#')
					return (false);
			}
		}
	}
	value = removeAfterSemicolon(value);
	if (value.size() == 0)
		return (false);
	return (true);
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

	for (;iter != src_end_iter; iter++)
	{
		std::vector<std::string>	word_list;
		std::string					clean_str;

		clean_str = ft_strtrim(*iter, this->whitespace);
		if (clean_str == "" || clean_str[0] == '#')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
	 	if (findOpenCurlyBracket(word_list.begin(), word_list.end(), server_flag, location_flag, error_flag))
		{
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
		}
		else if (findCloseCurlyBracket(word_list.begin(), word_list.end(), error_flag))
		{
			if (location_stack.empty() == false)
			{
				location_stack.pop();
				location_flag = false;
			}
			else if (server_stack.empty() == false)
			{
				server_stack.pop();
				begin_iter++; // block내부만 가리키게 만듦
				end_iter = iter;
				return (true);
			}
		}
		if (error_flag == false)
			return (false);
	}
	return (false);
}

std::map<std::string, ConfigInfo::ValidateFieldInfo>	ConfigInfo::getValidateServerFieldMap()
{
	std::map<std::string, ConfigInfo::ValidateFieldInfo>	validate_server_field_map;
	ValidateFieldInfo										validate_field_info;

	validate_field_info.setValidateFieldType(NESSARY_UNIQUE);
	validate_server_field_map["host"] = validate_field_info;
	validate_server_field_map["port"] = validate_field_info;
	validate_server_field_map["root"] = validate_field_info;
	validate_server_field_map["index"] = validate_field_info;
	validate_server_field_map["client_max_body_size"] = validate_field_info;
	validate_field_info.setValidateFieldType(OPTION_UNIQUE);
	validate_server_field_map["server_name"] = validate_field_info;
	validate_field_info.setValidateFieldType(OPTION_MULTI);
	validate_server_field_map["error_page"] = validate_field_info;
	validate_server_field_map["location"] = validate_field_info;
	return (validate_server_field_map);
}

std::map<std::string, ConfigInfo::ValidateFieldInfo>	ConfigInfo::getValidateLocationFieldMap()
{
	std::map<std::string, ConfigInfo::ValidateFieldInfo>	validate_location_field_map;
	ValidateFieldInfo										validate_field_info;

	validate_field_info.setValidateFieldType(NESSARY_UNIQUE);
	validate_location_field_map["location"] = validate_field_info;
	validate_field_info.setValidateFieldType(OPTION_UNIQUE);
	validate_location_field_map["allow_method"] = validate_field_info;
	validate_location_field_map["autoindex"] = validate_field_info;
	validate_location_field_map["root"] = validate_field_info;
	validate_location_field_map["index"] = validate_field_info;
	validate_location_field_map["redirect"] = validate_field_info;
	return (validate_location_field_map);
}

/*
- location field가 있고 해당 location field에 root항목이 비어있다면 
  sever config block에 있는 root를 넣어준다.
*/
void	ConfigInfo::setRootToLocationConfig(void)
{
	std::vector<ServerConfig>					webserv_config;
	std::vector<ServerConfig>::const_iterator	webserv_config_iter;

	webserv_config = getWebservConfig();
	webserv_config_iter = webserv_config.begin();
	for (;webserv_config_iter != webserv_config.end(); webserv_config_iter++)
	{
		ServerConfig											server_config;
		std::map<std::string, LocationConfig>					location_config_map;
		std::map<std::string, LocationConfig>::const_iterator	location_config_map_iter;
		
		server_config = *webserv_config_iter;
		location_config_map = server_config.getLocations();
		location_config_map_iter = location_config_map.begin();
		for (;location_config_map_iter != location_config_map.end(); location_config_map_iter++)
		{
			LocationConfig	location_config;

			location_config = location_config_map_iter->second;
			if (location_config.getRoot() == "")
			{
				location_config.printLocationConfingContent(BRW);
				location_config.setRoot(server_config.getRoot());
				std::cout << std::endl << std::endl << "===== after setRoot ==========" << std::endl << std::endl<< std::endl;
				location_config.printLocationConfingContent(BRW);
			}
		}

	}
}

bool	ConfigInfo::parse(const std::string &file_content)
{
	std::string							str;
	std::vector<std::string>			file_content_vector;
	std::vector<std::string>::iterator 	cur_iter;
	std::vector<std::string>::iterator 	begin_iter;
	std::vector<std::string>::iterator 	end_iter;
	
	file_content_vector = ft_split(file_content, "\n");
	cur_iter = file_content_vector.begin();
	while (cur_iter != file_content_vector.end())
	{
		std::string	clean_str;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#')
		{
			cur_iter++;
			continue ;
		}
		std::vector<std::string>	word_list;
		
		word_list = ft_split(*cur_iter, this->whitespace);
		if (word_list[0] != "server")
			return (false);
		if (findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter) == false)
			return (false);
		std::vector<std::string>	serverBlockVector(begin_iter, end_iter);

		if (parseServerBlock(serverBlockVector) == false)
			return (false);
		cur_iter++;
	}
	setRootToLocationConfig();
	return (true);
}

bool ConfigInfo::parseServerBlock(std::vector<std::string> server_block_vec)
{
	ServerConfig										server_config;
	std::map<std::string, ValidateFieldInfo>			server_field_map;
	std::map<std::string, ValidateFieldInfo>::iterator	map_iter;
	std::map<std::string, ValidateFieldInfo>::iterator	server_field_map_iter;
	std::vector<std::string>::iterator 					cur_iter;
	std::vector<std::string>::iterator 					begin_iter;
	std::vector<std::string>::iterator 					end_iter;
	std::vector<std::string>::iterator 					src_begin_iter;
	std::vector<std::string>::iterator 					src_end_iter;

	src_begin_iter = server_block_vec.begin();
	src_end_iter = server_block_vec.end();
	server_field_map = getValidateServerFieldMap();
	cur_iter = src_begin_iter;
	while (cur_iter != src_end_iter)
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;
		std::string					first_word;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#' || clean_str[0] == '{' || clean_str[0] == '}')
		{
			cur_iter++;
			continue ;
		}
		word_list = ft_split(clean_str, this->whitespace);
		first_word = *(word_list.begin());
		if (first_word == "location")
		{
			
			if (findLocationBlock(cur_iter, src_end_iter, begin_iter, end_iter) == false)
				return (false);
			if (parseLocationBlock(begin_iter, end_iter, server_config) == false)
				return (false);
		}
		else
		{
			server_field_map_iter = server_field_map.find(first_word);
			if (server_field_map_iter == server_field_map.end())
			{
				std::cerr << RED <<  "Could not found field " << first_word << WHI<<std::endl;
				return (false);
			}
			else
			{
				std::string							field_value;

				field_value = *(word_list.begin() + 1);
				field_value = removeAfterSemicolon(field_value);
				if (first_word == "host")
					server_config.setHost(field_value);
				else if (first_word == "port")
					server_config.setPort(std::atoi(field_value.c_str()));
				else if (first_word == "root")
					server_config.setRoot(field_value);
				else if (first_word == "index")
					server_config.setIndex(field_value);
				else if (first_word == "client_max_body_size")
					server_config.setClientMaxBodySize(std::atoi(field_value.c_str()));
				else if (first_word == "server_name")
					server_config.setServerName(field_value);
				else if (first_word == "error_page")
				{
					std::map<int, std::string>			error_page_map;

					parseErrorPageConfigField(clean_str, error_page_map);
					std::map<int, std::string>::const_iterator error_page_iter = error_page_map.begin();
					for (; error_page_iter != error_page_map.end(); error_page_iter++)
						server_config.addErrorPageElement(error_page_iter->first, error_page_iter->second);
				}
				else
				{
					std::cerr << RED <<  "Could not found field " << first_word << WHI<<std::endl;
					return (false);
				}
			}
		}
		cur_iter++;
	}
	this->server_config_vector.push_back(server_config);
	return (true);
}

/*
    error_page 403 404 400 500 /50x.html
    형식이 제대로 맞는지도 확인필요
    ;이 있는가?
    마지막 단어에 있어야한다.
	;다음에 #이나 문자가 있는 경우 문제됨
*/
bool	ConfigInfo::parseErrorPageConfigField(std::string error_page, std::map<int, std::string>& error_page_map)
{
	std::vector<std::string>			error_page_vector;
	std::string							error_page_end_value;
	std::vector<std::string>::iterator	error_page_end_value_iter;
	std::vector<std::string>::iterator	error_page_value_iter;

	error_page = removeAfterSemicolon(error_page);
	error_page = ft_strtrim(error_page, this->whitespace);
	error_page_vector = ft_split(error_page, this->whitespace);
	error_page_value_iter = error_page_vector.begin() + 1;
	error_page_end_value_iter = error_page_vector.end() - 1;
	error_page_end_value = *error_page_end_value_iter;
	for (std::vector<std::string>::iterator iter = error_page_value_iter; iter != error_page_end_value_iter; iter++)
	{
		std::string	error_status_code_str;
		int			error_status_code;

		error_status_code_str = *iter;
		error_status_code = std::atoi(error_status_code_str.c_str());
		error_page_map[error_status_code] = error_page_end_value;
	}
    return (true);
}

bool        ConfigInfo::validateServerBlock(std::vector<std::string> server_block_vec)
{
	std::map<std::string, ValidateFieldInfo>			validate_server_field_map;
	std::map<std::string, ValidateFieldInfo>::iterator	map_iter;
	std::map<std::string, ValidateFieldInfo>::iterator	validate_server_field_map_iter;
	std::vector<std::string>::iterator 					cur_iter;
	std::vector<std::string>::iterator 					begin_iter;
	std::vector<std::string>::iterator 					end_iter;
	std::vector<std::string>::iterator 					src_begin_iter;
	std::vector<std::string>::iterator 					src_end_iter;

	src_begin_iter = server_block_vec.begin();
	src_end_iter = server_block_vec.end();
	validate_server_field_map = getValidateServerFieldMap();
	cur_iter = src_begin_iter;
	while (cur_iter != src_end_iter)
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;
		std::string					first_word;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#' || clean_str[0] == '{' || clean_str[0] == '}')
		{
			cur_iter++;
			continue ;
		}
		word_list = ft_split(clean_str, this->whitespace);
		first_word = *(word_list.begin());
		if (first_word == "location")
		{
			if (findLocationBlock(cur_iter, src_end_iter, begin_iter, end_iter) == false)
				return (false);
			if (validateLocationBlock(begin_iter, end_iter) == false)
				return (false);
			validate_server_field_map[first_word]++;
		}
		else
		{
			validate_server_field_map_iter = validate_server_field_map.find(first_word);
			if (validate_server_field_map_iter == validate_server_field_map.end())
			{
				std::cerr << RED <<  "Could not found field " << first_word << WHI<<std::endl;
				return (false);
			}
			else
			{
				std::vector<std::string>::iterator	vec_iter;
				ValidateFieldInfo					validate_field_info;
				std::string							field_name;
				std::string							field_value;

				field_name = validate_server_field_map_iter->first;
				validate_field_info = validate_server_field_map_iter->second;
				field_value = *(word_list.begin() + 1);
				if (first_word == "host")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (checkHostConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "port")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (checkPortConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "root")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "index")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "client_max_body_size")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (checkClientMaxBodySizeConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "server_name")
				{
					if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "error_page")
				{
					if (checkErrorPageConfigField(clean_str) == false)
						return (false);
				}
				else if (first_word == "cgi_pass")
				{
					if (checkErrorPageConfigField(clean_str) == false)
						return (false);
				}
				else
				{
					std::cerr << RED <<  "Could not found field " << first_word << WHI<<std::endl;
					return (false);
				}
			}
		}
		cur_iter++;
	}
	if (checkNessaryOrUniqueField(validate_server_field_map) == false)
		return (false);
	return (true);
}

bool ConfigInfo::parseLocationBlock(std::vector<std::string>::iterator &src_begin_iter, 
									const std::vector<std::string>::iterator &src_end_iter,
									ServerConfig& server_config)
{
	LocationConfig										location_config;
	std::map<std::string, ValidateFieldInfo>			location_field_map;
	std::map<std::string, ValidateFieldInfo>::iterator	map_iter;
	std::map<std::string, ValidateFieldInfo>::iterator	location_field_map_iter;
	std::vector<std::string>::iterator 					cur_iter;
	std::string 										location_path;

	location_field_map = getValidateLocationFieldMap();
	cur_iter = src_begin_iter; // location path {} skip
	for (; cur_iter != src_end_iter; cur_iter++)
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;
		std::string					first_word;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#' || clean_str[0] == '{' || clean_str[0] == '}')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
		first_word = *(word_list.begin());
		location_field_map_iter = location_field_map.find(first_word);
		if (location_field_map_iter == location_field_map.end())
			return (false);
		else
		{
			std::string							field_name;
			std::string							field_value;
			std::vector<std::string>			allow_method_vector;
			std::vector<std::string>::iterator	allow_method_value_iter;

			field_name = location_field_map_iter->first;

			field_value = *(word_list.begin() + 1);
			field_value = removeAfterSemicolon(field_value); // important!
			if (first_word == "location")
			{
				location_path = field_value;
				location_config.setLocationPath(location_path);
			}
			else if (first_word == "allow_method")
			{
				std::map<MethodType, bool>			allow_method_map;
				std::vector<std::string>			allow_method_vector;
				std::vector<std::string>::iterator	field_value_iter;

				field_value_iter = word_list.begin() + 1;
				allow_method_map[GET] = false;
				allow_method_map[POST] = false;
				allow_method_map[DELETE] = false;
				for (;field_value_iter != word_list.end(); field_value_iter++)
				{
					field_value = removeAfterSemicolon(*field_value_iter); // important!s
					if (field_value == "GET")
						allow_method_map[GET] = true;
					else if (field_value == "POST")
						allow_method_map[POST] = true;
					else if (field_value == "DELETE")
						allow_method_map[DELETE] = true;
					else
					{
						std::cerr << RED <<  "Unkwon allow method type : " << *field_value_iter << WHI<<std::endl;
						return (false);
					}
				}
				location_config.setAllowMethod(allow_method_map);
			}
			else if (first_word == "autoindex")
			{
				bool	autoindex_flag;

				autoindex_flag = false;
				if (field_value == "on")
					autoindex_flag = true;
				location_config.setAutoindex(autoindex_flag);
			}
			else if (first_word == "root")
				location_config.setRoot(field_value);
			else if (first_word == "index")
				location_config.setIndex(field_value);
			else if (first_word == "redirect")
				location_config.setRedirect(field_value);
			else
			{
				std::cerr << RED <<  "Could not found field in validateLocationBlock()" << first_word << WHI<<std::endl;
				return (false);
			}
		}
	}
	std::map<std::string, LocationConfig>::const_iterator loc_iter = server_config.getLocations().find(location_path);
	if (loc_iter != server_config.getLocations().end())
	{
		return (false);
	}
	server_config.addLocationElement(location_path, location_config);
	return (true);
}


/*
    1. 유효필드 검사를 위해 map<std::string, ValidateFieldInfo>를 사용한다.
        struct ValidateFieldInfo
        {
            VaildateFieldType	validate_field_type;
            size_t			count;
        };

        enum	VaildateFieldType
        {
            NESSARY_UNIQUE
            NESSARY_MULTI
            OPTION_UNIQUE
            OPTION_MULTI
        };

    2. 각 줄을 읽으면서 validate한지 확인한다.
        - 각 field 마다 value type, value range 등의 제한조건이 다르다.
        - ;
        - # 확인 필요
    3. locatoin이 나오면 find location block을 쓴다.

*/
bool        ConfigInfo::validateLocationBlock(std::vector<std::string>::iterator& src_begin_iter,
										const std::vector<std::string>::iterator& src_end_iter)
{
	std::map<std::string, ValidateFieldInfo>			validate_location_field_map;
	std::map<std::string, ValidateFieldInfo>::iterator	map_iter;
	std::map<std::string, ValidateFieldInfo>::iterator	validate_location_field_map_iter;
	std::vector<std::string>::iterator 					cur_iter;

	validate_location_field_map = getValidateLocationFieldMap();
	cur_iter = src_begin_iter;
	for (; cur_iter != src_end_iter; cur_iter++)
	{
		std::string					clean_str;
		std::vector<std::string>	word_list;
		std::string					first_word;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#' || clean_str[0] == '{' || clean_str[0] == '}')
			continue ;
		word_list = ft_split(clean_str, this->whitespace);
		first_word = *(word_list.begin());
		validate_location_field_map_iter = validate_location_field_map.find(first_word);
		if (validate_location_field_map_iter == validate_location_field_map.end())
			return (false);
		else
		{
			std::vector<std::string>::iterator	vec_iter;
			ValidateFieldInfo					validate_field_info;
			std::string							field_name;

			field_name = validate_location_field_map_iter->first;
			validate_field_info = validate_location_field_map_iter->second;

			if (first_word == "allow_method")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
					return (false);
				if (checkAllowMethodConfigField(clean_str) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "autoindex")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (checkCommonConfigLineForm(word_list) == false)
					return (false);
				std::string 	value_with_semicolon_comment;
				std::string 	value;
				
				value_with_semicolon_comment = *(word_list.begin() + 1);
				value = removeAfterSemicolon(value_with_semicolon_comment);
				if (value != "on" && value != "off")
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "root")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "index")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "redirect")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "location")
			{
				if (checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					validate_location_field_map[field_name]++;
			}
			else
			{
				std::cerr << RED <<  "Could not found field in validateLocationBlock()" << first_word << WHI<<std::endl;
				return (false);
			}
		}
	}
	return (true);
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
	
	file_content_vector = ft_split(file_content, "\n");
	cur_iter = file_content_vector.begin();
	while (cur_iter != file_content_vector.end())
	{
		std::string	clean_str;

		clean_str = ft_strtrim(*cur_iter, this->whitespace);
		if (clean_str.size() == 0 || clean_str[0] == '#')
		{
			cur_iter++;
			continue ;
		}
		std::vector<std::string>	word_list;
		
		word_list = ft_split(*cur_iter, this->whitespace);
		if (word_list[0] != "server")
			return (false);
		if (findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter) == false)
			return (false);
		std::vector<std::string>	serverBlockVector(begin_iter, end_iter);

		if (validateServerBlock(serverBlockVector) == false)
			return (false);
		std::cout << std::endl << std::endl << std::endl;
		cur_iter++;
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
	if (checkCurlyBracketPair(file_content) == false)
		return (false);
	if (checkWhole(file_content) == false)
		return (false);
	return (true);
}


ConfigInfo::ValidateFieldInfo::ValidateFieldInfo(void)
	:count(0)
{}

ConfigInfo::ValidateFieldInfo &ConfigInfo::ValidateFieldInfo::operator++(int)
{
	this->count++;
	return (*this);
}

void ConfigInfo::ValidateFieldInfo::setValidateFieldType(const VaildateFieldType& _validate_field_type)
{
	this->validate_field_type = _validate_field_type;
}

void ConfigInfo::ValidateFieldInfo::setCount(const size_t &_count)
{
	this->count = _count;
}

const ConfigInfo::VaildateFieldType &ConfigInfo::ValidateFieldInfo::getValidateFieldType(void) const
{
	return (this->validate_field_type);
}

const size_t &ConfigInfo::ValidateFieldInfo::getCount(void) const
{
	return (this->count);
}

void ConfigInfo::printContent(const std::string &str, const std::string &str_name, const std::string &color)
{
	std::cout << color << str_name << " : $" << str << "$" << WHI << std::endl;
}

void ConfigInfo::printVector(std::vector<std::string> &word_list, const std::string &str_name, const std::string &color)
{
	std::vector<std::string>::iterator	iter;

	for(iter = word_list.begin(); iter != word_list.end(); iter++)
		printContent(*iter, str_name, color);
}

// bool ConfigInfo::testConvUriToPath(const std::string &URI, const unsigned short &port)
// {
//     return false;
// }
// bool ConfigInfo::checkFilePathofURI(const std::string &URI, const std::string &truncated_URI)
// {
// 	printContent(truncated_URI, "truncated_URI", BRW);
// 	if (truncated_URI.size())
// 	{
// 		printFilePathofURI(URI, truncated_URI);
// 	    return false;
// 	}
// 	else // 제대로 걸렀다면 else문을 타서는 안된다.
// 	{
// 		std::cout << "there are no path('/') sign" << std::endl;
// 	    return true;
// 	}
// }

void ConfigInfo::printFilePathofURI(const std::string &URI, const std::string &truncated_URI)
{
	printContent(URI, "URI\t\t\t", RED);
	printContent(truncated_URI, "truncated_URI\t\t", GRN);
}

// bool ConfigInfo::checkMatchFilePathToLocationConfig(const std::string &startline_of_URI, const std::string &file_path_request_URI, const LocationConfig &location_config, bool is_matched_location_config)
// {
// 	if (is_matched_location_config)
// 	{
// 		checkFilePathofURI(startline_of_URI, file_path_request_URI);
// 		location_config.printLocationConfingContent(PUP);
// 	}
// 	else
// 	{

// 		std::cout << "is_matched_location_config is false" << std::endl;
// 	}
//     return is_matched_location_config;
// }

std::string getFilePathFromRequestURI(const std::string& startline_of_URI)
{
	size_t		file_path_start_idx;
	std::string	file_path_request_URI;

	file_path_start_idx = startline_of_URI.find('/');
	if (file_path_start_idx == std::string::npos)
		return ("");
	file_path_request_URI = startline_of_URI.substr(file_path_start_idx,  startline_of_URI.size() - file_path_start_idx);
	return (file_path_request_URI);
}

// location_config으로 설정된 location_config을 받음
// bool ConfigInfo::matchFilePathToLocationConfig(const std::string &file_path_request_URI, const unsigned short &port, LocationConfig &location_config)
// {
// 	if (getLocationConfig(port, file_path_request_URI, location_config))
// 		return (true);
// 	return (false);
// }

/*
	location block안에 redirect 정보가 있는가?
*/
bool ConfigInfo::checkRedirect(const std::string& file_path_request_URI, const unsigned short &port, std::string& file_path)
{
	LocationConfig	location_config;
	
	if (getLocationConfig(port, file_path_request_URI, location_config) && \
		(location_config.getRedirect() != ""))
	{
		// std::cout  << RED << "===========" << WHI << std::endl;
		// std::cout  << RED << "redirect ||" << WHI << std::endl;
		// std::cout  << RED << "===========" << WHI << std::endl;
		// location_config.printLocationConfingContent(RED);
		file_path = location_config.getRedirect();
		return (true);
	}
	return (false);
}

/*
- location block안에서 찾았다면
	- location block root + file_path_request_URI

	- server block root + file_path_request_URI
	root뒤에 '/'가 있다면 제거
*/
std::string ConfigInfo::getAbsFilePath(const std::string &file_path_request_URI, const unsigned short &port)
{
	LocationConfig	location_config;
	std::string		root_path;

	if (getLocationConfig(port, file_path_request_URI, location_config))
	{
		if (location_config.getRoot() != "")
			return (file_path_request_URI + location_config.getRoot());
		else
		{
			ServerConfig	server_config;

			if (getServerConfig(port, server_config))
				return (location_config.getRoot() + file_path_request_URI);
			else // 만약 serverConfig를 못찾았어도 문제. 하지만 config 파일에서 port를 읽어서 처리할것으로 예상되므로 괜찮을듯
				return ("");
		}
	}
	else
	{
		ServerConfig	server_config;

		if (getServerConfig(port, server_config))
			return (server_config.getRoot() + file_path_request_URI);
		else
			return ("");
	}
}

/*
stat()으로 해당 경로에 존재하는지 확인
예외처리 필요

*/
enum FileExistanceType ConfigInfo::getFileExistanceType(const std::string &file_path)
{
	struct stat	statbuf;

	
	if (stat(file_path.c_str(), &statbuf) < 0) {
		std::cerr << "stat error" << std::endl;
        exit(1);
    }
    if (S_ISREG(statbuf.st_mode))
    {
        std::cout << "file_path.c_str() is Regular file : " << file_path.c_str() << std::endl;
		return (EXIST_DIRECTORY);
    }
	else if (S_ISDIR(statbuf.st_mode))
    {
        std::cout << "file_path.c_str() is directory : " << file_path.c_str() << std::endl;
		return (EXIST_FILE);
    }
	
	// (void)file_path;

    return (NO_EXIST);
}

/*
	.php가 마지막에 위치하는지 확인 필요
*/
bool ConfigInfo::isCgiRequest(const std::string &file_path)
{
	(void)file_path;
	// std::vector<std::string>	
	//  ft_split(file_path, ".php");
	// if (file_path.compare(".php") || file_path.compare(".py"))
	// 	return (true);
    return (false);
}

/*
    - URI로 들어오는 것이 서버 내에 있는지
        있다면 어떤 경로에 있는지 찾아준다.
    - URI로 들어올 수 있는 것들의 경우
        - full path [servern_name]:[port][directory_path][file_name_with_extension]
            - localhost:4242/index.html
        - servern_name:port
            - localhost:4242
            - with file_path
                localhost:4242/
        - 127.0.0.1:4242/index.php

    - 어떤 port에 대해서 들어오는지 알아야함
        나의 경우 가장 처음 발생하는 '/'로 잘라야한다
        [host][port]'/'[file_path]
        - '/'가 없이 들어올 수도 있는가?
            그런 경우 404
    - 레퍼런스로 넘어온 file_path에 찾은 경로를 넣어줌

    "URI    "
    '\t'' '
    trim(whitepace )
    rfind
    .php
    입력은 정상적으로 들어온다고 가정한다.
    "index.php " -> 의도해서 사용자가 보냈다고 생각. ' '가 ascii로 변한됨
    isCGIRequest()호출하여 사용됨
    //

    string (1)
    int compare (const string& str) const;
    substrings (2)
    int compare (size_t pos, size_t len, const string& str) const;
    int compare (size_t pos, size_t len, const string& str, size_t subpos, size_t sublen) const;
    c-string (3)
    int compare (const char* s) const;int compare (size_t pos, size_t len, const char* s) const;
    buffer (4)
    int compare (size_t pos, size_t len, const char* s, size_t n) const;
    enum PathState
    {
        PATH_NOTFOUND,
        PATH_VALID,
        PATH_AUTOINDEX,
        PATH_CGI, //
        PATH_REDIRECT,
    };
*/
enum PathState ConfigInfo::convUriToPath(const std::string& startline_of_URI, const unsigned short& port, std::string& file_path)
{
	std::string				file_path_request_URI;
	// enum FileExistanceType	file_existance_type;

	// file_path = "";
	// host:port/file_path/file_name
	// "/file_path/file_name"
	// "/file_path/file_name.html"
	file_path_request_URI = getFilePathFromRequestURI(startline_of_URI);
	if (file_path_request_URI == "")
		return (PATH_NOTFOUND);
	// (void)port;
	// is_matched_location_config = matchFilePathToLocationConfig(file_path_request_URI, port, location_config);
	/*
		- redirect인지 확인
		해당 location block에 
		redirect가 있다면 해당 블럭으로 넘김
	*/
	// 확인 필요
	if (checkRedirect(file_path_request_URI, port, file_path))
		return (PATH_REDIRECT);
	/*
		location block root + location_path
		server block root + location_path
	*/
	file_path = getAbsFilePath(file_path_request_URI, port);
	printContent(file_path, "file_path", GRN);

	// enum FileExistanceType getFileExistanceType(cont std::string& abs_file_path_of_server)
	/*
		존재하는가?
		한다면 파일인가 디렉터리인가?
	*/
	//file_path에 계속 더해줌
	// file_existance_type = getFileExistanceType(file_path);
	// switch (file_existance_type)
	// {
	// 	case EXIST_FILE:
	// 	{
	// 		if (isCgiRequest(file_path)); // TODO
	// 			return (PATH_CGI);
	// 		return (PATH_VALID);
	// 		break ;
	// 	}
	// 	case EXIST_DIRECTORY:
	// 	{
	// 		// (file_path, port);
	// 		// index에 걸리는지
	// 		// location block
	// 		// server block
	// 		break ;
	// 	}
	// 	case NO_EXIST:
	// 	{
	// 		return (PATH_NOTFOUND);
	// 		break ;	
	// 	}
	// 	default:
	// 	{
	// 		std::cerr << "error in convUriToPath()" << std::endl;
	// 		return (PATH_NOTFOUND);
	// 		break ;
	// 	}
	// }
	return (PATH_NOTFOUND);
}
