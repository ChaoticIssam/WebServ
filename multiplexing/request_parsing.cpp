#include"webserve.hpp"
bool		pars_http_version(std::string	HTTP_version){
	std::string first, second;
	size_t	pos1;

	if(strncmp(HTTP_version.c_str(), "HTTP/", 5) == 0)
	{
		HTTP_version = HTTP_version.substr(5);
		pos1 = HTTP_version.find(".");
        if(pos1 != std::string::npos){
            first = HTTP_version.substr(0, pos1);
            second = HTTP_version.substr(pos1 + 1);
            if(first.length() != 1 || second.length() != 1)
                return false;
            if(!isdigit(first[0]) || !isdigit(second[0]))
                return false;
            return true;
        }
        return false;
	}
	return(false);
}

int   request_line(std::map<int , Webserve>&multi_fd, Helpers *help, std::string temporaire){
		int fd = help->events[help->i].data.fd;
		size_t pos0;
		std::istringstream iss(temporaire);
		pos0 = temporaire.find("\r\n");
		if(pos0 != std::string::npos)
		{
			temporaire = temporaire.substr(pos0 + 2);
			if(!getline(iss, multi_fd[fd].HTTP_method, ' '))
				return (-1);
			if(!getline(iss, multi_fd[fd].request_URI, ' '))
				return (-1);
			if(!getline(iss, multi_fd[fd].HTTP_version, '\r'))
				return (-1);
			if(!pars_http_version(multi_fd[fd].HTTP_version))
				return -1;
			multi_fd[fd].flag = 1;
		}
		else
			return(-1);
		return 0;
}

void    create_the_request_file(std::map<int , Webserve>&multi_fd, Helpers *help, Response& res){
	get_the_content_of_the_file(multi_fd, help, res);
	
}

size_t str_to_size_t(const std::string& str) {
    size_t result = 0;
    bool is_negative = false;
    size_t i = 0;

    if (str[0] == '-') {
        is_negative = true;
        i = 1;
    }

    for (; i < str.length(); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            throw std::invalid_argument("Invalid input");
        }
    }

    return is_negative ? -result : result;
}
int    get_headers(std::map<int , Webserve>&multi_fd, Helpers *help, std::string temporaire, Response& res){
	size_t pos0;
	
	int fd = help->events[help->i].data.fd;

		multi_fd[fd].header.append(temporaire.c_str(),temporaire.size());
		pos0 = multi_fd[fd].header.find("\r\n\r\n");
		if(pos0 != std::string::npos)
		{
			temporaire = multi_fd[fd].header.substr(pos0 + 4, multi_fd[fd].header.length() - (pos0 + 4));
			multi_fd[fd].headers = multi_fd[fd].header.substr(0, pos0);
			if(content_length(multi_fd, help, res) == -1)
				return (-1);
            if(content_type(multi_fd, help, res) == -1){

				return (-1);
			}

            if(multi_fd[fd].HTTP_method == "POST")
			{
				if(multi_fd[fd].postCheck == false){
					res._Rpnse = true;
					res._Method = true;
					return -1;
				}
				post_cases(multi_fd, help, res);
				if(res._Rpnse == true)
					return -1;
				if(multi_fd[fd].content_type == 'L')
				{

					if(multi_fd[fd].content_Length > _srv[help->server_index]._maxLength){
						res._statusCode = "413";
						res._message = "413 Request Entity Too Large";
						res._contentType = "text/html";
						res._Rpnse = true;
						return (-1);
					}

					multi_fd[fd].dec = str_to_size_t(multi_fd[fd].len.c_str());
					multi_fd[fd].dec -= (temporaire.length());
					multi_fd[fd].Body.append(temporaire.c_str(),temporaire.size());
					if(multi_fd[fd].dec < 0)
						multi_fd[fd].Body = multi_fd[fd].Body.substr(0, multi_fd[fd].content_Length);
					if(multi_fd[fd].dec == 0){
						multi_fd[fd].Body.append("\0");
						create_the_request_file(multi_fd, help, res);
						return 0;
					}
				}
				else if(multi_fd[fd].content_type == 'C'){
					size_t pos1 = temporaire.find("\r\n0\r\n");
					if(pos1 != std::string::npos)
					{

						multi_fd[fd].i = pos0;
						for(; multi_fd[fd].header[multi_fd[fd].i + 4] != '\r'; multi_fd[fd].i++);
						multi_fd[fd].chunk_len = multi_fd[fd].header.substr(pos0 + 4, multi_fd[fd].i);
						multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
						multi_fd[fd].dec = multi_fd[fd].chunk_len_dec;
						multi_fd[fd].max_length += multi_fd[fd].chunk_len_dec;
						if(multi_fd[fd].max_length > _srv[help->server_index]._maxLength){
							res._statusCode = "413";
							res._message = "413 Request Entity Too Large";
							res._contentType = "text/html";
							res._Rpnse = true;
							return (-1);
						}
						temporaire = temporaire.substr(multi_fd[fd].i - pos0 + 2, pos1 - (multi_fd[fd].i - pos0));
						multi_fd[fd].dec -= (temporaire.length());
						multi_fd[fd].first_chunk.append(temporaire.c_str(),temporaire.size());
						if(multi_fd[fd].dec <= 0){
							multi_fd[fd].Body.append("\0");
							create_the_request_file(multi_fd, help, res);
							return 0;
						}
					}
					else
						multi_fd[fd].first_chunk.append(temporaire.c_str(),temporaire.size());
					
				}
				multi_fd[fd].flag1 = 1;
			}
		}
		return (0);
}

int    get_Body_part(std::map<int , Webserve>&multi_fd, Helpers *help,char *buff, Response& res){
	int fd = help->events[help->i].data.fd;
	if(multi_fd[fd].content_type == 'L'){
			if(multi_fd[fd].content_Length > _srv[help->server_index]._maxLength)
				return (-1);
			multi_fd[fd].Body.append(buff,multi_fd[fd].k);
			multi_fd[fd].dec -= multi_fd[fd].k;
			if(multi_fd[fd].dec < 0)
				multi_fd[fd].Body = multi_fd[fd].Body.substr(0, multi_fd[fd].content_Length);
			if(multi_fd[fd].dec <= 0){
				multi_fd[fd].Body.append("\0");
				create_the_request_file(multi_fd, help, res);
				return (0);
			}
		}
		else if(multi_fd[fd].content_type == 'C'){
			if(multi_fd[fd].flag2 == 0){
				multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
				size_t p = multi_fd[fd].first_chunk.find("\r\n");
				if(!p)
				{
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(2);
					p = multi_fd[fd].first_chunk.find("\r\n");
				}
				multi_fd[fd].chunk_len = multi_fd[fd].first_chunk.substr(0, p);
				multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
				multi_fd[fd].dec1 = multi_fd[fd].chunk_len_dec;
				multi_fd[fd].max_length += multi_fd[fd].chunk_len_dec;
				if(multi_fd[fd].max_length > _srv[help->server_index]._maxLength)
					return (-1);
				if(multi_fd[fd].dec1 == 0){
					multi_fd[fd].Body.append("\0");

					create_the_request_file(multi_fd, help, res);
					return (0);
				}
				multi_fd[fd].dec1 -= multi_fd[fd].first_chunk.size() ;
				multi_fd[fd].dec1 += p + 2;
				multi_fd[fd].flag2 = 1;
			}
			else
			{
				multi_fd[fd].dec1 -= multi_fd[fd].k;
				
				if(multi_fd[fd].dec1 == 0)
				{
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].first_chunk.find("\r\n") + 2);
					multi_fd[fd].Body.append(multi_fd[fd].first_chunk, multi_fd[fd].first_chunk.length());
					multi_fd[fd].first_chunk.clear();
					multi_fd[fd].flag2 = 0;
				}
				else if(multi_fd[fd].dec1 < 0)
				{
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].first_chunk.find("\r\n") + 2);
					multi_fd[fd].Body.append(multi_fd[fd].first_chunk.c_str(), multi_fd[fd].chunk_len_dec);
					multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(multi_fd[fd].chunk_len_dec);
					{
						size_t p = multi_fd[fd].first_chunk.find("\r\n");
						if(p == std::string::npos)
						{
							multi_fd[fd].flag2 = 0;
							return (0);
						}
						else
							multi_fd[fd].first_chunk = multi_fd[fd].first_chunk.substr(2);

						p = multi_fd[fd].first_chunk.find("\r\n");
						if(p == std::string::npos)
						{
							multi_fd[fd].flag2 = 0;
							return (0);
						}
						multi_fd[fd].chunk_len = multi_fd[fd].first_chunk.substr(0, p);
						multi_fd[fd].chunk_len_dec = hexToDecimal(multi_fd[fd].chunk_len);
						multi_fd[fd].dec1 = multi_fd[fd].chunk_len_dec;
						multi_fd[fd].max_length += multi_fd[fd].chunk_len_dec;
						if(multi_fd[fd].max_length > _srv[help->server_index]._maxLength)
							return (-1);
						
						if(multi_fd[fd].dec1 == 0){
							create_the_request_file(multi_fd, help, res);
							return (0);
						}
						multi_fd[fd].dec1 -= multi_fd[fd].first_chunk.size();
						multi_fd[fd].dec1 += p + 2;
						multi_fd[fd].flag2 = 1;
					}
				}
				else if(multi_fd[fd].dec1 > 0){
					multi_fd[fd].first_chunk.append(buff,multi_fd[fd].k);
				}
			}                
		}
		return (0);
}

std::string size_tToString(size_t value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

void    pars_request(Response &res, std::map<int , Webserve>&multi_fd, Helpers *help,char *buff){
	int fd = help->events[help->i].data.fd;
	std::string newbuffer, bufbody, bufbody1;
	if (buff != NULL) {
		std::string temporaire(buff,multi_fd[fd].k);
		if(multi_fd[fd].flag != 1){
			if(request_line(multi_fd, help, temporaire) == -1){
				res._statusCode = "400";
				res._message = "400 Bad Request";
				res._contentType = "text/html";
				res._Rpnse = true;
				return ;
			}
			res.uriParss(multi_fd, fd, help, help->server_index);
			res._listContent = 0;
		}
		if (res._URI.find(".php") != std::string::npos || res._URI.find(".py") != std::string::npos) {
			multi_fd[fd].get_cgi = true;
			multi_fd[fd].post_cgi = true;
		}
		if(multi_fd[fd].flag1 != 1 && multi_fd[fd].flag == 1)
		{

			if(get_headers(multi_fd, help, temporaire, res) == -1){
				if(!res._Rpnse){
					res._statusCode = "400";
					res._message = "400 Bad Request";
					res._contentType = "text/html";
					res._Rpnse = true;
				}
				if(!res._Method && !res._Rpnse){
					res._statusCode = "404";
					res._message = "404 Not Found";
					res._contentType = "text/html";
				}
				return ;
			}

			if(multi_fd[fd].HTTP_method == "POST") {

				return ;
			}
		}
	}

	multi_fd[fd].content_body = "NULL";
	multi_fd[fd].content_l = 0;
	if(multi_fd[fd].HTTP_method == "POST")
	{
		if(multi_fd[fd].postCheck == false)
		{
			res._statusCode = "404";
			res._message = "404 Not Found";
			res._contentType = "text/html";
			res._Rpnse = true;
			return ;
		}
		if(multi_fd[fd].flag1 == 1)
			if(get_Body_part(multi_fd, help, buff, res) == -1){
				res._statusCode = "413";
				res._message = "413 Request Entity Too Large";
				res._contentType = "text/html";
				res._Rpnse = true;
				return ;
			}

	}
	else if (multi_fd[fd].HTTP_method == "GET" && res._statusCode != "403") {
		if(multi_fd[fd].getCheck == false){
			res._statusCode = "501";
			res._message = "Not Implemented";
			res._contentType = "text/html";
			res._Rpnse = true;
			return ;
		}
		res.getMethod(multi_fd, fd, help);
		res._Rpnse = true;
	}
	else if (multi_fd[fd].HTTP_method == "DELETE") {
		if(multi_fd[fd].deleteCheck == false){
			res._statusCode = "404";
				res._message = "404 Not Found";
				res._contentType = "text/html";
				res._Rpnse = true;
				return ;
		}
		delete_method(multi_fd, fd, help, res);
	}
	else if (res._statusCode == "403" || res._isReturn){
		res._Rpnse = true;
		return ;
	}
	else{
		res._statusCode = "501";
		res._message = "Not Implemented";
		res._contentType = "text/html";
		res._Rpnse = true;
		return ;
	}
}
