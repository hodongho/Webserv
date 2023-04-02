# 문제이해
webserver config file을 읽고 룰에 맞게 파싱한다.

룰에 맞게란 무엇인가?
룰에 맞지 않으면 어떻게 에러를 뱉을것인가?
error만 띄워주고 실행되지 않게 한다.
    - error를 구체적으로 줄지는 추후 여유가 되면 추가
index, root의 경우 server바로 밑에도 넣을 수 있다.
하지만 location block내부에 정의하면 location 내부에 있는 index,root로 대체된다.
index의 경우 nginx에서는 여러개 들어와도 처리해주지만 
    우선은 한개만 받아준다!

    - 다 받아준다면 vector를 쓴다.
        - 앞에 index 파일 중에 있는 것으로 선택된다.
            - index  index.htm index.html index_true.html index.htmll;
            - 2,3번이 실제 있는 파일일때 2번이 걸린다.
- nginx 같은 값이 여러개 있다면 list로 추가된다.
    - e.g)
        listen, index, root
    - 난 중복되면 ERROR 처리한다.

- 유일해야할 "Key" 값들
    - host
    - port
    - server_name
    - index (같은 영역 내에서)
    - root (같은 영역 내에서)
    - client_max_body_size
    - location_path (in location)
    - allow_method (in location)
    - autoindex (in location)
    - redirection (in location)
- 여러개 들어와도 되는 "Key" 값들
    - error page
        e.g)
            error_page 400 404 40x.html;
            error_page 502 501 50x.html;
        
- 여러개 들어왔을때 list 혹은 map으로 저장되어야할 것
    - location
    - error page

# 문제재정의


# 문제해결방법
1. config 파일을 읽는다
2. config 파일의 유효성 검사를 한다.
    - 파일명 형식이 맞는지 확인 .conf
    - 파일 내부를 읽어서 {}짝이 맞는지
        sever block, location block 확인 필요
        {}로 되어 있는가?
            - pair는 확인했으니까
            - 각 라인을 확인할때 룰에 맞지 않을때 걸르는 것으로 확인 될것이라 생각든다.
    - 각 line별로 확인
        - validation_list 필요
            - server,location 필수항목이 다르므로 그리고 현재 확인하는 영역이 server인지 location인지 모르므로 
                !!!영역별로 확인이 필요하다.!!!
                - 영역을 어떻게 알 수 있는가?
                    - { block으로 확인한다.}
                    - block을 확인하고서 
                        block의 시작과 끝을 파악해야한다.
                        시작과 끝을 기준으로 끊어서 validateServerBlock()을 써야한다.
                        같은 방식으로 server block에서
                        location block의 시작과 끝을 파악하고
                        validateLocationBlock()을 사용한다.
                        ft_split()을 이용해서 "\n"기준으로 나누고 그 결과를 vector에 담는다.
                        vector를 순회하면서 server block인지, location block인지 확인한다.
                        server, location block에서 한줄씩 읽으면서 validate한지 확인힌다.
                        sever만의 유효필드인지
                        location만의 유효필드인지
                        또한 값의 중복은 없는지
                        여러개의 값이 들어올 수 있는 필드인지 확인힌다.
                    - server {
                        location
                        }
                    - block의 시작과 끝 위치를 찾는다.
                    - {}
                - 혹은 한줄씩 읽으면서 "{"가 나오면 시작이며 }나오면 끝으로 인식한다.
                    어떤 validation으로 확인해야할지 mode를 전환한다.
            - server
                - host, port, index, root, client_max_body_size, locatoin, error_page(여러개 가능), server_name(없어됨)
            - location
                - location_path, allow_method, root, index
            - 유일해야할 "Key" 값들

    유효성 검사 처리 과정
    -ㄱ 읽은 config 파일을 "\n"기준으로 split하여 vector<std::string> file_each_lines에 저장한다.
    -ㄴ file_each_lines를 순회하면서 server block을 확인한다.
        - 어떻게?
        - "server {" -> "server", "{"
            서버블록 시작
            혹은 "server"가 나온 순간부터 server block
            <!-- - validate 확인을 server block이 나온 이후로 미루고 싶지만
            server block이라는 것이 안나올수도 있다.
            그렇게 된다면 각 라인이 유효한지 확인하면서 block의 시작과 끝을 파악해야한다.
            block을 찾는 작업중에 모든 유효성을 검사할 수 없으므로
            기초적인것과 block이 없을때 확인할 수 있는 것들을 한다. -->
            block이 안나오면 그 자체가 error이다.
            {는 해당 라인을 split했을때 나오는 vector의 처음이나 혹은 server, location 다음에 있다.
            server, location이 아닌데 {}이 되어있으면 error
            즉, block 영역을 확인할때 유효한 필드는 server, location 밖에 없다.
            server 확인 중에 location이 나올 수 있다.
            server '}'를 찾다가 착각해서 location의 '}'을 찾을수도 있다.
            그런 상황은 어떻게 방지하는가?
            server block을 찾다가 location block이 나온다면
            location block의 끝이 나와야 server block의 끝으로 친다.
            둘다 server_bracket_stack, locaton_bracket_stack으로 관리한다.

            block을 찾았으면 해당 block 영역을 잘라서 (start_iter, end_iter) validateServerBlock()함수를 호출한다.

            validateServerBlock()함수에서는 
            한줄씩 읽으면서 파싱을 진행하며 location이 나타나면 findLocationBlock()을 이용하여 영역을 찾아서 validateLocationBlcok(start_iter, end_iter)를 호출하여 확인한다.
            
            마찬가지로 validateLocationBlcok() 에서는 한줄씩 읽으며 location에 유효한 filed인지 확인한다.
            - 유효필드
                미리 map<std::string, int> 타입의 server_validation_info, location_validation_info를 가지고 유효필드인지,중복인지, 비어있는 유효필드는 없는지 확인한다.
                그외에는 세부룰
                    - port는 std::atoi 이용 0~65525 범위인지 
                    - ip는 x.x.x.x 같은 IPv4 형식인지
                        3자리는 숫자 하나는 .
                        split(string, ".")으로 가능할듯
                    - location path의 경우 /가 없어도 nginx config error는 없음 단지 404
                    - allow method
                     vector<std::string>
                     정확히 일치하는지 확인 필요
                    - autoindex
                        on,off만 허용
                    - root
                        단 하나의 문자열
                    - index
                        단 하나의 문자열


            - 유효필드 중복여부
            - 유효필드 값이 있는지
                - 값이 있을때 ';' 은 제대로 있는지
            - 주석이 있는지


            
        
/*
    - host
    - port
    - server_name
    - index (같은 영역 내에서)
    - root (같은 영역 내에서)
    - client_max_body_size
    - location_path (in location)
    - allow_method (in location)
    - autoindex (in location)
    - redirection (in location)
- 여러개 들어와도 되는 "Key" 값들
    - error page
        e.g)
            error_page 400 404 40x.html;
            error_page 502 501 50x.html;
        
- 여러개 들어왔을때 list 혹은 map으로 저장되어야할 것
    - location
    - error page

*/

        - 내부 값들이 있어야할것이 있고 없어도 되는게 있는지 확인한다.


3. config parsing
    - 유효성 검사도 끝났으니 파싱하여 저장
    - 한줄씩 읽은 뒤 trim으로 whitespace를 제거한다.
        - split(line, whitespace)로 공백기준으로 쪼갠다.
            - 반환값 타입은 vector<std::string>
        - 맨 왼쪽에 있는 문자열이 key가 된다.
            - 구조체에 담으며 이후에 오는 문자열은 value가 된다.
        - 각 config에 따라 달라진다.
            - error_page의 경우 마지막에 오는 것이 값이며 중간 값들은 각 status_code가 된다.
    - server, location 같은 block이 아닌 key로 시작한다면 ;이 있어야한다.
        - server block 확인
        - locatoin block 확인
        - curly bracket을 확인하기는 했지만 server block인지, location block인지는 모른다.
            단지 curly bracket pair가 맞는지 확인했을뿐이다.
        - server block parsing
            "server"  "{" 이거나 
            "server"
            "{" 이어야 server block이 시작된 것이다.
            
        
    
        value인것 맞고 ;이 있어야했다면 vector 마지막에 pop_back()으로 ;을 제거한다.



# 검증


# 실행


# 회고


