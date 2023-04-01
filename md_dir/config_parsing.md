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


# 검증


# 실행


# 회고


