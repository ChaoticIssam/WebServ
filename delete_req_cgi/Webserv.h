#ifndef WEBSERV
# define WEBSERV

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include "./Request/Request.h"

#define MAX_CLIENTS 10
#define MAX_EVENTS 10
#define MAX_SIZE 1024

int     file_exists(const std::string &url);
int     fileLoad(std::string url, int currentSocket);

#endif