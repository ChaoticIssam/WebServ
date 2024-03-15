#include "configParss.hpp"

std::vector<configParss> _srv;

int main(){
    configParss obj;
    location    locationScoop;
    std::ifstream ifile;
    int lock_4 = 1;
    int lock_0 = 1;
    int lock_2 = 1;
    ifile.open("default.config");
    std::string file;
    try{
    if (ifile.is_open()){
        int Index = 0;
        while (ifile.good()){
            ifile >> file;
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
                    if (file == "method:") {
                        Index++;
                        ifile >> file;                
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
                        continue;
                    }
                    else if (file == "cgiStatus:"){
                         Index++;
                        ifile >> file;
                        if (file == "on")
                            locationScoop._cgiStatus = true;
                        else
                            locationScoop._cgiStatus = false;
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
    }
        if (obj.getPort().empty() || obj.getHost().empty())
            throw std::runtime_error("Error:");
    }
    catch(std::runtime_error &e){
            std::cout << "Error:\n something went wrong during reading the configFile." << std::endl;
            return -1;   
    }
}