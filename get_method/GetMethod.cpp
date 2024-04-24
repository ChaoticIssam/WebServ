#include "GetMethod.hpp"

void Response::getMethod(std::map<int, Webserve>& multi_fd, int fd, Helpers* help) {
	convertExtention();
	(void)multi_fd;
	(void)help;
	(void)fd;
	int type = resourceType();
	std::cout << "type: " << type << std::endl;
	if (type == NOT_FOUND) {
		if (access(_URI.c_str(), F_OK) == 0){
			_statusCode = "403";
			_message = "Forbidden";
			return ;
		}
			// throw ResponseException("403", "Forbidden - 1");
		else{
			std::cout << "not found" << std::endl;
			_statusCode = "404";
			_message = "Not Found";
			return ;
		}
			// throw ResponseException("404", "Not Found - 1");
	}
	if (type == DIREC) {
		if (_URI[_URI.length() - 1] != '/') {
			_oldURI = _URI;
			_URI += "/";
			// std::cout << "hna >>>URI: " << _URI << std::endl;
			_statusCode = "301";
			_message = "Moved Permanently";
			return;
		} 
		else if (access(_URI.c_str(), X_OK) == -1){
			_statusCode = "403";
			_message = "Forbidden";
			return;
		}
		else {
			_contentType = "text/html";
			_isDirectory = true;
			std::cout << "1 - status code : " << _statusCode << std::endl;
			if (_statusCode != "301"){
				_statusCode = "200";
				_message = "OK";
			}
			return;
		}
	} else if (type == FILE) {
		std::string extension = getExtension();
		if (_extensions[extension].empty())
			_contentType = "text/plain";
		else
			_contentType = _extensions[extension];
		_statusCode = "200";
		_message = "OK";
	}
}