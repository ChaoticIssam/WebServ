#ifndef GETMETHOD_HPP
# define GETMETHOD_HPP

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
# include <filesystem>

# include "../multiplexing/webserve.hpp"

#define DIREC 0
#define FILE 1
#define NOT_FOUND -1

class cgi;
class Helpers;
class Webserve

class   Response{
    Webserve       *_client;
    unsigned int _statusCode;
    int          _deleteError;
    std::map<int, std::string> _errorLine;
    std::string _statusLine;
    std::string _responseHead;
    char        _responseBUFFER[BUFFER_SIZE];
    std::string _URI;
    std::string _oldURI;
    std::string _query;
    std::string _contentType;
    std::string _contentLength;
    std::string _response;
    std::string _errorPage;
    std::string _filePath;
    Helpers.locationScoop    _locationScoop;
    Helpers.obj              _config;
    bool        _locationFound;
    std::fstream _file;
    int         _errorfileGood;
    std::string _ext;
    std::map<std::string, std::string> _extensions;
    std::map<std::string, std::string> _Headers;
    bool    _isCompleted;
    bool    _isHeader;
    int     _responseLen;
    int     _allResponseBodyLen;
    cgi *CGI;
    //....
    public:
        std::string _fileName;
        bool        has_cgi;
        Response(unsigned int statusCode, Webserve &client);
        ~Response();
        bool    checkLocation(std::string &path);
        int     pathType();
        void    uriParss();
        void    uriMatch();
        int     resourceType();
        void    findFiles();
        void    convertExtention();
        void    getErrorLine();
        void    getMethod();
        void    deleteMethod();
        void    postMethod();
        void    send();
        void    setBody();
        void    clearDir(const std::string &p);
        void    responde();
        void    setHeaders();
        void    setCGI(cgi *);
        void    setFilePath(const std::string &);
        void    setStatusCode(unsigned int);
        void    setFileName(std::string);
        void    setError();
        std::string getExtension() const;
        bool        isCompleted() const;
        bool        isHeader() const;
        const std::string& oldURI()const;
        const std::string& methodGetter() const;
        Webserve*   getClient()const;
        const location& getLocation()const;
        const std::string& getQuery() const;
        const int &getresponseLength() const;
}

#endif