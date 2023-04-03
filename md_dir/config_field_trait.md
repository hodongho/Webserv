# parsing 

1. config file name extention rule check
2. config file open and read 
3. file content validate
	- curly bracket
	- server, location block 
		- block으로 분리
		- 각 block 내부에서 유효성 검사
			- 각 라인 검사할때 공통사항
				- 맨 앞에 나오는 단어가 #주석이면 스킵
				- keyword가 유효하면 value개수가 맞는지와 ;은 잘 있는지 확인
					root   C:/Users/ranch/nginx-1.22.1/nginx-1.22.1/html ;
					띄어져 있는 경우도 가능하므로 마지막 단어에 ";"없을수도 있다.
					cpp도 ';'이 떨어져있어도 가능하므로 개행을 포함한 whitespace이면 인정해준다.
			- server
				- host (필수)
				- port (필수)
				- server_name (선택, std::string)
				- root (필수)
				- index (필수)
				- error_page (필수) -> 멤버변수 status_code
				
				- default_error_page (400,500)
			- location 
				- locatoin_path (필수)
				- allow_method (적지 않으면 default 값으로처리)
				- root (선택, std::string)
				- index (선택, std::string)
				- return(redirection) (선택, 멤버변수 status_code)
4. parsing
    - validate에서 쓴 함수를 재활용하거나
    block을 나눌때 아예 저장한다.
    이렇게되면 validate파트에서는 curly bracket pair만 확인하게됨
			
5. cgi 추가


