#ifndef ConfigInfo_HPP
# define ConfigInfo_HPP
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "manageStdout.hpp"
# include <vector>
enum PathState
{
	PATH_NOTFOUND,
	PATH_VALID,
	PATH_AUTOINDEX,
	PATH_CGI,
	PATH_REDIRECT
};

enum	FileExistanceType
{
	NO_EXIST,
	EXIST_FILE,
	EXIST_DIRECTORY
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
	bool    checkCgiPassConfigField(std::string cgi_pass);
	bool    checkNessaryOrUniqueField(std::map<std::string, ConfigInfo::ValidateFieldInfo> validate_server_field_map);

	// config util
	std::string	removeAfterSemicolon(const std::string& origin_value);
	bool		findOpenCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
							const std::vector<std::string>::iterator& end_iter,
							bool& server_flag,
							bool& location_flag,
							bool& error_flag);
	bool		findCloseCurlyBracket(const std::vector<std::string>::iterator& begin_iter,
							const std::vector<std::string>::iterator& end_iter,
							bool& error_flag);

	bool    parseErrorPageConfigField(std::string error_page, std::map<int, std::string>& error_page_map);
	bool    parseCgiPassConfigField(std::string cgi_pass, std::string& cgi_extension, std::string& cgi_program_path);

	// get validate info                                            
	std::map<std::string, ValidateFieldInfo>	getValidateServerFieldMap(void);
	std::map<std::string, ValidateFieldInfo>	getValidateLocationFieldMap(void);

	// parse
	bool    parse(const std::string& file_content);
	bool    parseServerBlock(std::vector<std::string> server_block_vec);
	bool    parseLocationBlock(std::vector<std::string>::iterator& src_begin_iter,
								const std::vector<std::string>::iterator& src_end_iter,
								ServerConfig& server_config);
	void	setRootToLocationConfig(ServerConfig& server_config);

	//print
	void	    printContent(const std::string& str, const std::string& str_name, const std::string& color);
	void	    printVector(std::vector<std::string>& word_list, const std::string& str_name, const std::string& color);
	

	//for convUriToPath test
	// bool			testConvUriToPath(const std::string& URI, const unsigned short& port);
	bool			checkFilePathofURI(const std::string& URI, const std::string &truncated_URI);
	void			printFilePathofURI(const std::string &URI, const std::string &truncated_URI);
	bool			checkMatchFilePathToLocationConfig(const std::string& startline_of_URI, const std::string& file_path_request_URI, const LocationConfig& location_config, bool is_matched_location_config);

	// convUriToPath utils
	// bool					matchFilePathToLocationConfig(const std::string& file_path_request_URI, const unsigned short& port, LocationConfig& location_config);
	bool					checkRedirect(const std::string& file_path_request_URI, const unsigned short &port, std::string& file_path);
	std::string				getFilePathFromRequestURI(const std::string& startline_of_URI);
	std::string				getAbsFilePath(const std::string &file_path_request_URI, const unsigned short &port);
	enum FileExistanceType	getFileExistanceType(const std::string& file_path);
	bool					isCgiRequest(const std::string& file_path);
	bool					isLastPartOfStr(const std::string& origin_str, const std::string& find_str);
	void					removeLastSlashSignOfStr(std::string& str);

public:
	ConfigInfo(void);
	virtual ~ConfigInfo();
	void    parseConfig(const char *file);
	void    printWebservConfig(void);

	// std::vector<ServerConfig>	webserv_config;
	const std::vector<ServerConfig>& getWebservConfig(void) const;
	bool							getServerConfig(const unsigned short &port, ServerConfig& server_config) const;
	bool							getLocationConfig(const unsigned short &port, const std::string &find_path, LocationConfig& location_path) const;

	// for using config info 
	enum PathState		convUriToPath(const std::string& startline_of_URI, const unsigned short& port, std::string& file_path);
	bool        isAllowedMethod(const std::string& URI, const unsigned short& port, const enum MethodType& method);
	int		    getErrorPage(StatusCode stat_code, const unsigned short& port, std::string& err_file_path);
	size_t	    getMaxBodySize(const unsigned short& port);
	std::string getCgiProgramPath(const std::string& cgi_extension, const unsigned short& port);
};

#endif