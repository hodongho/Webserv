#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP
# include "configStructure.hpp"

class ConfigParser
{
    private:
        std::vector<ServerConfig>	webserv_config;
    public:
        void    parseConfig(const char *file);
        void    parseServer();
        void    parseServerBlock(size_t );
};

#endif