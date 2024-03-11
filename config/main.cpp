#include "configParss.hpp"

std::vector<configParss> _srv;

int main(){
    configParss obj;
    location    locationScoop;
    std::ifstream ifile;
    int lock_4 = 1;
    int lock_0 = 1;
    int lock_1 = 1;
    int lock_2 = 1;
    int lock_3 = 1;
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
                lock_4 = 0;
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
            else if (file == "server_name:" && Index > 4){
                Index++;
                ifile >> obj._servernameTMP;
                if (!ifile.good())
                    throw std::runtime_error("Error: something went wrong with the server name insert.");
                obj._servernamesHolder.push_back(obj._servernameTMP);
                continue;
            }
            // else if ((Index == 5 && file == "error_page:") || (Index == 5 && file == "location:")){
            //     std::cout << "file >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << file << std::endl;
            //     Index++;
            //     obj._servernameTMP = obj.getHost();
            //     obj._servernamesHolder.push_back(obj._servernameTMP);
            // }
            else if (file == "error_page:"){
                if (Index == 5){
                    obj._servernameTMP = obj.getHost();
                    obj._servernamesHolder.push_back(obj._servernameTMP);
                }
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
                if (Index == 5){
                    obj._servernameTMP = obj.getHost();
                    obj._servernamesHolder.push_back(obj._servernameTMP);
                }
                Index++;
                locationScoop._locationPath = "";
                locationScoop._autoIndex = false;
                locationScoop._postCheck = false;
                locationScoop._getCheck = false;
                locationScoop._deleteCheck = false;
                locationScoop._Index = "";
                locationScoop._return = "";
                locationScoop._uploadStore = "";
                ifile >> locationScoop._locationPath;
                    if (!ifile.good())
                        throw   std::runtime_error("Error: something went wrong with the location scoop config.");
                
                ifile >> file;
                if (file == "{"){
                    while (!file.empty() && file != "}"){
                    ifile >> file;
                         std::cout << "->>" << file << std::endl;
                    if (file == "method:") {
                        Index++;
                        ifile >> file;                
                         std::cout << "->>" << file << std::endl;
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
                    else if (file == "cgiStatus:"){
                         Index++;
                        ifile >> file;
                        if (file == "on")
                            locationScoop._cgiStatus = true;
                        else
                            locationScoop._cgiStatus = false;
                        // lock_0 = 0;
                        Index++;
                        continue;
                    }
                    else if (file == "cgi_path:"){
                        Index++;
                        ifile >> locationScoop._cgiExtensionHolder;
                        if (!ifile.good())
                            throw   std::runtime_error("Error: something went wrong with the cgi path config.");
                        ifile >> locationScoop._cgiPathHolder;
                        if (!ifile.good())
                            throw   std::runtime_error("Error: something went wrong with the cgi path config.");
                        locationScoop._cgiPath[locationScoop._cgiExtensionHolder] = locationScoop._cgiPathHolder;
                        continue;
                    }
                    else if (file == "}"){
                        obj._locationScoops.push_back(locationScoop);
                        break;
                    }
                    else
                        throw std::runtime_error("Error:");
                }
                // continue;
                }
            }
            else if (file == "}" && lock_4 == 0){
                _srv.push_back(obj);
                Index = 0;
                lock_4 = 1;
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
        if (obj.getPort().empty() || obj.getHost().empty())
            throw std::runtime_error("Error:");
    }
    catch(std::runtime_error &e){
            std::cout << "Error:\n something went wrong during reading the configFile." << std::endl;
            return -1;   
    }
    std::cout << "port -> " << _srv[0].getPort() << std::endl;
    // std::cout << "port -> " << _srv[1].getPort() << std::endl;
    std::cout << "host -> " << _srv[0].getHost() << std::endl;
    // std::cout << "host -> " << _srv[1].getHost() << std::endl;
    std::cout << "server name -> " << _srv[0]._servernamesHolder[0] << std::endl;
    // std::cout << "server name -> " << _srv[1]._servernamesHolder[1] << std::endl;
    // std::cout << "server name -> " << obj._servernamesHolder[1] << std::endl;
    // std::cout << "server name -> " << obj._servernamesHolder[2] << std::endl;
    // std::cout << "server error -> " << obj.errorHolder[404] << std::endl;
    std::cout << "server error 0 -> " << obj.errorHolder[420] << std::endl;
    std::cout << "server error 1 -> " << obj.errorHolder[420] << std::endl;
    std::cout << "location path 0 -> " << obj._locationScoops[0]._locationPath << std::endl;
    std::cout << "location path 1 -> " << obj._locationScoops[1]._locationPath << std::endl;
    // std::cout << "post check 0 -> " << obj._locationScoops[0]._postCheck << std::endl;
    // std::cout << "post check 1 -> " << obj._locationScoops[1]._postCheck << std::endl;
    std::cout << "cgiPath 0 -> " << obj._locationScoops[1]._cgiPath[".py"] << std::endl;
    std::cout << "post check 0 -> " << obj._locationScoops[0]._postCheck << std::endl;
    std::cout << "post check 1 -> " << obj._locationScoops[1]._postCheck << std::endl;
    std::cout << "get check 0 -> " << obj._locationScoops[0]._getCheck << std::endl;
    std::cout << "get check 1 -> " << obj._locationScoops[1]._getCheck << std::endl;
    std::cout << "delete check 0 -> " << obj._locationScoops[0]._deleteCheck << std::endl;
    std::cout << "delete check 1 -> " << obj._locationScoops[1]._deleteCheck << std::endl;
    std::cout << "autoIndex 0 -> " << obj._locationScoops[0]._autoIndex << std::endl;
    std::cout << "autoIndex 1 -> " << obj._locationScoops[1]._autoIndex << std::endl;
    std::cout << "Index 0 -> " << obj._locationScoops[0]._Index << std::endl;
    std::cout << "Index 1 -> " << obj._locationScoops[1]._Index << std::endl;
    std::cout << "return 0 -> " << obj._locationScoops[0]._return << std::endl;
    std::cout << "return 1 -> " << obj._locationScoops[1]._return << std::endl;
    std::cout << "uploadStore 0 -> " << obj._locationScoops[0]._uploadStore << std::endl;
    std::cout << "uploadStore 1 -> " << obj._locationScoops[1]._uploadStore << std::endl;
}