# config_test_case.md

# 문제이해
config parsing case를 추가한다.
어떤 케이스가 있을 것인가?

각 항목과 구성을 생각한다
어떤 구성인가?

server들로 이루어져 있다
server에는 ip, host, (server_name), root, index,lient_max_body_size들로 이루져 있다.


- server_name
- ip
- root 
    - server에 root가 있으면 서버 것으로 인지한다.
    - location에도 있으면 location것으로 덮여쓰여진다.
- port 
- index
- 

# Case