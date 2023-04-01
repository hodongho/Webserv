#include <iostream>
#include "../include/ConfigParser.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "please input config file for argument like ./webvserv server.conf" << std::endl;
        return (1);
    }
    ConfigParser serv_config;

    serv_config.parseConfig(argv[1]);
    // ConfigParser serv_config(argv[1]);
    // ConfigParser(argv[1]);
    return (0);
}