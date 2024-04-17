#include "GetMethod.hpp"

void getMethod(std::map<int, Webserve>& multi_fd, int fd, Helpers* help) {
	Response Res;
	Res.convertExtention();
	Res.uriParss(multi_fd, fd, help);
	int type = Res.resourceType();
	if (type == NOT_FOUND) {
		if (access(Res._URI.c_str(), F_OK) == 0)
			throw ResponseException("403", "Forbidden - 1");
		else
			throw ResponseException("404", "Not Found - 1");
	}
	if (type == DIREC) {
		if (Res._URI[Res._URI.size() - 1] != '/') {
			std::cout << "1???????????????????????????????????????????????????????????????????????????????????????" << std::endl;
			Res._URI += "/";
			throw ResponseException("301", "Moved Permanently");
			return;
		} else {
			std::cout << "2???????????????????????????????????????????????????????????????????????????????????????" << std::endl;
			Res.findFiles(); //***
			Res._contentType = "text/html";
			return;
		}
	} else if (type == FILE) {
		std::string extension = Res.getExtension();
		Res._file.open(Res._URI.c_str(), std::ios::in | std::ios::out);
		if (!Res._file.good()) {
			throw ResponseException("403", "Forbidden - 4");
			return;
		}
		if (Res._extensions[Res.getExtension()].empty())
			Res._contentType = "text/plain";
		else
		Res._contentType = Res._extensions[Res.getExtension()];
		Res._responseHead = "HTTP/1.1 200 OK\r\n";
		Res._responseHead += "Content-Type: " + Res._contentType + "\r\n";
		Res._responseHead += "Content-Length: ";
		Res._file.seekg(0, std::ios::end);
		Res._contentLength = Res._file.tellg();
		Res._file.seekg(0, std::ios::beg);
		std::stringstream ss;
		ss << Res._contentLength;
		ss >> Res._contentLength;
		Res._responseHead += ss.str() + "\r\n\r\n";
		send(fd, Res._responseHead.c_str(), Res._responseHead.length(), 0);
		while (true) {
			Res._file.read(Res._responseBUFFER, BUFFER_SIZE);
			std::streamsize bytesRead = Res._file.gcount();
			if (bytesRead == 0)
				break;
			send(fd, Res._responseBUFFER, bytesRead, 0);
			if (bytesRead < BUFFER_SIZE)
				Res._file.seekg(-bytesRead, std::ios::cur);
		}
		close(fd);
		return;
	}
}