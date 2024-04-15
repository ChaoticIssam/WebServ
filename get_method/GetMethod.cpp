#include "GetMethod.hpp"

GET::GET(){}

bool checkLocation(std::string &path)   {
    
    if (access(path.c_str(), F_OK) == 0)
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

void	GET::findFiles(){
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

void    GET::convertExtention() {
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

int	GET::resourceType(){
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

std::string GET::getExtension()const{
	size_t pos = _URI.rfind(".");
	if (pos == std::string::npos)
		return ("");
	else
		return _URI.substr(pos + 1, std::string::npos);
}

void	getMethod(std::map<int, Webserve>&multi_fd, int fd, Helpers *help){
	GET	g;
	int	type = g.resourceType();
	if (type == NOT_FOUND){
		if ((multi_fd[fd].request_URI.c_str(), F_OK) == 0)
			throw ResponseException("403", "Forbidden");
			// _statusCode = 403;
		else
			throw ResponseException("404", "Not Found");
			// _statusCode = 404;
	}
	else if (type == DIREC){
		if (multi_fd[fd].request_URI[multi_fd[fd].request_URI.size() - 1] != '/'){
			g._oldURI += "/";
			g._statusCode = 301;
			return ;
		}
		else{
			for (std::vector<location>::iterator it = help->obj._locationScoops.begin(); it != help->obj._locationScoops.end(); it++){
				if (multi_fd[fd].request_URI.find(help->locationScoop._locationPath) == 0){
					if (!help->locationScoop._Index.empty() || !help->obj._rootIndex.empty()){//hna khassni location lijat frequest
						if (!help->locationScoop._Index.empty())
							multi_fd[fd].request_URI += help->locationScoop._Index;
						else if (!help->obj._rootIndex.empty())
							multi_fd[fd].request_URI += help->obj._rootIndex;
						std::string extension = g.getExtension();
						// std::map<std::string, std::string>::iterator it = help->locationScoop._cgiPath.find(extension);
						g._file.open(multi_fd[fd].request_URI.c_str(), std::ios::in);
						if (!g._file.good()){
							throw ResponseException("403", "Forbidden");
							// _statusCode = 403;
							return;
						}
						g._contentType = g._extensions[g.getExtension()];
					}
					else{
						if (help->locationScoop._autoIndex){
							g.findFiles(); //***
							g._contentType = "text/html";
							return ;
						}
						else{
							throw ResponseException("403", "Forbidden");
							// _statusCode = 403;
							return ;
						}
					}
				}
			}
		}
	}
	else if (type == FILE){
		std::string extension = g.getExtension();
		// std::map<std::string, std::string>::iterator it = help->locationScoop._cgiPath.find(extension);
		g._file.open(multi_fd[fd].request_URI.c_str(), std::ios::in | std::ios::out);
		if (!g._file.good()){
			throw ResponseException("403", "Forbidden");
			// _statusCode = 403;
			return ;
		}
		if (g._extensions[g.getExtension()].empty())
			g._contentType = "text/plain";
		else
			g._contentType = g._extensions[g.getExtension()];
		return ;
	}
}