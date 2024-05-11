#include "GetMethod.hpp"

void Response::getMethod(std::map<int, Webserve>& multi_fd, int fd, Helpers* help) {
	convertExtention();
	(void)multi_fd;
	(void)help;
	(void)fd;
	int type = resourceType();
	if (type == NOT_FOUND) {
		if (access(_URI.c_str(), F_OK) == 0){
			_statusCode = "403";
			_message = "Forbidden";
			return ;
		}
		else{
			_statusCode = "404";
			_message = "Not Found";
			return ;
		}
	}
	if (type == DIREC) {
		if (_URI[_URI.length() - 1] != '/') {
			_oldURI = _URI;
			_URI += "/";
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
			if (_statusCode != "301"){
				_statusCode = "200";
				_message = "OK";
			}
			return;
		}
	} else if (type == FILE) {
		std::string extension = getExtension();
		if (extension.empty())
			_contentType = "text/plain";
		else
			_contentType = _extensions[extension];
		std::cout << "Extension: " << _contentType << std::endl;
		_statusCode = "200";
		_message = "OK";
	}
}