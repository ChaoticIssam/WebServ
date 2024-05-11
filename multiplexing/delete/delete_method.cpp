// #include "delete_method.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "delete_method.h"

char getLastElement(const char* str) {
    if (str[0] == '\0') {
        return '\0';
    }

    size_t len = std::strlen(str);

    return str[len - 1];
}

void    remove_dir(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, const char* path, Response& res) {
	// std::cout << "here is the path " << path << std::endl;
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
				// std::cout << "new path: " << entry_path << std::endl;
				if ( (access(entry_path.c_str(), W_OK)) || (access(entry_path.c_str(), X_OK)) ) { //directory has no write or execute permission
					// throw ResponseException("403", "403 forbidden");
					res._statusCode = "403";
					res._message = "403 Forbidden";
				}
				else
					remove_dir(multi_fd, fd, help, entry_path.c_str(), res);
			}
			else {
				// std::cout << "here is the entry name: " << entry_path<< std::endl;
				if (access(path, W_OK) == 0) { //file has no write permissions
						// continue ;
					if (unlink(entry_path.c_str()) == -1) {
						// throw ResponseException("400", "Bad Request");
						res._statusCode = "400";
						res._message = "400 Bad Request";
						return ;
					}
				}
				else {
					res._statusCode = "403";
					res._message = "403 Forbidden";
					// throw ResponseException("403", "403 forbidden");
				}
			}
		}
	}
	closedir(dir);
	// Remove the empty directory itself
	if (rmdir(path) == 0) {
		// throw ResponseException("200", "Ok");
		// throw ResponseException("400", "Bad Request");
		res._statusCode = "200";
		res._message = "200 Ok";
	}
	// else {
	// 	// multi_fd[fd].status = 200;
	// 	// std::cout << "Directory removed correctly" << std::endl;
	// 	throw ResponseException("200", "Ok");
	// 	// res._statusCode = "200";
	// 	// res._message = "200 Ok";
	// }
}

void    path_check(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {

	if (!multi_fd[fd].resource.empty()) {
		struct stat info;
		const char *path = multi_fd[fd].resource.c_str();
		// std::cout << "multi_fd[fd].resource : " << multi_fd[fd].resource << std::endl;
		// std::cout << "here is the path: " << path << std::endl;

		if (stat(path, &info) == 0) {
			if (info.st_mode && S_ISDIR(info.st_mode)) { //the resource is directory
				std::cout << "resource is directory : " << path << std::endl;
				// if (multi_fd[fd].resource[multi_fd[fd].resource.length() - 1] != '/') {
				// 	res._statusCode = "409";
				// 	res._message = "409 Conflict Error";
				// 	// throw ResponseException("409", "409 conflict");
				// }
				// else {
					/////////////////////////////////////////////////
					// need to check if directory has / at the end //
					/////////////////////////////////////////////////

					if ( (access(path, W_OK)) || (access(path, X_OK)) ) { //directory has no write or execute permission
						// throw ResponseException("403", "403 forbidden");
						res._statusCode = "403";
						res._message = "403 Forbidden";
					}
					else { // dierctory has the right permissions
						remove_dir(multi_fd, fd, help, path, res);
					}
				// }

			}
			if (info.st_mode && S_ISREG(info.st_mode)) { //the resource is a regular file

			/////////////////////////////////////////////////////////////////
			// need to check if the file has / at the end, it mustn't work //
			/////////////////////////////////////////////////////////////////

				if (multi_fd[fd].resource[multi_fd[fd].resource.length() - 1] == '/') {
					// throw ResponseException("404", "404 Not found");
					res._statusCode = "404";
					res._message = "404 Not Found";
				}
				else {
					if (access(path, W_OK) == 0) { //the file has no write permission
						// std::cout << "path ====> " << path << std::endl;
						if (unlink(path) == -1) {
							// throw ResponseException("400", "400 error");
							res._statusCode = "400";
							res._message = "400 Bad Request";
						}
						else {
							// throw ResponseException("200", "Ok");
							res._statusCode = "200";
							res._message = "200 Ok";
						}
					}
					else { // the requested file has the right permissions
						// throw ResponseException("403", "403 forbidden");
						res._statusCode = "403";
						res._message = "403 Forbidden";	
					}
				}

			}
		}
		else {
			throw ResponseException("404", "Not found");
		}
	}
	else {
		throw ResponseException("404", "404 Not Found");
	}
}

void	server_protection(std::map<int, Webserve>&multi_fd, int fd) {

	if (multi_fd[fd].resource.find("/multiplexing") != std::string::npos)
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/error_pages") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/delete_req_cgi") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/config") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/cgi-bin") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/cgi") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/bin") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
	if (multi_fd[fd].resource.find("/Response") != std::string::npos) 
		throw ResponseException("403", "403 Forbidden");
}

void    delete_method(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	(void) help;
	multi_fd[fd].resource = res._URI;

	// std::cout << "here is the request resource: " << multi_fd[fd].resource << std::endl;
	try
	{
		const char	*relative_path = "../";
		char 		resolved_path[PATH_MAX];

		char *result = realpath(relative_path, resolved_path);
		if (result != NULL) {
			std::string path(resolved_path);
			// std::cout << "here is the new path " << path << "\n";
			path += '/';
			if( path == res._URI ) {
				throw ResponseException ("403", "403 Forbidden");
			}
		}
		server_protection(multi_fd, fd);
		path_check(multi_fd, fd, help, res);
	}
	catch (const ResponseException& e)
	{
		// std::cout << "Exception caught: " << e.get_message() << " ======> " << e.get_status() << '\n';
		res._statusCode = e.get_status();
		res._message = e.get_message();
	}
		std::cout << "Exception caught: " << res._statusCode << " ======> " << res._message << '\n';
		res._contentType = "text/html";
		res._Rpnse = true;
}