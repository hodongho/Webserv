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

        bool	    checkFileNameExtension(const char *file_name_parms);
        std::string readFile(std::string file_name);
        bool	    validateConfigFile(const std::string& file_content);
        bool	    checkCurlyBrackeyPair(const std::string& file_content);
        bool	    checkWhole(const std::string& file_content);
        void	    printContent(const std::string& str, const std::string& str_name);

        void    parseServer();
        void    parseServerBlock();
        void    parseLocationBlock();

    public:
        void    parseConfig(const char *file);

};

#endif