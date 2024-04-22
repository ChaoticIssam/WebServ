#include "../../multiplexing/webserve.hpp"

std::vector<configParss> _srv;

int issam_main(int ac,  char **av, Helpers &help) {
    // configParss help.obj;
    // location    help.locationScoop;
    std::ifstream ifile;
    int lock_4 = 1;
    int lock_0 = 1;
    int lock_2 = 1;
        if (ac != 2)
            exit(1);
    ifile.open(av[1]);
    std::string file;
    try{   
    if (ifile.is_open()){
        int Index = 0;
        while (ifile.good()){
            ifile >> file;
            if (file == "server{" && Index == 0){
                Index++;
                lock_4 = 0;
                help.obj._locationScoops.clear();
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
                help.obj.setPort(file);
                Index++;
                continue;
            }
            else if (file == "host:" && Index == 3){
                Index++;
                continue;
            }
            else if (!file.empty() && Index == 4){
                Index++;
                help.obj.setHost(file);
                continue;
            }
            else if (file == "server_name:" && Index > 4){
                Index++;
                ifile >> help.obj._servernameTMP;
                if (!ifile.good())
                    throw std::runtime_error("Error: something went wrong with the server name insert.");
                help.obj._servernamesHolder.push_back(help.obj._servernameTMP);
                continue;
            }
            else if (file == "error_page:"){
                if (Index == 5){
                    help.obj._servernameTMP = help.obj.getHost();
                    help.obj._servernamesHolder.push_back(help.obj._servernameTMP);
                }
                Index++;
                ifile >> help.obj._errorPagenum;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the error page config.");
                ifile >> help.obj._errorPage;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the error page config.");
                help.obj.errorHolder[help.obj._errorPagenum] = help.obj._errorPage;
                continue;
            }
			else if (file == "root_directory:")
			{
				Index++;
				ifile >> help.obj._rootDirectory;
				if (!ifile.good())
					throw   std::runtime_error("Error: something went wrong with the root directory config.");
				// std::cout << "root_directory: " << help.obj._rootDirectory << std::endl;
				continue;
			}
            else if (file == "rootIndex: "){
                Index++;
                ifile >> help.obj._rootIndex;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the error page config.");
                continue;
            }
            else if (file == "max_length:"){
                Index++;
                ifile >> help.obj._maxLength;
                if (!ifile.good())
                    throw   std::runtime_error("Error: something went wrong with the max length config.");
                continue;
            }
            else if (file == "location:"){
                if (Index == 5){
                    help.obj._servernameTMP = help.obj.getHost();
                    help.obj._servernamesHolder.push_back(help.obj._servernameTMP);
                }
                Index++;
                help.locationScoop._locationPath.clear();
                help.locationScoop._autoIndex = false;
                help.locationScoop._postCheck = false;
                help.locationScoop._getCheck = false;
                help.locationScoop._deleteCheck = false;
				help.locationScoop._cgiStatus = false;
				help.locationScoop._rootDirectoryLocation.clear();
				help.locationScoop._cgiPath.clear();
                help.locationScoop._Index.clear();
                help.locationScoop._return.clear();
                help.locationScoop._uploadStore.clear();
                ifile >> help.locationScoop._locationPath;
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
                            help.locationScoop._postCheck = true;
                        continue;
                        }
                        else if (file == "get"){
                            help.locationScoop._getCheck = true;
                            continue;
                        }
                        else if (file == "delete"){
                            help.locationScoop._deleteCheck = true;
                            continue;
                        }
                        else
                            throw std::runtime_error("Error:\n undefined method.");
                    }
                    else if (file == "autoIndex:"){
                        Index++;
                        ifile >> file;
                        if (file == "on")
                            help.locationScoop._autoIndex = true;
                        else if (file == "off")
                            help.locationScoop._autoIndex = false;
						else
							throw std::runtime_error("Error: something went wrong with the autoIndex config.");
                        lock_0 = 0;
                        Index++;
                        continue;
                    }
					else if (file == "root_directory:"){
						Index++;
						ifile >> help.locationScoop._rootDirectoryLocation;
						// std::cout << "root_directory: " << help.locationScoop._rootDirectoryLocation << std::endl;
						if (!ifile.good())
							throw   std::runtime_error("Error: something went wrong with the root directory config.");
						continue;
					}
                    else if (file == "Index:"){
                        Index++;
                        ifile >> help.locationScoop._Index;
						if (!ifile.good())
							throw   std::runtime_error("Error: something went wrong with the root directory config.");
                        continue;
                    }
                    else if (file == "return:"){
                        Index++;
                        ifile >> help.locationScoop._return;
						if (!ifile.good())
							throw   std::runtime_error("Error: something went wrong with the root directory config.");
                        lock_2 = 0;
                        continue;
                    }
                    else if (file == "upload_store:"){
                        Index++;
                        ifile >> help.locationScoop._uploadStore;
						if (!ifile.good())
							throw   std::runtime_error("Error: something went wrong with the root directory config.");
                        continue;
                    }
                    else if (file == "cgiStatus:"){
                         Index++;
                        ifile >> file;
                        if (file == "on")
                            help.locationScoop._cgiStatus = true;
                        else if (file == "off")
                            help.locationScoop._cgiStatus = false;
						else
							throw std::runtime_error("Error: something went wrong with the cgi status config.");
                        Index++;
                        continue;
                    }
                    else if (file == "cgi_path:"){
                        Index++;
                        ifile >> help.locationScoop._cgiExtensionHolder;
                        if (!ifile.good())
                            throw   std::runtime_error("Error: something went wrong with the cgi path config.");
                        ifile >> help.locationScoop._cgiPathHolder;
                        if (!ifile.good())
                            throw   std::runtime_error("Error: something went wrong with the cgi path config.");
                        help.locationScoop._cgiPath[help.locationScoop._cgiExtensionHolder] = help.locationScoop._cgiPathHolder;
                        // std::cout << "first: " << help.locationScoop._cgiPath[help.locationScoop._cgiExtensionHolder]<< " === second: " << help.locationScoop._cgiExtensionHolder << std::endl;
                        continue;
                    }
                    else if (file == "}"){
                        help.obj._locationScoops.push_back(help.locationScoop);
                        break;
                    }
                    else
                        throw std::runtime_error("Error:");
                }
                }
            }
            else if (file == "}" && lock_4 == 0){
                _srv.push_back(help.obj);
                Index = 0;
                lock_4 = 1;
                continue ;
            }
            else
                throw std::runtime_error("Error:");
            if (lock_0 == 1)
                help.locationScoop._autoIndex = false;
            if (lock_0 == 1)
                help.locationScoop._Index = "/";
            if (lock_2 == 1)
                help.locationScoop._return = "";
        }
    }
        if (help.obj.getPort().empty() || help.obj.getHost().empty())
            throw std::runtime_error("Error:");
    }
    catch(std::runtime_error &e){
            std::cout << "Error:\n something went wrong during reading the configFile." << std::endl;
            return -1;   
    }
    return 0;
}