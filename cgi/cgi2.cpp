#include "cgi.h"

std::string	fd_str;
int			alarm_number = 0;

void alarmHandler(int signum) {
	// sleep(1);
	(void)signum;
	std::string filename = "pid_file" + fd_str;
	std::ifstream fd(filename.c_str());
	std::string fd_line;
	std::getline(fd, fd_line);
	fd.close();
	std::remove(filename.c_str());
	std::remove(("script_output" + fd_str).c_str());
	pid_t pid = std::atoi(fd_line.c_str());
	if (pid != 0) {
		if (kill(pid, SIGKILL) == 0) {
			alarm_number = 1;
		}
	}
}

void    cgi::extension_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res) {

	size_t  dot_pos = res._URI.find_last_of('.');

	if (dot_pos != std::string::npos && dot_pos < res._URI.length() - 1) {
		multi_fd[fd].extension = res._URI.substr(dot_pos);
	}
}

void    cgi::env_setter(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, std::string script_filename, Response& res) {
	this->env["REQUEST_METHOD"] = multi_fd[fd].HTTP_method;
	this->env["REQUEST_URI"] = res._URI;
	this->env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->env["SERVER_NAME"] = help->obj.getHost();
	this->env["SERVER_PORT"] = help->obj.getPort();
	this->env["REDIRECT_STATUS"] = "200";
	this->env["SCRIPT_FILENAME"] = script_filename;
	this->env["GATEWAY_INTERFACE"] = "CGI/1.1";

	if (multi_fd[fd].request_URI.find('?') != std::string::npos) { //URI has query string
		int start_pos = multi_fd[fd].request_URI.find('?');
		int end_pos = multi_fd[fd].request_URI.find('#');
		this->env["QUERY_STRING"] = multi_fd[fd].request_URI.substr(start_pos, end_pos);
	}
	else
		this->env["QUERY_STRING"] = "NULL";
	if (multi_fd[fd].HTTP_method == "POST") {
		this->env["CONTENT_TYPE"] = multi_fd[fd].content_Type;
		this->env["CONTENT_LENGTH"] = multi_fd[fd].content_Length;
	}
	else {
		this->env["CONTENT_TYPE"] = "NULL";
		this->env["CONTENT_LENGTH"] = "NULL";
	}
	// if (multi_fd[fd].request_URI.find('.') != std::string::npos) { //extract the extension
	// 	int start_pos = multi_fd[fd].request_URI.find_last_of('.') + 1;
	// 	int end_pos = multi_fd[fd].request_URI.find('?');
	// 	multi_fd[fd].extension = multi_fd[fd].request_URI.substr(start_pos, end_pos);
	// }
}

std::string cgi_body_getter(std::map<int, Webserve>&multi_fd, int fd, std::string file) {
	std::ifstream cgi_file(file.c_str());

	if (!cgi_file.is_open()) {
		std::cerr << "Error openning the file" << std::endl;
		throw ResponseException("500", "Internal Server Error1");
	}

	std::ostringstream  oss;
	oss << cgi_file.rdbuf();
	cgi_file.close();
	std::string file_outcome = oss.str();

	//extract the body
	std::string	 body;
	if (multi_fd[fd].extension == "php") {
		size_t  pos = file_outcome.find("\n\n");
		if (pos == std::string::npos) {
			std::cerr << "Blank line not found" << std::endl;
			exit (1);
		}
		body = file_outcome.substr(pos + 2);
	}
	else
		body = file_outcome;
	return body;
}

size_t calculateContentLength(const std::string& content) {
    std::ostringstream oss;
    oss << content;
    return oss.str().length();
}

bool isDirectory(const char* path) {
    struct stat info;
    if (stat(path, &info) != 0) {
        return false; // Failed to get file info
    }
    return S_ISDIR(info.st_mode);
}
void   cgi::execute_cgi(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	std::ostringstream oss;
	oss << fd;
	fd_str = oss.str();
	if (multi_fd[fd].running == false) {
		// std::cout << "enter for the first time" << std::endl;
		multi_fd[fd].running = true;
		std::string path = res._URI;
		if (access(path.c_str(), X_OK) != 0 && this->on_off == "on") { //check script permissions
			throw ResponseException ("500", "500 Internal Server Error");
		}
		this->env_setter(multi_fd, fd, help, path, res);
		multi_fd[fd].startTime = clock();
		// startTime = std::time(NULL);
		// std::cout << "here is the start time: " << multi_fd[fd] << std::endl;
		multi_fd[fd].pid = fork();
		// pid = fork();
		if (multi_fd[fd].pid == -1) {
			// res._statusCode = "500";
			// res._message = "Internal Server Error2";
			throw ResponseException ("500", "500 Internal Server Error");
		}
		if (multi_fd[fd].pid == 0) {
			//file existing check
			struct stat buufer;
			if ((stat(this->cgi_path.c_str(), &buufer) == -1) || (stat(path.c_str(), &buufer) == -1)) {
				// std::cout << "problem with : " << this->cgi_path << " paths doesn't exist\n";
				throw ResponseException ("500", "500 Internal Server Error");
			}
			
			std::string command_line = "echo $$ > pid_file" + fd_str + " && " + this->cgi_path + " " + path + " > " + "script_output" + fd_str;

			// Allocate memory for the array of pointers
			char **av = new char*[4];
			av[3] = NULL;
			av[0] = new char[3];
			strcpy(av[0], "sh");
			av[1] = new char[3];
			strcpy(av[1], "-c");
			av[2] = new char[command_line.length() + 1];
			strcpy(av[2], command_line.c_str());
			
			//convert map env variables to char**
			char **envp = new char*[this->env.size() + 1];
			int i = 0;
			for (std::map<std::string, std::string>::iterator it = this->env.begin(); it != this->env.end(); it++)
			{
				envp[i] = new char[it->first.length() + it->second.length() + 2];
				strcpy(envp[i], (it->first + "=" + it->second).c_str());
				// std::cout << "env[] : " << envp[i] << std::endl;
				i++;
			}
			envp[i] = NULL;
			// std::cout << "command line : " << command_line << std::endl; 
			if (execve("/bin/sh", av, envp) == -1)
			{
				for (int j = 0; j < i; j++)
					delete[] envp[j];
				delete[] envp;
				delete[] av;
				exit(EXIT_FAILURE);
			}
		}

		// if (pid > 0) {
		// 	pid_t result = waitpid(pid, &this->status, WNOHANG);
		// 	std::cout << "result: " << result << std::endl;
		// 	if (result > 0) {
		// 		std::remove(("pid_file" + fd_str).c_str());
		// 		if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE) {
		// 			// res._statusCode = "500";
		// 			// res._message = "Internal Server Error4";
		// 			throw ResponseException ("500", "500 Internal Server Error");
		// 		}
		// 		std::ifstream tmp(("script_output" + fd_str).c_str());
		// 		if (tmp.is_open()) {
		// 			struct stat info;
		// 			if (stat(("script_output" + fd_str).c_str(), &info) == 0) {
		// 				std::string	s_file = "script_output" + fd_str;
						std::string body = cgi_body_getter(s_file);
		// 				res._cgiFlag = 1;
		// 				res._cgiScript = 1;
		// 				res._isDirectory = false;
		// 				res._bodyLength = calculateContentLength(body);
		// 				res._cgiBody = body;
		// 				std::cout << "here is the cgi body : " << res._cgiBody << ", and the content length: " << res._bodyLength << std::endl;
		// 				// res._statusCode = "200";
		// 				// res._message = "Success";
		// 				res._contentType = "text/html";
		// 				throw ResponseException ("200", "Ok");
		// 			}
		// 		}
		// 	}
		// }
	}
					// else {
						// sleep(10);
						// clock_t endTime = clock();
						// // std::cout << "END TIME >>>>>>>>>>>>>>>>>>>>>>>..>>>>>>>> " << endTime << std::endl;
						// clock_t elapsedTime = (endTime - startTime) / CLOCKS_PER_SEC;
						// // std::cout << "ENTER HERE " << elapsedTime << " \n";
						// if (elapsedTime > 5) {
						// 	std::string filename = "pid_file" + fd_str;
						// 	std::ifstream fd(filename.c_str());
						// 	std::string fd_line;
						// 	std::getline(fd, fd_line);
						// 	fd.close();
						// 	std::remove(filename.c_str());
						// 	std::remove(("script_output" + fd_str).c_str());
						// 	pid_t pid = std::atoi(fd_line.c_str());
						// 	if (pid != 0) {
						// 		if (kill(pid, SIGTERM) == 0) {
						// 			std::cout << "the pid killed succesfully \n";
						// 			throw ResponseException ("508", "508 Loop Detected");
						// 		}
						// 	}
						// 	throw ResponseException ("508", "508 Loop Detected");
						// }
					// }
}

std::string cgi::cgi_getter(void) {
	return this->cgi_body;
}

std::string get_absolute_path(const std::string& path) {
    char resolved_path[PATH_MAX];
    const char* c_path = path.c_str();
    char* abs_path = realpath(c_path, resolved_path);
    if (abs_path == NULL) {
		throw ResponseException("404", "404 Not Found");
    }
    return std::string(resolved_path);
}

int	cgi::locationMatch(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	(void)fd, (void)multi_fd;
	size_t  query_pos = res._URI.find('?');//extrat the path if res._URI has ?

	if (query_pos != std::string::npos) {
		res._URI = res._URI.substr(0, query_pos);
	}
	for (std::vector<location>::iterator it = _srv[help->server_index]._locationScoops.begin(); it != _srv[help->server_index]._locationScoops.end(); it++) {
		if (res._URI.find(it->_locationPath) != std::string::npos && it->_locationPath != "/") {
			if (!it->on_off.empty()) {
				this->on_off = it->on_off;
				if (it->on_off == "on") {
					if (multi_fd[fd].extension == ".php") {
						if (!it->_cgiPath[".php"].empty()) {
							this->cgi_path = it->_cgiPath[".php"];
							// std::cout << "php, exists" << std::endl;
						}
						else
							throw ResponseException("403", "403 Forbidden");
					}
					else if (multi_fd[fd].extension == ".py") {
						if (!it->_cgiPath[".py"].empty()) {
							this->cgi_path = it->_cgiPath[".py"];
							// std::cout << "py, exists" << std::endl;
						}
						else
							throw ResponseException("403", "403 Forbidden");
					}
					return 1;
				}
				else if (it->on_off == "off")
					return 2;
			}
			else
				return 2;
		}
	}
	return 0;
}

int	cgi::cgiCheck(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	if (isDirectory(res._URI.c_str())) {//uri is directory
		return 1;
	}
	else { //uri is file
		this->extension_getter(multi_fd, fd, res);
		if (multi_fd[fd].extension.empty()) {
				// std::cout << "there is no extension to this file" << std::endl;
				return 1;
		}
		else if (!multi_fd[fd].extension.empty() && (multi_fd[fd].extension == ".php" || multi_fd[fd].extension == ".py")) {
			// std::cout << "here is the extension: " << multi_fd[fd].extension << std::endl;
			if (locationMatch(multi_fd, fd, help, res) == 1) {
				
				// std::cout << "location exists and cgi on\n";
				return 0;
			}
			else if (locationMatch(multi_fd, fd, help, res) == 0) {
				// std::cout << "location does not exists" << std::endl;
				return 1;
			}
			else if (locationMatch(multi_fd, fd, help, res) == 2) {
				// std::cout << "location exists and cgi off\n";
				res._listContent = 1;
				return 1;
			}
		}
		else {
			return 1;
		}
	}
	return 0;
}

void	parent_handler(std::map<int, Webserve>&multi_fd, int fd, Response& res) {
	// if (multi_fd[fd].pid > 0) {
	pid_t result = waitpid(multi_fd[fd].pid, 0, WNOHANG);
	if (result > 0) {
		std::remove(("pid_file" + fd_str).c_str());
		// if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE) {
		// 	// res._statusCode = "500";
		// 	// res._message = "Internal Server Error4";
		// 	throw ResponseException ("500", "500 Internal Server Error");
		// }
		std::ifstream tmp(("script_output" + fd_str).c_str());
		if (tmp.is_open()) {
			struct stat info;
			if (stat(("script_output" + fd_str).c_str(), &info) == 0) {
				std::string	s_file = "script_output" + fd_str;
				std::string body = cgi_body_getter(multi_fd, fd, s_file);
				res._cgiFlag = 1;
				res._cgiScript = 1;
				res._isDirectory = false;
				res._bodyLength = calculateContentLength(body);
				res._cgiBody = body;
				// res._statusCode = "200";
				// res._message = "Success";
				res._contentType = "text/html";
				throw ResponseException ("200", "Ok");
			}
		}
	}
	else {
	// sleep(10);
		clock_t endTime = clock();
		// std::cout << "END TIME >>>>>>>>>>>>>>>>>>>>>>>..>>>>>>>> " << endTime << std::endl;
		clock_t elapsedTime = (endTime - multi_fd[fd].startTime) / CLOCKS_PER_SEC;
		// std::cout << "ENTER HERE " << elapsedTime << " \n";
		if (elapsedTime > 5) {
			std::string filename = "pid_file" + fd_str;
			std::ifstream fd(filename.c_str());
			std::string fd_line;
			std::getline(fd, fd_line);
			fd.close();
			std::remove(filename.c_str());
			std::remove(("script_output" + fd_str).c_str());
			pid_t pid = std::atoi(fd_line.c_str());
			if (pid != 0) {
				if (kill(pid, SIGTERM) == 0) {
					std::cout << "the pid killed succesfully \n";
					throw ResponseException ("508", "508 Loop Detected");
				}
			}
			throw ResponseException ("508", "508 Loop Detected");
		}
	}
}

int    cgi_handler(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	
	cgi cgi_c;

	res._cgiBody = "NULL";
	res._bodyLength = 0;
	res._cgiFlag = 0;
	res._listContent = 0;

	if (multi_fd[fd].get_cgi) {
		std::string path = res._URI;
		std::string abs_path = get_absolute_path(path);
		std::cout << "Absolute path: " << abs_path << std::endl;
		if (cgi_c.cgiCheck(multi_fd, fd, help, res) == 0) {
			cgi_c.execute_cgi(multi_fd, fd, help, res);
			std::cout << "have to execute " << std::endl;
			res._listContent = 0;
		}
		else {
			res._listContent = 1;
		}
	}
	else if (multi_fd[fd].post_cgi) {
		std::cout << "Post cgi" << std::endl;
	}
}