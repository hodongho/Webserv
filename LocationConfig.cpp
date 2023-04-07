#include "LocationConfig.hpp"
#include <iostream> // TODO remove

LocationConfig::LocationConfig(void)
    : root(""), index("index.html"), redirect(""), autoindex(false)
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

const std::string LocationConfig::getRoot(void) const
{
    return (this->root);
}

const std::string LocationConfig::getIndex(void) const
{
    return (this->index);
}

const std::string LocationConfig::getRedirect(void) const
{
    return (this->redirect);
}

bool LocationConfig::getAutoindex(void) const
{
    return (this->autoindex);
}

std::map<MethodType, bool> LocationConfig::getAllowMethod(void) const
{
    return (this->allow_method);
}


void LocationConfig::printLocationConfingContent(std::string color) const
{
    std::cout << color << "--------------------(LOCATION CONTENT_DATA)-------------------------" << std::endl;
    printContent(this->getRoot(), "this->getRoot()", GRN);
    printContent(this->getIndex(), "this->getIndex()", GRN);
    printContent(this->getRedirect(), "this->getRedirect()", GRN);
    printContent(this->getAutoindex(), "this->getAutoindexs()", GRN);
    std::cout << "--------------------(LOCATION MAP_DATA)-------------------------" << WHI << std::endl;
    printMapContent(this->getAllowMethod(), "this->getAllowMethod()", GRN);
}

