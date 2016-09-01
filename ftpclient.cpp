#include <stdio.h>
#include <curl/curl.h>

size_t writeFile(void *buffer, size_t size, size_t, nmemb, void *stream) {
	printf("We got a callback");

	return size;
}

int main(int argc, char *argv[]) {
	
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();

	if (curl) {
		curl_east_setopt(curl, CURLOPT_URL, "ftp://localhost/ftp_example");


	}
}
