#include "GetMethod.hpp"

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

void	Response::findFiles(){
	DIR *dir;
	struct dirent *d;
	struct stat s;
	char buff[18];
	std::map<std::string, unsigned char> files;
	std::string file, name, path;
	unsigned char type;
	file = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of ";
	file += _old_uri;
	file += "</title>\n</head>\n<body>\n<h1>Index of ";
	file += _old_uri + "</h1>\n<hr>\n<pre>\n<table>\n<tbody>\n";
	dir = opendir(_uri.c_str());
	if (dir){
		d = readdir(dir);
		while (d){
			name = d->d_name;
			if (name = "." || (name != ".." && name[0] == ".")){
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
			path = _uri + "/" + name;
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
    tmp += USER;
    tmp += "/.cache/autoindex.html"; 
    _file.open(tmp.c_str(), std::ios::out | std::ios::trunc);
    if (!_file.good())  {
        _status_code = 404;
        return;
    }
    _file.write(file.c_str(), file.length());
    std::stringstream sg;
    sg << file.length();
    sg >> _content_length;
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

std::string Response::getExtension()const{
	size_t pos = _URI.rfind(".");
	if (pos == std::string::npos)
		return ("");
	else
		return _URI.substr(pos + 1, std::string::npos);
}

void	Response::getMethod(){
	int	type = resourceType();
	if (type == NOT_FOUND){
		if ((_URI.c_str(), F_OK) == 0)
			_statusCode = 403;
		else
			_statusCode = 404;
	}
	else if (type == DIREC){
		if (_URI[_URI.size() - 1] != '/'){
			_oldURI += "/";
			_statusCode = 301;
			return ;
		}
		else{
			if (!_locationScoop._Index.empty() || !config._rootIndex.empty()){
				if (!_locationScoop._Index.empty())
					_URI += _locationScoop.Index;
				else if (!config._rootIndex.empty())
					_URI += config._rootIndex;
				for (int y = 0; CGI->env[y]; y++){
					std::string tmp = CGI->env[y];
					if(tmp.find("SCRIPT_FILENAME") != std::string::npos){
						tmp = tmp.substr(0, tmp.find("=") + 1);
						tmp += _URI;
						delete CGI->env[y];
						CGI->env[y] = const_cast<char*>((new std::string(tmp))->c_str());
						break;
					}
				}
				std::string extension = getExtension();
				std::map<std::string, std::string>::iterator it = locationScoop.cgi_path.find(extension);
				if ((extension == "php" && it != locationScoop.cgi_path.end()) || (extension == "py" && it != locationScoop.cgi_path.end())){
					try{
						if (extension == "php")
							CGI->phpSet("");//**set php (add it in CGI code)
						else
							CGI->pySet("");//**set py (add it in CGI code)
					}
					catch (std::exception &e){
						std::cout << e.what() << std::endl;
					}
					if (statusCode == 200){
						_file.open(_fileName.c_str(), std::ios::in);
						if (!_file.good()){
							std::cout << "fail" << std::endl;
							statusCode = 403;
							return;
						}
						return ;
					}

				}
			else{
				_file.open(_URI.c_str(), std::ios::in);
				if (!file.good()){
					statusCode = 403;
					return;
					}
				_contentType = _extensions[convertExtention()];
				}
			}
			else{
				if (locationScoop._autoIndex){
					findFiles(); //***
					_contentType = "text/html";
					return ;
				}
				else{
					_statusCode = 403;
					return ;
				}
			}
		}
	}
	else if (type == FILE){
		std::string extension = getExtension();
		std::map<std::string, std::string>::iterator it = locationScoop.cgi_path.find(extension);
		if ((extension == "php" && it != locationScoop.cgi_path.end()) || (extension == "py" && it != locationScoop.cgi_path.end())){
			try{
				if (extension == "php")
					CGI->phpSet("");
				else
					CGI->pySet("");
			}
			catch(const std::exception& e){
				std::cerr << e.what() << std::endl;
			}
			if (_statusCode == 200){
				_file.open(_fileName.c_str(), std::ios::in);
				if (!_file.good()){
					std::cout << "fail" << std::endl;
					_statusCode = 403;
					return ;
				}
				_contentType = "text/html";
			}
		}
		else{
			_file.open(_URI.c_str(), std::ios::in | std::ios::out);
			if (!_file.good()){
				_statusCode = 403;
				return ;
			}
			if (_extensions[getExtension()].empty())
				_contentType = "text/plain";
			else
				_contentType = _extensions[getExtension()];
		}
		return ;
	}
}