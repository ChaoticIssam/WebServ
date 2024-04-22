#ifndef DELETE_METHOD_HPP
# define DELETE_METHOD_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "../../Response/Res.hpp"
// #include "../Request.h"
#include "../webserve.hpp"

class Response;

class Webserve;

class Helpers;

class   ResponseException: public std::exception {
private:
    std::string     status;
    std::string     response_message;

public:
    std::string get_status(void) const { return status ; };
    std::string get_message(void) const { return response_message ; };

    ResponseException(std::string status_code, std::string message)  throw() : status(status_code), response_message(message) {};
    virtual ~ResponseException() throw() {}
};

void    delete_method(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, Response& res);


#endif