# 개요

본 프로젝트는 C++ 기반으로 HTTP 1.1 버전의 통신을 지원하는 간단한 서버 프로그램을 제작하는 프로젝트입니다.

# 목표

- C++의 소켓 함수들과 kqueue함수를 사용하여 클라이언트와 서버가 비동기적으로 연결되는 I/O Multiplexing 서버 구현
- Request로 들어온 HTTP METHOD GET, POST, DELETE를 파싱해 적절하게 처리(새 페이지 응답, 동적 페이지 응답, 파일 삭제) 후 Response를 생성하여 응답하는 기능 구현.

# 실행 방법

GNU make 사용해 make

```bash
  make
```

configuration file .conf 확장자로 지정하여 conf 디렉토리에 작성 후, 아래와 같은 명령어로 실행

```bash
  ./webserv <Configuration-file-name>.conf
```

# Conf 파일 작성법

## Server block

- host <IPv4-format>
  - 접속 ip 지정
- port <port-number>
  - 접속 port 지정
- server_name <server-name>
  - 서버 이름 지정
- root <root-directory>
  - root URL과 연결될 디렉토리 지정
- index <index-page-source>
  - root URL로 접속시 기본으로 연결될 index페이지 소스 지정
- client_max_body_size <integer-size>
  - 클라이언트로부터 받을 body의 최대 사이즈 지정
- error_page <error-code> <error-page-source-directory>
  - 각 에러코드에 해당하는 페이지 소스 지정
- cgi_pass <cgi-extension> <cgi-program-directory>
  - cgi 프로그램 지정

## Location block

- location <directory> {}
  - 특정 디렉토리로 리퀘스트가 들어왔을 경우 동작 지정
- autoindex <on/off>
  - 오토인덱스 기능 켜기/끄기 (기본값: 끄기)
- allow_method <HTTP-Methods>
  - 허용된 메소드 입력
- redirect <redirection-URL>
  - 리다이렉션 페이지 지정

## Configure file 예시

```plain text
server {
    host 127.0.0.1;
    port 4242;
    server_name webserv;
    root /Users/userName/webserv/var;
    index index.html;
    client_max_body_size 1000000;
    error_page 400 ../error_page/400.html;
    error_page 404 ../error_page/404.html;
    error_page 405 ../error_page/405.html;
    cgi_pass .php /Users/userName/webserv/cgi/php-cgi;
    cgi_pass .py /usr/local/bin/python3;

    location /uploads/ {
        autoindex on;
    }

    location /img {
        allow_method GET POST;
    }

    location /redirect {
        redirect https://google.com/;
    }

}
```

# 테스트

본 과제는 브라우저와 서버간 HTTP통신이 정상적으로 작동하는지 확인하기 위해 테스트페이지와 기본 conf 파일을 제공합니다.</br>
기본 conf파일에서는 root에 클론받은 디렉토리를 지정해 주어야 하며</br>
테스트 파일은 /var 디렉토리 안에 구성되어있습니다.</br>
서버 실행 이후 [http://localhost:4242](http://localhost:4242)에 접속하여 브라우저와 서버간 통신을 테스트 해보실 수 있습니다.
