#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
:method(NONE), URI("")
{
	version = "";
	body = "";
};

HTTPRequest::~HTTPRequest()
{};

int	HTTPRequest::validateStartLine(void)
{
	//일단은 METHOD가 GET이 아닐경우 에러처리, 프로토콜이 HTTP가 아닌 경우 Error
	if (this->method == NONE || this->version.find("HTTP/1.1") == std::string::npos)
		return (-1);

	//access를 통해 URI가 존재하는 경로인지 확인
	if (access(URI.c_str(), F_OK) != 0)
		return (-1);
	return (0);
}

int	HTTPRequest::parseStartLine(std::stringstream& request_stream)
{
	std::string	string_method, end;

	//stringstream으로 공백으로 나눠 변수에 저장
	request_stream >> string_method >> this->URI >> this->version;

	if (string_method == "GET")
		this->method = GET;
	else if (string_method == "POST")
		this->method = POST;
	else if (string_method == "DELETE")
		this->method = DELETE;

	//이번 행에서 남은 문자는 "\r\n"이어야만함으로 \n기준으로 라인을 받아오는데 "\r\n"이 아닌경우 400 Error
	std::getline(request_stream, end, '\n');
	if (end != "\r")
		return (-1);
	if (this->validateStartLine() == -1)
		return (-1);

	//Test
	std::cout	<< BLU << "\n[Check Parsing]\n\n"
				<< BRW << "method:" << CYN << " [" << this->method << "] "
				<< BRW << "URI:" << CYN << " [" << this->URI << "] "
				<< BRW << "version:" << CYN << " [" << this->version << "]\n" << std::endl;
	return (0);
}

int	HTTPRequest::validateHeaderField(void)
{
	//request 필수헤더 검사
	std::map<std::string, std::string>::iterator	end = header.end();
	if (header.find(HOST) == end ||
		header.find(USER_AGENT) == end ||
		header.find(ACCEPT) == end ||
		header.find(CONNECTION) == end)
		return (-1);
	return (0);
}

int	HTTPRequest::parseHeaderField(std::stringstream& request_stream)
{
	std::string header_name, header_value, end, white_space = " \t";

	while(true)
	{
		//':'을 구분자로 header_name을 따오고 \r을 기준으로 header_value를 추출, 그리고 다음행으로 커서 옮기기
		std::getline(request_stream, header_name, ':');
		std::getline(request_stream, header_value, '\r');
		std::getline(request_stream, end, '\n');

		//만약 끝까지 다 읽었으면 break
		if (request_stream.eof())
			break ;

		//'\r'까지 읽었기 때문에 남은 '\n'까지 읽는다면 문자열이 비어있는 경우가 정상케이스라고 생각
		if (end != "")
			return (-1);

		//header_value에서 맨 처음 공백 제거 만약 제거했는데도 공백이 있다면 Error
		if (header_value.find_first_not_of(white_space) != 0)
		{
			header_value.erase(0, 1);
			if (header_value.find_first_not_of(white_space) != 0)
				return (-1);
		}
		this->header[header_name] = header_value;

		//Test
		std::cout	<< BRW << "Name: " << CYN << "[" << header_name << "] "
		<< std::setw(25 - header_name.size()) << BRW << "Value: " << CYN << "[" << this->header[header_name] << "]" << std::endl;
	}
	if (this->validateHeaderField() == -1)
		return (-1);
	return (0);
}

int	HTTPRequest::parseRequestMessage(std::string& msg)
{
	std::stringstream	request_stream(msg);

	if (this->parseStartLine(request_stream) == -1)
		return (-1);
	if (this->parseHeaderField(request_stream) == -1)
		return (-1);

	std::cout << WHI << std::endl;
	return (0);
}

void	HTTPRequest::saveBody(std::string& _body)
{
	this->body.append(_body, _body.size());
}

//Getter
const std::string&							HTTPRequest::getVersion() const { return (this->version); }
const MethodType&							HTTPRequest::getMethod() const { return (this->method); }
const std::string&							HTTPRequest::getURI() const { return (this->URI); }
const std::string&							HTTPRequest::getBody() const { return (this->body); }
const std::map<std::string, std::string>&	HTTPRequest::getHeader() const { return (this->header); }

ssize_t		HTTPRequest::getContentLength()
{
	std::map<std::string, std::string>::iterator	content_length_it;
	ssize_t											content_length;

	content_length_it = header.find("Content-length");
	if (content_length_it == header.end())
		std::cerr << RED << "400 Bad Request!" << WHI << std::endl;

	for (size_t i = 0; i < content_length_it->second.size(); i++)
	{
		if (isdigit(content_length_it->second[i]) == 0)
			return (-1);
	}

	content_length = static_cast<ssize_t>(atoi(content_length_it->second.c_str()));
	return (content_length);
}


// std::cerr << RED << "400 Bad Request!" << WHI << std::endl;
