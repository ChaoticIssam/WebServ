#include "delete_method.h"

char getLastElement(const char* str) {
    if (str[0] == '\0') {
        return '\0';
    }

    size_t len = std::strlen(str);

    return str[len - 1];
}

void    remove_dir(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, const char* path, Response& res) {
	DIR* dir = opendir(path);
	if (!dir) {
		std::cerr << "Can not open the directory  " << path << std::endl;
		return ;
	}

	struct dirent *entry;
	for ( ; (entry = readdir(dir)) != NULL ; ) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			std::string entry_path;
			if (getLastElement(path) != '/')
				entry_path = std::string(path) + '/' + entry->d_name;
			else
				entry_path = std::string(path) + entry->d_name;
			if (entry->d_type == DT_DIR) {
				if ( (access(entry_path.c_str(), W_OK)) || (access(entry_path.c_str(), X_OK)) ) {
					res._statusCode = "403";
					res._message = "403 Forbidden";
				}
				else
					remove_dir(multi_fd, fd, help, entry_path.c_str(), res);
			}
			else {
				if (access(entry_path.c_str(), W_OK) == 0) {
					if (unlink(entry_path.c_str()) == -1) {
						res._statusCode = "400";
						res._message = "Bad Request";
						return ;
					}
				}
				else {
					res._statusCode = "403";
					res._message = "Forbidden";
				}
			}
		}
	}
	closedir(dir);
	if (rmdir(path) == 0) {
		res._statusCode = "204";
		res._message = "No Content";
	}
}

void    path_check(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {

	if (!multi_fd[fd].resource.empty()) {
		struct stat info;
		const char *path = multi_fd[fd].resource.c_str();

		if (stat(path, &info) == 0) {
			if (info.st_mode && S_ISDIR(info.st_mode)) {

				if ( (access(path, W_OK)) || (access(path, X_OK)) ) {
					res._statusCode = "403";
					res._message = "Forbidden";
				}
				else {
					remove_dir(multi_fd, fd, help, path, res);
				}

			}
			if (info.st_mode && S_ISREG(info.st_mode)) {

				if (multi_fd[fd].resource[multi_fd[fd].resource.length() - 1] == '/') {
					res._statusCode = "404";
					res._message = "Not Found";
				}
				else {
					if (access(path, W_OK) == 0) { 
						if (unlink(path) == -1) {
							res._statusCode = "400";
							res._message = "Bad Request";
						}
						else {
							res._statusCode = "204";
							res._message = "No Content";
						}
					}
					else {
						res._statusCode = "403";
						res._message = "Forbidden";	
					}
				}

			}
		}
		else {
			throw ResponseException("404", "Not found");
		}
	}
	else {
		throw ResponseException("404", "Not Found");
	}
}

void	server_protection(std::map<int, Webserve>&multi_fd, int fd) {

	if (multi_fd[fd].resource.find("/multiplexing") != std::string::npos)
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/config") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/cgi-bin") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/cgi") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/bin") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/get_method") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/Response") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/Videos") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
	if (multi_fd[fd].resource.find("/error") != std::string::npos) 
		throw ResponseException("403", "Forbidden");
}

void    delete_method(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	(void) help;
	multi_fd[fd].resource = res._URI;
	try
	{
		const char	*relative_path = "./";
		const char	*relative_path2 = res._URI.c_str();
		char 		resolved_path[PATH_MAX];
		char		resolved_path2[PATH_MAX];

		char *result = realpath(relative_path, resolved_path);
		char *result2 = realpath(relative_path2, resolved_path2);
		if (access(resolved_path2, F_OK)) {
			throw ResponseException ("404", "Not Found");
		}
		if (result != NULL && result2 != NULL) {
			std::string path(resolved_path);
			std::string urlPath(resolved_path2);
			path += '/';
			urlPath += '/';

			if( urlPath.find(path) == std::string::npos || path == urlPath) {
				throw ResponseException ("403", "Forbidden");
			}
		}
		server_protection(multi_fd, fd);
		path_check(multi_fd, fd, help, res);
	}
	catch (const ResponseException& e)
	{
		res._statusCode = e.get_status();
		res._message = e.get_message();
	}
	res._contentType = "text/html";
	res._errorfileGood = 0;
	res._Rpnse = true;
}