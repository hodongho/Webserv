#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <map>
# include <string>

enum Method {}; // temp

class LocationConfig
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