#include"webserve.hpp"

void    post_cases(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	size_t pos = multi_fd[(help->events[help->i].data.fd)].headers.find("Transfer-Encoding: chunked");
	size_t pos1 = multi_fd[(help->events[help->i].data.fd)].headers.find("boundry");
	if(pos1 != std::string::npos){
		res._statusCode = "501";
		res._message = "Not Implemented";
		res._contentType = "text/html";
		res._Rpnse = true;
	}
	else if(pos != std::string::npos)
		multi_fd[(help->events[help->i].data.fd)].content_type = 'C';
	else
		multi_fd[(help->events[help->i].data.fd)].content_type = 'L';
}

int    content_type(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	size_t pos1 = multi_fd[(help->events[help->i].data.fd)].headers.find("Content-Type: ");
	if(pos1 != std::string::npos)
	{
		std::string content_type = multi_fd[(help->events[help->i].data.fd)].headers.substr(pos1, multi_fd[(help->events[help->i].data.fd)].headers.length());
		pos1 =  content_type.find("\r\n");
		multi_fd[(help->events[help->i].data.fd)].content_Type = content_type.substr(strlen("Content-Type: "), pos1 - strlen("Content-Type: "));
		return(0);
	}
	else if (pos1 == std::string::npos && multi_fd[help->events[help->i].data.fd].HTTP_method == "POST"){

		// throw ResponseException("400" ,"Bad Request");
		res._statusCode = "400";
		res._message = "Bad Request";
		return(-1);
	}
	return(0);
}


int    content_length(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	int fd = help->events[help->i].data.fd;
	size_t pos = multi_fd[fd].headers.find("Content-Length: ");
	size_t pos1 = multi_fd[(help->events[help->i].data.fd)].headers.find("Transfer-Encoding: chunked");
	if(pos != std::string::npos)
	{
		std::string content_length = multi_fd[fd].headers.substr(pos, multi_fd[fd].headers.length());
		pos =  content_length.find("\r\n");
		multi_fd[fd].len = content_length.substr(strlen("Content-Length: "), pos - strlen("Content-Length: "));
		multi_fd[fd].content_Length = str_to_size_t(multi_fd[fd].len.c_str());
		if(multi_fd[fd].content_Length == 0 && multi_fd[fd].HTTP_method == "POST") {
			res._statusCode = "400";
			res._message = "Bad Request";
			return(-1);
			// throw ResponseException("400" ,"Bad Request");
		}  
	}
	else if (pos == std::string::npos && multi_fd[fd].HTTP_method == "POST" && pos1 == std::string::npos){
		// throw ResponseException("400" ,"Bad Request");
		res._statusCode = "400";
		res._message = "Bad Request";
		return(-1);
	}
	return(0);
}

std::string check_ext(std::map<int , Webserve>&multi_fd, Helpers *help){
	int fd = help->events[help->i].data.fd;
	if(multi_fd[fd].content_Type == "image/png")
		return(".png");
	else if (multi_fd[fd].content_Type == "application/x-httpd-php")
		return (".php");
	else if (multi_fd[fd].content_Type == "application/x-www-form-urlencoded")
		return (".php");
	else if (multi_fd[fd].content_Type == "application/octet-stream")
		return (".py");
	else if(multi_fd[fd].content_Type == "text/css")
		return(".css");
	else if(multi_fd[fd].content_Type == "text/csv")
		return(".csv");
	else if(multi_fd[fd].content_Type == "image/gif")
		return(".gif");
	else if(multi_fd[fd].content_Type == "text/html")
		return(".html");
	else if(multi_fd[fd].content_Type == "image/x-icon")
		return(".ico");
	else if(multi_fd[fd].content_Type == "image/jpeg")
		return(".jpg");
	else if(multi_fd[fd].content_Type == "application/javascript")
		return(".js");
	else if(multi_fd[fd].content_Type == "application/json")
		return(".json");
	else if(multi_fd[fd].content_Type == "application/pdf")
		return(".pdf");
	else if(multi_fd[fd].content_Type == "image/svg+xml")
		return(".svg");
	else if(multi_fd[fd].content_Type == "image/png")
		return(".png");
	else if(multi_fd[fd].content_Type == "text/plain")
		return(".txt");
	else if(multi_fd[fd].content_Type == "video/mp4")
		return(".mp4");
	else if(multi_fd[fd].content_Type == "image/avif")
		return(".avif");
	else if(multi_fd[fd].content_Type == "application/vnd.openxmlformats-officedocument.wordprocessingml.document")
		return(".docx");
	else if(multi_fd[fd].content_Type == "image/gif")
		return(".gif");
	else if(multi_fd[fd].content_Type == "audio/mpeg")
		return(".mp3");
	else if(multi_fd[fd].content_Type == "application/x-httpd-php")
		return(".php");
	else if(multi_fd[fd].content_Type == "audio/wav")
		return(".wav");
	else if(multi_fd[fd].content_Type == "video/webm")
		return(".webm");
	else if(multi_fd[fd].content_Type == "audio/webm")
		return(".weba");
	else if(multi_fd[fd].content_Type == "image/webp")
		return(".webp");
	else if(multi_fd[fd].content_Type == "application/xml")
		return(".xml");
	else if(multi_fd[fd].content_Type == "text/x-python")
		return(".py");
	return "non";
}


void setOut(std::map<int, Webserve>& multi_fd, Helpers* help){
	multi_fd[(help->events[help->i].data.fd)].out = "out_";
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int length = 2;
	std::srand(time(0));
	for (int i = 0; i < length; ++i)
		multi_fd[(help->events[help->i].data.fd)].out += characters[std::rand() % characters.size()];
}

void    get_the_content_of_the_file(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	setOut(multi_fd, help);

	int fd = help->events[help->i].data.fd;
	if(multi_fd[fd].post_cgi == false)
	{
		if(check_ext(multi_fd, help) == "non"){
			multi_fd[fd].res._statusCode = "501";
			multi_fd[fd].res._message = "Not Implemented";
			multi_fd[fd].res._contentType = "text/html";
			multi_fd[fd].res._Rpnse = true;
		}
		else{
			multi_fd[fd].out += check_ext(multi_fd, help);
			std::string fullPath = multi_fd[fd].post_upload + "/" + multi_fd[fd].out;
			multi_fd[fd].out_file.open(fullPath.c_str(), std::ios::binary);
			if (multi_fd[fd].out_file.is_open()) {
				const std::string& body = multi_fd[fd].Body;
				multi_fd[fd].out_file.write(body.c_str(), body.size());
				multi_fd[fd].out_file.close();
				multi_fd[help->events[help->i].data.fd].response_success = true;
				res._statusCode = "201";
				res._message = "Success";
			}
			else{
				res._statusCode = "500";
				res._message = "Internal Server Error";			
				res._Rpnse = true;
			}
		}
	}
	if (multi_fd[fd].post_cgi == true){
		multi_fd[fd].outfile_name = "./to_delete.txt";
		multi_fd[fd].cgi_file.open(multi_fd[fd].outfile_name.c_str());
		if (multi_fd[fd].cgi_file.is_open()) {
			const std::string& body = multi_fd[fd].Body;
			multi_fd[fd].cgi_file << body;
			multi_fd[fd].cgi_file.close();
			multi_fd[fd].post_cgi = true;
		} else {
			std::cerr << "Error opening file '" << multi_fd[fd].outfile_name << std::endl;
		}
	}
}