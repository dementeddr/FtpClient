#include <curl/curl.h>
#include <string>

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
		FtpHandler(Logger *logp);
		~FtpHandler();

		bool Initialize();
	//	size_t writeFile(void *buffer, size_t size, 
	//		size_t nmemb, void *stream);
		
		bool dumpMethod();
	
	private:
		CURL *curl;
		CURLcode res;
		Logger *log;
};

#endif
