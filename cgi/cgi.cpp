#include "cgi.h"

void    cgi::path_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res, Helpers* help) {
	size_t  query_pos = res._URI.find('?');//extrat the path id res._URI has ?

	std::cout << "uri before: " << res._URI << std::endl;
	if (query_pos != std::string::npos) {
		res._URI = res._URI.substr(0, query_pos);
	}

	for (std::vector<location>::iterator it = _srv[help->server_index]._locationScoops.begin(); it != _srv[help->server_index]._locationScoops.end(); it++) {
		if (res._URI.find(it->_locationPath) != std::string::npos) {
		std::cout << "url: " << res._URI << "     location name : " << it->_locationPath << std::endl;
			int flag = 1;
			status = it->_cgiStatus;
			std::string index = it->_Index;
			if (flag == 1 && status && (index == "index.php" || index == "index1.php" || index == "index1.py" || index == "index.py")) {
				if (this->extension == ".php") { 
					this->cgi_path = it->_cgiPath[".php"];
					multi_fd[fd].extension = "php";
				}
				else if (this->extension == ".py") {
					this->cgi_path = it->_cgiPath[".py"];
					multi_fd[fd].extension = "py";
				}
			}
		}
	}
}

void    cgi::extension_getter(std::string url) {
	size_t  dot_pos = url.find_last_of('.');

	if (dot_pos != std::string::npos && dot_pos < url.length() - 1) {
		this->extension = url.substr(dot_pos);
	}
}

void    cgi::env_setter(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, std::string script_filename) {
	this->env["REQUEST_METHOD"] = multi_fd[fd].HTTP_method;
	this->env["REQUEST_URI"] = multi_fd[fd].request_URI;
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
		std::cout << "here is the content type : " << this->env["CONTENT_TYPE"] << "\nTHE CONTENT LENGTH : " << this->env["CONTENT_LENGTH"] << std::endl;
	}
	else {
		this->env["CONTENT_TYPE"] = "NULL";
		this->env["CONTENT_LENGTH"] = "NULL";
	}
	if (multi_fd[fd].request_URI.find('.') != std::string::npos) { //extract the extension
		int start_pos = multi_fd[fd].request_URI.find_last_of('.') + 1;
		int end_pos = multi_fd[fd].request_URI.find('?');
		this->extension = multi_fd[fd].request_URI.substr(start_pos, end_pos);
	}
}

std::string cgi::cgi_body_getter(std::string file) {
	std::ifstream cgi_file(file.c_str());

	if (!cgi_file.is_open()) {
		std::cerr << "Error openning the file" << std::endl;
		throw ResponseException("500", "Internal Server Error");
	}

	std::ostringstream  oss;
	oss << cgi_file.rdbuf();
	cgi_file.close();
	std::string file_outcome = oss.str();

	//extract the body
	std::string	 body;
	if (this->extension == "php") {
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

void   cgi::execute_cgi(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	std::ostringstream oss;
	oss << fd;
	std::string fd_str = oss.str();
	if (!this->running) {
		this->running = true;
		this->extension_getter(res._URI);
		path_getter(multi_fd, fd, res, help);
		std::string path = res._URI;
		if (access(path.c_str(), X_OK) != 0) { //check script permissions
			res._statusCode = "500";
			res._message = "Internal Server Error1";
		}
		this->env_setter(multi_fd, fd, help, path);
		time_t startTime = std::time(NULL);
		// std::cout << "here is the start time: " << startTime << std::endl;
		this->pid = fork();
		if (this->pid == -1) {
			res._statusCode = "500";
			res._message = "Internal Server Error2";
		}
		if (this->pid == 0) {
			//file existing check
			struct stat buufer;
			if ((stat(this->cgi_path.c_str(), &buufer) == -1) || (stat(path.c_str(), &buufer) == -1)) {
				std::cout << "problem with : " << this->cgi_path << " paths doesn't exist\n";
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
		else if (this->pid > 0) {
			int count = 0;
			while (true) {
				std::cout << "true " << "counter: "  << count << "\n";
				count++;
				sleep(1);
				pid_t result = waitpid(this->pid, &this->status, WNOHANG);
				// if (result == -1) {
				// 	throw ResponseException("500", "Internal Server Error3");
				// 	break ;
				// }
				if (result > 0) {
					std::remove(("pid_file" + fd_str).c_str());
					if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE) {
						res._statusCode = "500";
						res._message = "Internal Server Error4";
					}
					std::ifstream tmp(("script_output" + fd_str).c_str());
					if (tmp.is_open()) {
						struct stat info;
						if (stat(("script_output" + fd_str).c_str(), &info) == 0) {
							std::string	s_file = "script_output" + fd_str;
							std::string body = cgi_body_getter(s_file);
							res._contentLength = calculateContentLength(body);
							res._cgiBody = body.c_str();
							std::cout << "here is the cgi body : " << res._cgiBody << ", and the content length: " << res._contentLength << std::endl;
							res._statusCode = "200";
							res._message = "Success";
							res._contentType = "text/html";
						}
						break ;
					}
				}
				else {
					time_t endTime = std::time(NULL);
					time_t elapsedTime = endTime - startTime;
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
								res._statusCode = "508";
								res._message = "Loop Detected";
								break ;
							}
						}
						res._statusCode = "508";
						res._message = "Loop Detected";
						break ;
					}
					// break ;
				}
			}
		}
	}
}

std::string cgi::cgi_getter(void) {
	return this->cgi_body;
}

void    cgi_handler(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {

	cgi cgi_c;

	// for (std::vector<location>::iterator it = help->obj._locationScoops.begin(); it != help->obj._locationScoops.end(); it++) {
	// 	if (multi_fd[fd].request_URI.find(it->_locationPath) == 0) {
	// 		flag = 1;
	// 		status = it->_cgiStatus;
	// 		index = it->_Index;
	// 		if (flag == 1 && status && index == "index.php") {
	// 			cgi_c.execute_cgi(multi_fd, fd, help, it);
	// 		}
	// 	}
	// }
	// std::cout << "here is the extension: " << res._extensions[res._ext] << std::endl;
	// cgi_c.extension_getter(res._URI);
	// std::cout << "here is the extension : " << cgi_c.extension << std::endl;
	// std::cout << "here is "
	cgi_c.execute_cgi(multi_fd, fd, help, res);
	// if (flag == 0) {
	// 	if (multi_fd[fd].request_URI.find(help->obj._rootDirectory) == 0) {
	// 		flag = 2;
	// 	}
	// }
	// if (flag == 2) {
	// 	//need to handle the case when cgi exists on the server root directory
	// 	;
	// }
}