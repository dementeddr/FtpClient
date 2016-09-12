#include <curl/curl.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

#include "logger.h"
#include "ftphandler.h"


void InputLoop(Logger *log, FtpHandler *handler) {

	std::string input;
	std::string cmd;
	std::string word;
	
	CURLcode res = CURLE_OK;

	printf("Entering Input loop\n");

	do {
		printf("[%s]> ", handler->GetPath().c_str());

		std::vector<std::string> params;

		getline(std::cin, input);
		std::stringstream iss(input);

		iss >> cmd;

		while (iss >> word) {
			params.push_back(word);
		}

		if (cmd == "list" || cmd == "ls") {
			if (params.size() == 0) {
				handler->FtpList("");
			} else if (params.size() == 1) {
				handler->FtpList(params[0]);
			} else {
				log->Out(HANDLER, "Too many arguments for list command");
			}

		} else if (cmd == "retr" || cmd == "get") {
			if (params.size() == 2) {
				handler->FtpRetrieve(params[0], params[1]);
			} else if (params.size() == 1) {
				handler->FtpRetrieve(params[0], params[0]);
			} else {
				log->Out(HANDLER, "Too many arguments for list command"); 
			}

		} else if (cmd == "stor" || cmd == "put") {
			if (params.size() == 2) {
				//handler->FtpRetrieve(params[0], params[1]);
			} else {
				//log->Out(HANDLER, "Retrieve command requires two arguments"); 
			}

		} else if (cmd == "cd"   || cmd == "cwd") {
			if (params.size() == 0) {
				handler->FtpChangeDir("");
			} else if (params.size() == 1) {
				handler->FtpChangeDir(params[0]);
			} else {
				log->Out(HANDLER, "Too many arguments for cd command");
			}

		} else if (cmd == "exit" || cmd == "quit") {
			break;
		} else{
			log->Out(HANDLER, "\"" + cmd + "\" is not an ftpclient command.");
		}

	} while (true);
}


int main(int argc, char *argv[]) {

	Logger log;
	CURLcode res;
	std::string username;
	std::string password;
	std::string hostname;

	if (argc !=2) {
		printf("Usage: ftpclient <hostname>\n");
		return 1; // The actual return value isn't important
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
		// Couldn't start CURL for some reason. The actual 
		// number isn't important.
		return 2; 
	}

	//res = handler.FtpList("."); 

/*
	if (res != CURLE_OK) {
		return res; 
	}
*/
	InputLoop(&log, &handler);
}
