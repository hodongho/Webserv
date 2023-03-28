#include <stdio.h>
#include <arpa/inet.h>

int main()
{
	int i;
	int test = 4242;
	char* ptr = (char*)&test; // 1 바이트만을 가리키는 포인터를 생성함.

	for (i = 0; i < sizeof(int); i++)
	{
		printf("%x", ptr[i]); // 1 바이트씩 순서대로 그 값을 출력함.
	}

    return 0;
}
