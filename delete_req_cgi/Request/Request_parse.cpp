/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_parse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bhazzout <bhazzout@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 17:46:27 by bhazzout          #+#    #+#             */
/*   Updated: 2024/02/01 22:49:37 by bhazzout         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.h"

void    RequestParser::requestPrint(Request http) {
    std::cout << "Method : " << http.method << std::endl;
    std::cout << "Url : " << http.uri << std::endl;
    std::cout << "Resource: " << http.resource << std::endl;
    std::cout << "Version : " << http.version << std::endl;
    std::cout << "Headers : " << std::endl;
    // std::cout << "Path location: " << http.path_location << std::endl;
    for (std::map<std::string, std::string>::iterator it = http.headers.begin(); it != http.headers.end(); it++) {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    std::cout << "Body : " << http.body << std::endl;
}

void    RequestParser::headerParse(Request &request, const std::string &line) {
    std::size_t colonPos = line.find(':');

    if (colonPos != std::string::npos) {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1, line.size());
        request.headers[key] = value;
    }
}

void    RequestParser::lineParse(Request &request, const std::string &requestline) {
    std::istringstream  line(requestline);
    line >> request.method >> request.uri >> request.version;
    request.resource = request.uri.substr(1);
}

Request RequestParser::requestParse(std::string rawRequest) {
    Request             httpRequest;
    std::istringstream  requestStream(rawRequest);
    std::string         line;

    //init the resources map
    issam_main(httpRequest);
    // init_resources(httpRequest);
    // std::cout << "this is the location path : " << httpRequest.locations[0]._locationPath << std::endl;

    //parse the request line
    if (std::getline(requestStream, line)) {
        lineParse(httpRequest, line);
    }

    //parse the header
    while (std::getline(requestStream, line))
    {
        // std::cout << "This is the line " << line << std::endl;
        headerParse(httpRequest, line);
    }
    
    //parse the body
    std::getline(requestStream, httpRequest.body, '\0');
    return (httpRequest);
}
