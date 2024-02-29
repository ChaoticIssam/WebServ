#include "configParss.hpp"

int main(){
    configParss obj;
    int i = 0;
    std::ifstream ifile;
    ifile.open("default.config");
    std::string file;
    try{
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
            else if (file == "server_name:"){
                ifile >> obj._servernameTMP;
                if (!ifile.good()){
                    std::cout << "Error: something went wrong with the server name insert." << std::endl;
                    exit(1);
                }
                obj._servernamesHolder.push_back(obj._servernameTMP);
                std::cout << "server name ra9m " << i << " ->>>" << obj._servernamesHolder[i] << std::endl;
                i++;
            }
            else if (file == "error_page:"){
                // Index++;
                ifile >> obj._errorPagenum;
                if (!ifile.good()){
                    std::cout << "Error: something went wrong with the error page config." << std::endl;
                    exit(1);
                }
                ifile >> obj._errorPage;
                if (!ifile.good()){
                    std::cout << "Error: something went wrong with the error page config." << std::endl;
                    exit(1);
                }
                obj.errorHolder[obj._errorPagenum] = obj._errorPage;
                std::cout << "wewe 1 ->>>>>" << obj.errorHolder[404] << std::endl;
                std::cout << "wewe 2 ->>>>>" << obj.errorHolder[420] << std::endl;
                std::cout << "wewe 3 ->>>>>" << obj.errorHolder[410] << std::endl;
                continue;
            }
            // else if (Index >= 8){
                
            //         // std::cout << "page error code assingned and it's -> " << obj.geterrorPagenum() << std::endl;
            // }
            else{
                throw std::runtime_error("error in config");
            }
        }
    }}
    catch(std::runtime_error &e){
            std::cout << "Error: in config file." << std::endl;
            return -1;
        
    }
    // std::cout << obj.getHost() << std::endl;
    // std::cout << obj.getPort() << std::endl;
}