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
    std::string                         cgi_extension;
    std::string                         script_name;
    std::string                         cgi_path;
    bool                                running;
    clock_t                             start;
    pid_t                               pid;
    int                                 status;
    int                                 file_length;
    std::string                         cgi_body;
    std::string                         extension;

public:
    std::map<std::string, std::string>  env;
    std::string                         cgi_getter(void);
    std::string                         cgi_body_getter(std::string file);
    void                                path_getter(std::map<int, Webserve>&multi_fd, int fd, Response& res, Helpers *help);
    void                                extension_getter(std::string url);
    void                                env_setter(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, std::string script_filename);
    void                                execute_cgi(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res);
    // void    cgi_clean(std::map<int>)
};

void    cgi_handler(std::map<int , Webserve>&mutli_fd, int fd, Helpers *help, Response& res);


#endif