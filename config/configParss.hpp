#ifndef CONFIGPARSS_CPP
#define CONFIGPARSS_CPP

#include <iostream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <map>
#include <vector>

class   configParss{
    std::string _port;
    std::string _host;
    // std::string _errorScoop;
    public:
        std::string _servernameTMP;
        std::vector<std::string> _servernamesHolder;
        int         _errorPagenum;
        std::string _errorPage;
        std::map<int, std::string>  errorHolder;
        std::string readConfigFile(void);
        void        fillInfo(std::string file);
        void        setPort(std::string port);
        void        setHost(std::string host);
        void        setServerName(std::string serverName);
        void        seterrorPagenum(int errorPagenum);
        void        seterrorPage(std::string errorPage);
        std::string getPort(void);
        std::string getHost(void);
        int         geterrorPagenum(void);
        std::string geterrorPage(void);
        static void  funcP(void){};
};
int     myStoi(std::string line);
void    getErrors(std::string line);

#endif