#include "Res.hpp"

Response::Response(){
	_statusCode = "200";
	_message = "OK";
	_contentType = "";
	_contentLength = "";
	_responseHead = "";
	_response = "";
	_errorfileGood = 0;
	_file.open("", std::ios::in);
	_oldURI = "";
	_URI = "";
	_extensions.clear();
	_isDirectory = false;
	_isHeader = false;
	_fileSize = 0;
	convertExtention();
}
Response::~Response(){
	_file.close();
}

void Response::sendResponse(std::map<int, Webserve>&multi_fd ,int fd){
	char buff[BUFFER_SIZE];
	std::cout << "Server index : " << _serverIndex << std::endl;
	if (_statusCode == "301") {
		std::string relativeURI = _URI.substr(_URI.find("/", 30));
		_responseHead += "HTTP/1.1 " + _statusCode + " " + _message + "\r\n";
		_responseHead += "Location: " + relativeURI + "\r\n";
		_responseHead += "Content-Length: " + to_string(0) + "\r\n\r\n";
		_isHeader = true;
		if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
			close(fd);
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
			multi_fd.erase(fd);
			return;
		}
		close(fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		multi_fd.erase(fd);
		return;
	}
	if ((_statusCode == "404" || _statusCode == "403" || _statusCode == "408" || _statusCode == "500"
			|| _statusCode == "501" || _statusCode == "400" || _statusCode == "413"
			|| (_statusCode == "201" && multi_fd[fd].response_success == true)) && !_errorfileGood) {
				std::cout << "status : " << _statusCode << std::endl;
				std::cout << "server index : " << _serverIndex << std::endl;
		std::map<int, std::string>::iterator it = _srv[_serverIndex].errorHolder.find(atoi(_statusCode.c_str()));
		if (it != _srv[_serverIndex].errorHolder.end()){
			_URI = _srv[_serverIndex].errorHolder[atoi(_statusCode.c_str())];
			_file.open(_URI.c_str(), std::ios::in | std::ios::out);
			if (_file.good()){
				_errorfileGood = 1;
				_file.seekg(0, std::ios::end);
				std::streampos fileSize = _file.tellg();
				_file.seekg(0, std::ios::beg);
				_fileSize = static_cast<size_t>(fileSize);
				_contentType = "text/html";
				createHeader(to_string(_fileSize));
				if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
					close(fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					multi_fd.erase(fd);
					return;
				}
			}
			else{
				_statusCode = "404";
				_message = "Not Found";
				createHtmlResponse(multi_fd ,fd, buff);
				return;
			}
			_errorfileGood = 1;
		}
		else{
			createHtmlResponse(multi_fd ,fd, buff);//** send simple html page
			return ;

		}
		
	}
	if(!_isHeader && !_isDirectory){
		_file.open(_URI.c_str(), std::ios::in | std::ios::out);
		if (!_file.good() && !_isDirectory) { // file not found
			_statusCode = "403";
			_message = "Forbidden";
			if (!_isHeader){
				createHeader("0");
				if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
					close(fd);
					epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
					multi_fd.erase(fd);
					return ;
				}
			}
			close(fd);
			return ;
		}
		// file found
		if (_statusCode == "200" && !_isDirectory){
			_file.seekg(0, std::ios::end);
			std::streampos fileSize = _file.tellg();
			_file.seekg(0, std::ios::beg);
			_fileSize = static_cast<size_t>(fileSize);
			createHeaderChunk();// header dyl file
		}
		if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
			close(fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			return ;
		}
	}
	else if (_statusCode == "200" && _isDirectory){//listing directory content
		findFiles(fd);
		close (fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		multi_fd.erase(fd);
		return ;
	}
	else if ((_statusCode == "200" && !_isDirectory) || _errorfileGood) {//chunked start
		char chunkk[BUFFER_SIZE];
		if (!checkHeader(_responseHead)){
			_statusCode = "400";
			_message = "Bad Request";
			createHtmlResponse(multi_fd ,fd, chunkk);
			return ;
		}
		_file.read(chunkk, BUFFER_SIZE);
		std::streamsize bytesRead = _file.gcount();
		if (bytesRead > 0) {
			std::ostringstream chunkHeader;
			chunkHeader << std::hex << bytesRead << "\r\n";
			std::string chunkHeaderStr = chunkHeader.str();

			if (send(fd, chunkHeaderStr.c_str(), chunkHeaderStr.size(), 0) == -1) {
				close(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				multi_fd.erase(fd);
				return;
			}
			if (send(fd, chunkk, bytesRead, 0) == -1) {
				close(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				multi_fd.erase(fd);
				return;
			}
			const char* chunkEndMarker = "\r\n";
			if (send(fd, chunkEndMarker, 2, 0) == -1) {
				close(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				multi_fd.erase(fd);
				return;
			}

			if (bytesRead < BUFFER_SIZE)
				_file.seekg(-bytesRead, std::ios::cur);
		}
		else {
			const char* lastChunk = "0\r\n\r\n";
			if (send(fd, lastChunk, 5, 0) == -1) {
				_errorfileGood = 0;
				close(fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
				multi_fd.erase(fd);
				return;
			}
		}
	}
	return ;
}

int	timeoutRes(std::string message, std::string status, int fd, std::map<int, Webserve>&multi_fd) {
	//check if the reponse is from a cgi program
	// (void)status;
	std::string body;
	std::string response;
	// Response	res;
	// int fd = help->events[help->i].data.fd;
	// int fd = help->events[help->i].data.fd;
	// if (strcmp(res._cgiBody.c_str(), "NULL") != 0) {
	// 	body = res._cgiBody;
	// 	// std::cout << "here is the bodyyyyyyy" << body << " \n";
	// }
	// else {
		std::ostringstream  response_stream;
	// std::cout << "here is the message: " << message << ", and the status is : " << status << std::endl; 
		response_stream << "<!DOCTYPE html>\n"
						<< "<html>\n"
						<< "<style>\n"
						<< "    body {\n"
						<< "        background-color: black;\n"
						<< "        display: flex;\n"
						<< "        justify-content: center;\n"
						<< "        align-items: center;\n"
						<< "        height: 100vh;\n"
						<< "        margin: 0;\n"
						<< "    }\n"
						<< "    h1 {\n"
						<< "        color: white;\n"
						<< "    }\n"
						<< "</style>\n"
						<< "<head><title>" << message << "</title></head>\n"
						<< "<body>\n"
						<< "<h1>" << message << "</h1>\n"
						<< "</body>\n"
						<< "</html>\n";
		body = response_stream.str();
	// }

	// std::cout << "here is the body: " << body << std::endl;
	response += "HTTP/1.1 ";
	response += " " + status + " ok \r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + size_tToString(body.size()) + "\r\n";
	response += "\r\n"; // Empty line to separate headers from body
	response += body;
	// std::cout << "=======\nHere is the response : " << response << std::endl;
	if(send(fd, response.c_str(), response.size(), 0) == -1)
		std::cout << "send response failed ." << std::endl;
	close(fd);
	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
	multi_fd.erase(fd);
	return 1;
}
