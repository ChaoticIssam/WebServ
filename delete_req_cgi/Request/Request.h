/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bhazzout <bhazzout@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/23 16:25:40 by bhazzout          #+#    #+#             */
/*   Updated: 2024/03/15 17:48:38 by bhazzout         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <map>
#include <vector>
#include <sstream>
// #include "delete/delete_method.h"
#include "../../config/config/configParss.hpp"

class location;

class	Request {
public:
	int                                 status;
	std::string                         uri;
	std::string							resource;
	std::map<std::string, std::string>	response_headers;
	std::string							response_body;
	std::vector<location>				locations;
};

class	Resource {
public:
	std::string	resource_type;
	int			has_slash;
	
};

class   RequestParser {
private:
	void    lineParse(Request &request, const std::string &requestline);
	void    headerParse(Request &request, const std::string &line);

public:
	Request requestParse(std::string rawRequest);
	void    requestPrint(Request http);
};

// int fileLoad(std::string url);
void    requestHandle(Request http);
bool    isUnreservedCharacter(char ch);
bool    isReservedCharacter(char ch);
bool    containsNotAllowedCharacter(const std::string& uri);
void    Get_handle(Request http);

#endif