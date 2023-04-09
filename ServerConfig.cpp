#include "ServerConfig.hpp"
#include <map>
#include <string>
#include <iostream> // TODO remove
#include "ServerConfig.hpp"


ServerConfig::ServerConfig(void)
    : server_name(""), host(""), port(0), index("index.html"), client_max_body_size(0)
{
    // std::map<int, std::string>              _default_error_page;

    // _default_error_page[403] = "/";
    // _default_error_page[404] = "/";
    // _default_error_page[500] = "/";
    // this->default_error_page = _default_error_page;
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

void ServerConfig::setPort(const unsigned short & _port)
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

// void ServerConfig::setDefaultErrorPage(const std::map<int, std::string> &_default_error_page)
// {
//     this->default_error_page = _default_error_page;
// }

void ServerConfig::setErrorPage(const std::map<int, std::string> & _error_page)
{
    this->error_page = _error_page;
}

void ServerConfig::addErrorPageElement(const int& error_status_code, const std::string& erro_page_uri)
{
    this->error_page[error_status_code] = erro_page_uri;
}

void ServerConfig::setLocations(const std::map<std::string, LocationConfig>& _locations)
{
    this->locations = _locations;
}

void ServerConfig::addLocationElement(const std::string &location_path, const LocationConfig &location_config)
{
    this->locations[location_path] = location_config;
}
const std::string &ServerConfig::getServerName(void) const
{
    return (this->server_name);
}

const std::string& ServerConfig::getHost(void) const
{
    return (this->host);
}

const unsigned short& ServerConfig::getPort(void) const
{
    return (this->port);
}

const std::string& ServerConfig::getRoot(void) const
{
    return (this->root);
}

const std::string& ServerConfig::getIndex(void) const
{
    return (this->index);
}

const size_t& ServerConfig::getClientMaxBodySize(void) const
{
    return (this->client_max_body_size);
}

// const std::map<int, std::string>& ServerConfig::getDefaultErrorPage(void) const
// {
//     return (this->default_error_page);
// }

const std::map<int, std::string>& ServerConfig::getErrorPage(void) const
{
    return (this->error_page);
}

const std::map<std::string, LocationConfig>& ServerConfig::getLocations(void) const
{
    return (this->locations);
}

bool ServerConfig::getLocationBlock(const std::string &find_path, LocationConfig& location_config) const
{
    const std::map<std::string, LocationConfig>& location_config_map = this->getLocations();

    std::map<std::string, LocationConfig>::const_iterator location_iter = location_config_map.begin();
    for (;location_iter != location_config_map.end() ;location_iter++)
    {
        std::string		location_path;

        location_path = location_iter->first;
        if (location_path == find_path)
        {
            location_config = location_iter->second;
            // location_config.printLocationConfingContent(BRW);
            return (true);
        }
    }
    return (false);
}

void ServerConfig::printServerConfingContent(void) const
{
    std::cout << "--------------------(CONTENT_DATA)-------------------------" << std::endl;
    this->printContent(this->getServerName(), "this->getServerName()",GRN);
    this->printContent(this->getHost(), "this->getHost()",GRN);
    this->printContent(this->getPort(), "this->getPort()",GRN);
    this->printContent(this->getRoot(), "this->getRoot()",GRN);
    this->printContent(this->getIndex(), "this->getIndex()",GRN);
    this->printContent(this->getClientMaxBodySize(), "this->getClientMaxBodySize()",GRN);
    std::cout << "--------------------(MAP_DATA)-------------------------" << std::endl;
    // this->printMapContent(this->default_error_page, "this->getDefaultErrorPage()", GRN);
    this->printMapContent(this->error_page, "this->getErrorPage()", GRN);
    this->printMapContent(this->locations, "this->getLocations()", GRN);
}

