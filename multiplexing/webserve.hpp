#pragma once

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <vector>
#include "../delete_req_cgi/Request/Request.h"
#include "delete/delete_method.h"
#include "../config/config/configParss.hpp"
#include "../cgi/cgi.h"

extern int epoll_fd;

#define MAX_SIZE 1024
#define PORT 8080

class Helpers;

class   location{
    public:
    std::string _locationPath;
    bool    _postCheck;
    bool    _getCheck;
    bool    _deleteCheck;
    bool    _autoIndex;
    bool    _cgiStatus;
	std::string _rootDirectoryLocation;
    std::string _cgiExtensionHolder;
    std::string _cgiPathHolder;
    std::map<std::string, std::string> _cgiPath;
    std::string _Index;
    std::string _return;
    std::string _uploadStore;
};
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
		std::string _rootDirectory;
        std::string _rootIndex;
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
        std::vector<location> _locationScoops;
};
extern std::vector<configParss> _srv;
int     myStoi(std::string line);
void    getErrors(std::string line);
int issam_main(int ac,  char **av, Helpers &help);

class   Helpers{
    public :
        Helpers();
        ~Helpers();
        int i;
        int sosocket;
        struct epoll_event events[1000];

        //config info
        class location locationScoop;
        class configParss obj;
};


class   Webserve{

    public : 
        Webserve();
        Webserve(const Webserve& copy);
        Webserve &operator=(const Webserve &copy);
        ~Webserve();
        std::string request_line;
        std::string HTTP_method;
        std::string request_URI;
        std::string HTTP_version;
        std::string headers;
        std::string Body;
        std::string body;
        std::string len; //content_length;
        std::string line;
        std::string header;
        std::string update0;
        std::string update;
        std::string content_Type;
        int content_Length;
        std::string out;
        std::map<int , std::string>multi_fd;
        std::ofstream out_file;
        // std::string buffer;
        char        buffer[MAX_SIZE];
        int         readed_buff;
        int         count;
        int         k;
        int         dec;
        int         dec1;

        int         i;
        int         j;
        char        content_type;
        int         flag;
        int         flag1;
        int         flag2;
        // chunked 
        std::string chunk_len;
        std::string chunk_str;
        int         chunk_len_dec;
        std::string first_chunk;
        // response
        std::string   response;
        int   response_success;
        int             content_l;
        std::string     content_body;
        std::string     extension;

        //resources
        int status;
        std::string             resource;
        std::string             resource_path;
        std::vector<location>   locations;

        // int status;

};

int         creat_socket_and_epoll(Helpers *help);
void        post_cases(std::map<int , Webserve>&multi_fd, Helpers *help);
void        content_type(std::map<int , Webserve>&multi_fd, Helpers *help);
std::string check_ext(std::map<int , Webserve>&multi_fd, Helpers *help);
void        setOut(std::map<int, Webserve>& multi_fd, Helpers* help);
void        get_the_content_of_the_file(std::map<int , Webserve>&multi_fd, Helpers *help);
void        content_length(std::map<int , Webserve>&multi_fd, Helpers *help);
void        call_request_functions(std::map<int , Webserve>&multi_fd, Helpers *help,char *buff);
void        success_response(std::map<int , Webserve>&multi_fd, Helpers *help);
std::string size_tToString(size_t value);
void        create_response(std::map<int, Webserve>&multi_fd, std::string message, std::string status, Helpers *help);
int         hexToDecimal(const std::string& hexStr);

//delete
// void    delete_method(std::map<int, Webserve>&multi_fd, int fd, char* buff);

//Response
std::string cgi_body_getter(std::string file);
// std::string get_Cgi_response();