#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <string>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int ac, char **av)
{
	int sock;
	std::string	recvMsg;
	char message[] = "";
	int str_len, recv_len, recv_cnt;
	struct sockaddr_in serv_addr;

	if (ac != 3)
	{
		std::cout << "Usage : " << av[0] << "<IP> <port>" << std::endl;
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET ,av[1] ,&serv_addr.sin_addr);
	serv_addr.sin_port = htons(atoi(av[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_handling("connect() error");
	else
		std::cout << "Connected....." << std::endl;

	while(1)
	{
		std::cout << "Input message(Q to quit):" << std::endl;
		std::cin >> message;

		if (!strcmp(message, "q") || !strcmp(message, "Q"))
			break ;

		send(sock, message, strlen(message), 0);
		std::cout << message << std::endl;
		char buf[BUF_SIZE];
		recv_cnt = 0;
		int i = 0;
		while(recv_cnt < BUF_SIZE - 1)
		{
			i++;
			memset(buf, 0, BUF_SIZE);
			recv_cnt = recv(sock, buf, BUF_SIZE - 1, 0);
			if (recv_cnt < 0)
				error_handling("read() error");
			recvMsg += buf;
			std::cout << i << std::endl;
			std::cout << buf << std::endl;
			break;
		}
		// recvMsg[recv_len] = 0;
		std::cout << recvMsg << std::endl;
		break ;
	}

	close(sock);

	return 0;
}

void	error_handling(char *message)
{
	std::cerr << message << std::endl;
	exit (1);
}
