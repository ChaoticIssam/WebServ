#include"webserve.hpp"

void    post_cases(std::map<int , Webserve>&multi_fd, Helpers *help){
    size_t pos = multi_fd[(help->events[help->i].data.fd)].headers.find("Transfer-Encoding: chunked");
    if(pos != std::string::npos)
        multi_fd[(help->events[help->i].data.fd)].content_type = 'C';
    else
        multi_fd[(help->events[help->i].data.fd)].content_type = 'L';
}

void    content_type(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
    size_t pos1 = multi_fd[(help->events[help->i].data.fd)].headers.find("Content-Type: ");
    if(pos1 != std::string::npos)
    {
        std::string content_type = multi_fd[(help->events[help->i].data.fd)].headers.substr(pos1, multi_fd[(help->events[help->i].data.fd)].headers.length());
        pos1 =  content_type.find("\r\n");
        multi_fd[(help->events[help->i].data.fd)].content_Type = content_type.substr(strlen("Content-Type: "), pos1 - strlen("Content-Type: "));
    }
    else if (pos1 == std::string::npos && multi_fd[help->events[help->i].data.fd].HTTP_method == "POST"){
        // throw ResponseException("400" ,"Bad Request");
        res._statusCode = "400";
        res._message = "400 Bad Request";
    }
}

void    content_length(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
    int fd = help->events[help->i].data.fd;
    size_t pos = multi_fd[fd].headers.find("Content-Length: ");
    if(pos != std::string::npos)
    {
        std::string content_length = multi_fd[fd].headers.substr(pos, multi_fd[fd].headers.length());
        pos =  content_length.find("\r\n");
        multi_fd[fd].len = content_length.substr(strlen("Content-Length: "), pos - strlen("Content-Length: "));
        multi_fd[fd].content_Length = atoi(multi_fd[fd].len.c_str());
        if(multi_fd[fd].content_Length == 0 && multi_fd[fd].HTTP_method == "POST") {
            res._statusCode = "400";
            res._message = "400 Bad Request";
            // throw ResponseException("400" ,"Bad Request");
        }   
    }
    else if (pos == std::string::npos && multi_fd[fd].HTTP_method == "POST"){
        // throw ResponseException("400" ,"Bad Request");
        res._statusCode = "400";
        res._message = "400 Bad Request";
    }
}

std::string check_ext(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
    int fd = help->events[help->i].data.fd;
    content_type(multi_fd, help, res);
    if(multi_fd[fd].content_Type == "image/png")
        return(".png");
    else if(multi_fd[fd].content_Type == "text/css")
        return(".css");
    else if(multi_fd[fd].content_Type == "text/csv")
        return(".csv");
    else if(multi_fd[fd].content_Type == "image/gif")
        return(".gif");
    else if(multi_fd[fd].content_Type == "text/html")
        return(".html");
    else if(multi_fd[fd].content_Type == "image/x-icon")
        return(".ico");
    else if(multi_fd[fd].content_Type == "image/jpeg")
        return(".jpg");
    else if(multi_fd[fd].content_Type == "application/javascript")
        return(".js");
    else if(multi_fd[fd].content_Type == "application/json")
        return(".json");
    else if(multi_fd[fd].content_Type == "application/pdf")
        return(".pdf");
    else if(multi_fd[fd].content_Type == "image/svg+xml")
        return(".svg");
    else if(multi_fd[fd].content_Type == "image/png")
        return(".png");
    else if(multi_fd[fd].content_Type == "text/plain")
        return(".txt");
    else if(multi_fd[fd].content_Type == "video/mp4")
        return(".mp4");
    return(".txt");
}

void setOut(std::map<int, Webserve>& multi_fd, Helpers* help){
    multi_fd[(help->events[help->i].data.fd)].out = "out_";
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int length = 2;
    std::srand(time(0));
    for (int i = 0; i < length; ++i)
        multi_fd[(help->events[help->i].data.fd)].out += characters[std::rand() % characters.size()];
}

void    get_the_content_of_the_file(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
    setOut(multi_fd, help);
    int fd = help->events[help->i].data.fd;
    multi_fd[fd].out += check_ext(multi_fd, help, res);
    std::string directory = "uploads/";
    std::string fullPath = directory + multi_fd[fd].out;
    multi_fd[fd].out_file.open(fullPath.c_str(), std::ios::binary);
    if (multi_fd[fd].out_file.is_open()) {
        const std::string& body = multi_fd[fd].Body;
        multi_fd[fd].out_file.write(body.c_str(), body.size());
        multi_fd[fd].out_file.close();
        // throw ResponseException("200", "OK");
        multi_fd[help->events[help->i].data.fd].response_success = true;
        res._statusCode = "200";
        res._message = "Ok";
    }
}