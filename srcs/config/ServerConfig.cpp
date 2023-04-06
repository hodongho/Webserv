#include "../../include/ServerConfig.hpp"
#include <map>
#include <string>
#include <iostream> // TODO remove

ServerConfig::ServerConfig(void)
    : server_name("")
{
    // config에는 없지만 error_page에 없는 status code에 대한 default error page 경로를 담고 있음, 항상 값이 있음
    std::map<int, std::string>  _default_error_page;

    _default_error_page[403] = "";
    _default_error_page[404] = "";
    _default_error_page[500] = "";
    // _default_error_pag[400] = "";
    this->default_error_page = _default_error_page;
    // std::cout << "this->locations.size() : " << this->locations.size() << std::endl;
    // std::cout << "this->default_error_page.size() : " << this->default_error_page.size() << std::endl;
}

ServerConfig::~ServerConfig(void)
{}
