#ifndef CGI_HPP
# define CGI_HPP

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctime>
#include "../config/config/configParss.hpp"
#include "../multiplexing/webserve.hpp"
#include "../Response/Res.hpp"

class Response;

class Helpers;

class Webserve;

class cgi {
private:
    int                                 status;
    int                                 file_length;
    std::string                         cgi_body;
    std::string                         extension;

public:
    std::string                         on_off;
    std::string                         cgi_path;
    std::map<std::string, std::string>  env;
    std::string                         cgi_getter(void);
    int                                 path_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res, Helpers help);
    void                                extension_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res);
    void                                env_setter(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, std::string script_filename, Response& res);
    void                                execute_cgi(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res);
    int                                 cgiCheck(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res);
    int                                 locationMatch(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res);
};

std::string                         cgi_body_getter(std::map<int, Webserve>&multi_fd, int fd, std::string file);
int                                 cgi_handler(std::map<int , Webserve>&mutli_fd, int fd, Helpers *help, Response& res);
void                                parent_handler(std::map<int, Webserve>&multi_fd, int fd, Response& res);

#endif