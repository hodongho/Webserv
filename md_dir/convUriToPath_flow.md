convUriToPath flow 

convUriToPath(request_URI)
{
    enum PathState  path_type;
    bool            redirect_flag;
    bool            is_matched_location_config;
    <!-- 1. URI에서 file path부분만 가져오기(host,port제거) -->
    file_path_request_URI = getFilePathFromRequestURI(request_URI);
    if (file_path_request_URI == "")
        return (PATH_NOTFOUND);
    <!-- 2. Config 파일내에 Location으로 설정되어있는지 확인. -->
    <!-- LocationConfig& -->
    <!-- enum MatchedType matchFilePathToLocationConfig(); -->
    LocationConfig location_config;
    is_matched_location_config = matchFilePathToLocationConfig(file_path_request_URI, & location_config);
    checkRedirect
    <!-- 3. 서버에서 해당 URI file의 절대경로를 가져온다. -->
    <!-- 내부적으로config location에서 찾거나 server block root와 조합하여 절대경로를 찾는다. --> 
    root + location + index
    root + index
    root + file_path_request_URI
    // locatoin block내부에 redirect가 있으면 1순위로 찾아야한다.
    abs_file_path_of_server = getAbsFilePath(file_path, matched_type, & location_config, & redirect_flag);
    redirect_flag = 
    location,



    getAbsFilePath(file_path, port);
    getFileExistanceType(abs_file_path_of_server);


    <!-- file_path = getAbsFilePath(); 로 대체가능-->
    <!-- 3.파일이 존재하는지 유형을 가져온다.-->
    <!-- enum FileExistanceType getFileExistanceType(cont std::string& abs_file_path_of_server) -->
    <!-- redriect 고려필요 -->
    file_exist_type = getFileExistanceType(cont std::string& abs_file_path_of_server);
    swich (file_exist_type)
    {
 
        case EXIST_FILE:
        {
            <!-- cgi인지 확인 -->
            file_path = abs_file_path_of_server;
            <!-- if (redirect_flag)
                return (PATH_REDIRECT); -->
            else if (isCgiRequest(file_path, matched_type, location_config))
                return (PATH_CGI);
            return (PATH_VALID);
            <!-- break ; -->
        }
        case EXIST_DIRECTORY:
        {
            <!-- EXIST_DIRECTORY-1. 해당 디렉토리 경로 index에 걸리는 파일이 존재하는지 -->
            <!-- abs_file_path_of_server + index -->
            <!-- getFileExistanceType() 재활용 -->
            <!-- 존재한다면 file_path = abs_file_path_of_server + index; -->
            <!-- EXIST_DIRECTORY-2. index 존재하지 않는다면
                autoindex 인지 -->
            file_path = abs_file_path_of_server;
            if (location_config.getAutoindex())
                return ();
            return(PATH_NOTFOUND);
            autoindex
                
            break ;
        }
        case NO_EXIST:
        {
            return(PATH_NOTFOUND);
            break ;
        }
        default :
        {

            return(PATH_NOTFOUND);
        }
    }
    return(PATH_NOTFOUND);
}

절대경로를 가져오기 위해 필요한것 

- URI에서의 경로
- port정보
    location을 찾는데 쓰임

- 경우의 수
- location을 찾은 경우
    - redirect 가 있는지
        있다면 연결되는 경로로 접근하여 다시 getAbsFilePath()를 시도한다.
        redirect에 적혀있는 path와 port는 getAbsFilePath()에 기존 파라미터로 사용한다.
    - root + location_path
        - dir인지
            - index
                1. root + location_path + index return 
            - autoindex
                2. root + location_path
      "root + location_path"         
        - file인지
            해당 경로를 return

- config location을 못 찾은 경우


1. root + location_path
    - dir
    - file
2. dir 
    index 
    autoindex 