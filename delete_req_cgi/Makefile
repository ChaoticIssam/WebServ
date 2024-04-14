NAME	=	WebServer

CXX		=	c++

CXXFLAGS	=	-std=c++98 -Wall -Wextra -Werror -fsanitize=address -g3  

RM		=	rm -rf

SRC		=	../config/config/main.cpp ../config/config/configParss.cpp main2.cpp  Request/Request.cpp Request/Request_charcheck.cpp \
			Request/Request_matchlocation.cpp Request/Request_Get_handle.cpp \
			Request/Request_parse.cpp Request/delete/delete_method.cpp\

OBJ		=	$(SRC:.cpp=.o)

all		:	$(NAME)

$(NAME)	:	$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean	:
	$(RM) $(OBJ)

fclean	: clean
	$(RM) $(NAME)

re		:	fclean all