#include "webserve.hpp"

int count = 0;

int hexToDecimal(const std::string &hexStr)
{
	std::istringstream iss(hexStr);
	int decimalValue;
	iss >> std::hex >> decimalValue;
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

		int op = 1;
		if (setsockopt(help->sosocket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1)
		{
			std::cerr << "setsockopt error" << std::endl;
			return (1);
		}
		if (bind(help->sosocket, (struct sockaddr *)&addresses, sizeof(addresses)) == -1)
		{
			std::cerr << "bind error" << std::endl;
			return (1);
		}
		if (listen(help->sosocket, 10) == -1)
		{
			std::cerr << "listen error" << std::endl;
			return (1);
		}
		multipl.data.fd = help->sosocket;
		help->socketat.push_back(help->sosocket);
		multipl.events = EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, help->sosocket, &multipl);
	}
	std::map<int, Webserve> multi_fd;
	while (1)
	{
		int epoll_w = epoll_wait(epoll_fd, help->events, 1000, -1);
		for (help->i = 0; help->i < epoll_w; help->i++)
		{
			int flag = 0;
			if(help->events[help->i].events & EPOLLERR || help->events[help->i].events & EPOLLHUP || help->events[help->i].events & EPOLLRDHUP)
			{
				if(multi_fd[help->events[help->i].data.fd].running == 1 && multi_fd[help->events[help->i].data.fd].child_exited == 0)
				{
					kill(multi_fd[help->events[help->i].data.fd].pid, SIGKILL);
					waitpid(multi_fd[help->events[help->i].data.fd].pid, 0, 0);
					close(multi_fd[help->events[help->i].data.fd].pipefd[0]);
				}
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, help->events[help->i].data.fd, NULL);
				close(help->events[help->i].data.fd);
				std::map<int, Webserve>::iterator it = multi_fd.find(help->events[help->i].data.fd);
				if (it != multi_fd.end())
					multi_fd.erase(it);
				continue;
			}
			for (int l = 0; l < (int)help->socketat.size(); l++)
			{
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
					multipl.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
					multipl.data.fd = client_socket;
					epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &multipl);
					multi_fd[help->events[help->i].data.fd] = Webserve();
					multi_fd[help->events[help->i].data.fd].config = _srv[l];
					flag = 1;
					break;
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
			if ((help->events[help->i].events & EPOLLOUT)  &&(multi_fd[help->events[help->i].data.fd].res._Rpnse == true || multi_fd[help->events[help->i].data.fd].response_success == true || multi_fd[help->events[help->i].data.fd].error_response == true || (is_cgi(multi_fd, help->events[help->i].data.fd))))
			{
 				multi_fd[help->events[help->i].data.fd].time_out = clock();
				if( (is_cgi(multi_fd, help->events[help->i].data.fd)) && multi_fd[help->events[help->i].data.fd].running == 0 && multi_fd[help->events[help->i].data.fd].res._listContent == 0)
				{
					try
					{
						if (multi_fd[help->events[help->i].data.fd].running == 0)
						{
							forker(help->events[help->i].data.fd, multi_fd, help);
							multi_fd[help->events[help->i].data.fd].startTime = clock();
						}
						if (multi_fd[help->events[help->i].data.fd].pid == 0 && multi_fd[help->events[help->i].data.fd].res._listContent != 1)
						{
							child_exec(help->events[help->i].data.fd, multi_fd);
						}
						else
						{
							close(multi_fd[help->events[help->i].data.fd].pipefd[1]); 
							continue;
						}
					}
					catch(const ResponseException& e)
					{
						timeoutRes(e.get_message(), e.get_status(), help->events[help->i].data.fd, multi_fd, multi_fd[help->events[help->i].data.fd].res);
						continue;
					}
					
				}
				else if((is_cgi(multi_fd, help->events[help->i].data.fd)) && multi_fd[help->events[help->i].data.fd].running == 1 && multi_fd[help->events[help->i].data.fd].child_exited == 0)
				{
					
					try
					{
						timeOut(help->events[help->i].data.fd, multi_fd);
					}
					catch (const ResponseException &e)
					{
						timeoutRes(e.get_message(), e.get_status(), help->events[help->i].data.fd, multi_fd, multi_fd[help->events[help->i].data.fd].res);
						continue;
					}
				

				}
				if (multi_fd[help->events[help->i].data.fd].child_exited == 1)
				{
					cgi_response(epoll_fd, help->events[help->i].data.fd, multi_fd, multi_fd[help->events[help->i].data.fd].res);
					continue;
				}
				if(!(is_cgi(multi_fd, help->events[help->i].data.fd)) || (is_cgi(multi_fd, help->events[help->i].data.fd) && multi_fd[help->events[help->i].data.fd].res._listContent == 1))
				{
					multi_fd[help->events[help->i].data.fd].res._serverIndex = help->server_index;
					multi_fd[help->events[help->i].data.fd].res.sendResponse(multi_fd, help->events[help->i].data.fd);
				}
			}
			if (((double)(clock() - multi_fd[help->events[help->i].data.fd].time_out)) / CLOCKS_PER_SEC > 20)
			{
				multi_fd[help->events[help->i].data.fd].res._statusCode = "408";
				multi_fd[help->events[help->i].data.fd].res._message = "408 Request Timeout";
				multi_fd[help->events[help->i].data.fd].res._contentType = "text/html";
				multi_fd[help->events[help->i].data.fd].res.sendResponse(multi_fd, help->events[help->i].data.fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, help->events[help->i].data.fd, &multipl);
				close(help->events[help->i].data.fd);
			}
			
		}
	}
	close(help->sosocket);
	return (0);
}