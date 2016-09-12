#include <curl/curl.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "logger.h"
#include "ftphandler.h"

int main(int argc, char *argv[]) {

	Logger log;
	std::string username;
	std::string password;
	std::string hostname;

	if (argc !=2) {
		printf("Usage: ftpclient <hostname>\n");
		return 1;
	}

	// If this was a real program, I would have the client make
	// the sure the server exists *before* querying for the
	// credentials, as is general practice. Since I don't want
	// to waste time dicking around with more libraries or trying
	// to torture CURL into doing it, I'm going to cheat a bit.
	printf("Enter your ftp username: ");
	std::getline (std::cin, username);
	password = getpass("Enter your ftp password: ");

	printf("Username: %s  Password: %s\n", username.c_str(), password.c_str()); // TODO remove this.

	log.Out(MAIN, "About to start ftp");

	hostname = argv[1];
	FtpHandler handler(hostname, username, password, &log);

	if (!handler.Initialize()) {
		return 2; //Couldn't start CURL for some reason.
	}

	handler.ExecuteFtp();
}
