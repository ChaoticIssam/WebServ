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
#include "../Response/Res.hpp"
#include "../get_method/GetMethod.hpp"
#include <sstream>
#include <cctype>
#include <vector>
#include "delete/delete_method.h"
#include "../config/config/configParss.hpp"
#include "../cgi/cgi.h"
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
# include "../multiplexing/delete/delete_method.h"
# include "../get_method/GetMethod.hpp"

#define DIREC 0
#define FILE 1
#define NOT_FOUND -1
#define BUFFER_SIZE 1024

extern int epoll_fd;

#define MAX_SIZE 1024
#define PORT 8080


class GET;
class cgi;
class Helpers;
class location;
class configParss;

class Response {
	public:
	Webserve                   *_client;
	int                         _fd;
	int                         _cgiFlag;
	int                         _cgiScript;
	int                         _listContent;
	size_t                      _bodyLength;
	int 						_serverIndex;
	bool						_isReturn;
	bool                        _isCgi;
	bool						_isHEADmethod;
	std::string					_return;
	std::string                 _cgiBody;
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
	size_t                      _fileSize;
	int                         _responseLen;
	int                         _allResponseBodyLen;
	bool                        _Rpnse;
	bool                        _Method;
	Response();
	~Response();
	void        createHeader(std::string contentLength);
	void        createHeaderChunk();
	std::string getExtension()const;
	int         resourceType();
	void        convertExtention();
	void        uriParss(std::map<int, Webserve>& multi_fd, int fd,Helpers* help, int serverIndex);
	void	    findFiles(int fd);
	bool        checkLocation(std::string &path);
	void        sendResponse(std::map<int, Webserve>&multi_fd ,int fd);
	void        getMethod(std::map<int, Webserve> &multi_fd, int fd, Helpers *help);
	void        createHtmlResponse(std::map<int, Webserve>&multi_fd ,int fd, char *buff);
	void        NewFunction(int fd);
};

class   location{
	public:
	std::string _locationPath;
	bool    _postCheck;
	bool    _getCheck;
	bool    _deleteCheck;
	bool    _autoIndex;
	bool    _cgiStatus;
	std::string on_off;
	std::string _rootDirectoryLocation;
	std::string _cgiExtensionHolder;
	std::string _cgiPathHolder;
	std::map<std::string, std::string> _cgiPath;
	std::string _Index;
	std::string _return;
	std::string _uploadStore;
	location();
};
class   configParss{
	std::string _port;
	std::string _host;

	public:
		std::string _servernameTMP;
		std::vector<std::string> _servernamesHolder;
		int         _errorPagenum;
		std::string _errorPage;
		std::map<int, std::string>  errorHolder;
		std::string _rootDirectory;
		std::string _rootIndex;
		std::string _maxLength_str;
		size_t _maxLength;
		configParss();
		~configParss();
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
int     issam_main(int ac,  char **av, Helpers &help);

class   Helpers{
	public :
		Helpers();
		~Helpers();
		int server_index;
		int sosocket;
		int i;
		int s;
		std::vector<int>socketat;
		struct epoll_event multipl;
		std::vector<struct epoll_event>epofd;

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
		std::string new_url;
		std::string request_line;
		std::string HTTP_method;
		std::string request_URI;
		std::string HTTP_version;
		std::string headers;
		std::string Body;
		std::string body;
		std::string len;
		std::string line;
		std::string header;
		std::string update0;
		std::string update;
		std::string content_Type;
		size_t content_Length;
		std::string out;
		std::map<int , std::string>multi_fd;
		std::ofstream out_file;
		std::ofstream cgi_file;
		Response    res;
		char        buffer[MAX_SIZE];
		int         readed_buff;
		int         count;
		int         k;
		ssize_t         dec;
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
		size_t         chunk_len_dec;
		std::string first_chunk;
		// response
		std::string   response;
		int   response_success;
		int     error_response;
		int             content_l;
		std::string     content_body;
		std::string     extension;

		//resources
		int status;
		std::string             resource;
		std::string             resource_path;
		std::vector<location>   locations;

		//delete
		size_t max_length;
		clock_t time_out;

		// cgi
		bool    postCheck;
		bool    getCheck;
		bool    deleteCheck;
		std::string post_upload;
		configParss config;
		location    locat;
		int        running;
		bool        post_cgi;
		bool        get_cgi;
		clock_t     startTime;
		pid_t       pid;
		int         pipefd[2];
		cgi         cgi_c;
		int         child_exited;
		std::string	outfile_name;
		std::string query;
};

struct cgi_args
{
	char *args[10];
	char *env[10];
};

void        cgiResponse(std::map<int, Webserve>&multi_fd ,int fd, Response& res);
int         creat_socket_and_epoll(Helpers *help);
void    	post_cases(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res);
int			content_type(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res);
std::string check_ext(std::map<int , Webserve>&multi_fd, Helpers *help);
void        setOut(std::map<int, Webserve>& multi_fd, Helpers* help);
void        get_the_content_of_the_file(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res);
int			content_length(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res);
void        call_request_functions(Response &res, std::map<int , Webserve>&multi_fd, Helpers *help,char *buff);
void        success_response(std::map<int , Webserve>&multi_fd, Helpers *help);
std::string size_tToString(size_t value);
void        create_response(std::map<int, Webserve>&multi_fd, std::string message, std::string status, Helpers *help);
int         hexToDecimal(const std::string& hexStr);
void    	pars_request(Response &res, std::map<int , Webserve>&multi_fd, Helpers *help,char *buff);
void        forker( int client_fd, std::map<int, Webserve> &multi_fd, Helpers *help);
void        child_exec(int client_fd, std::map<int, Webserve> &multi_fd);
int         timeOut(int client_fd, std::map<int, Webserve>&multi_fd);
int			timeoutRes(std::string message, std::string status, int fd, std::map<int, Webserve>&multi_fd, Response &res);
int 		cgi_response(int epoll_fd, int client_fd, std::map<int, Webserve>&multi_fd, Response& res);
size_t 		str_to_size_t(const std::string& str);
