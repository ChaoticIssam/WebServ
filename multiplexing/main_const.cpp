#include"webserve.hpp"

Webserve::Webserve() : flag(0), flag1(0), flag2(0), response_success(0){
};
Helpers::Helpers(){};

Webserve::Webserve(const Webserve& copy){
    *this = copy;
}

Webserve &Webserve::operator=(const Webserve &copy){
    if (this != &copy) {
        
        this->request_line  = copy.request_line;
        this->HTTP_method   = copy.HTTP_method;
        this->HTTP_version  = copy.HTTP_version;
        this->request_URI   = copy.request_URI;
        this->headers       = copy.headers;
        this->Body          = copy.Body;
        this->body          = copy.body;
        this->header        = copy.header;
        this->len           = copy.len;
        this->line          = copy.len;
        this->update        = copy.update;
        this->out           = copy.out;
        this->update0       = copy.update0;
        this->content_Type  = copy.content_Type;
        this->out_file.close();  // Close the current file if it's open
        this->out_file.clear();  // Clear any error flags

        // Reopen the file with the same name
        this->out_file.open((copy.out).c_str(), std::ios_base::binary);
        // Or copy the file name and open the file
        // this->out_file.open(copy.out_fileName.c_str(), std::ios_base::binary);

        // Check if the file opened successfully
        if (!this->out_file.is_open()) 
        this->out_file.copyfmt(copy.out_file);
        for (int i = 0; i < MAX_SIZE; i++)
            this->buffer[i] = copy.buffer[i];
        this->readed_buff   = copy.readed_buff;
        this->readed_buff   = copy.readed_buff;
        this->count         = copy.count;
        this->k             = copy.k;
        this->dec           = copy.dec;
        this->content_type  = copy.content_type;
        this->flag1         = copy.flag1;
        this->flag          = copy.flag;
        this->flag2          = copy.flag2;
        this->dec           = copy.dec;
        this->chunk_len     = copy.chunk_len;
        this->chunk_len_dec = copy.chunk_len_dec;
        this->chunk_str = copy.chunk_str;
        this->response = copy.response;
        this->response_success = copy.response_success;
        return(*this);
    }
    return(*this);
}
int epoll_fd;

int main(int ac, char **av){

    Helpers help;
    issam_main(ac, av, help);
    // std::cout << "lmain" << std::endl << std::endl << std::endl << std::endl;
    if(creat_socket_and_epoll(&help)){
        std::cout << "the socke couldnt be created ." << std::endl;
        exit(1);
    }
}

Webserve::~Webserve(){};
Helpers::~Helpers(){};
