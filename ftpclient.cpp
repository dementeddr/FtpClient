#include <curl/curl.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "logger.h"
#include "ftphandler.h"

int main(int argc, char *argv[]) {

	Logger log();
	std::string username;
	std::string password;

	if (argc !=4) {
		printf("Usage: ftpclient <hostname> <target-file> <local-filename>\n");
		return 1;
	}

	printf("Enter your ftp username: ");
	//getline(stdin, username);
	std::getline (std::cin, username);

	password = getpass("Enter your ftp password: ");

	//FtpHandler handler(&log);

	printf("Username: %s  Password: %s\n", username.c_str(), password.c_str());
}
