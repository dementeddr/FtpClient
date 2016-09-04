#include <stdio.h>
#include <curl/curl.h>

struct FtpFile {
	const char *filename;
	FILE *stream;
};

size_t writeFile(void *buffer, size_t size, size_t nmemb, void *stream) {
	printf("We got a callback\n");

	
	return size;
}

int main(int argc, char *argv[]) {
	
	CURL *curl;
	CURLcode res;

	struct FtpFile ftpfile = {
		"/home/mumbler/output/ftp_output.txt",
		NULL
	};

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "ftp://localhost/input/ftpexample");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
		curl_easy_setopt(curl, CURLOPT_USERNAME, "mumbler");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "ASDFqwer1234"); //Changed my password temporarily for this program.
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			printf("FTP could not complete. Curl error code: %d\n", res); 
		}
		
	} else {
		printf("Curl could not initialize\n");
	}

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	curl_global_cleanup();

	return 0;
}
