#include"webserve.hpp"

//exemple of an HTTP response 

    // HTTP/1.1 200 OK ====> HTTP version || response code  || response code description.

    // Content-Type: text/html; charset=UTF-8
    // Content-Length: 1270

    // <!doctype html>
    // <html>
    // <head>
    //  <title>Example Domain</title>
    // ...

//***************************************************************************************

// void    success_response(std::map<int , Webserve>&multi_fd, Helpers *help){
//     std::ofstream   response_file("response.html");
//     int fd = help->events[help->i].data.fd;
//     std::string responce;
//     char buffer[20];
//     sprintf(buffer, "%lu", multi_fd[fd].Body.size());
//     std::cout << "buff :" << buffer << std::endl;
//     if(response_file.is_open()){
//         response_file.write(multi_fd[fd].HTTP_version.c_str(), multi_fd[fd].HTTP_method.size());
//         response_file.write(" ", 1);
//         response_file.write("200 OK\r\n",8);
//         response_file.write("Content-Type: " , 14);
//         response_file.write(multi_fd[fd].content_Type.c_str(), multi_fd[fd].content_Type.size());
//         response_file.write("\r\n", 2);
//         response_file.write("Content-Length: " , 16);
//         response_file.write(buffer, strlen(buffer));
//         response_file.write("\r\n\r\n", 4);
//         response_file.write(multi_fd[fd].Body.c_str(), multi_fd[fd].Body.size());
//         response_file.close();
//     }
// }

// std::string get_Cgi_response()c

// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
// std::string size_tToString(size_t value) {
//     std::ostringstream oss;
//     oss << value;
//     return oss.str();
// }

void    success_response(std::map<int , Webserve>&multi_fd, Helpers *help){
   
    int fd = help->events[help->i].data.fd;
    std::ostringstream response;
    response << "<!DOCTYPE html>\n"
             << "<html>\n"
             << "<head><title>File Upload Success</title></head>\n"
             << "<body>\n"
             << "<h1>File Upload Successful</h1>\n"
             << "<p>Your files have been successfully uploaded.</p>\n"
             << "</body>\n"
             << "</html>\n";
    multi_fd[fd].response += multi_fd[fd].HTTP_version + " 201 \r\n";
    multi_fd[fd].response += "Content-Type: text/html\r\n";
    multi_fd[fd].response += "Content-Length: " + size_tToString((response.str()).size()) + "\r\n";
    multi_fd[fd].response += "\r\n"; // Empty line to separate headers from body
    multi_fd[fd].response += response.str();
    // std::cout << "response : " << multi_fd[fd].response << std::endl;
    // printf("--> %zd", send(help->sosocket, multi_fd[fd].response.c_str(), (int)multi_fd[fd].response.size(), 0));
    if(send(fd, multi_fd[fd].response.c_str(), multi_fd[fd].response.size(), 0) == -1)
        std::cout << "send response failed ." << std::endl;
    close(fd);
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
    multi_fd.erase(fd);
}
