#include "configParss.hpp"

std::vector<configParss> _srv;

int main(){
    configParss obj;
    location    locationScoop;
    std::ifstream ifile;
    int lock_0 = 1;
    int lock_1 = 1;
    int lock_2 = 1;
    int lock_3 = 1;
    int counts = 0;
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
            else if ((file == "port:" && Index != 1) || (file == "host:" && Index != 3)){
                throw std::runtime_error("Error: something went wrong with the server name insert."); 
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
                Index++;
                obj.setHost(file);
                continue;
            }
            else if (file == "server_name:"){
                Index++;
                ifile >> obj._servernameTMP;
                if (!ifile.good())
                    throw std::runtime_error("Error: something went wrong with the server name insert.");
                obj._servernamesHolder.push_back(obj._servernameTMP);
                continue;
            }
            else if (file == "error_page:"){
                Index++;
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
                Index++;
                ifile >> locationScoop._locationPath;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the location scoop config.");
                ifile >> file;
            }
            else if (file == "method:") {
                Index++;
                ifile >> file;                
                //  std::cout << "->>" << file << std::endl;
                if (file == "post"){
                    locationScoop._postCheck = true;
                    continue;
                }
                else if (file == "get"){
                    locationScoop._getCheck = true;
                    continue;
                }
                else if (file == "delete"){
                    locationScoop._deleteCheck = true;
                    continue;
                }
                else
                    throw std::runtime_error("Error:\n undefined method.");

            }
            else if (file == "autoIndex:"){
                Index++;
                ifile >> file;
                if (file == "on")
                    locationScoop._autoIndex = true;
                else
                    locationScoop._autoIndex = false;
                lock_0 = 0;
                Index++;
                continue;
            }
            else if (file == "Index:"){
                Index++;
                ifile >> locationScoop._Index;
                lock_1 = 0;
                continue;
            }
            else if (file == "return:"){
                Index++;
                ifile >> locationScoop._return;
                lock_2 = 0;
                continue;
            }
            else if (file == "upload_store:"){
                Index++;
                ifile >> locationScoop._uploadStore;
                lock_3 = 0;
                continue;
            }
            else if (file == "}"){
                counts++;
                continue ;
            }
            else
                throw std::runtime_error("Error:");
            if (lock_0 == 1)
                locationScoop._autoIndex = false;
            if (lock_0 == 1)
                locationScoop._Index = "/";
            if (lock_2 == 1)
                locationScoop._return = "";
        }
        // _srv.push_back(obj);
    }
        if (obj.getPort().empty() || obj.getHost().empty() ||
             locationScoop._locationPath.empty() || counts != 2)
            throw std::runtime_error("Error:");
    }
    catch(std::runtime_error &e){
            std::cout << "Error:\n something went wrong during reading the configFile." << std::endl;
            return -1;   
    }
    std::cout << "port -> " << obj.getPort() << std::endl;
    std::cout << "host -> " << obj.getHost() << std::endl;
    std::cout << "server name -> " << obj._servernamesHolder[0] << std::endl;
    // std::cout << "server name -> " << obj._servernamesHolder[1] << std::endl;
    // std::cout << "server name -> " << obj._servernamesHolder[2] << std::endl;
    // std::cout << "server error -> " << obj.errorHolder[404] << std::endl;
    std::cout << "server error -> " << obj.errorHolder[410] << std::endl;
    std::cout << "server error -> " << obj.errorHolder[420] << std::endl;
    std::cout << "location path -> " << locationScoop._locationPath << std::endl;
    std::cout << "post check -> " << locationScoop._postCheck << std::endl;
    std::cout << "get check -> " << locationScoop._getCheck << std::endl;
    std::cout << "delete check -> " << locationScoop._deleteCheck << std::endl;
    std::cout << "autoIndex -> " << locationScoop._autoIndex << std::endl;
    std::cout << "Index -> " << locationScoop._Index << std::endl;
    std::cout << "return -> " << locationScoop._return << std::endl;
    std::cout << "uploadStore -> " << locationScoop._uploadStore << std::endl;
}