/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bhazzout <bhazzout@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 17:50:41 by bhazzout          #+#    #+#             */
/*   Updated: 2024/03/15 02:24:21 by bhazzout         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.h"

bool fileExists(const char *filePath) {
    std::ifstream fileStream(filePath);
    return fileStream.good();
}

int uri_map(Request http) {
    // for (std::map<std::string, std::string>::iterator it = http.locations.begin(); it != http.locations.end(); it++) {
    //     if (it->first == http.uri) {
    //         return 1;
    //     }
    // }
	std::string full_resource_path = http.locations[0]._locationPath + http.resource;
	if (fileExists(full_resource_path.c_str())) {
		std::cout << "file exists" << std::endl;
		return 1;
	}
	else
		std::cout << "file doesn't exist at " << full_resource_path << std::endl;
    return 0;
}

int method_allowance(Request http) {
	if ((http.method != "DELETE") && (http.method != "GET") && (http.method != "POST")) {
		return 1;
	}
	return 0;
}

void    requestHandle(Request http) {
	std::map<std::string, std::string> header = http.headers;
	int T_exist = 0;
	int CL_exist = 0;

	//check for the transfer encoding if it exists 
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++) {
		if (it->first == "Transfer-Encoding") {
			T_exist = 1;
			//Transfer-Encoding exist and is different to “chunked”
			if (it->second != "chunked") {
				throw std::runtime_error("501 not implemented");
			}
		}
		if (it->first == "Content-Length") {
			CL_exist = 1;
		}
	}

	//Transfer-Encoding not exist Content-Length not exist The method is Post
	if (T_exist == 0 && CL_exist == 0 && http.method == "POST") {
		throw std::runtime_error("400 Bad Request");
	}

	//Request uri contain a character not allowded
	if (containsNotAllowedCharacter(http.uri)) {
		throw std::runtime_error("400 Bad Request");
	}

	//Uri contains more thatn 2048 chars
	if (http.uri.length() > 2048) {
		throw std::runtime_error("414 Request-URI Too Long");
	}

	//location for the requested uri
	if (http.uri == "/")
	    ;
	else if (uri_map(http) == 0 && http.uri != "/") {
	    throw std::runtime_error("404 Not Found");
	}

	//allowance of the method
	if (method_allowance(http)) {
		throw std::runtime_error("405 Method Not Allowed");
	}

	//Check which requested method
	// if (http.method == "GET")
	//     Get_handle(http);
	if (http.method == "DELETE")
	    delete_method(http);
}