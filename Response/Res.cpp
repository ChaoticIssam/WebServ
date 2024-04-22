#include "Res.hpp"

Response::Response(){
	// std::cout << "Response constructor" << std::endl;
	_statusCode = "200";
	_message = "OK";
	_contentType = "";
	_contentLength = "";
	_responseHead = "";
	// _responseBUFFER = NULL;
	_response = "";
	_errorfileGood = 0;
	_file.open("", std::ios::in);
	_oldURI = "";
	_URI = "";
	_extensions.clear();
	_isDirectory = false;
	_isHeader = false;
	_fileSize = 0;
	// _Rpnse = false;
	convertExtention();
}
Response::~Response(){
	// std::cout << "Response destructor" << std::endl;
	_file.close();
}

bool Response::checkLocation(std::string &path)   {
    // std::cout << "path is : " << path << std::endl;
    if (access(path.c_str(), F_OK) == 0){
		// std::cout << "dkhlat true" << std::endl;
        return true;
	}
    else {
        size_t pos = path.find("%20");
        while (pos != std::string::npos)   {
            std::string tmp = path.substr(0, pos);
            tmp += " ";
            tmp += path.substr(pos + 3);
            if (access(tmp.c_str(), F_OK) == 0) {
                path = tmp;
                return true;
            }
            pos = path.find("%20", pos + 1);
        }
        return false;
    }
}

void	Response::findFiles(int fd){
	(void)fd;
	DIR *dir;
	struct dirent *d;
	struct stat s;
	char buff[18];
	std::map<std::string, unsigned char> files;
	std::string file, name, path;
	unsigned char type;
	file = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of ";
	file += _oldURI;
	file += "</title>\n</head>\n<body>\n<h1>Content: ";
	file += _oldURI + "</h1>\n<hr>\n<pre>\n<table>\n<tbody>\n";
	dir = opendir(_URI.c_str());
	if (dir){
		d = readdir(dir);
		while (d){
			name = d->d_name;
			if (name == "." || (name != ".." && name[0] == '.')){
				d = readdir(dir);
				continue;
			}
			type = d->d_type;
			files.insert(std::pair<std::string, unsigned char>(name, type));
            d = readdir(dir);
		}
		for(std::map<std::string, unsigned char>::iterator it = files.begin(); it != files.end(); it++) {
            name = it->first;
            type = it->second;
			path = _URI + "/" + name;
			stat(path.c_str(), &s);
			file += "<tr>\n<td><a href=" + name;
			if (type == DT_DIR)
				file += "/>" + name + "/</a></td>\n";
			else
				file += ">" + name + "</a></td>\n";
			strftime(buff, sizeof(buff), "%d-%b-%Y %H:%M", gmtime(&s.st_mtim.tv_sec));
			file += "<td>";
			file += buff;
			file += "</td>";
            file += "<td>";
            std::stringstream ss;
            std::string tmp;
            ss << s.st_size;
            ss >> tmp;
            if (type == DT_DIR)
                file += "-";
            else
                file += tmp;
            file += "</td></tr>\n";
		}
		file += "</tbody>\n</table>\n<hr>\n</pre>\n</body>\n</html>";
	}
	_statusCode = "200";
	_message = "OK";
	_isDirectory = true;
	// closedir(dir);
    // std::string tmp = "/home/";
    // tmp += "chaotic";
    // tmp += "/.cache/autoindex.html"; 
    // _file.open(tmp.c_str(), std::ios::out | std::ios::trunc);
    // if (!_file.good())  {
	// 	std::cout << "not good" << std::endl;
	// 	_statusCode = "403";
	// 	_message = "Forbidden";
	// 	return ;
    // }
    _file.write(file.c_str(), file.length());
    // std::stringstream sg;
    // sg << file.length();
    // sg >> _contentLength;
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	// response += "Content-Length: " + std::to_string(file.length()) + "\r\n";
	response += "\r\n";
	response += file;
	if (send(fd, response.c_str(), response.length(), 0) == -1){
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		// multi_fd.erase(fd);
		return ;
	}
    _file.close();
    // _file.open(tmp.c_str(), std::ios::in);
}

void    Response::convertExtention() {
    const char* ext[] = {"avif", "css", "csv", "docx", "gif", "html", "jpeg",
        "jpg", "js", "json", "mp3", "mp4", "png", "pdf", "php", "txt", "wav",
        "weba", "webm", "webp", "xml", ""};
    const char* mime[] = {"image/avif", "text/css", "text/csv", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        "image/gif", "text/html", "image/jpeg", "image/jpeg", "text/javascript",
        "application/json", "audio/mpeg", "video/mp4", "image/png", "application/pdf",
        "application/x-httpd-php", "text/plain", "audio/wav", "audio/webm", "video/webm",
        "image/webp", "application/xml", "application/octet-stream"};
    size_t size = sizeof(ext) / sizeof(ext[0]);
    for(size_t i = 0; i < size; i++)    {
        _extensions[ext[i]] = mime[i];
    }
}

int	Response::resourceType(){
	if (!checkLocation(_URI))
		return NOT_FOUND;
	DIR *dir = opendir(_URI.c_str());
	if (dir){
		closedir(dir);
		return DIREC;
	}
	std::fstream file(_URI.c_str());
	if (!file.fail()){
		file.close();
		return FILE;
	}
	return NOT_FOUND;
}

void   Response::uriParss(std::map<int, Webserve>& multi_fd, int fd,Helpers* help)   {
	_URI = multi_fd[fd].request_URI;
	for(std::vector<location>::iterator it = help->obj._locationScoops.begin(); it != help->obj._locationScoops.end(); it++){
		if (_URI.find((*it)._locationPath) == 0){
			if ((*it)._autoIndex == 0){
				_statusCode = "403";
				_message = "Forbidden";
				return ;
			}
			if (!(*it)._Index.empty() || !(*it)._rootDirectoryLocation.empty() || !help->obj._rootDirectory.empty()){
				std::string first = _URI;
				_URI.clear();
				if (!(*it)._rootDirectoryLocation.empty())
					_URI = (*it)._rootDirectoryLocation + first;
				else if (!help->obj._rootDirectory.empty())
					_URI = help->obj._rootDirectory + first;
				if (!(*it)._Index.empty())
					_URI += (*it)._Index;
				// std::cout << "1............................................................. " << _statusCode << std::endl;
				return ;
			}
		}
	}
	_statusCode = "404";
	_message = "Not Found";
}

std::string Response::getExtension()const{
	size_t pos = _URI.rfind(".");
	if (pos == std::string::npos)
		return ("");
	else
		return _URI.substr(pos + 1, std::string::npos);
}
std::string to_string(size_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void Response::createHeader(size_t contentLength) {
	_responseHead += "HTTP/1.1 " + _statusCode + " " + _message + "\r\n";
	_responseHead += "Content-Type: " + _contentType + "\r\n";
	_responseHead += "Content-Length: " + to_string(contentLength) + "\r\n\r\n";
	_isHeader = true;
}

void Response::createHeaderChunk() {
    _responseHead += "HTTP/1.1 " + _statusCode + " " + _message + "\r\n";
    _responseHead += "Content-Type: " + _contentType + "\r\n";
	_responseHead += "Transfer-Encoding: chunked\r\n"; 
    _responseHead += "\r\n";
	// std::cout << ">>>>>>>>>>>>>>>>> contentLength: " << contentLength << std::endl;
    // _responseHead += "Content-Length: " + to_string(contentLength) + "\r\n\r\n";
    _isHeader = true;
}



void Response::sendResponse(std::map<int, Webserve>&multi_fd ,int fd){
	char *buff = new char[BUFFER_SIZE];
	if(!_isHeader){
		std::cout << "HEADER if condition \n";
		if (_statusCode == "301"){
			std::cout << "301 if condition \n";
			createHeader(1000);
			send(fd, _responseHead.c_str(), _responseHead.length(), 0);
			close(fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			std::cout << "first erase\n";
			return ;
		}
		if (_statusCode == "404" || _statusCode == "403" || _statusCode == "500" || _statusCode == "501" || _statusCode == "505"){
			std::cout << "404 if condition \n";
			std::string send_message;
			std::ostringstream  response_stream;
			_contentType = "text/html";
			createHeader(1200);
			send(fd, _responseHead.c_str(), _responseHead.length(), 0);
			response_stream << "<!DOCTYPE html>\n"
						<< "<html>\n"
						<< "<head>\n"
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
						<< "<title>" << _message << "</title>\n"
						<< "</head>\n"
						<< "<body>\n"
						<< "<h1>" << _message << "</h1>\n"
						<< "</body>\n"
						<< "</html>\n";
			send_message = response_stream.str();
			strcpy(buff, send_message.c_str());
			send(fd, buff, send_message.length(), 0);

			close(fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			std::cout << "second erase\n";
			return ;
			
		}
		std::cout << "FINNA OPEN FILE \n";
		_file.open(_URI.c_str(), std::ios::in | std::ios::out);
		if (!_file.good() && !_isDirectory) {
			std::cout << "file not good condition" << std::endl;
			_errorfileGood = errno;
			_statusCode = "403";
			_message = "Forbidden";
			if (!_isHeader){
				std::cout << "NO HEADER condition\n";
				createHeader(_fileSize);
				if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
					close(fd);
					epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
					multi_fd.erase(fd);
					std::cout << "third erase\n";
					return ;
				}
			}
			close(fd);
			return ;
		}

		_file.seekg(0, std::ios::end);
		std::streampos fileSize = _file.tellg();
		_file.seekg(0, std::ios::beg);
		_fileSize = (size_t)fileSize;
		if (_statusCode == "200" && !_isDirectory)
			createHeaderChunk();
		else if (_statusCode == "200" && _isDirectory)
			createHeader(_fileSize);
		if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
			close(fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			std::cout << "fourth erase\n";
			return ;
		}
	}
	else if (_statusCode == "200" && _isDirectory){
		std::cout << "isDirectory: " << std::endl;
			findFiles(fd);
			close (fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			std::cout << "fifth erase\n";
			return ;
	}
else if ((_statusCode == "200") && !_isDirectory) {//chunked start
    char buff[BUFFER_SIZE];
    _file.read(buff, BUFFER_SIZE);
    std::streamsize bytesRead = _file.gcount();
    if (bytesRead > 0) {
        std::ostringstream chunkHeader;
        chunkHeader << std::hex << bytesRead << "\r\n"; // Chunk size in hexadecimal
        std::string chunkHeaderStr = chunkHeader.str();

        if (send(fd, chunkHeaderStr.c_str(), chunkHeaderStr.size(), 0) == -1) {
            close(fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            multi_fd.erase(fd);
			std::cout << "sixth erase\n";
            return;
        }

        if (send(fd, buff, bytesRead, 0) == -1) {
            close(fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            multi_fd.erase(fd);
			std::cout << "seventh erase\n";
            return;
        }

        // Send the end of chunk marker (\r\n)
        const char* chunkEndMarker = "\r\n";
        if (send(fd, chunkEndMarker, 2, 0) == -1) {
            close(fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            multi_fd.erase(fd);
			std::cout << "eighth erase\n";
            return;
        }

        if (bytesRead < BUFFER_SIZE)
            _file.seekg(-bytesRead, std::ios::cur);
    }
    else {
        // Send the last chunk (zero length)
        const char* lastChunk = "0\r\n\r\n";
        if (send(fd, lastChunk, 5, 0) == -1) {
            close(fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            multi_fd.erase(fd);
			std::cout << "ninth erase\n";
            return;
        }
    }
}//chunked end
	else if (!_isDirectory){

		std::ostringstream  response_stream;
		_contentType = "text/html";
		createHeader(_fileSize);
		strcpy(buff, _responseHead.c_str());
		if (send(fd, buff, _responseHead.length(), 0) == -1){
			close(fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
			multi_fd.erase(fd);
			std::cout << "tenth erase\n";
			return ;
		}
    	response_stream << "<!DOCTYPE html>\n"
                    << "<html>\n"
                    << "<head>\n"
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
                    << "<title>" << _message << "</title>\n"
                    << "</head>\n"
                    << "<body>\n"
                    << "<h1>" << _message << "</h1>\n"
                    << "</body>\n"
                    << "</html>\n";
		_response = response_stream.str();
		strcpy(buff, _response.c_str());
		send(fd, buff, _response.length(), 0);
	}
	std::cout << "everything is done\n";
	return ;
}