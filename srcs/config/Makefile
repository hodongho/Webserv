NAME = ConfigInfo
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
CPPFLAGS = -MMD -I ../../include

SRCS = ConfigInfo.cpp \
	LocationConfig.cpp \
	ServerConfig.cpp \

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