# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yolee <yolee@student.42seoul.kr>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/31 01:14:02 by yolee             #+#    #+#              #
#    Updated: 2023/04/09 01:03:55 by yolee            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

VPATH = ./srcs
INCLUDE_DIR = ./include

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS = -MMD -I$(INCLUDE_DIR)

SRCS = ServerHandler.cpp \
	utils.cpp \
	webserv.cpp \
	HTTPMessage.cpp \
	HTTPRequest.cpp \
	HTTPResponse.cpp \
	ConfigInfo.cpp \
	ServerConfig.cpp \
	LocationConfig.cpp \

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp
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
