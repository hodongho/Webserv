#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <map>
# include <string>
# include "HTTPMessage.hpp"

class LocationConfig
{
	private:
		// std::string					location_path;	// 필수 필드 key로 이동 (std::map<std::string, LocationConfig>	locations)
		std::string						root;			// 선택 필드, 입력값 없으면 ""
		std::string						index;			// 선택 필드, 입력값 없으면 ""
		std::string						redirect;		// 선택 필드, 입력값 없으면 "" , default로 301 code만 들어온다고 가정
		bool							autoindex;		// 선택 필드, 입력값 없으면 default를 false -> "off"를 의미
		std::map<MethodType, bool>		allow_method;	// 선택 필드, 입력값 없으면 default를 MethodType enum으로 초기화
		/* to be added cgi infomation!!! */
		//예상 std::map<std::string, CgiInfo>	cgis;
	public:
		LocationConfig(void);
		virtual ~LocationConfig();
};

#endif