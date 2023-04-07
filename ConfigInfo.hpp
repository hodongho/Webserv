#ifndef ConfigInfo_HPP
# define ConfigInfo_HPP
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "manageStdout.hpp"
# include <vector>
/*
    - 가져다 쓸떄는 config 정보가 담겨있어서 쓸텐데 config parser라는 이름이 맞을 것인가?
    - 그렇다고 ConfigInfo라고 하기에는 주요 동작이 parse이다.
*/

enum PathState
{
   PATH_NOTFOUND,
   PATH_VALID,
   PATH_AUTOINDEX,
   PATH_CGI // 
};


class ConfigInfo
{
    private:
        enum	VaildateFieldType
		{
			NESSARY_UNIQUE,
			NESSARY_MULTI,
			OPTION_UNIQUE,
			OPTION_MULTI
		};

        /*
        struct ValidateFieldInfo
        {
            size_t              count;
            VaildateFieldType   validate_field_tnfo;

            ValidateFieldInfo(void)
        };
        */
        
        class  ValidateFieldInfo
        {
            private:
                size_t              count;
                VaildateFieldType   validate_field_type;

            public:
                ValidateFieldInfo(void);

                ValidateFieldInfo&          operator++(int);

                void                        setValidateFieldType(const VaildateFieldType& _validate_field_type);
                void                        setCount(const size_t& _count);
                const VaildateFieldType&    getValidateFieldType(void) const;
                const size_t&               getCount(void) const;
        };

    private:
        const std::string           whitespace;
        std::vector<ServerConfig>	server_config_vector;
        // std::map<std::string, ServerConfig>	webserv_config;

        bool	    checkFileNameExtension(const char *file_name_parms);
        std::string readFile(std::string file_name);
        bool	    validateConfigFile(const std::string& file_content);
        bool	    checkCurlyBracketPair(const std::string& file_content);
        bool	    checkWhole(const std::string& file_content);
        bool        validateServerBlock(std::vector<std::string> server_block_vec);
        bool        validateLocationBlock(std::vector<std::string>::iterator& src_begin_iter,
                                        const std::vector<std::string>::iterator& src_end_iter);

        // find {} block
        bool	findServerBlock(std::vector<std::string>::iterator& src_begin_iter, \
                                                    const std::vector<std::string>::iterator& src_end_iter, \
													std::vector<std::string>::iterator& begin_iter, \
													std::vector<std::string>::iterator& end_iter);
        bool	findLocationBlock(std::vector<std::string>::iterator& src_begin_iter, \
                                                    const std::vector<std::string>::iterator& src_end_iter, \
													std::vector<std::string>::iterator& begin_iter, \
													std::vector<std::string>::iterator& end_iter);    

        // check function for validate
        bool    checkCommonConfigLineForm(std::vector<std::string> word_list);
        // bool    checkCommonConfigLineForm(std::string config_line);
        bool	checkDuplicateConfigField(const ValidateFieldInfo& validate_field_info);
        bool    checkHostConfigField(std::string field_value);
        bool    checkIpClass(const std::string& ip_class);
        bool    checkPortConfigField(std::string port);
        bool    checkClientMaxBodySizeConfigField(std::string port);
        bool    checkErrorPageConfigField(std::string error_page);
        bool    checkErrorPageStatusCodde(const std::string& status_code);
        bool    checkAllowMethodConfigField(std::string allow_method);

        bool    checkNessaryOrUniqueField(std::map<std::string, ConfigInfo::ValidateFieldInfo> validate_server_field_map);

        bool    parseErrorPageConfigField(std::string error_page, std::map<int, std::string>& error_page_map);

        // get validate info                                            
        std::map<std::string, ValidateFieldInfo>	getValidateServerFieldMap(void);
        std::map<std::string, ValidateFieldInfo>	getValidateLocationFieldMap(void);

        // parse
        bool    parse(const std::string& file_content);
        bool    parseServerBlock(std::vector<std::string> server_block_vec);
        bool    parseLocationBlock(std::vector<std::string>::iterator& src_begin_iter,
                                    const std::vector<std::string>::iterator& src_end_iter,
                                    ServerConfig& server_config);
        

        //util
        void	    printContent(const std::string& str, const std::string& str_name, const std::string& color);
        void	    printVector(std::vector<std::string>& word_list, const std::string& str_name, const std::string& color);
        
    public:
        ConfigInfo(void);
        virtual ~ConfigInfo();
        void    parseConfig(const char *file);

        // std::vector<ServerConfig>	webserv_config;
        const std::vector<ServerConfig> getWebservConfig(void) const;
        void    printWebservConfig(void);

        // for using config info 
        enum PathState convUriToPath(const std::string& URI, std::string& file_path);

};

#endif