# 문제이해
- 파라미터로 넘겨받는 request URI를 

    .php
    .py

    google.com:80/test/t1234/seoul.php
    google.com:80/test/t1234/seoul.php

    => "/test/t1234/seoul.php"
    실제 서버에서 위치하고 있는 절대 경로로 넘겨준다.
    - 존재여부에 따라서 아래와 같이 달라진다.
        PATH_NOTFOUND,
            - 실제 서버내에 존재하지 않음
                1. config내에 location에 일치하는것이 없다.
                    - server block에 root와 request_URI_file_path조합으로 실제 서버에 파일이나 디렉토리가 존재하는지 확인한다.
                    - 해당 경로가 존재
                        - 파일인 경우
                            파일 경로를 file_path&에 저장
                        - 디렉토리인 경우 stat에 -1로 나옴 추가적으로 errno를 통해 파일인지 확인 필요
                            - index에 설정한 파일이 있는지 확인
                                있다면 해당 파일의 서버내의 절대경로를 file_path&에 저장 및 PATH_VALID로 return
                            -설정파일이 없다면
                                - autoindex on인지 확인
                                    - on이라면 해당 디렉터리의 서버내의 절대경로를 file_path&에 저장 및 PATH_AUTOINDEX return
                                - 그렇지 않다면
                                    - PATH_NOTFOUND

                2. config location에 일치하지만 
                    location내부 root + request_URI_file_path조합으로 존재하는 파일이 없는 경우
		PATH_VALID
            - 일치하는 경로가 있음
                일치하는 경우
                1. location에 매칭됨
                    - location root + request_URI_file_path
                    a. file
                        isCGIRequest()
                        .php
                        .py
                    b. directory
                        - index에 매칭됨
                        - autoindex 매칭
                2. 절대경로로 매칭
                    - server root + request_URI_file_path
                    a. file
                    b. directory
                        - index에 매칭됨
                        - autoindex는 server block에서는 하지않음
                           location block에만 있는 옵션으로 해놓았기에 수정해야지만 가능
                           - 시간 여유가 된다면 업데이트.
                          
		PATH_AUTOINDEX
            - config location으로 설정되어있어야하며
            - 찾고자하는 URI가 디렉터리로 있어야하며
            - 해당 config location block에 autoindex로 설정되어 있어야한다.
		PATH_CGI
            - 찾고자하는 것이 CGI인지 알려줌
            어떻게 CGI인지 알게되는가?
            확장자 명이 들어가는가?
            ".php" or ".py" isCGIRequest() 함수로 확인한다. 모듈화 진행.
            - CGI일때 서버내에서 어떻게 찾는가?
                - location 밑에 있는게 나은가?
                    server block에서만 놔둔다.
                - cgi-pass [.extension]
                - cgi-pass [.extension] [cgi program path]
            server{
                cgi-pass ".py" py-cgi-program-path; // POST
                cgi-pass ".php" php-cgi-program-path; // POST

            }

            server{
                cgi_pass .py cgi-program-path;
                cgi_pass .php cgi-program-path;

                location 
                {

                    
                }
                
            }
            /*
                cgi
            */




# 문제재정의
Request message URI가 서버에 존재하는지, 존재한다면 어떤 형태인지, 해당 자원의 서버 내에서의 절대경로를 file_path&에 담아서 넘겨준다.


# 문제해결방법
## convUriToPath() 흐름
- host, port를 제외한 file path만 따로 담는다.
    - '/'를 기준으로 잘라서  /부터 끝까지 substr()함수를 이용해서 저장한다.
        <!-- const std::string& getFilePathFromRequestURI(const std::string& request_URI); -->
        - 함수 타고들어갈 필요 없다면 안만듦
        - 재활용성이 있는가?
        - 코드 읽기에 불편해지지는 않는가?

- file_path_of_URI
    - config location에 설정된것이 있는지 확인한다.
    - 설정되어있는 경우 (config location)
        root는 location block에 root가 있다면 
        root + file_path_of_URI가 실제 존재하는지 확인힌다.
        <!-- bool    isExistPath(cont std::string& abs_file_path); -->
        <!-- File Path Existance Type  -->
        enum FileExistanceType getFileExistanceType(cont std::string& abs_file_path);
        enum FileExistanceType 
        {
            EXIST_FILE,
            EXIST_DIRECTORY,
            NO_EXIST,
        };
        
        
        - 실제 존재한다면 파일인가 디렉토리인가
        - 존재하는경우
            - 파일
                해당 경로 file_path&에 저장
                return (PATH_VALID);
            - 디렉터리
                - location 블럭에 index가 있는가?
                    있다면
        - 존재 안하는 경우
            return (PATH_NOTFOUND);
    - 설정되어있는 않.은 경우 (config location)
        root + file_path_of_URI를 이용해서 존재하는지 확인

## 주의
- root + location을 할때 root맨뒤에 '/'이 있다면 제거한다.
- index항목에 /가 있으면 어떻게 되는가?
    - index /index.html을 그대로 가져다 붙이기 때문에 404발생

- redirect loop도는 경우는 없는가?
    redirect일때 
    


# 검증



# 실행



# 회고 



# 