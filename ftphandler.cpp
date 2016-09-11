#include "ftphandler.h"

FtpHandler::FtpHandler(Logger *logp) {
	log = logp;
}

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

}


size_t writeFile(void *buffer, size_t size, size_t nmemb, void *stream) {

	// Casting
	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Out(HANDLER, "Received file of size %d,");

	if (output && !output->stream) {
		output->stream = fopen(output->filename.c_str(), "wb");
		
		if (!output->stream) {
			
			return -1;
		}

		return fwrite(buffer, size, nmemb, output->stream);
	}
}

// To be sorted out later
bool FtpHandler::dumpMethod() {

	struct FtpFile ftpfile = {
		"/home/mumbler/output/ftp_output.txt",
		NULL,
		log
	};

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	log->Out(HANDLER, "About to attempt the Curl call");

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "ftp://localhost/input/ftpexample");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
		curl_easy_setopt(curl, CURLOPT_USERNAME, "mumbler");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "ASDFqwer1234"); //Changed my password temporarily for this program. Yes, this is a BAD idea.
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

	curl_global_cleanup();

	return 0;
}

