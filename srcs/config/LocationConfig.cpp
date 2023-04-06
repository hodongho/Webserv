#include "../../include/LocationConfig.hpp"
#include <iostream> // TODO remove

// LocationConfig::LocationConfig(root ,index)
// TODO
// 서버 파싱 이후에 location block의 root, index가 ""이면 server block의 root, index로 덮여



LocationConfig::LocationConfig(void)
    : root(""), index(""), redirect(""), autoindex(false)
{
    // std::map<MethodType, bool>		_allow_method;

    this->allow_method[GET] = true;
    this->allow_method[POST] = true;
    this->allow_method[DELETE] = true;
    // this->allow_method = _allow_method;
    // std::cout << "this->allow_method.size() : " << this->allow_method.size() << std::endl;
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


void LocationConfig::printLocationConfingContent(void) const
{
    std::cout << "--------------------(LOCATION CONTENT_DATA)-------------------------" << std::endl;
    printContent(this->getRoot(), "this->getRoot()", GRN);
    printContent(this->getIndex(), "this->getIndex()", GRN);
    printContent(this->getRedirect(), "this->getRedirect()", GRN);
    printContent(this->getAutoindex(), "this->getAutoindexs()", GRN);
    std::cout << "--------------------(LOCATION MAP_DATA)-------------------------" << std::endl;
    printMapContent(this->getAllowMethod(), "this->getAllowMethod()", GRN);
}
