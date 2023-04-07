#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <map>
# include <string>
# include "LocationConfig.hpp"
# include <unistd.h>
// class LocationConfig;

class ServerConfig
{
	private:
		std::string								server_name;			// 선택 필드, 유일값, 입력 없으면 ""
		std::string								host;					// 필수 필드, IPv4 style "0.0.0.0"
		unsigned short							port;					// 필수 필드, 0~65525 범위 내에 존재해야함
		std::string								root;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
		std::string								index;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
		size_t									client_max_body_size;	// 필수 필드, 양수만 허용, atoi로 변환하여 처리, 숫자가 아닌 문자열인 경우는?, isdigit()사용
		std::map<int, std::string>				default_error_page;		// config에는 없지만 error_page에 없는 status code에 대한 default error page 경로를 담고 있음, 항상 값이 있음
		std::map<int, std::string>				error_page;				// 선택 필드, 복수값 입력가능, 입력값 없으면 size() == 0
		std::map<std::string, LocationConfig>	locations;				// 선택 필드, 입력값 없으면 size() == 0
	
	public:
		ServerConfig(void);
		virtual	~ServerConfig();

		// setter()
		void	setServerName(const std::string& _server_name);
		void	setHost(const std::string& _host);
		void	setPort(const unsigned short& _port);
		void	setRoot(const std::string& _root);
		void	setIndex(const std::string& _index);
		void	setClientMaxBodySize(const size_t& _client_max_body_size);
		void	setDefaultErrorPage(const std::map<int, std::string>& _default_error_page);
		void	setErrorPage(const std::map<int, std::string>& _error_page);
		void	addErrorPageElement(const int& error_status_code, const std::string& erro_page_uri);
		void	setLocations(const std::map<std::string, LocationConfig>& _locations);
		void	addLocationElement(const std::string& location_path, const LocationConfig& location_config);

		// getter()
		const std::string&							getServerName(void) const;			// 선택 필드, 유일값, 입력 없으면 ""
		const std::string&							getHost(void) const ;					// 필수 필드, IPv4 style "0.0.0.0"
		const unsigned short&								getPort(void) const ;					// 필수 필드, 0~65525 범위 내에 존재해야함
		const std::string&							getRoot(void) const ;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
		const std::string&							getIndex(void) const ;					// 필수 필드, 빈 문자열 입력은 허용하지 않음
		const size_t&								getClientMaxBodySize(void) const ;	// 필수 필드, 양수만 허용, atoi로 변환하여 처리, 숫자가 아닌 문자열인 경우는?, isdigit()사용
		const std::map<int, std::string>&			getDefaultErrorPage(void) const ;		// config에는 없지만 error_page에 없는 status code에 대한 default error page 경로를 담고 있음, 항상 값이 있음
		const std::map<int, std::string>&			getErrorPage(void) const ;				// 선택 필드, 복수값 입력가능, 입력값 없으면 size() == 0
		const std::map<std::string, LocationConfig>&getLocations(void) const ;				// 선택 필드, 입력값 없으면 size() == 0


		// print 
		template <typename T>
		void	printContent(const T &val, std::string name, std::string color) const;

		void	printMapContent(const std::map<int, std::string> &pair_map, std::string name, std::string color) const ;
		void	printMapContent(const std::map<std::string, LocationConfig> &pair_map, std::string name, std::string color) const ;
		
		void	printServerConfingContent(void)  const;
};


#include <iostream>
template <typename T>
inline void ServerConfig::printContent(const T &val, std::string name, std::string color) const
{
	std::cout << color << name << " : $" <<  val << "$" << WHI << std::endl;
}

inline void ServerConfig::printMapContent(const std::map<int, std::string> &pair_map, std::string name, std::string color) const
{
	std::cout << color << "-----" << name << "-----" << ": " << pair_map.size() << std::endl;
	for (std::map<int, std::string>::const_iterator iter = pair_map.begin(); iter != pair_map.end(); iter++)
	{
		std::cout << "$" << iter->first << ", " << iter->second << "$" << std::endl;
	}
	std::cout << WHI << std::endl;
}

inline void ServerConfig::printMapContent(const std::map<std::string, LocationConfig> &pair_map, std::string name, std::string color) const
{
	std::cout << color << "-----" << name << ": " << pair_map.size() << "-----" << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator iter = pair_map.begin(); iter != pair_map.end(); iter++)
	{
		std::cout << "(Location) " << iter->first << std::endl;
		std::cout << WHI;
		iter->second.printLocationConfingContent(GRN);
	}	
}

#endif