#include "ServerHandler.hpp"
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
#include <sys/stat.h>

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
// localhost:80/find_path/
// /find_path/
bool	ConfigInfo::getLocationConfig(const unsigned short &port, const std::string &find_path, LocationConfig& location_config) const
{
	ServerConfig	server_config;

	if (this->getServerConfig(port, server_config) && \
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
bool	ConfigInfo::findOpenCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
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

bool	ConfigInfo::findCloseCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
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
	 	if (this->findOpenCurlyBracket(word_list.begin(), word_list.end(), server_flag, location_flag, error_flag))
		{
			if (location_flag == true)
			{
				location_flag = false;
				location_stack.push("{");
				begin_iter = iter;
			}
		}
		else if (this->findCloseCurlyBracket(word_list.begin(), word_list.end(), error_flag))
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
std::string	ConfigInfo::removeAfterSemicolon(const std::string& origin_value)

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

	field_value = this->removeAfterSemicolon(field_value);
	dot_count = 0;
	for (size_t	idx = 0; idx < field_value.length(); idx++)
	{
		if (field_value[idx] == '.')
		{
			if (this->checkIpClass(ip_class) == false)
				return (false);
			ip_class = "";
			dot_count++;
			continue ;
		}
		ip_class += field_value[idx];
	}
	if (this->checkIpClass(ip_class) == false)
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

	port = this->removeAfterSemicolon(port);
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

	client_body_size = this->removeAfterSemicolon(client_body_size);
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
	allow_method = this->removeAfterSemicolon(allow_method);
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
	error_page = this->removeAfterSemicolon(error_page);
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
		if (this->checkErrorPageStatusCodde(*iter) == false)
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
	value = this->removeAfterSemicolon(value);
	if (value.size() == 0)
		return (false);
	return (true);
}

bool	ConfigInfo::checkCgiPassConfigField(std::string	cgi_pass)
{
	std::string							value;
	size_t 								semicolon_pos;
	size_t								comment_pos;
	std::vector<std::string>			cgi_pass_vector;
	std::vector<std::string>::iterator	cgi_extension_iter;
	std::vector<std::string>::iterator	cgi_program_path_iter;

	semicolon_pos = cgi_pass.find(';');
	if (semicolon_pos == std::string::npos)
		return (false);
	cgi_pass = removeAfterSemicolon(cgi_pass);
	comment_pos = cgi_pass.find('#');
	if (comment_pos != std::string::npos)
	{
		if (semicolon_pos > comment_pos)
			return (false);
		else if (semicolon_pos + 1 != comment_pos)
			return (false);
	}
	cgi_pass = ft_strtrim(cgi_pass, this->whitespace);
	cgi_pass_vector = ft_split(cgi_pass, this->whitespace);
	cgi_extension_iter = cgi_pass_vector.begin() + 1;
	cgi_program_path_iter = cgi_pass_vector.begin() + 2;
	if (cgi_extension_iter == cgi_pass_vector.end() ||
		cgi_program_path_iter == cgi_pass_vector.end() ||
		(cgi_program_path_iter + 1) != cgi_pass_vector.end())
		return (false);
	if (*cgi_extension_iter != ".php" && *cgi_extension_iter != ".py")
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
	 	if (this->findOpenCurlyBracket(word_list.begin(), word_list.end(), server_flag, location_flag, error_flag))
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
		else if (this->findCloseCurlyBracket(word_list.begin(), word_list.end(), error_flag))
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
	validate_server_field_map["cgi_pass"] = validate_field_info;
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
void	ConfigInfo::setRootToLocationConfig(ServerConfig& server_config)
{
	std::map<std::string, LocationConfig>				location_config_map;
	std::map<std::string, LocationConfig>::iterator		location_config_map_iter;
	
	// server_config.
	location_config_map = server_config.getLocations();
	location_config_map_iter = location_config_map.begin();
	for (;location_config_map_iter != location_config_map.end(); location_config_map_iter++)
	{
		LocationConfig&	location_config = location_config_map_iter->second;
		
		if (location_config.getRoot() == "")
		{
			// location_config.printLocationConfingContent(BRW);
			location_config.setRoot(server_config.getRoot());
			// std::cout << std::endl << std::endl << "===== after setRoot ==========" << std::endl << std::endl<< std::endl;
			// location_config.printLocationConfingContent(BRW);
		}
	}
	server_config.setLocations(location_config_map);
}

bool	ConfigInfo::checkDuplicatedPort(void)
{
	std::vector<ServerConfig>					server_config_vector;
	std::vector<ServerConfig>::const_iterator	server_config_vector_iter;
	std::vector<unsigned short>					port_vector;

	server_config_vector = this->getWebservConfig();
	server_config_vector_iter = server_config_vector.begin();
	for ( ;server_config_vector_iter != server_config_vector.end(); server_config_vector_iter++)
	{
		std::vector<unsigned short>::const_iterator	port_vector_iter;

		port_vector_iter = port_vector.begin();
		for (; port_vector_iter != port_vector.end(); port_vector_iter++ )
		{
			unsigned short	cur_port;

			cur_port = *port_vector_iter;
			if (server_config_vector_iter->getPort() == cur_port)
				return (false);
		}
		port_vector.push_back(server_config_vector_iter->getPort());
	}
	return (true);
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
		if (this->findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter) == false)
			return (false);
		std::vector<std::string>	serverBlockVector(begin_iter, end_iter);

		if (this->parseServerBlock(serverBlockVector) == false)
			return (false);
		cur_iter++;
	}

	if (this->checkDuplicatedPort() == false)
		return (false);
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
	server_field_map = this->getValidateServerFieldMap();
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
			if (this->findLocationBlock(cur_iter, src_end_iter, begin_iter, end_iter) == false)
				return (false);
			if (this->parseLocationBlock(begin_iter, end_iter, server_config) == false)
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
				field_value = this->removeAfterSemicolon(field_value);
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

					this->parseErrorPageConfigField(clean_str, error_page_map);
					std::map<int, std::string>::const_iterator error_page_iter = error_page_map.begin();
					for (; error_page_iter != error_page_map.end(); error_page_iter++)
						server_config.addErrorPageElement(error_page_iter->first, error_page_iter->second);
				}
				else if (first_word == "cgi_pass")
				{
					std::string	cgi_extension;
					std::string	cgi_program_path;

					parseCgiPassConfigField(clean_str, cgi_extension, cgi_program_path);
					server_config.addCgiPassElement(cgi_extension, cgi_program_path);
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
	// server_config
	this->setRootToLocationConfig(server_config);
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

	error_page = this->removeAfterSemicolon(error_page);
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

bool	ConfigInfo::parseCgiPassConfigField(std::string cgi_pass, std::string& cgi_extension, std::string& cgi_program_path)
{
	std::vector<std::string>	cgi_pass_vector;

	cgi_pass = removeAfterSemicolon(cgi_pass);
	cgi_pass = ft_strtrim(cgi_pass, this->whitespace);
	cgi_pass_vector = ft_split(cgi_pass, this->whitespace);
	cgi_extension = *(cgi_pass_vector.begin() + 1);
	cgi_program_path = *(cgi_pass_vector.begin() + 2);
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
	validate_server_field_map = this->getValidateServerFieldMap();
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
			if (this->findLocationBlock(cur_iter, src_end_iter, begin_iter, end_iter) == false)
				return (false);
			if (this->validateLocationBlock(begin_iter, end_iter) == false)
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
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (this->checkHostConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "port")
				{
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (this->checkPortConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "root")
				{
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "index")
				{
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "client_max_body_size")
				{
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					if (this->checkClientMaxBodySizeConfigField(field_value) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "server_name")
				{
					if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
					if (this->checkCommonConfigLineForm(word_list) == false)
						return (false);
					validate_server_field_map[field_name]++;
				}
				else if (first_word == "error_page")
				{
					if (this->checkErrorPageConfigField(clean_str) == false)
						return (false);
				}
				else if (first_word == "cgi_pass")
				{
					if (this->checkCgiPassConfigField(clean_str) == false)
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
	if (this->checkNessaryOrUniqueField(validate_server_field_map) == false)
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

	location_field_map = this->getValidateLocationFieldMap();
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
			field_value = this->removeAfterSemicolon(field_value); // important!
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
				allow_method_map[METHOD_GET] = false;
				allow_method_map[METHOD_POST] = false;
				allow_method_map[METHOD_DELETE] = false;
				for (;field_value_iter != word_list.end(); field_value_iter++)
				{
					field_value = this->removeAfterSemicolon(*field_value_iter); // important!s
					if (field_value == "GET")
						allow_method_map[METHOD_GET] = true;
					else if (field_value == "POST")
						allow_method_map[METHOD_POST] = true;
					else if (field_value == "DELETE")
						allow_method_map[METHOD_DELETE] = true;
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

	validate_location_field_map = this->getValidateLocationFieldMap();
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
				if (this->checkDuplicateConfigField(validate_field_info) == false)
					return (false);
				if (this->checkAllowMethodConfigField(clean_str) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "autoindex")
			{
				if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (this->checkCommonConfigLineForm(word_list) == false)
					return (false);
				std::string 	value_with_semicolon_comment;
				std::string 	value;

				value_with_semicolon_comment = *(word_list.begin() + 1);
				value = this->removeAfterSemicolon(value_with_semicolon_comment);
				if (value != "on" && value != "off")
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "root")
			{
				if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (this->checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "index")
			{
				if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (this->checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "redirect")
			{
				if (this->checkDuplicateConfigField(validate_field_info) == false)
						return (false);
				if (this->checkCommonConfigLineForm(word_list) == false)
					return (false);
				validate_location_field_map[field_name]++;
			}
			else if (first_word == "location")
			{
				if (this->checkDuplicateConfigField(validate_field_info) == false)
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
		if (this->findServerBlock(cur_iter, file_content_vector.end(), begin_iter, end_iter) == false)
			return (false);
		std::vector<std::string>	serverBlockVector(begin_iter, end_iter);

		if (this->validateServerBlock(serverBlockVector) == false)
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
	if (this->checkCurlyBracketPair(file_content) == false)
		return (false);
	if (this->checkWhole(file_content) == false)
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
		this->printContent(*iter, str_name, color);
}

std::string	ConfigInfo::getFilePathFromRequestURI(const std::string& startline_of_URI)
{
	size_t		file_path_start_idx;
	std::string	file_path_request_URI;

	file_path_start_idx = startline_of_URI.find('/');
	if (file_path_start_idx == std::string::npos)
	{
		return ("");
	}
	file_path_request_URI = startline_of_URI.substr(file_path_start_idx,  startline_of_URI.size() - file_path_start_idx);
	size_t	cur_idx;

	cur_idx = startline_of_URI.find(':');
	if (file_path_start_idx != 0)
	{
		if (cur_idx == std::string::npos)
			return ("");

		// std::cout << BLU << "cur_idx : " << cur_idx << WHI << std::endl;
		// std::cout << CYN << "file_path_start_idx : " << file_path_start_idx << WHI << std::endl;
		cur_idx++; // ':' 기호 다음을 가리키게 만듦 
		for (size_t	idx = cur_idx; idx < file_path_start_idx;idx++) //port이어여함
		{
			// printContent(&startline_of_URI[idx], "startline_of_URI[idx]", BLU);
			// std::cout << "idx : " << idx << std::endl;
			// std::cout << "startline_of_URI : " << startline_of_URI << std::endl;
			// std::cout << "startline_of_URI[idx] : " << startline_of_URI[idx] << std::endl;
			if (std::isdigit(startline_of_URI[idx]) == false)
				return ("");
		}

	}
	return (file_path_request_URI);
}

/*
	location block안에 redirect 정보가 있는가?
*/
bool ConfigInfo::checkRedirect(const std::string& file_path_request_URI, const unsigned short &port, std::string& file_path)
{
	LocationConfig	location_config;

	if (this->getLocationConfig(port, file_path_request_URI, location_config) && \
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
	URI 및 Config root마지막에 있는 slash를 제거하는 용도로 쓰인다.
*/
void	ConfigInfo::removeLastSlashSignOfStr(std::string& str)
{
	int	last_char_idx;

	last_char_idx = str.size() - 1;
	while ((last_char_idx >= 0) && str[last_char_idx] == '/')
	{
		str.erase(last_char_idx, 1);
		last_char_idx = str.size() - 1;
	}
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

	if (this->getLocationConfig(port, file_path_request_URI, location_config))
	{
		root_path = location_config.getRoot();
		// // std::cout << WHI << "root_path : " << root_path <<  WHI << std::endl;
		removeLastSlashSignOfStr(root_path);
		// std::cout << BRW << "Found file_path_request_URI in location config root_path : " << root_path <<  WHI << std::endl;
	}
	else
	{
		ServerConfig	server_config;

		if (this->getServerConfig(port, server_config))
		{
			root_path = server_config.getRoot();
			// removeLastSlashSignOfStr(std::string& str) 대체할 것 단, 테스트 필요
			// // std::cout << WHI << "root_path : " << root_path <<  WHI << std::endl;
			removeLastSlashSignOfStr(root_path);
			// std::cout << BRW << "Found file_path_request_URI in server config root_path : " << root_path <<  WHI << std::endl;
		}
		else
		{
			std::cerr << RED << "Could not found server config in getAbsFilePath()" << std::endl;
			return ("");
		}
	}
	return (root_path + file_path_request_URI);
}

/*
stat()으로 해당 경로에 존재하는지 확인
예외처리 필요
(ClientSocketData* const & client_socket)
*/
enum FileExistanceType ConfigInfo::getFileExistanceType(const std::string &file_path, ClientSocketData* const & client_socket)
{
	struct stat	statbuf;


	if (stat(file_path.c_str(), &statbuf) < 0)
		throwServerError("stat sys call error: ", client_socket);
    if (S_ISREG(statbuf.st_mode))
		return (EXIST_FILE);
	else if (S_ISDIR(statbuf.st_mode))
		return (EXIST_DIRECTORY);
    return (NO_EXIST);
}

bool	ConfigInfo::isLastPartOfStr(const std::string& origin_str, const std::string& find_str)
{
	size_t	find_str_pos;

	find_str_pos = origin_str.rfind(find_str);
	if (find_str_pos != std::string::npos &&
		find_str_pos + find_str.size() == origin_str.size())
		return (true);
	return (false);
}

/*
	하드코딩된 extension을 
	getExtension을 이용해서 처리하게 변경할 수 있다.
	그렇게되면 config에 cgi-pass에 있는 extension 타입에 따라 우리의 cgi 적용범위를 더욱 넓게 처리할 수 있다.
*/
bool ConfigInfo::isCgiRequest(const std::string &file_path, const unsigned short& port)
{
	// config에 있는 cgi정보 이용 필요
	ServerConfig	server_config;

	if (getServerConfig(port, server_config))
	{
		const std::map<std::string, std::string>&			cgi_pass_map = server_config.getCgiPass();
		std::map<std::string, std::string>::const_iterator	cgi_pass_map_iter;

		cgi_pass_map_iter = cgi_pass_map.begin();
		for (;cgi_pass_map_iter != cgi_pass_map.end(); cgi_pass_map_iter++)
		{
			std::string	cgi_type_str;
			
			cgi_type_str = cgi_pass_map_iter->first;
			// printContent(cgi_type_str, "in isCgiRequest", BRW);
			// printContent(file_path, "in isCgiRequest if statement. file_path", BRW);
			if (this->isLastPartOfStr(file_path, ".php") &&
				this->isLastPartOfStr(cgi_type_str, ".php"))
				return (true);
			else if (this->isLastPartOfStr(file_path, ".py") &&
				this->isLastPartOfStr(cgi_type_str, ".py"))
				return (true);
		}
	}
	else
	{
		std::cerr << RED << "!!!! Could not found server config to match using arument port In isCgiRequest()" << WHI << std::endl;
		return (false);
	}
	// printContent(file_path, "after if statement in isCgiRequest. file_path", BRW);
	return (false);
}
/*
	.php가 마지막에 위치하는지 확인 필요
	- config에 있는 cgi항목 확인필요 
	- (.php or .py ) and ((cgi_pass .php /sdffsd) or (cgi_pass .py /sdffsd) )
*/
// bool ConfigInfo::isCgiRequest(const std::string &file_path)
// {
// 	// config에 있는 cgi정보 이용 필요
// 	if (isLastPartOfStr(file_path, ".php") || isLastPartOfStr(file_path, ".py"))
// 		return (true);
//     return (false);
// 	// if (isCgiRequestToBe(file_path))
// 	// 	return (true);
// 	// return (false);
// }

enum PathState ConfigInfo::convUriToPath(const std::string& startline_of_URI, const unsigned short& port, std::string& file_path, ClientSocketData* const & client_socket)
{
    std::string             file_path_request_URI;
    std::string             abs_file_path_of_server;
    enum FileExistanceType  file_existance_type;

	file_path_request_URI = this->getFilePathFromRequestURI(startline_of_URI);
	if (file_path_request_URI == "")
	{
		std::cout << RED << "PATH_NOTFOUND : " << startline_of_URI << WHI << std::endl;
		return (PATH_NOTFOUND);
	}
	// this->printContent(file_path_request_URI, "file_path_request_URI", BRW);
	if (this->checkRedirect(file_path_request_URI, port, file_path))
	{
		// std::cout << BLU << "PATH_REDIRECT : " << startline_of_URI << WHI << std::endl;
		return (PATH_REDIRECT);
	}
	abs_file_path_of_server = this->getAbsFilePath(file_path_request_URI, port);
	file_existance_type = this->getFileExistanceType(abs_file_path_of_server, client_socket);
	/*
		존재하는가?
		한다면 파일인가 디렉터리인가?
	*/
	// file_path에 계속 더해줌
	switch (file_existance_type)
	{
		case EXIST_FILE:
		{
			file_path = abs_file_path_of_server;
			// this->printContent(file_path, "file_path match to index file in EXIST_FILE before PATH_VALID of PATH_CGI", RED);
			if (this->isCgiRequest(file_path, port)) // TODO adjust cgi config info
			{
				// this->printContent(file_path, "file_path match to index file in EXIST_FILE, PATH_CGI", CYN);
				return (PATH_CGI);
			}
			// this->printContent(file_path, "file_path match to index file in EXIST_FILE, PATH_VALID", GRN);
			return (PATH_VALID);
			break ;
		}
		case EXIST_DIRECTORY:
		{
			LocationConfig	location_config;

			// index에 걸리는지
			if (this->getLocationConfig(port, file_path_request_URI, location_config))
			{
				std::string				index_file;
				enum FileExistanceType	recycle_file_existance_type;

				// add slash
				if (this->isLastPartOfStr(abs_file_path_of_server, "/") == false)
				{
					// this->printContent(abs_file_path_of_server, "abs_file_path_of_server last is '/'", BLU);
					abs_file_path_of_server.push_back('/');
				}
				index_file = location_config.getIndex();
				recycle_file_existance_type = this->getFileExistanceType(abs_file_path_of_server + index_file, client_socket);
				if (recycle_file_existance_type == EXIST_FILE)  // index를 여러개 받는 것으로 변경한다면 반복문 안에서 이 동작을 수행
				{
					file_path = abs_file_path_of_server + index_file;
					// this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_VALID", GRN);
					return (PATH_VALID);
				}
				else //NO_EXIST or EXIST_DIRECTORY
				{
					// autoindex check
					if (location_config.getAutoindex())
					{
						file_path = abs_file_path_of_server;
						// this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_AUTOINDEX", BLU);
						return (PATH_AUTOINDEX);
					}
					else
					{
						// this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_NOTFOUND", RED);
						return (PATH_NOTFOUND);
					}
				}
				// else if (EXIST_DIRECTORY)
				// 	;
			}
			else
			{
				ServerConfig	server_config;
				// server config에 index를 찾거나
				// index와 매칭되지 않았으면  PATH_NOTFOUND
				// std::cerr << "error in EXIST_DIRECTORY of convUriToPath()" << std::endl;

				// (const std::string& startline_of_URI, const unsigned short& port, std::string& file_path)
				if (this->getServerConfig(port, server_config))
				{
					std::string				index_file;
					enum FileExistanceType	recycle_file_existance_type;

					// add slash
					if (this->isLastPartOfStr(abs_file_path_of_server, "/") == false)
					{
						// this->printContent(abs_file_path_of_server, "abs_file_path_of_server last is '/'", BLU);
						abs_file_path_of_server.push_back('/');
					}
					index_file = server_config.getIndex();
					// this->printContent(abs_file_path_of_server + index_file, "abs_file_path_of_server + index_file in EXIST_DIRECTORY", BRW);
					recycle_file_existance_type = this->getFileExistanceType(abs_file_path_of_server + index_file, client_socket);
					if (recycle_file_existance_type == EXIST_FILE)  // index를 여러개 받는 것으로 변경한다면 반복문 안에서 이 동작을 수행
					{
						if (this->isLastPartOfStr(abs_file_path_of_server, "/") == false)
						{
							// this->printContent(abs_file_path_of_server, "abs_file_path_of_server last is '/'", BLU);
							abs_file_path_of_server.push_back('/');
						}
						file_path = abs_file_path_of_server + index_file;
						// this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_VALID", GRN);
						return (PATH_VALID);
					}
					else //NO_EXIST or EXIST_DIRECTORY
					{
						// this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_NOTFOUND", BRW);
						return (PATH_NOTFOUND);
					}
					// else if (EXIST_DIRECTORY)
					// 	;
				}
				else
				{
					this->printContent(file_path, "file_path match to index file in EXIST_DIRECTORY, PATH_NOTFOUND location config N.O.T found!!!!!", RED);
					return (PATH_NOTFOUND);
				}
			}
			break ;
		}
		case NO_EXIST:
		{
			// this->printContent(file_path, "file_path match to index file in NO_EXIST, PATH_NOTFOUND", RED);
			// this->printContent(abs_file_path_of_server, "file_path match to index file in NO_EXIST, PATH_NOTFOUND", RED);
			return (PATH_NOTFOUND);
			break ;	
		}
		default:
		{
			std::cerr << BRW << "error in convUriToPath()" << WHI << std::endl;
			return (PATH_NOTFOUND);
			break ;
		}
	}
	return (PATH_NOTFOUND);
}

bool ConfigInfo::isAllowedMethod(const std::string& URI, const unsigned short& port, const enum MethodType& method)
{
	size_t													URI_start_idx;
	std::string												origin_URI;
	std::vector<ServerConfig>::iterator 					server_config_iter;
	ServerConfig											server_config;
	std::map<std::string, LocationConfig>::const_iterator	location_iter;
	std::map<MethodType, bool>								allowed_method_type_map;

    URI_start_idx = URI.find('/');
    if (URI_start_idx == std::string::npos)
        return (false);
    origin_URI = URI.substr(URI_start_idx, URI.size() - URI_start_idx);

	for (server_config_iter = this->server_config_vector.begin(); server_config_iter != this->server_config_vector.end(); server_config_iter++)
	{
		if (server_config_iter->getPort() == port)
		{
			server_config = (*server_config_iter);
			break;
		}
	}
	if (server_config_iter == this->server_config_vector.end())
		return (false);
	/*
	if (!this->getServerConfig(port, server_config))
		return (false);
	*/
	location_iter = server_config.getLocations().find(origin_URI);
	if (location_iter != server_config.getLocations().end())
	{
		allowed_method_type_map = location_iter->second.getAllowMethod();
		return (allowed_method_type_map.find(method)->second);
	}
	else
		return (true);
	/*
	if (this->getLocationBlock())
	{
		allowed_method_type_map = location_iter->second.getAllowMethod();
		return (allowed_method_type_map.find(method)->second);
	}
	else
		return (true);
 	*/

}

int ConfigInfo::getErrorPage(StatusCode stat_code, const unsigned short& port, std::string& err_file_path, ClientSocketData* const & client_socket)
{
	int											err_code;
	std::vector<ServerConfig>::iterator			server_config_iter;
	std::map<int, std::string>::const_iterator	err_page_iter;

	server_config_iter = this->server_config_vector.begin();
	while (server_config_iter->getPort() != port)
		server_config_iter++;

	const std::map<int, std::string>& err_page = server_config_iter->getErrorPage();
	switch (stat_code)
	{
	case STATCODE_BADREQ:
		err_code = 400;
		break;
	case STATCODE_NOTFOUND:
		err_code = 404;
		break;
	case STATCODE_NOTALLOW:
		err_code = 405;
		break;
	case STATCODE_SERVERR:
		err_code = 500;
		break;
	default:
		return (-1);
		break;
	}
	err_page_iter = err_page.find(err_code);
	if (err_page_iter == err_page.end())
		return (-1);
	else
	{
		std::string	serverRoot = server_config_iter->getRoot();

		// slash_sign_idx = serverRoot.find('/');
		while (this->isLastPartOfStr(serverRoot, "/"))
			serverRoot.erase(serverRoot.size() - 1, 1);
		err_file_path = serverRoot + '/' + err_page_iter->second;
		if (this->getFileExistanceType(err_file_path, client_socket) != EXIST_FILE)
			return (-1);
		return (0);
	}
}

size_t ConfigInfo::getMaxBodySize(const unsigned short& port)
{
	std::vector<ServerConfig>::iterator			server_config_iter;

	server_config_iter = this->server_config_vector.begin();
	while (server_config_iter->getPort() != port)
		server_config_iter++;
	return (server_config_iter->getClientMaxBodySize());
}
// .php
std::string	ConfigInfo::getCgiProgramPath(const std::string& cgi_extension, const unsigned short& port)
{
	std::vector<ServerConfig>::iterator			server_config_iter;

	server_config_iter = this->server_config_vector.begin();
	while (server_config_iter->getPort() != port)
		server_config_iter++;
	return(server_config_iter->getCgiProgramPath(cgi_extension));
}
