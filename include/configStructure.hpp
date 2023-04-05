#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <vector>
# include <map>
# include <string>

struct ServerConfig;
struct LocationConfig;

struct ServerConfig
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

enum Method {}; // temp

struct LocationConfig
{
	// std::string					location_path;	// 필수 필드 key로 이동 (std::map<std::string, LocationConfig>	locations)
	std::map<Method, bool>		allow_method;	// 선택 필드, 입력값 없으면 default를 MethodType enum으로 초기화
	bool						autoindex;		// 선택 필드, 입력값 없으면 default를 false -> "off"를 의미
	std::string					root;			// 선택 필드, 입력값 없으면 ""
	std::string					index;			// 선택 필드, 입력값 없으면 ""
	std::string					redirect;		// 선택 필드, 입력값 없으면 "" , default로 301 code만 들어온다고 가정
	/* to be added cgi infomation!!! */
	//예상 std::map<std::string, CgiInfo>	cgis;
};

#endif