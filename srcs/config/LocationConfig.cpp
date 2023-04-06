#include "../../include/LocationConfig.hpp"
#include <iostream> // TODO remove

LocationConfig::LocationConfig(void)
    : root(""), index(""), redirect(""), autoindex(false)
{
    std::map<MethodType, bool>		_allow_method;

    _allow_method[GET] = true;
    _allow_method[POST] = true;
    _allow_method[DELETE] = true;
    this->allow_method = _allow_method;
}

LocationConfig::~LocationConfig()
{}
