#include "cgi.h"

std::string    cgi::path_getter(std::map<int, Webserve>&multi_fd, int fd, std::vector<location>::iterator &it) {
	std::string path;

	size_t  query_pos = multi_fd[fd].request_URI.find('?');//extrat the path id multi_fd[fd].request_URI has ?
	size_t  start_pos = multi_fd[fd].request_URI.find_last_of('/') + 1;

	if (query_pos != std::string::npos) {
		this->script_name = multi_fd[fd].request_URI.substr(start_pos, query_pos);
	}
	else
		this->script_name = multi_fd[fd].request_URI.substr(start_pos);
	path = "/home/bhazzout/Desktop/webserver/" + it->_rootDirectoryLocation + '/' + this->script_name;
	if (this->extension == ".php") { 
		this->cgi_path = it->_cgiPath[".php"];
		multi_fd[fd].extension = "php";
	}
	else if (this->extension == ".py") {
		this->cgi_path = it->_cgiPath[".py"];
		multi_fd[fd].extension = "py";
	}
	return path;
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

void   cgi::execute_cgi(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, std::vector<location>::iterator &it) {
	std::ostringstream oss;
	oss << fd;
	std::string fd_str = oss.str();
	// (void)help, (void)it;
	if (!this->running) {
		this->running = true;
		this->extension_getter(multi_fd[fd].request_URI);
		std::string path = path_getter(multi_fd, fd, it);
		// std::cout << "When path is : " << path << " the cgi path is : " << this->cgi_path << std::endl;
		if (access(path.c_str(), X_OK) != 0) { //check script permissions
			throw ResponseException("500", "Internal Server Error1");
		}
		this->env_setter(multi_fd, fd, help, path);
		time_t startTime = std::time(NULL);
		// std::cout << "here is the start time: " << startTime << std::endl;
		this->pid = fork();
		if (this->pid == -1) {
			throw ResponseException("500", "Internal Server Error2");
		}
		if (this->pid == 0) {
			//file existing check
			struct stat buufer;
			if ((stat(this->cgi_path.c_str(), &buufer) == -1) || (stat(path.c_str(), &buufer) == -1)) {
				std::cout << "paths doesn't exist\n";
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
				if (result == -1) {
					throw ResponseException("500", "Internal Server Error");
					break ;
				}
				else if (result > 0) {
					std::remove(("pid_file" + fd_str).c_str());
					if (WEXITSTATUS(this->status) && WEXITSTATUS(this->status) == EXIT_FAILURE) {
						throw ResponseException("500", "Internal Server Error");
					}
					std::ifstream tmp(("script_output" + fd_str).c_str());
					if (tmp.is_open()) {
						struct stat info;
						if (stat(("script_output" + fd_str).c_str(), &info) == 0) {
							multi_fd[fd].content_l = info.st_size;
							multi_fd[fd].content_body = "script_output" + fd_str;
							std::cout << "script output: " << multi_fd[fd].content_body << std::endl;
						}
					}
					break ;
				}
				else {
					time_t endTime = std::time(NULL);
					time_t elapsedTime = endTime - startTime;
					// std::cout << startTime <<  " " << endTime << std::endl;
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
								throw ResponseException("508", "Loop Detected");
								break ;
							}
						}
						throw ResponseException("508", "Loop Detected");
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

void    cgi_handler(std::map<int, Webserve>&multi_fd, int fd, Helpers *help) {
	cgi cgi_c;

	for (std::vector<location>::iterator it = help->obj._locationScoops.begin(); it != help->obj._locationScoops.end(); it++) {
		if (multi_fd[fd].request_URI.find(it->_locationPath) == 0) {
			if (it->_Index == "index.php" && it->_cgiStatus) {
				std::cout << "entered \n";
				cgi_c.execute_cgi(multi_fd, fd, help, it);
			}
		}
	}

}