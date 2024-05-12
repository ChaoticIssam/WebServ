#include "../../multiplexing/webserve.hpp"
#include "stdio.h"

configParss::configParss(){
    _port = "";
    _host = "";
    _servernameTMP = "";
    _errorPagenum = 0;
    _errorPage = "";
    _rootDirectory = "";
    _rootIndex = "";
    _maxLength = 1000;
}

location::location(){
    _locationPath = "";
    _postCheck = false;
    _getCheck = false;
    _deleteCheck = false;
    _autoIndex = false;
    _cgiStatus = false;
    _rootDirectoryLocation = "";
    _cgiExtensionHolder = "";
    _cgiPathHolder = "";
    _Index = "";
    _return = "";
    _uploadStore = "";
}

configParss::~configParss(){
}

void    configParss::setPort(std::string port){
    _port = port;
}
void    configParss::setHost(std::string host){
    _host = host;
}
void    configParss::seterrorPagenum(int errorPagenum){
    _errorPagenum = errorPagenum;
}
void    configParss::seterrorPage(std::string errorPage){
    _errorPage = errorPage;
}
std::string configParss::getPort(void){
    return (_port);
}
std::string configParss::getHost(void){
    return (_host);
}
int         configParss::geterrorPagenum(void){
    return (_errorPagenum);
}
std::string configParss::geterrorPage(void){
    return (_errorPage);
}
int myStoi(std::string line){
    const char  *str = line.c_str();
    char        *end;
    int         num = strtol(str, &end, 10);
    return num;
}