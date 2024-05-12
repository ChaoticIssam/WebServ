#include "../multiplexing/webserve.hpp"

void fill_envirements(std::map<int, Webserve> &multi_fd, int fd, cgi_args *cgi)
{
	std::string script;
	std::string path;

	script = "SCRIPT_FILENAME=" + multi_fd[fd].res._URI ;
	path = "PATH_INFO=" + multi_fd[fd].res._URI ;
	if (multi_fd[fd].request_URI.find('?') != std::string::npos) {
		int start_pos = multi_fd[fd].request_URI.find('?');
		multi_fd[fd].query = "QUERY_STRING=";
		multi_fd[fd].query += multi_fd[fd].request_URI.substr(start_pos + 1);
	}
	else {
		multi_fd[fd].query = "QUERY_STRING=";
		multi_fd[fd].query += strdup("");
	}
	std::stringstream ss;
	ss << multi_fd[fd].content_Length;
	std::string str = "CONTENT_LENGTH=";
	str += ss.str();
	std::string content_type = "CONTENT_TYPE=";
	content_type += multi_fd[fd].content_Type;

	if ((multi_fd[fd].res._URI.find(".php")) != std::string::npos || (multi_fd[fd].res._URI.find(".py")) != std::string::npos) {
		cgi->args[0] = (char*)multi_fd[fd].cgi_c.cgi_path.c_str();
	}
	cgi->args[1] = (char*)multi_fd[fd].res._URI.c_str();
	
	cgi->args[2] = NULL;
	if (multi_fd[fd].HTTP_method == "GET")
	{

		cgi->env[0] =(char *)"REQUEST_METHOD=GET" ;
		cgi->env[1] = (char*)multi_fd[fd].query.c_str();
		cgi->env[2] = (char*)"REDIRECT_STATUS=200";
		cgi->env[3] = strdup(script.c_str());
		cgi->env[4] = strdup(path.c_str());
		cgi->env[5] = NULL;
	}
	else if (multi_fd[fd].HTTP_method == "POST")
	{
		cgi->env[0] =(char *)"REQUEST_METHOD=POST" ;
		cgi->env[1] = strdup(str.c_str());
		cgi->env[2] =  strdup(content_type.c_str());
		cgi->env[3] = (char*)"REDIRECT_STATUS=200";
		cgi->env[4] = strdup(script.c_str());
		cgi->env[5] = strdup(path.c_str());
		cgi->env[6] = (char *)"PHPRC=directorie/php.ini";

		cgi->env[7] = NULL;
	}
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
	size_t  query_pos = res._URI.find('?');

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
							this->cgi_path = get_absolute_path(it->_cgiPath[".php"]);
							std::string realPath = get_absolute_path("../bin/php-cgi");
							if (this->cgi_path != realPath) { 
								throw ResponseException("501", "501 Internal Server Error");
							}
						}
						else
							throw ResponseException("403", "403 Forbidden");
					}
					else if (multi_fd[fd].extension == ".py") {
						if (!it->_cgiPath[".py"].empty()) {
							this->cgi_path = get_absolute_path(it->_cgiPath[".py"]);
							std::string realPath = get_absolute_path("/usr/bin/python3");
							if (this->cgi_path != realPath) { 
								throw ResponseException("501", "501 Internal Server Error");
							}
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

bool isDirectory(const char* path) {
	struct stat info;
	if (stat(path, &info) != 0) {
		return false;
	}
	return S_ISDIR(info.st_mode);
}

void    cgi::extension_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res) {

	size_t  start_pos = res._URI.find_last_of('.');
	if (start_pos != std::string::npos) {
		if (res._URI.find('?') != std::string::npos) {
			if (res._URI.find(".py") != std::string::npos ){
				multi_fd[fd].extension = ".py";
			}
			if ( res._URI.find(".php") != std::string::npos){
				multi_fd[fd].extension = ".php";
			}
		}
		else
			multi_fd[fd].extension = res._URI.substr(start_pos);
	}
}

int	cgi::cgiCheck(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res) {
	if (isDirectory(res._URI.c_str())) {//uri is directory
		return 1;
	}
	else { //uri is file
		this->extension_getter(multi_fd, fd, res);
		if (multi_fd[fd].extension.empty()) {
				return 1;
		}
		else if (!multi_fd[fd].extension.empty() && (multi_fd[fd].extension == ".php" || multi_fd[fd].extension == ".py")) {
			if (locationMatch(multi_fd, fd, help, res) == 1) {
				std::string path = res._URI;
				if (access(path.c_str(), X_OK) != 0) {
					throw ResponseException ("500", "500 Internal Server Error");
				}
				return 0;
			}
			else if (locationMatch(multi_fd, fd, help, res) == 0) {
				return 1;
			}
			else if (locationMatch(multi_fd, fd, help, res) == 2) {
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

void forker(int fd, std::map<int, Webserve> &multi_fd, Helpers *help)
{
	if (multi_fd[fd].cgi_c.cgiCheck(multi_fd, fd, help, multi_fd[fd].res) == 0){
		multi_fd[fd].running = 1;
		if (pipe(multi_fd[fd].pipefd) == -1)
			perror("pipe");
		multi_fd[fd].pid = fork();
		if (multi_fd[fd].pid  == -1)
			perror("fork");
	}
	else {
		multi_fd[fd].res._listContent = 1;
		multi_fd[fd].res._contentType = "application/x-www-form-urlencoded";
	}
}

void child_exec(int client_fd, std::map<int, Webserve> &multi_fd)
{
	cgi_args args;
	const int buffer_size = 4096;
	char buffer[buffer_size];
	memset(buffer, 0, BUFFER_SIZE);
	close(multi_fd[client_fd].pipefd[0]); 
	dup2(multi_fd[client_fd].pipefd[1], 1); 
	close(multi_fd[client_fd].pipefd[1]);
	int fd;
	if(multi_fd[client_fd].HTTP_method == "POST")
	{
		fd = open(multi_fd[client_fd].outfile_name.c_str(), O_RDONLY | std::ios::binary);
		int fd1 = open(multi_fd[client_fd].outfile_name.c_str(), O_RDONLY | std::ios::binary);
		if (fd < 0 || fd1 < 0){
			std::cerr << "failed to open !\n";
			close (fd);
			close (fd1);
			return ;
		}
		read(fd, buffer, buffer_size);
		dup2(fd1, 0);
		close (fd);
	}
	fill_envirements(multi_fd, client_fd, &args);
	if (execve(args.args[0], args.args, args.env) == -1)
	{
		std::cerr << "exec error !\n";
		close (fd);
		return ;
	}
	exit(127);
}

int timeOut(int client_fd, std::map<int, Webserve>&multi_fd)
{
	pid_t result = waitpid(multi_fd[client_fd].pid, 0, WNOHANG);
	if (result == 0)
	{
		clock_t end = clock();
		double elapsedTime = static_cast<double>(end - multi_fd[client_fd].startTime) / CLOCKS_PER_SEC;
		if(elapsedTime >= 5)
		{
			std::remove(multi_fd[client_fd].outfile_name.c_str());
			close(multi_fd[client_fd].pipefd[0]);
			kill(multi_fd[client_fd].pid, SIGKILL);
			waitpid(multi_fd[client_fd].pid, 0, 0);
			throw ResponseException("508", "loop Detected");
		} 
	}
	else {
		multi_fd[client_fd].child_exited = 1;
	}
	return 1;
}

int cgi_response(int epoll_fd, int client_fd, std::map<int, Webserve>&multi_fd, Response& res)
{
	(void)res;
	char buffer[100000];
	ssize_t bytesRead;
	if ((bytesRead = read(multi_fd[client_fd].pipefd[0], buffer, 99999)) > 0)
	{
		std::string body;
		std::string response;
		body.append(buffer,bytesRead);
		int body_start = body.find("\r\n\r\n") + 4;
		body = body.substr(body_start);
		res._contentType = "text/html";
		res.createHeader(size_tToString(body.size()));
		response += res._responseHead;
		response += body;
		if (send(client_fd, response.c_str(), response.size(), 0) == -1){
			close(client_fd);
			epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
			multi_fd.erase(client_fd);
			return 0;
		}
		std::remove(multi_fd[client_fd].outfile_name.c_str());
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, multi_fd[client_fd].pipefd[0], NULL);
		close(multi_fd[client_fd].pipefd[0]);
		close(client_fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
		multi_fd.erase(client_fd);
	}
	if(bytesRead <= 0)
	{
		return 0;
	}
	return 0;
}