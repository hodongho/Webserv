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

void ServerConfig::setServerName(const std::string & _server_name)
{
    this->server_name = _server_name;
}

void ServerConfig::setHost(const std::string & _host)
{
    this->host = _host;
}

void ServerConfig::setPort(const uint16_t & _port)
{
    this->port = _port;
}

void ServerConfig::setRoot(const std::string & _root)
{
    this->root = _root;
}

void ServerConfig::setIndex(const std::string & _index)
{
    this->index = _index;
}

void ServerConfig::setClientMaxBodySize(const size_t &_client_max_body_size)
{
    this->client_max_body_size = _client_max_body_size;
}

void ServerConfig::setDefaultErrorPage(const std::map<int, std::string> &_default_error_page)
{
    this->default_error_page = _default_error_page;
}

void ServerConfig::setErrorPage(const std::map<int, std::string> & _error_page)
{
    this->error_page = _error_page;
}

void ServerConfig::setLocations(const std::map<std::string, LocationConfig> & _locations)
{
    this->locations = _locations;
}

const std::string ServerConfig::getServerName(void) const
{
    return (this->server_name);
}

const std::string ServerConfig::getHost(void) const
{
    return (this->host);
}

const uint16_t ServerConfig::getPort(void) const
{
    return (this->port);
}

const std::string ServerConfig::getRoot(void) const
{
    return (this->root);
}

const std::string ServerConfig::getIndex(void) const
{
    return (this->index);
}

const size_t ServerConfig::getClientMaxBodySize(void) const
{
    return (this->client_max_body_size);
}

const std::map<int, std::string> ServerConfig::getDefaultErrorPage(void) const
{
    return (this->default_error_page);
}

const std::map<int, std::string> ServerConfig::getErrorPage(void) const
{
    return (this->error_page);
}

const std::map<std::string, LocationConfig> ServerConfig::getLocations(void) const
{
    return (this->locations);
}
