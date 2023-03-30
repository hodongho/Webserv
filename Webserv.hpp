#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <exception>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/event.h>
# include <netinet/in.h> //IPPROTO_TCP
# include "utils.hpp"
# include "Socket.hpp"

# define PORT 4242

# define BLK "\e[0;30m"
# define RED "\e[0;31m"
# define GRN "\e[0;32m"
# define CYN "\e[0;34m"

#endif
