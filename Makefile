# src =	multiplexing/test.cpp delete_req_cgi/message.cpp delete_req_cgi/Request/Request_charcheck.cpp delete_req_cgi/Request/Request_Get_handle.cpp \
# 		delete_req_cgi/Request/Request_matchlocation.cpp delete_req_cgi/Request/Request_parse.cpp delete_req_cgi/Request/Request.cpp delete_req_cgi/Request/delete/delete_method.cpp\
# O_files = $(src:.cpp=.o)
# HEADERS =  
# CXXFLAGS = -Wall -Wextra -Werror -std=c++98
# CXX = c++
# NAME = webserve

# all: $(NAME)

# $(NAME) : $(O_files)
# 	$(CXX)  $(CXXFLAGS) $(O_files) -o $(NAME)

# %.o: %.cpp $(HEADERS)
# 	$(CXX)  $(CXXFLAGS) -c $< -o $@

# clean : 
# 	rm -rf $(O_files)

# fclean : clean
# 	rm -rf $(NAME)

# re : fclean $(NAME)

src =	config/config/main.cpp config/config/configParss.cpp multiplexing/main_const.cpp #delete_req_cgi/Request/Request_charcheck.cpp delete_req_cgi/Request/Request_Get_handle.cpp \
		delete_req_cgi/Request/Request_matchlocation.cpp delete_req_cgi/Request/Request_parse.cpp \
		delete_req_cgi/Request/Request.cpp delete_req_cgi/Request/delete/delete_method.cpp config/config/main.cpp config/config/configParss.cpp#
		delete_req_cgi/Request/Request.cpp delete_req_cgi/Request/delete/delete_method.cpp config/config/main.cpp config/config/configParss.cpp#
O_files = $(src:.cpp=.o)
# HEADERS =
CXXFLAGS = -Wall -Wextra -Werror -fsanitize=address -g3 -std=c++98
CXX = c++
NAME = webserve

all: $(NAME)

$(NAME): $(O_files)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(O_files)

fclean: clean
	rm -rf $(NAME)

re: fclean all
