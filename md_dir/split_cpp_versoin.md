# split_cpp_versoin.md
# 문제이해
std::vector<std::string> split(const std::string& origin_str, char delimeter);

문자열을 받아서 delimeter로 구분해서 단어로 끊어서 vector<std::string>으로 return한다.
- delimeter를 string으로 할지는 고민이 필요하다.
    - string으로 한다면 string delimeter에 해당하는 것들이 나올때마다 끊어준다? 등의 정립이 필요하다.   


# 문제 재정의
origin_str == "hello world", delimeter == " " -> "hello", "world"
origin_str == "hello world", delimeter == "w" -> "hello ", "orld"

# 문제 해결방법 
1. 단어 파악하여 바로 추가
    - 문자열 사이에 delimeter가 계속 나오면 skip하고 그 다음에 다른 문자여야 끊는다.


# 검증



# 실행


# 회고

