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

	if (curl) {
		log->Out(HANDLER, "CURL initialized");
		return true;

	} else {
		log->Out(HANDLER, "ERROR: CURL could not initialize");
		return false;
	}
}


FtpHandler::~FtpHandler() {
	curl_global_cleanup();
}


size_t writeFile(void *buffer, size_t size, size_t nmemb, void *stream) {

	// Casting
	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Out(HANDLER, "Received file of size " + ((int) size));

	if (output && !output->stream) {
		output->stream = fopen(output->filename.c_str(), "wb");
		
		if (!output->stream) {
			
			return -1;
		}

		return fwrite(buffer, size, nmemb, output->stream);
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
}


// To be sorted out later
bool FtpHandler::ExecuteFtp() {

	struct FtpFile ftpfile = {
		"/home/mumbler/output/ftp_output.txt",
		NULL,
		log
	};

	//ftpfile = ffile;

	log->Out(HANDLER, "About to attempt the Curl call");

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, GetFullUrl());
		curl_easy_setopt(curl, CURLOPT_STDERR, log->logfile);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str()); //Changed my password temporarily for this program. Yes, this is a BAD idea.
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			log->Out(HANDLER, "FTP could not complete. Curl error code: " + res); 
		} else {
			log->Out(HANDLER, "FTP was successful");
		}
		
	} else {
		log->Out(HANDLER, "Curl could not initialize");
	}

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	return 0;
}

