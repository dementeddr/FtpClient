#include <curl/curl.h>
#include "logger.h"

struct FtpFile {
	const char *filename;
	FILE *stream;
};

size_t writeFile(void *buffer, size_t size, size_t nmemb, void *stream) {

	// Eventually, this whole thing will be an object and we can
	// log through the logger. Since this is a specific callback,
	// we can't really pass it in.
	printf("We got a callback\n");

	// Casting
	struct FtpFile *output=(struct FtpFile *) stream;

	if (output && !output->stream) {
		output->stream = fopen(output->filename, "wb");
		
		if (!output->stream) {
			// Log an error here
			return -1;
		}

		return fwrite(buffer, size, nmemb, output->stream);
	}
}

int main(int argc, char *argv[]) {
	
	CURL *curl;
	CURLcode res;
	Logger log;

	struct FtpFile ftpfile = {
		"/home/mumbler/output/ftp_output.txt",
		NULL
	};

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	log.Out("About to attempt the Curl call");

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "ftp://localhost/input/ftpexample");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
		curl_easy_setopt(curl, CURLOPT_USERNAME, "mumbler");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "ASDFqwer1234"); //Changed my password temporarily for this program.
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			log.Out("FTP could not complete. Curl error code: " + res); 
		} else {
			log.Out("FTP was successful");
		}
		
	} else {
		log.Out("Curl could not initialize");
	}

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	curl_global_cleanup();

	return 0;
}
