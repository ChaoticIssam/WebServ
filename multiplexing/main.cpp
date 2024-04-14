#include "webserve.hpp"

int hexToDecimal(const std::string& hexStr) {
	std::cout << hexStr << std::endl;
	std::istringstream iss(hexStr);
	int decimalValue;
	iss >> std::hex >> decimalValue;
	std::cout << decimalValue << std::endl;
	return decimalValue;
}

int creat_socket_and_epoll(Helpers *help){

	//create socket and epool instence .
	help->sosocket = socket(AF_INET, SOCK_STREAM, 0);
	if(help->sosocket == -1){
		std::cout << "error socket" << std::endl;
		return(1);
	}
	epoll_fd = epoll_create(1);
	struct epoll_event multipl;
	struct sockaddr_in addresses;
	addresses.sin_family = AF_INET;
	addresses.sin_addr.s_addr = INADDR_ANY;
	addresses.sin_port = htons(atoi(help->obj.getPort().c_str()));

	//bind socket and bind many sockets with same ip address ND PORT.

	int op = 1;
	if(setsockopt(help->sosocket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1){
		perror("");
		std::cout << "setsockopt error" << std::endl;
		return(1);
	}
	if(bind(help->sosocket, (struct sockaddr *)&addresses, sizeof(addresses)) == -1){
		std::cout << "bind error" << std::endl;
		return(1);        
	}
	// now is the listen step where the socket listen to a connection .
	if(listen(help->sosocket, 10) == -1){
		std::cout << "listen error" << std::endl;
		return(1);
	}
	// control the epoll with adding modifying or deleting a file discreptor;
	multipl.events = EPOLLIN;
	multipl.data.fd = help->sosocket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, help->sosocket, &multipl);
	std::map<int , Webserve>multi_fd;
	while(1){
			int client_socket = -1;
			int epoll_w = epoll_wait(epoll_fd, help->events, 1000, -1);
			for(help->i = 0; help->i < epoll_w; help->i++){
				
				if(help->events[help->i].data.fd == help->sosocket){
					client_socket = accept(help->sosocket, 0, 0);
					if(client_socket == -1){
						std::cout << "accept error" << std::endl;
						return(1);
					}
					multipl.events = EPOLLIN | EPOLLOUT;
					multipl.data.fd = client_socket;
					epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &multipl);
					continue;
				}
				if(multi_fd.count(help->events[help->i].data.fd) == 0)
				{
					multi_fd[help->events[help->i].data.fd] = Webserve();
				}
				try {
					if(help->events[help->i].data.fd & EPOLLIN && !multi_fd[help->events[help->i].data.fd].response_success){
						char buff[MAX_SIZE];
						memset(buff,0,(MAX_SIZE));
						multi_fd[help->events[help->i].data.fd].k = read(help->events[help->i].data.fd, buff, (MAX_SIZE - 1));
						// std::cout << buff << std::endl;
						call_request_functions(multi_fd, help, buff);
					}
					else {
						std::cout << "did not enter call_request\n";
						exit (1);
					}

				}
				catch (const ResponseException& e){
					if(help->events[help->i].data.fd & EPOLLOUT){
						std::cout << "Exception caught: " << e.get_message() << ", Status: " << e.get_status() << std::endl;
						create_response(multi_fd, e.get_message(), e.get_status(), help);
					}
					else {
						std::cout << "did not enter create response\n";
						exit (1);
					}
				}

		}
	}
	close(help->sosocket);
						std::cout << "haaaaaaaaaaaaa\n";
	return(0);
}