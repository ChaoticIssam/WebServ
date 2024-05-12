#include"webserve.hpp"

Webserve::Webserve() : dec(0), dec1(0), flag(0), flag1(0), flag2(0), response_success(0), error_response(0), max_length(0), time_out(clock()), post_cgi(0){
    child_exited = 0;
    running = 0;
    startTime = 0;
    post_cgi = 0;
    get_cgi = 0;
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
        this->startTime     = copy.startTime;
        this->post_cgi      = copy.post_cgi;
        this->get_cgi       = copy.get_cgi;
        
        // this->query         = copy.query;
        this->out_file.close();  // Close the current file if it's open
        this->out_file.clear();  // Clear any error flags
        this->running = copy.running;
        this->child_exited = copy.child_exited;
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
        this->dec1           = copy.dec1;

        this->content_type  = copy.content_type;
        this->flag1         = copy.flag1;
        this->flag          = copy.flag;
        this->flag2          = copy.flag2;
        this->response_success = copy.response_success;
        this->error_response = copy.error_response;
        this->dec           = copy.dec;
        this->chunk_len     = copy.chunk_len;
        this->chunk_len_dec = copy.chunk_len_dec;
        this->chunk_str = copy.chunk_str;
        this->response = copy.response;
        this->max_length = copy.max_length;
        this->response_success = copy.response_success;
        this->time_out = copy.time_out;
        return(*this);
    }
    return(*this);
}
int epoll_fd;

int main(int ac, char **av){

    Helpers help;
    issam_main(ac, av, help);
        // exit(1);
        struct sigaction sa = {};
        sa.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &sa, 0);
        // std::cout << "lmain" << std::endl << std::endl << std::endl << std::endl;
        if(creat_socket_and_epoll(&help)){
            std::cerr << "the socket couldnt be created ." << std::endl;
            exit(1);
        }
}

Webserve::~Webserve(){};
Helpers::~Helpers(){};