#include "configParss.hpp"

std::vector<configParss> _srv;

int main(){
    configParss obj;
    location    locationScoop;
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
                if (!ifile.good())
                    throw std::runtime_error("Error: something went wrong with the server name insert.");
                obj._servernamesHolder.push_back(obj._servernameTMP);
                continue;
            }
            else if (file == "error_page:"){
                // Index++;
                ifile >> obj._errorPagenum;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the error page config.");
                ifile >> obj._errorPage;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the error page config.");
                obj.errorHolder[obj._errorPagenum] = obj._errorPage;
                continue;
            }
            else if (file == "location:"){
                ifile >> locationScoop._locationPath;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the location scoop config.");
                ifile >> file;
            }
            else if (file == "methods:") {
                while (file != "autoIndex:"){
                    ifile >> file;
                    std::cout << "->>" << file << std::endl;
                    if (file == "post"){
                        locationScoop._postCheck = true;
                        
                    }
                    else if (file == "get")
                        locationScoop._getCheck = true;
                    else if (file == "delete")
                        locationScoop._deleteCheck = true;
                    else
                        throw std::runtime_error("Error:\n undefined method.");
                }
                std::cout <<"post is : " << locationScoop._postCheck << "delete is :" << locationScoop._deleteCheck << "get is : " << locationScoop._getCheck;
            }
            else{
                throw std::runtime_error("Error: something went wrong with the location scoop config.");
            }
        }
        // _srv.push_back(obj);
    }
    }
    catch(std::runtime_error &e){
            std::cout << "Error:\n something went wrong during reading the configFile." << std::endl;
            return -1;   
    }
    // std::cout << obj.getHost() << std::endl;
    // std::cout << obj.getPort() << std::endl;
}