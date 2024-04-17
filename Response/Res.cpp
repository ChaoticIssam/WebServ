#include "Res.hpp"

Response::Response(){}
Response::~Response(){}

bool Response::checkLocation(std::string &path)   {
    std::cout << "path is : " << path << std::endl;
    if (access(path.c_str(), F_OK) == 0){
		std::cout << "dkhlat true" << std::endl;
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

void	Response::findFiles(){
	DIR *dir;
	struct dirent *d;
	struct stat s;
	char buff[18];
	std::map<std::string, unsigned char> files;
	std::string file, name, path;
	unsigned char type;
	file = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of ";
	file += _oldURI;
	file += "</title>\n</head>\n<body>\n<h1>Index of ";
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
	closedir(dir);
    std::string tmp = "/nfs/homes/";
    tmp += _USER;
    tmp += "/.cache/autoindex.html"; 
    _file.open(tmp.c_str(), std::ios::out | std::ios::trunc);
    if (!_file.good())  {
        _statusCode = 404;
        return;
    }
    _file.write(file.c_str(), file.length());
    std::stringstream sg;
    sg << file.length();
    sg >> _contentLength;
    _file.close();
    _file.open(tmp.c_str(), std::ios::in);
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
			if (!(*it)._Index.empty() || !help->obj._rootDirectory.empty()){
				std::string first = _URI;
				_URI.clear();
				_URI = help->obj._rootDirectory + first;
				if (!(*it)._Index.empty())
					_URI += (*it)._Index;
				std::cout << "............................................................. " << _URI << std::endl;
			}
		}
		else
			ResponseException("404", "Not Found - 2");
	}
}

std::string Response::getExtension()const{
	size_t pos = _URI.rfind(".");
	if (pos == std::string::npos)
		return ("");
	else
		return _URI.substr(pos + 1, std::string::npos);
}