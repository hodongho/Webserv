#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP
# include "configStructure.hpp"


/*
    - 가져다 쓸떄는 config 정보가 담겨있어서 쓸텐데 config parser라는 이름이 맞을 것인가?
    - 그렇다고 ConfigInfo라고 하기에는 주요 동작이 parse이다.
*/

enum configValidationInfo
{
    NESSARY_UNIQUE_FIELD,
    NESSARY_MULTIPLE_FIELD,
    OPTIONAL_UNIQUE_FIELD,
};

class ConfigParser
{
    private:
        std::vector<ServerConfig>	webserv_config;
        const std::string           whitespace = " \t\n\v\f\r";
    public:
        bool    checkValidation();
        bool    checkPairOfCurlyBracket();
        // bool    checkEachLine();// parsing하면서 할지 고려
        
        void    parseConfig(const char *file);
        void    parseServer();
        void    parseServerBlock();
        void    parseLocationBlock();
};

#endif