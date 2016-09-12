#include "ftphandler.h"

// This constructor is not actually supported.
FtpHandler::FtpHandler() :
	username("anonymous"),
	password(""),
	log(NULL) {
	
	urlPieces.push_back("ftp://");
	}

FtpHandler::FtpHandler(std::string hostname, std::string username, std::string password, Logger *log) : 
	username(username),
	password(password),
	log(log) {
	
	urlPieces.push_back("ftp://" + hostname);
}

// Ideally this would just be in the constructor, but you need to
// check the curl_easy_init return and kill the program if it
// failed.
bool FtpHandler::Initialize() {

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl == NULL) {
		log->Out(HANDLER, "ERROR: CURL could not initialize");
		return false;
	}

	log->Out(HANDLER, "CURL initialized");

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_STDERR, log->logfile);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str()); 
	
	return true;
}


FtpHandler::~FtpHandler() {
	curl_global_cleanup();
}


size_t writeCallback(void *buffer, size_t size, size_t nmemb, void *stream) {

	// Casting
	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Out(HANDLER, "Received file of size " + ((int) size));

	if (output && !output->stream) {
		output->stream = fopen(output->filename.c_str(), "wb");
		
		if (!output->stream) {
			output->log->Out(HANDLER, "Could not open file " + output->filename + " for writing.");
			return -1;
		}

		return fwrite(buffer, size, nmemb, output->stream);
	}
}


size_t listCallback(void *buffer, size_t size, size_t nmemb, void *stream) {
	
	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Out(HANDLER, "Received list of size " + std::to_string(size));

	if (output && !output->stream) {
		return fwrite(buffer, size, nmemb, stdout);
	}
}


std::string FtpHandler::GetFullUrl() {

	std::string url = "";

	for(std::string &part : urlPieces) {
		url.append(part);

		if (part.back() != '/') {
			url.push_back('/');
		}
	}

	return url;
}


CURLcode FtpHandler::FtpList(std::string input) {
	
	struct FtpFile ftpfile = {
		"unused",
		NULL,
		log
	};

	//std::string url = GetFullUrl() + input;
	std::string request = "LIST " + input;

	printf("FtpList: %s\nRequest: %s\n", GetFullUrl().c_str(), request.c_str());

	curl_easy_setopt(curl, CURLOPT_URL, GetFullUrl().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, listCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
//	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "LIST " + input);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());

	CURLcode res = ExecuteFtp();

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	return res;
}


// To be sorted out later
CURLcode FtpHandler::ExecuteFtp() {

	log->Out(HANDLER, "About to attempt the Curl call");

	if (curl) {

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			log->Out(HANDLER, "FTP could not complete. Curl error code: " + std::to_string((unsigned int) res)); 
		} else {
			log->Out(HANDLER, "FTP was successful");
		}
		
	} else {
		log->Out(HANDLER, "Curl could not initialize");
	}

	return res;
}

