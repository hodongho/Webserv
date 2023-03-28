#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>

bool    is_int(char *c)
{
    while (*c != '\0')
    {
        if (*c < '0' || *c > '9')
            return false;
        c++;
    }
}


int main(int argc, char **argv)
{
    if (argc != 2 || !is_int(argv[1]))
    {
        std::cerr << "[ERROR] Port is not provided via command line parameters!\n";
        return -1;
    }

    // Create a socket & get the file descriptor
    int sock_listener = socket(AF_INET, SOCK_STREAM, 0);
    // Create If the socket is created
    if (sock_listener < 0)
    {
        std::cerr << "[ERROR] Socket cannot be created!\n";
        return -2;
    }

    std::cout << "[INFO] Socket has been created.\n";
    // Address info to bind socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::atoi(argv[1]));
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    // OR
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

    char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(sock_listener, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "[ERROR] Created socket cannot be binded to ( "
                << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
                << ":" << ntohs(server_addr.sin_port) << ")\n";
        return -3;
    }

    std::cout << "[INFO] Sock is binded to ("
                << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
                << ":" << ntohs(server_addr.sin_port) << ")\n";

    // Start listening
    if (listen(sock_listener, SOMAXCONN) < 0)
    {
        std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
        return -4;
    }
    std::cout << "[INFO] Socket is listening now.\n";

    // Accept a call
    sockaddr_in client_addr;
    socklen_t   client_addr_size = sizeof(client_addr);
    int sock_client;
    if ((sock_client = accept(sock_listener, (sockaddr*)&client_addr, &client_addr_size)) < 0)
    {
        std::cerr << "[ERRROR] Connections cannot be accepted for a reason.\n";
        return -5;
    }
    std::cout << "[INFO] A connection is accepted now.\n";
    close(sock_listener);
    std::cout << "[INFO] Main listenr socket is closed.\n";

    // Get name info
    char    host[NI_MAXHOST];
    char    svc[NI_MAXSERV];
    return 0;
}