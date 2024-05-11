#include <iostream>
#include <cctype>
#include <cstring>

bool		pars_http_version(std::string	HTTP_version){
	std::string first, second;
	int	f, s, pos1;
    std::cout << HTTP_version << std::endl;

	if(strncmp(HTTP_version.c_str(), "HTTP/", 5) == 0)
	{
        std::cout << HTTP_version << std::endl;

		HTTP_version = HTTP_version.substr(5);

        std::cout << HTTP_version << std::endl;

		pos1 = HTTP_version.find(".");
        if(pos1 != std::string::npos){
            first = HTTP_version.substr(0, pos1);
            std::cout << first << std::endl;

            second = HTTP_version.substr(pos1 + 1);
            if(first.length() != 1 || second.length() != 1)
                return false;
            // std::cout << f << std::endl;
            if(!isdigit(first[0]) || !isdigit(second[0]))
                return false;
            return true;
        }
        return false;
	}
	return(false);
}

int main(){
    std::cout << pars_http_version("HTTP/ 1.0") << std::endl;
}