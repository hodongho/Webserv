#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <sys/event.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

int error_handling(std::string msg, int ret)
{
    std::cerr << msg << std::endl;
    return (ret);
}

void    change_events(std::vector<struct kevent>& change_list, uintptr_t ident,
                        int16_t filter, unsigned short flags, uint32_t fflags, intptr_t data, void* udata)
{
    struct kevent   temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}

void    disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "@client disconnectd : " << client_fd << std::endl;
    int ret = close(client_fd);
    std::cout << "@disconnect ret : " << ret << std::endl;

    clients.erase(client_fd);
}

std::string read_file(std::string file_name)
{
    std::string     s;
    std::ifstream   ifs(file_name);

    if (ifs.is_open())
    {
        ifs.seekg(0, std::ios::end);
        int size = ifs.tellg();
        s.resize(size);
        ifs.seekg(0, std::ios::beg);
        ifs.read(&s[0], size);
    }
    else
    {
        error_handling("Fail to open file", 1);
        exit(1);
    }
    return (s);
}

int create_socket()
{
    int listen_socket_fd;

    listen_socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket_fd == -1)
        return (-1);
    if (fcntl(listen_socket_fd, F_SETFL, O_NONBLOCK) == -1)
        return (-1);
    return (listen_socket_fd);
}

void    init_sock_addr(struct sockaddr_in& server_sock_addr, std::string ip, int port)
{
    memset(&server_sock_addr, 0, sizeof(server_sock_addr));
    server_sock_addr.sin_family = AF_INET;
    server_sock_addr.sin_addr.s_addr = inet_addr(ip.c_str()); //inet_ptob
    server_sock_addr.sin_port = htons(port);
}

int main(int argc, char *argv[1])
{
    int                 listen_socket_fd;
    int                 kq;
    struct sockaddr_in  server_sock_addr;
    std::string         s;

    if (argc != 2)
        return (error_handling("please input port infomation like ./webserver_00 4242", 1));
    listen_socket_fd = create_socket();
    if (listen_socket_fd == -1)
        return (error_handling("socket error", 2));
    // init_sock_addr(struct sockaddr_in& server_sock_addr, port);
    init_sock_addr(server_sock_addr, "0.0.0.0", std::atoi(argv[1]));
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &server_sock_addr, sizeof(server_sock_addr));
    if (bind(listen_socket_fd, (struct sockaddr *)&server_sock_addr, sizeof(server_sock_addr)) == -1)
        return (error_handling("bind error", 2));
    if (listen(listen_socket_fd, 5) == -1)
        return (error_handling("listen error", 2));
    s = read_file("example_korean.html");
    kq = kqueue();
    if (kq == -1)
        return (error_handling("kqueue error", 2));

    std::map<int, std::string>  clients;
    std::vector<struct kevent>  change_list;
    struct kevent               event_list[8]; // 왜 8인가?
    
    change_events(change_list, listen_socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    std::cout << "My kqueue reponse server start!" << std::endl;

    int             new_events;
    struct kevent*  curr_event;

    while (1)
    {
        char    buf[1024];
        int     recv_size;

        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
        if (new_events == -1)
            return (error_handling("kevent error", 3));
        change_list.clear(); // clear change_list for new changes
        for (int i = 0; i < new_events; ++i)
        {
            curr_event  = &event_list[i];
            if (curr_event->flags & EV_ERROR)
            {
                if (curr_event->ident == listen_socket_fd)
                    return (error_handling("server socket error", 3));
                else
                {
                    std::cerr << "client socket error" << std::endl;
                    std::cerr << "client curr_event->ident : " << curr_event->ident << std::endl;
                    
                    disconnect_client(curr_event->ident, clients);
                }
            }
            else if (curr_event->filter == EVFILT_READ)
            {
                if (curr_event->ident == listen_socket_fd)
                {
                    // accept new client
                    int             client_fd;
                    struct sockaddr client_sockaddr;
                    socklen_t       client_socklen;

                    client_socklen = sizeof(client_sockaddr);
                    client_fd = accept(listen_socket_fd, &client_sockaddr, &client_socklen);
                    if (client_fd == -1)
                        return (error_handling("client accept error", 2));
                    setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &client_sockaddr, client_socklen);
                    std::cout << "accept new client: " << client_fd << std::endl;
                    fcntl(client_fd, F_SETFL, O_NONBLOCK);
                    // add event for client socket - add read && write event
                    change_events(change_list, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    change_events(change_list, client_fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
                    clients[client_fd] = "";
                }
                else if (clients.find(curr_event->ident) != clients.end())
                {
                    char    buf[1024];
                    int     n;

                    n = recv(curr_event->ident, buf, sizeof(buf), 0);
                    if (n <= 0)
                    {
                        if (n < 0)
                            std::cerr << "client read error!" << std::endl;
                        disconnect_client(curr_event->ident, clients);
                    }
                    else
                    {
                        buf[n] = '\0';
                        clients[curr_event->ident] += buf;
                        std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
                    }
                }
            }
            else if (curr_event->filter == EVFILT_WRITE)
            {
                std::map<int, std::string>::iterator    it;
                // std::cout << "new_events : " << new_events <<  std::endl;
                // http://localhost:4242/

                it = clients.find(curr_event->ident);
                if (it != clients.end())
                {
                    if (clients[curr_event->ident] != "")
                    {
                        std::string send_msg;
                        int         sent_size;
                        size_t      body_size;

                        // TO be use Content-Length
                        body_size = s.size();
                        send_msg = "HTTP/1.0 200 OK\r\n\r\n" + s;
                        sent_size = send(curr_event->ident, send_msg.c_str(), send_msg.size(), 0);
                        // memset(buf, 0, recv_size);
                        std::cout << "sent_size : " << sent_size <<  std::endl;
                        if (sent_size == -1)
                        {
                            disconnect_client(curr_event->ident, clients);
                            close(listen_socket_fd);
                            return (error_handling("send error", 2));
                        }
                        else
                        {
                            clients[curr_event->ident].clear();
                            disconnect_client(curr_event->ident, clients);
                        }
                    }
                }
            }
        }
    }
    // close(client_fd);
    close(listen_socket_fd);
    return (0);
}
