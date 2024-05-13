src = multiplexing/main.cpp\
		multiplexing/main_const.cpp\
		multiplexing/Content_parsing.cpp\
		multiplexing/request_parsing.cpp\
		multiplexing/response.cpp\
		config/config/configParss.cpp\
		config/config/main.cpp\
		multiplexing/delete/delete_method.cpp\
		cgi/cgi.cpp\
		get_method/GetMethod.cpp\
		Response/Res.cpp\
		Response/utils.cpp

O_files = $(src:.cpp=.o)
HEADERS =  multiplexing/webserve.hpp
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
CXX = c++
NAME = webserve

all: $(NAME)

$(NAME) : $(O_files)
	$(CXX)  $(CXXFLAGS) $(O_files) -o $(NAME)

%.o: %.cpp $(HEADERS)
	$(CXX)  $(CXXFLAGS) -c $< -o $@

clean : 
	rm -rf $(O_files)

fclean : clean
	rm -rf $(NAME)

re : fclean $(NAME)

#111111111111111111111111111111111response.cpp