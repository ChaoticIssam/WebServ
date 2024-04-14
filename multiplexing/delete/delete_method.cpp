// #include "delete_method.h"
#include <sys/stat.h>
#include <sys/types.h>
#include "delete_method.h"




void    remove_dir(std::map<int, Webserve>&multi_fd, int fd, Helpers *help, const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        std::cerr << "Can not open the directory" << std::endl;
        return ;
    }

    struct dirent *entry;
    for ( ; (entry = readdir(dir)) != NULL ; ) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            std::string entry_path = std::string(path) + '/' + entry->d_name;
            if (entry->d_type == DT_DIR) {
                remove_dir(multi_fd, fd, help, path);
            }
            else {
                if (unlink(entry_path.c_str()) == -1) {
                    throw ResponseException("400", "Bad Request");
                    return ;
                }
                else
                    multi_fd[fd].status = 200;
            }
        }
    }

    closedir(dir);
    // Remove the empty directory itself
    if (rmdir(path) != 0) {
        throw ResponseException("400", "Bad Request");
    }
    else {
        multi_fd[fd].status = 204;
        // std::cout << "Directory removed correctly" << std::endl;
        throw ResponseException("204", "No content");
    }
}

void    path_check(std::map<int, Webserve>&multi_fd, int fd, Helpers *help) {
    // int flag = 0;

    // for (std::vector<location>::iterator it = help->obj._locationScoops.begin(); it != help->obj._locationScoops.end(); it++) {
    //     std::cout << "the Rootdirecriont: " << it->_rootDirectoryLocation << ", and the URL : " << multi_fd[fd].request_URI << std::endl;
    //     if (!(it->_rootDirectoryLocation.empty()) && multi_fd[fd].request_URI.find(it->_rootDirectoryLocation) == 0) {
    //         std::cout << "the location matches\n";
    //         flag = 1;
    //         multi_fd[fd].resource = ".." +  multi_fd[fd].request_URI;
    //     }
    // }

    // if (flag == 0) {
    //     if (multi_fd[fd].request_URI.find(help->obj._rootDirectory) == 0) {
    //         std::cout << "the uri matches the root location\n";
    //         multi_fd[fd].resource = ".." +  multi_fd[fd].request_URI;
    //     }
    // }

    // std::string resource = multi_fd[fd].request_URI.substr(1);
    // multi_fd[fd].resource = "/nfs/homes/bhazzout/Desktop/new" + help->obj._locationScoops[0]._locationPath + resource;
    // std::cout << "this location path: " << multi_fd[fd].resource << std::endl;
    // sleep(1);
    if (!multi_fd[fd].resource.empty()) {
        struct stat info;
        const char *path = multi_fd[fd].resource.c_str();

        if (stat(path, &info) == 0) {
            if (info.st_mode && S_ISDIR(info.st_mode)) { //the resource is directory

                if (multi_fd[fd].resource[multi_fd[fd].resource.length() - 1] != '/')
                    throw ResponseException("409", "409 conflict");
                else {
                    /////////////////////////////////////////////////
                    // need to check if directory has / at the end //
                    /////////////////////////////////////////////////

                    if ( (access(path, W_OK)) || (access(path, X_OK)) ) { //directory has no write or execute permission
                        throw ResponseException("403", "403 forbidden");
                    }
                    else { // dierctory has the right permissions
                        remove_dir(multi_fd, fd, help, path);
                    }
                }

            }
            if (info.st_mode && S_ISREG(info.st_mode)) { //the resource is a regular file

            /////////////////////////////////////////////////////////////////
            // need to check if the file has / at the end, it mustn't work //
            /////////////////////////////////////////////////////////////////

                if (multi_fd[fd].resource[multi_fd[fd].resource.length() - 1] == '/') {
                    throw ResponseException("404", "404 Not found");
                }
                else {
                    if (access(path, W_OK)) { //the file has no write permission
                        throw ResponseException("403", "403 forbidden");
                    }
                    else { // the requested file has the right permissions
                        if (unlink(path) == -1) {
                            throw ResponseException("400", "400 error");
                        }
                        else {
                            throw ResponseException("204", "No content");
                        }
                            
                    }
                }

            }
        }
        else {
        // Print error message
        throw ResponseException("404", "Not found");
        }
    }
    else {
        multi_fd[fd].status = 404;
        throw ResponseException("404", "404 the requested URL is not found");
    }
}

void    delete_method(std::map<int, Webserve>&multi_fd, int fd, Helpers *help) {

    // try {
        path_check(multi_fd, fd, help);
    // }
    // catch(const ResponseException& e)
    // {
    //     std::cout << "Exception caught: " << e.get_message() << ", Status: " << e.get_status() << std::endl;
    // }
}