CXX		= c++
# CXXFLAGS= -Wall -Wextra -Werror -std=c++98

NAME	= server

SRCS	= main.cpp Server.cpp Socket.cpp utils.cpp HTTPRequest.cpp\
			HTTPResponse.cpp
OBJS	= $(SRCS:.cpp=.o)
HEADERS	= Webserv.hpp Server.hpp Socket.hpp utils.hpp HTTPRequest.hpp\
			HTTPResponse.hpp

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean :
	$(RM) $(OBJS)

fclean :
	make clean
	$(RM) $(NAME)

re :
	make fclean
	make all

$(OBJS)	: $(HEADERS)

.PHONY : all bonus clean fclean re
