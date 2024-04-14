// #include "Request.h"

// std::string accurate_location(Request http) {
//     for (std::map<std::string, std::string>::iterator it = http.locations.begin(); it != http.locations.end(); it++) {
//         if (it->first == http.uri) {
//             return it->second;
//         }
//     }
//     return NULL;
// }

// void    Directory_handle(Request http, std::string &requested_resource) {
//     // if the requested directory doesn't have / at the end, we need to redirect it to "/directory/"
//     if (requested_resource[ requested_resource.length() - 1 ] != '/') {
//         std::string directory_location = accurate_location(http);
//         std::cout << "Before: " << directory_location << std::endl;
//         directory_location += '/';
//         std::cout << "301 Moved Permanently." << std::endl;
//     }
// }

// void    Get_handle(Request http) {
//     std::string requested_resource;

//     for (std::map<std::string, std::string>::iterator it = http.locations.begin(); it != http.locations.end(); it++) {
//         if (it->first == http.uri) {
//             requested_resource = it->second;
//         }
//     }

//     if (!requested_resource.empty()) {
//         struct stat info;
//         const char *path = requested_resource.c_str();

//         if (stat(path, &info) == 0) {
//             if (S_ISDIR(info.st_mode)) {
//                 std::cout << "Directory\n";
//                 Directory_handle(http, requested_resource);
//                 // return true;
//             } else {
//                 // return false;
//                 std::cout << "File\n";
//             }
//         }
//     }
//     // struct stat info;
//     // std::string root_path = "/resources";
//     // std::string full_path = root_path + http.uri;
//     // const char *path = full_path.c_str();

//     // std::cout << "full path is : " << path << std::endl;
//     // if (stat(path, &info) == 0) {
//     //     if (S_ISDIR(info.st_mode)) {
//     //         std::cout << "Directory\n";
//     //         // return true;
//     //     } else {
//     //         // return false;
//     //         std::cout << "File\n";
//     //     }
//     // }
//     // std::cout << "this is the path: " << path << std::endl;
//     // return 1;
// }