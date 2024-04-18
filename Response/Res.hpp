#ifndef Res_HPP
# define Res_HPP

# include <dirent.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sstream>
# include <fstream>
# include <filesystem>
# include <sys/socket.h>
# include <fcntl.h>
#include "../config/config/configParss.hpp"
# include <filesystem>

# include "../delete_req_cgi/Request/Request.h"
# include "../multiplexing/delete/delete_method.h"
# include "../get_method/GetMethod.hpp"

#define DIREC 0
#define FILE 1
#define NOT_FOUND -1
#define BUFFER_SIZE 1024

class cgi;
class Helpers;
class Webserve;
class location;
class configParss;

class Response{
    public:
    Webserve                   *_client;
    int                         _fd;
    // configParss _config;
    // location    _locationScoop;
    std::string                 _USER;
    std::string                 _statusCode;
    std::string                 _message;
    int                         _deleteError;
    std::map<int, std::string>  _errorLine;
    std::string                 _statusLine;
    std::string                 _responseHead;
    char                        _responseBUFFER[BUFFER_SIZE];
    std::string                 _URI;
    std::string                 _oldURI;
    std::string                 _query;
    std::string                 _contentLength;
    std::string                 _response;
    std::string                 _errorPage;
    std::string                 _filePath;
    std::string                 _contentType;
    bool                        _locationFound;
    std::fstream                _file;
    int                         _errorfileGood;
    std::string                 _ext;
    bool                        _isDirectory;
    std::map<std::string, std::string>  _extensions;
    std::map<std::string, std::string>  _Headers;
    bool                        _isCompleted;
    bool                        _isHeader;
    int                         _responseLen;
    int                         _allResponseBodyLen;
    Response();
    ~Response();
    void        createHeader();
    std::string getExtension()const;
    int         resourceType();
    void        convertExtention();
    void        uriParss(std::map<int, Webserve>& multi_fd, int fd,Helpers* help);
    void	    findFiles(int fd);
    bool        checkLocation(std::string &path);
    void        sendResponse(std::map<int, Webserve>&multi_fd ,int fd);
    void getMethod(std::map<int, Webserve> &multi_fd, int fd, Helpers *help);
    void NewFunction(int fd);
};

#endif