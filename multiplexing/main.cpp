#include "webserve.hpp"

int count = 0;

int hexToDecimal(const std::string &hexStr)
{
	std::cout << hexStr << std::endl;
	std::istringstream iss(hexStr);
	int decimalValue;
	iss >> std::hex >> decimalValue;
	std::cout << decimalValue << std::endl;
	return decimalValue;
}

int is_cgi(std::map<int, Webserve> &multi_fd, int fd)
{
	if ((multi_fd[fd].post_cgi == true || multi_fd[fd].get_cgi == true))
	{
		return 1;
	}
	return 0;
}

int creat_socket_and_epoll(Helpers *help)
{
	// create socket and epool instence .
	struct epoll_event multipl;
	struct sockaddr_in addresses;
	help->s = 0;
	epoll_fd = epoll_create(1);
	if(epoll_fd == -1){
		std::cerr << "error epoll" << std::endl;
		return (1);
	}
	for (std::vector<configParss>::iterator it = _srv.begin(); it != _srv.end(); it++)
	{
		help->sosocket = socket(AF_INET, SOCK_STREAM, 0);
		// std::cout << "socket : " << help->sosocket << std::endl;
		if (help->sosocket == -1)
		{
			std::cerr << "error socket" << std::endl;
			return (1);
		}

		addresses.sin_family = AF_INET;

		addresses.sin_addr.s_addr = inet_addr(it->getHost().c_str());
		if (addresses.sin_addr.s_addr == INADDR_NONE)
		{
			std::cerr << "Invalid IP address" << std::endl;
			return (1);
		}
		addresses.sin_port = htons(atoi(it->getPort().c_str()));

		// bind socket and bind many sockets with same ip address ND PORT.
		int op = 1;
		if (setsockopt(help->sosocket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1)
		{
			// perror("");
			std::cerr << "setsockopt error" << std::endl;
			return (1);
		}
		if (bind(help->sosocket, (struct sockaddr *)&addresses, sizeof(addresses)) == -1)
		{
			std::cerr << "bind error" << std::endl;
			return (1);
		}
		// now is the listen step where the socket listen to a connection .
		if (listen(help->sosocket, 10) == -1)
		{
			std::cerr << "listen error" << std::endl;
			return (1);
		}
		// help->socketat.resize(help->s + 1);
		// std::cout << help->sosocket <<" " << it->getHost() << std::endl;
		multipl.data.fd = help->sosocket;
		help->socketat.push_back(help->sosocket);
		multipl.events = EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, help->sosocket, &multipl);
		// control the epoll with adding modifying or deleting a file discreptor;
	}
	std::map<int, Webserve> multi_fd;
	while (1)
	{
		// signal(SIGPIPE, SIG_IGN);
		// std::cerr << "wait\n";
		int epoll_w = epoll_wait(epoll_fd, help->events, 1000, 0);
		help->server_index = 0;
		for (help->i = 0; help->i < epoll_w; help->i++)
		{
			int flag = 0;
			for (int l = 0; l < (int)help->socketat.size(); l++)
			{
			// std::cerr << "enter to getsflskdjflskdf: "  << epoll_w << "======= >" << count << std::endl;
			// // sleep (1);
			// count++;
				int client_socket;
				if (help->events[help->i].data.fd == help->socketat[l])
				{
					help->server_index = l;
					client_socket = accept(help->socketat[l], 0, 0);
					if (client_socket == -1)
					{
						std::cerr << "accept error" << std::endl;
						continue;
					}
					multipl.events = EPOLLIN | EPOLLOUT;
					multipl.data.fd = client_socket;
					epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &multipl);
					multi_fd[help->events[help->i].data.fd] = Webserve();
					multi_fd[help->events[help->i].data.fd].config = _srv[l];
					flag = 1;
				}
			}
			if (flag == 1)
				continue;
			if ((help->events[help->i].events & EPOLLIN) && !multi_fd[help->events[help->i].data.fd].response_success)
			{
				char buff[MAX_SIZE];
				memset(buff, 0, MAX_SIZE - 1);
				multi_fd[help->events[help->i].data.fd].k = read(help->events[help->i].data.fd, buff, (MAX_SIZE - 1));
				multi_fd[help->events[help->i].data.fd].time_out = clock();
				if (multi_fd[help->events[help->i].data.fd].k > 0)
					pars_request(multi_fd[help->events[help->i].data.fd].res, multi_fd, help, buff);
			}
			if ((help->events[help->i].events & EPOLLOUT) && (multi_fd[help->events[help->i].data.fd].res._Rpnse == true || multi_fd[help->events[help->i].data.fd].response_success == true || multi_fd[help->events[help->i].data.fd].error_response == true || (is_cgi(multi_fd, help->events[help->i].data.fd))))
			{
				multi_fd[help->events[help->i].data.fd].time_out = clock();
				if( (is_cgi(multi_fd, help->events[help->i].data.fd)) && multi_fd[help->events[help->i].data.fd].running == 0 && multi_fd[help->events[help->i].data.fd].res._listContent == 0)
				{
					// std::cout << "Url: " << multi_fd[help->events[help->i].data.fd].res._URI << std::endl;
					try
					{
						if (multi_fd[help->events[help->i].data.fd].running == 0)
						{
							forking(help->events[help->i].data.fd, multi_fd, help);
							multi_fd[help->events[help->i].data.fd].startTime = clock();
						}
						if (multi_fd[help->events[help->i].data.fd].pid == 0 && multi_fd[help->events[help->i].data.fd].res._listContent != 1)
						{
							std::cerr << "list content: " << multi_fd[help->events[help->i].data.fd].res._listContent << std::endl;
							child_proc(help->events[help->i].data.fd, multi_fd);
						}
						else
						{
							continue;
						}
					}
					catch(const ResponseException& e)
					{
						std::cout << "message: " << e.get_message() << " , status : " << e.get_status() << std::endl;
						timeoutRes(e.get_message(), e.get_status(), help->events[help->i].data.fd, multi_fd);
						continue;
					}
					
				}
				else if((is_cgi(multi_fd, help->events[help->i].data.fd)) && multi_fd[help->events[help->i].data.fd].running == 1 && multi_fd[help->events[help->i].data.fd].child_exited == 0)
				{
					
					try
					{
						checking_timeout(help->events[help->i].data.fd, multi_fd);
					}
					catch (const ResponseException &e)
					{
						std::cout << "message: " << e.get_message() << " , status : " << e.get_status() << std::endl;
						timeoutRes(e.get_message(), e.get_status(), help->events[help->i].data.fd, multi_fd);
						continue;
					}
				

				}
				std::cerr << "cgi : " << is_cgi(multi_fd, help->events[help->i].data.fd) << " , list content: " << multi_fd[help->events[help->i].data.fd].res._listContent << std::endl;
				if (multi_fd[help->events[help->i].data.fd].child_exited == 1)
				{
					
					// std::string message = "ok";
					// std::string status = "200";
					// std::cout << "chilf finished " <<std::endl;
					// for(int i = 0 ;i < epoll_w ; i++ )
					// 	if(multi_fd[help->events[help->i].data.fd].pipefd[0] == help->events[i].data.fd)
							cgi_response(epoll_fd, help->events[help->i].data.fd, multi_fd, multi_fd[help->events[help->i].data.fd].res);
					// timeoutRes(message, status, help->events[help->i].data.fd, multi_fd);
					continue;
				}
				///////////
				
				if(!(is_cgi(multi_fd, help->events[help->i].data.fd)) || (is_cgi(multi_fd, help->events[help->i].data.fd) && multi_fd[help->events[help->i].data.fd].res._listContent == 1))
				{
					multi_fd[help->events[help->i].data.fd].res.sendResponse(multi_fd, help->events[help->i].data.fd);
				}
			}
			if (((double)(clock() - multi_fd[help->events[help->i].data.fd].time_out)) / CLOCKS_PER_SEC > 20)
			{
				std::cout << "time : " << (((double)(clock() - multi_fd[help->events[help->i].data.fd].time_out)) / CLOCKS_PER_SEC) << std::endl;
				multi_fd[help->events[help->i].data.fd].res._statusCode = "408";
				multi_fd[help->events[help->i].data.fd].res._message = "408 Request Timeout";
				multi_fd[help->events[help->i].data.fd].res._contentType = "text/html";
				multi_fd[help->events[help->i].data.fd].res._serverIndex = help->server_index;
				multi_fd[help->events[help->i].data.fd].res.sendResponse(multi_fd, help->events[help->i].data.fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, help->events[help->i].data.fd, &multipl);
				close(help->events[help->i].data.fd);
				// 408 Request Timeout
				// exit(1);
			}
		}
	}
	close(help->sosocket);
	return (0);
}