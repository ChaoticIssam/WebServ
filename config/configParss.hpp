#ifndef CONFIGPARSS_CPP
#define CONFIGPARSS_CPP

#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

class   configParss{
    std::string _port;
    std::string _host;
    std::string _serverName;
    int         _errorPagenum;
    std::string _errorPage;
    public:
        std::string readConfigFile(void);
        void        fillInfo(std::string file);
        void        setPort(std::string port);
        void        setHost(std::string host);
        void        setServerName(std::string serverName);
        std::string getPort(void);
        std::string getHost(void);
};

#endif