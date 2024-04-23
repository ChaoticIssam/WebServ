#include"webserve.hpp"

void    request_line(std::map<int , Webserve>&multi_fd, Helpers *help,char *buff, std::string temporaire){
	int fd = help->events[help->i].data.fd;

	size_t pos0;
		multi_fd[fd].line.append(buff, multi_fd[fd].k);
		std::istringstream iss(multi_fd[fd].line);
		pos0 = multi_fd[fd].line.find("\r\n");
	
		if(pos0 != std::string::npos)
		{
			temporaire = multi_fd[fd].line.substr(pos0 + 2);
			getline(iss, multi_fd[fd].HTTP_method, ' ');
			getline(iss, multi_fd[fd].request_URI, ' ');
			getline(iss, multi_fd[fd].HTTP_version, '\r');
			multi_fd[fd].flag = 1;
		}
}

void    create_the_request_file(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	get_the_content_of_the_file(multi_fd, help, res);
	
}

void    get_headers(std::map<int , Webserve>&multi_fd, Helpers *help, std::string temporaire, Response& res){
	size_t pos0;
	int fd = help->events[help->i].data.fd;

		multi_fd[fd].header.append(temporaire.c_str(),temporaire.size());
		pos0 = multi_fd[fd].header.find("\r\n\r\n");
		if(pos0 != std::string::npos)
		{
			temporaire = multi_fd[fd].header.substr(pos0 + 4, multi_fd[fd].header.length() - (pos0 + 4));
			multi_fd[fd].headers = multi_fd[fd].header.substr(0, pos0);
			content_length(multi_fd, help, res);
            content_type(multi_fd, help, res);
            if(multi_fd[fd].HTTP_method == "POST") 
			{
				post_cases(multi_fd, help);
				if(multi_fd[fd].content_type == 'L')
				{
					content_length(multi_fd, help, res);
					multi_fd[fd].dec = atoi(multi_fd[fd].len.c_str());
					multi_fd[fd].dec -= (temporaire.length());
					multi_fd[fd].Body.append(temporaire.c_str(),temporaire.size());
					if(multi_fd[fd].dec <= 0){
						create_the_request_file(multi_fd, help, res);
						return ;
					}
				}
				else if(multi_fd[fd].content_type == 'C'){
					size_t pos1 = temporaire.find("\r\n0\r\n");
					if(pos1 != std::string::npos)
					{
						multi_fd[fd].i = pos0;
						for(; multi_fd[fd].header[multi_fd[fd].i + 4] != '\r'; multi_fd[fd].i++);
						// std::cout << "---------------------------> "  << i - pos0 << std::endl;
						multi_fd[fd].chunk_len = multi_fd[fd].header.substr(pos0 + 4, multi_fd[fd].i);
						multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
						multi_fd[fd].dec = multi_fd[fd].chunk_len_dec;
						temporaire = temporaire.substr(multi_fd[fd].i - pos0 + 2, pos1 - (multi_fd[fd].i - pos0));
						// temporaire = temporaire.substr(0,pos1);
						multi_fd[fd].dec -= (temporaire.length());
						multi_fd[fd].first_chunk.append(temporaire.c_str(),temporaire.size());
						// temporaire = multi_fd[fd].Body.substr(0, multi_fd[fd].Body.length());
						// multi_fd[fd].flag2 = 1;
						if(multi_fd[fd].dec <= 0){
							create_the_request_file(multi_fd, help, res);
							return ;
						}
					}
					else
						multi_fd[fd].first_chunk.append(temporaire.c_str(),temporaire.size());
					
				}
				multi_fd[fd].flag1 = 1;
			}
		}
}

void    get_Body_part(std::map<int , Webserve>&multi_fd, Helpers *help,char *buff, Response& res){
	int fd = help->events[help->i].data.fd;
	if(multi_fd[fd].content_type == 'L'){
			multi_fd[fd].Body.append(buff,multi_fd[fd].k);
			// std::cout << "here is the body: " << multi_fd[fd].Body << std::endl;
			multi_fd[fd].dec -= multi_fd[fd].k;
			if(multi_fd[fd].dec <= 0){
				create_the_request_file(multi_fd, help, res);
				return ;
			}
		}
		else if(multi_fd[fd].content_type == 'C'){
			if(multi_fd[fd].flag2 == 0){
				multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
				size_t p = multi_fd[fd].first_chunk.find("\r\n");
				if(!p)
				{
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(2);
					p = multi_fd[fd].first_chunk.find("\r\n");
				}
				multi_fd[fd].chunk_len = multi_fd[fd].first_chunk.substr(0, p);
				multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
				multi_fd[fd].dec1 = multi_fd[fd].chunk_len_dec;

				if(multi_fd[fd].dec1 == 0){
					create_the_request_file(multi_fd, help, res);
					return ;
				}
				multi_fd[fd].dec1 -= multi_fd[fd].first_chunk.size() ;
				multi_fd[fd].dec1 += p + 2;
				multi_fd[fd].flag2 = 1;
			}
			else
			{
				multi_fd[fd].dec1 -= multi_fd[fd].k;
				if(multi_fd[fd].dec1 == 0)
				{
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].first_chunk.find("\r\n") + 2);
					multi_fd[fd].Body.append(multi_fd[fd].first_chunk, multi_fd[fd].first_chunk.length());
					multi_fd[fd].first_chunk.clear();
					multi_fd[fd].flag2 = 0;
				
				}
				else if(multi_fd[fd].dec1 < 0)
				{
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].first_chunk.find("\r\n") + 2);
					multi_fd[fd].Body.append(multi_fd[fd].first_chunk.c_str(), multi_fd[fd].chunk_len_dec);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].chunk_len_dec);
					{
						size_t p = multi_fd[fd].first_chunk.find("\r\n");
						if(p == std::string::npos)
						{
							multi_fd[fd].flag2 = 0;
							return;
						}
						else
						{
							multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(2);
						}
						p = multi_fd[fd].first_chunk.find("\r\n");
						if(p == std::string::npos)
						{
							multi_fd[fd].flag2 = 0;
							return;
						}
						multi_fd[fd].chunk_len = multi_fd[fd].first_chunk.substr(0, p);
						multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
						multi_fd[fd].dec1 = multi_fd[fd].chunk_len_dec;

						if(multi_fd[fd].dec1 == 0){
							create_the_request_file(multi_fd, help, res);
							return ;
						}
						multi_fd[fd].dec1 -= multi_fd[fd].first_chunk.size();
						multi_fd[fd].dec1 += p + 2;
						multi_fd[fd].flag2 = 1;
					}
				}
				else if(multi_fd[fd].dec1 > 0)
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
			}                
		}
}

std::string size_tToString(size_t value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}


// void create_response(std::map<int, Webserve>&multi_fd, std::string message, std::string status, Helpers *help) {
// 	//check if the reponse is from a cgi program
// 	(void)status;
// 	std::string body;
// 	// Response	res;
// 	int fd = help->events[help->i].data.fd;
// 	if (strcmp(multi_fd[fd].content_body.c_str(), "NULL") != 0) {
// 	std::cout << "here is the bodyyyyyyy \n";
// 		body = cgi_body_getter(multi_fd[fd].content_body, multi_fd, fd);
// 	}
// 	else {
// 		std::ostringstream  response_stream;
// 	// std::cout << "here is the message: " << message << ", and the status is : " << status << std::endl; 
// 		response_stream << "<!DOCTYPE html>\n"
// 						<< "<html>\n"
// 						<< "<head><title>" << message << "</title></head>\n"
// 						<< "<body>\n"
// 						<< "<h1>" << message << "</h1>\n"
// 						<< "</body>\n"
// 						<< "</html>\n";
// 		body = response_stream.str();
// 	}

// 	// // std::cout << "here is the body: " << body << std::endl;
// 	// 	multi_fd[fd].response += multi_fd[fd].HTTP_version + " " + status + " ok \r\n";
// 	// 	// multi_fd[fd].response += "Content-Type: " + res._contentType + "\r\n";
// 	// 	multi_fd[fd].response += "Content-Type: text/html\r\n";
// 	// 	// multi_fd[fd].response += "Content-Length: " + size_tToString((body).size()) + "\r\n";
// 	// 	multi_fd[fd].response += "Content-Length: " + size_tToString(body.size()) + "\r\n";
// 	// 	multi_fd[fd].response += "\r\n\r\n"; // Empty line to separate headers from body
// 	// 	multi_fd[fd].response += body;
// 	// std::cout << "=======\nHere is the response : " << multi_fd[fd].response << std::endl;
// 	// if(send(fd, multi_fd[fd].response.c_str(), multi_fd[fd].response.size(), 0) == -1)
// 	// 	std::cout << "send response failed ." << std::endl;
// 	// // std::cout << "->>>>>>>>>>>>> content type : " << res._contentType << std::endl;
// 	// // res._responseHead = "HTTP/1.1 200 OK\r\n";
// 	// // if (multi_fd[fd].HTTP_method == "GET")
// 	// // 	res._responseHead += "Content-Type: " + res._contentType + "\r\n";
// 	// // res._responseHead += "Content-Length: ";
// 	// // res._responseHead += res._contentLength + "\r\n\r\n";
// 	// // send(fd, res._responseHead.c_str(), res._responseHead.length(), 0);
// 	// close(fd);
// 	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
// 	multi_fd.erase(fd);
// }

void    pars_request(Response &res, std::map<int , Webserve>&multi_fd, Helpers *help,char *buff){
	int fd = help->events[help->i].data.fd;
	// Response res;
	
	// std::cout << "here is the buff: " << buff << std::endl;
	std::string newbuffer, bufbody, bufbody1;
	if (buff != NULL) {
		std::string temporaire(buff,multi_fd[fd].k);
		if(multi_fd[fd].flag != 1)
			request_line(multi_fd, help, buff, temporaire);
		if(multi_fd[fd].flag1 != 1 && multi_fd[fd].flag == 1)
		{
			// std::cout << "heeeeeeeeeeeeeeeer\n";
			get_headers(multi_fd, help, temporaire, res);
			if(multi_fd[fd].HTTP_method == "POST")
				return ;
		}
	}

	multi_fd[fd].content_body = "NULL";
	multi_fd[fd].content_l = 0;

	// try { //try catch blocks for response and errors
		//cgi
		// std::cout << "status code: " << res._statusCode << " and message : " << res._message <<std::endl;
		res.uriParss(multi_fd, fd, help);
		if(multi_fd[fd].HTTP_method == "POST")
		{
			if(multi_fd[fd].flag1 == 1)
				get_Body_part(multi_fd, help, buff, res);
		}
		// //hna ghanzid get method dyali//
		else if (multi_fd[fd].HTTP_method == "GET") {
			res.getMethod(multi_fd, fd, help);
			res._Rpnse = true;
		}
		else if (multi_fd[fd].HTTP_method == "DELETE") {
		    delete_method(multi_fd, fd, help, res);
		}
		// std::cout << "\t\t\t status code: " << res._statusCode << " and message : " << res._message <<std::endl;
		// std::cout << ">>>>>>>>>>>>> here is the uri: " << res._URI << std::endl;
		// res.sendResponse(multi_fd, fd);
		// cgi_handler(multi_fd, fd, help, res);
		// std::cout << "here is the content length: " << multi_fd[fd].content_l << std::endl;
		// throw ResponseException("200", "OK");

}

// void    call_request_functions(Response &res,std::map<int , Webserve>&multi_fd, Helpers *help,char *buff) {
// 	pars_request(res,multi_fd, help,buff);
// }