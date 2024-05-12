#include"webserve.hpp"

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
    if(send(fd, multi_fd[fd].response.c_str(), multi_fd[fd].response.size(), 0) == -1)
        std::cerr << "send response failed ." << std::endl;
    close(fd);
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,NULL);
    multi_fd.erase(fd);
}
