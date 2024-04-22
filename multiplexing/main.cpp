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
	struct epoll_event multipl;
	struct sockaddr_in addresses;
	help->s = 0;
	epoll_fd = epoll_create(1);
	for(std::vector<configParss>::iterator it = _srv.begin(); it != _srv.end(); it++)
	{
		help->sosocket = socket(AF_INET, SOCK_STREAM, 0);
		std::cout << "socket : " << help->sosocket << std::endl;
		if(help->sosocket == -1){
			std::cout << "error socket" << std::endl;
			return(1);
		}
		
		addresses.sin_family = AF_INET;
		
		addresses.sin_addr.s_addr = inet_addr(it->getHost().c_str());
		if(addresses.sin_addr.s_addr == INADDR_NONE)
		{
			std::cout << "Invalid IP address" << std::endl;
			return(1);
		}
		addresses.sin_port = htons(atoi(it->getPort().c_str()));

		//bind socket and bind many sockets with same ip address ND PORT.
		int op = 1;
		if(setsockopt(help->sosocket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1){
			// perror("");
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
		// help->socketat.resize(help->s + 1);
		std::cout << help->sosocket <<" " << it->getHost() << std::endl;
		multipl.data.fd = help->sosocket;
		help->socketat.push_back(help->sosocket);
		multipl.events = EPOLLIN ;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD,help->sosocket, &multipl);
		// control the epoll with adding modifying or deleting a file discreptor;
	}
	std::map<int , Webserve>multi_fd;
				help->server_index = 0;
	while(1){
			std::cout << "waiting for connection" << std::endl;
				int epoll_w = epoll_wait(epoll_fd, help->events, 1000, 0);
				for(help->i = 0; help->i < epoll_w; help->i++){
					int flag = 0;
					for(int l = 0; l < (int)help->socketat.size(); l++){
						int client_socket ;
						if(help->events[help->i].data.fd == help->socketat[l])
						{
							help->server_index = l;
							client_socket = accept(help->socketat[l], 0, 0);
							if(client_socket == -1){
								std::cout << "accept error" << std::endl;
								return(1);
							}
							multipl.events = EPOLLIN | EPOLLOUT;
							multipl.data.fd = client_socket;
							epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &multipl);
							multi_fd[help->events[help->i].data.fd] = Webserve();
							multi_fd[help->events[help->i].data.fd].config = _srv[l];
							flag = 1;	
						}
					}
					if(flag == 1)
						continue;
					if((help->events[help->i].events  & EPOLLIN))
					{
						char buff[MAX_SIZE];
						memset(buff,0,MAX_SIZE-1);
						multi_fd[help->events[help->i].data.fd].k = read(help->events[help->i].data.fd, buff, (MAX_SIZE - 1));
						if(multi_fd[help->events[help->i].data.fd].k > 0)
							call_request_functions(multi_fd[help->events[help->i].data.fd].res ,multi_fd, help, buff);
					}
					// std::cout << "gg = "<<  multi_fd[help->events[help->i].data.fd].response_success << std::endl;
					if ((help->events[help->i].events & EPOLLOUT) && multi_fd[help->events[help->i].data.fd].res._Rpnse == true)
					{
						std::cout << " GET METHOD "<<std::endl;
						std::cout << multi_fd[help->events[help->i].data.fd].res._contentLength << std::endl;
						multi_fd[help->events[help->i].data.fd].res.sendResponse(multi_fd, help->events[help->i].data.fd);
					}
		}
	}
	close(help->sosocket);
	return(0);
}