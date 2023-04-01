# config parsing 흐름

1. ./webserv [config.conf]로 들어온 파일을 연다.
2. 파일을 읽는다
    - html파일 읽었을때 썼던 방식 사용
3. validation check
    - 파일형식 확인 .conf가 맞는가?
    - check pair of curly bracket
        - {}쌍이 맞는가?
            - 파일 전체를 읽는다.
    - check each line
        - 각 줄을 읽는다.
        - trim으로 whitespace를 제거한다.
    - split으로 쪼개기(delimiter는 whitespace)
        - vector<std::string> word_list 맨 왼쪽에 있는 것이 key가 되는 것
        - #이 나오면 주석이므로 건너뛴다.
        - server를 찾는다.
            - server가 나오기 전에 다른 것이 있다면 error 처리
        - server가 나왔으면 checkServerBlock 로 넘어간다.
        - checkServerBlock()
            - 유효항목이 아닌것이 있다면 error처리
                - 유효항목도 파일로 가지고 있다면 좋겠지만 처리가 애매하다
                    -입력은 config밖에 못받으며 위치를 기반으로 하면 해당 위치에 꼭 있어야한다.
                    품이 든다.
            - 유효항목은 define한다.혹은 const std::string or const char *로 가지고 있는다.
            - 유효항목 list
                - 꼭 있어야하는 것
                    - 유일해야하는 key
                        - host
                        - port
                        - index
                        - root
                        - client_max_body_size
                        - location
                            - location path
                            - allow_method
                            - autoindex
                            
                    - 여러개 들어와도 되는 Key값들
                        - error_page
                - 유효항목 확인은 std::map<std::string, int>를 통해확인한다.
                    - int자리에는 enum으로 정의한 값으로 유일한지, 없어도 되는지, 여러개가 들어와도 되는지 정의한다.
                - 없어도 되는 것
                    - server_name
                    - redirection(추가확인 필요)
                        location 내부에 return으로 처리됨!!
            

4. parsing
