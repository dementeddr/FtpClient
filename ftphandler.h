#include <curl/curl.h>
#include <string>
#include <vector>

#include "logger.h"

#ifndef FTPHANDLER_H
#define FTPHANDLER_H

struct FtpFile {
	const std::string filename;
	FILE *stream;
	Logger *log;
};


class FtpHandler {

	public:
		FtpHandler();
		FtpHandler(std::string hostname, std::string username, std::string password, Logger *log);
		~FtpHandler();

		bool Initialize();
		
		std::string GetPath();
		std::string GetUrl();

		CURLcode FtpList(std::string input);
		CURLcode FtpChangeDir(std::string input);
		CURLcode FtpRetrieve(std::string target, std::string destination);
		CURLcode ExecuteFtp();
	
	private:
		CURL *curl;
		CURLcode res;
		Logger *log;

		std::string username;
		std::string password;
		std::string hostname;

		std::vector<std::string> pathPieces;
};

#endif
