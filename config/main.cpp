#include "configParss.hpp"

int main(){
    configParss obj;
    std::ifstream ifile;
    ifile.open("default.config");
    std::string file;
    if (ifile.is_open()){
        int Index = 0;
        while (ifile.good()){
            ifile >> file;
            std::cout << file << std::endl;
            if (file == "server{" && Index == 0){
                Index++;
                continue;
            }
            else if (file == "port:" && Index == 1){
                Index++;
                continue;
            }
            else if (!file.empty() && Index == 2){
                obj.setPort(file);
                Index++;
                continue;
            }
            else if (file == "host:" && Index == 3){
                Index++;
                continue;
            }
            else if (!file.empty() && Index == 4){
                obj.setHost(file);
                Index++;
                continue;
            }
            else if (file == "server_name:" && Index == 5)
            {
                Index++;
                continue;
            }
            else if (!file.empty() && Index == 6){
                obj.setServerName(file);
                Index++;
                continue;
            }
            else if (file == "error_page:" && Index == 7){
                Index++;
                continue;
            }
            else if (Index == 8){
                if (isdigit(stoi(file.c_str()))){
                    std::cout << "hello" << std::endl;
                }
            }
            // else{
            //     std::cout << "Error: in config file." << std::endl;
            //     return 1;
            // }
        }
    }
    // std::cout << obj.getHost() << std::endl;
    // std::cout << obj.getPort() << std::endl;
}