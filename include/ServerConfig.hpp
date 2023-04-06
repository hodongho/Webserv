#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <map>
# include <string>

class LocationConfig;

class ServerConfig
{
	std::string								host;					// 필수 필드, IPv4 style "0.0.0.0"
	uint16_t								port;					// 필수 필드, 0~65525 범위 내에 존재해야함
	std::string								root;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
	std::string								index;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
	size_t									client_max_body_size;	// 필수 필드, 양수만 허용, atoi로 변환하여 처리, 숫자가 아닌 문자열인 경우는?, isdigit()사용
	std::map<int, std::string>				default_error_page;		// config에는 없지만 error_page에 없는 status code에 대한 default error page 경로를 담고 있음, 항상 값이 있음
	std::map<int, std::string>				error_page;				// 선택 필드, 복수값 입력가능, 입력값 없으면 size() == 0
	std::string								server_name;			// 선택 필드, 유일값, 입력 없으면 ""
	std::map<std::string, LocationConfig>	locations;				// 선택 필드, 입력값 없으면 size() == 0
};


#endif