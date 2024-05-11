#include "Res.hpp"

bool Response::checkLocation(std::string &path)   {
    if (access(path.c_str(), F_OK) == 0 && access(path.c_str(), R_OK) == 0)
        return true;
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
    _file.write(file.c_str(), file.length());
	std::string response;
	response += file;
	_contentLength = to_string(response.size());
	createHeader(_contentLength);
	closedir(dir);
	dir = NULL;
	if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		return ;
	}
	if (send(fd, response.c_str(), response.length(), 0) == -1){
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		return ;
	}
    _file.close();

}

void    Response::convertExtention() {
    const char* ext[] = {"avif", "css", "csv", "docx", "gif", "html", "jpeg",
        "jpg", "js", "json", "mp3", "mp4", "png", "pdf", "php", "txt", "wav",
        "weba", "webm", "webp", "xml", "py"};
    const char* mime[] = {"image/avif", "text/css", "text/csv", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        "image/gif", "text/html", "image/jpeg", "image/jpeg", "text/javascript",
        "application/json", "audio/mpeg", "video/mp4", "image/png", "application/pdf",
        "application/x-httpd-php", "text/plain", "audio/wav", "audio/webm", "video/webm",
        "image/webp", "application/xml", "text/x-python", "application/octet-stream"};
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

void   Response::uriParss(std::map<int, Webserve>& multi_fd, int fd,Helpers* help, int serverIndex)   {
    size_t pos = multi_fd[fd].request_URI.find("?");
    _URI = multi_fd[fd].request_URI.substr(0, pos);
    _serverIndex = serverIndex;
    std::vector<location>& locationScoops = _srv[serverIndex]._locationScoops;
    for (std::vector<location>::iterator it = locationScoops.begin(); it != locationScoops.end(); it++){
        if (_URI.find((*it)._locationPath) == 0){
            multi_fd[fd].postCheck = (*it)._postCheck;
            multi_fd[fd].deleteCheck = (*it)._deleteCheck;
            multi_fd[fd].getCheck = (*it)._getCheck;
            multi_fd[fd].post_upload = (*it)._uploadStore;

			if (!(*it)._return.empty()){
				std::cout << "return \n";
				_URI.clear();
				_URI = (*it)._return;
				return;
            }
            else if (!(*it)._Index.empty() || !(*it)._rootDirectoryLocation.empty() || !help->obj._rootDirectory.empty()){
                std::string first = _URI;
                _URI.clear();
				std::cout << "auto index: " << (*it)._autoIndex << std::endl;
                if (!(*it)._rootDirectoryLocation.empty())
                    _URI = (*it)._rootDirectoryLocation + first;
                else if (!help->obj._rootDirectory.empty())
                    _URI = help->obj._rootDirectory + first;
                if (!(*it)._Index.empty())
                    _URI += (*it)._Index;
				else if ((*it)._autoIndex == 0){
					_statusCode = "403";
					_message = "Forbidden";
					return ;
            	}
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

void Response::createHeader(std::string contentLength) {
	_responseHead += "HTTP/1.1 " + _statusCode + " " + _message + "\r\n";
	_responseHead += "Content-Type: " + _contentType + "\r\n";
	_responseHead += "Content-Length: " +contentLength + "\r\n\r\n";
	_isHeader = true;
}

void Response::createHeaderChunk() {
    _responseHead += "HTTP/1.1 " + _statusCode + " " + _message + "\r\n";
    _responseHead += "Content-Type: " + _contentType + "\r\n";
	_responseHead += "Transfer-Encoding: chunked\r\n"; 
    _responseHead += "\r\n";
    _isHeader = true;
}

bool checkHeader(std::string &header) {
    size_t pos = header.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;
    return true;
}

void    Response::createHtmlResponse(std::map<int, Webserve>&multi_fd ,int fd, char *buff){
    std::string send_message;
	std::ostringstream  response_stream;
	_contentType = "text/html";
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
	std::string contentLength = to_string(send_message.length());
	createHeader(contentLength);
	if (send(fd, _responseHead.c_str(), _responseHead.length(), 0) == -1){
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		multi_fd.erase(fd);
		return;
	}
	strcpy(buff, send_message.c_str());
	if (send(fd, buff, send_message.length(), 0) == -1){
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
		multi_fd.erase(fd);
		return;
	}
	close(fd);
	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
	multi_fd.erase(fd);
}
