#ifndef ConfigInfo_HPP
# define ConfigInfo_HPP
# include "configStructure.hpp"
# include "manageStdout.hpp"

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


class ConfigInfo
{
    private:
        // Key를 기준으로 함
        // Key가 필수인가 or 없어도 되는가, Key가 유일한가 or 여러개 있어도 되는가
        // value가 여러개 있는지 여부를 의미하지는 않음.
        // e.g) error_page 403 404 400 500 /50x.html
        enum	VaildateFiledType
		{
			NESSARY_UNIQUE,
			NESSARY_MULTI,
			OPTION_UNIQUE,
			OPTION_MULTI,
		};

        /*

        */
        class  ValidateFieldInfo
        {
            private:
                size_t              count;
                VaildateFiledType   validate_filed_type;

            public:
                ValidateFieldInfo(void);
                // ValidateFieldInfo(VaildateFiledType _validate_filed_type);

                ValidateFieldInfo&          operator++(int);

                void                        setValidateFiledType(const VaildateFiledType& _validate_filed_type);
                void                        setCount(const size_t& _count);
                const VaildateFiledType&    getValidateFiledType(void) const;
                const size_t&               getCount(void) const;
        };

    private:
        const std::string           whitespace;
        std::vector<ServerConfig>	webserv_config;

        bool	    checkFileNameExtension(const char *file_name_parms);
        std::string readFile(std::string file_name);
        bool	    validateConfigFile(const std::string& file_content);
        bool	    checkCurlyBracketPair(const std::string& file_content);
        bool	    checkWhole(const std::string& file_content);
        bool        validateServerBlock(std::vector<std::string> server_block_vec);
        // bool        validateServerBlock(std::vector<std::string>::iterator& begin_iter,
        //                                 const std::vector<std::string>::iterator& end_iter);
        bool        validateLocationBlock(std::vector<std::string>::iterator& begin_iter,
                                        const std::vector<std::string>::iterator& end_iter);

        // find block
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

        // get validate info                                            
        std::map<std::string, ValidateFieldInfo>	getValidateServerFiledMap(void);
        std::map<std::string, ValidateFieldInfo>	getValidateLocationFiledMap(void);
                                        
        void    parseServer();
        void    parseServerBlock();
        void    parseLocationBlock();

        //util
        void	    printContent(const std::string& str, const std::string& str_name, const std::string& color);
        void	    printVector(std::vector<std::string>& word_list, const std::string& str_name, const std::string& color);
        
    public:
        ConfigInfo(void);
        void    parseConfig(const char *file);

};

#endif