# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yolee <yolee@student.42seoul.kr>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/31 01:14:02 by yolee             #+#    #+#              #
#    Updated: 2023/04/04 16:34:58 by yolee            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -MMD

SRCS = ServerHandler.cpp \
	utils.cpp \
	Webserv.cpp \
	HTTPMessage.cpp \
	HTTPRequest.cpp \
	HTTPResponse.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)

.cpp.o :
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean :
	$(RM) $(OBJS)
	$(RM) $(DEPS)

fclean :
	$(MAKE) clean
	$(RM) $(NAME)

re :
	$(MAKE) fclean
	$(MAKE) all

.PHONY : all clean fclean re

-include $(DEPS)
