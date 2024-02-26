#include "configParss.hpp"

void    configParss::setPort(std::string port){
    _port = port;
}
void    configParss::setHost(std::string host){
    _host = host;
}
void    configParss::setServerName(std::string serverName){
    _serverName = serverName;
}
std::string configParss::getPort(void){
    return (_port);
}
std::string configParss::getHost(void){
    return (_host);
}
