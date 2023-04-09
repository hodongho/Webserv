#include "LocationConfig.hpp"
#include <iostream> // TODO remove

LocationConfig::LocationConfig(void)
    : location_path(""), root(""), index("index.html"), redirect(""), autoindex(false)
{
    this->allow_method[GET] = true;
    this->allow_method[POST] = true;
    this->allow_method[DELETE] = true;
}

LocationConfig::~LocationConfig()
{}

void LocationConfig::setRoot(const std::string &_root)
{
    this->root = _root;
}

void LocationConfig::setIndex(const std::string &_index)
{
    this->index = _index;
}

void LocationConfig::setRedirect(const std::string &_redirect)
{
    this->redirect = _redirect;
}

void LocationConfig::setAutoindex(const bool &_autoindex)
{
    this->autoindex = _autoindex;
}

void LocationConfig::setAllowMethod(const std::map<MethodType, bool> &_allow_method)
{
    this->allow_method = _allow_method;
}

void LocationConfig::setLocationPath(const std::string &_location_path)
{
    this->location_path = _location_path;
}

const std::string &LocationConfig::getLocationPath(void) const
{
    // TODO: 여기에 return 문을 삽입합니다.
    return (this->location_path);
}

const std::string& LocationConfig::getRoot(void) const
{
    return (this->root);
}

const std::string& LocationConfig::getIndex(void) const
{
    return (this->index);
}

const std::string& LocationConfig::getRedirect(void) const
{
    return (this->redirect);
}

const bool& LocationConfig::getAutoindex(void) const
{
    return (this->autoindex);
}

const std::map<MethodType, bool>& LocationConfig::getAllowMethod(void) const
{
    return (this->allow_method);
}

void LocationConfig::printLocationConfingContent(std::string color) const
{
    std::cout << color << "--------------------(LOCATION CONTENT_DATA)-------------------------" << std::endl;
    printContent(this->getLocationPath(), "this->getLocationPath()", GRN);
    printContent(this->getRoot(), "this->getRoot()", GRN);
    printContent(this->getIndex(), "this->getIndex()", GRN);
    printContent(this->getRedirect(), "this->getRedirect()", GRN);
    printContent(this->getAutoindex(), "this->getAutoindexs()", GRN);
    std::cout << "--------------------(LOCATION MAP_DATA)-------------------------" << WHI << std::endl;
    printMapContent(this->getAllowMethod(), "this->getAllowMethod()", GRN);
}

