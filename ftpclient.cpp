#include <curl/curl.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

#include "logger.h"
#include "ftphandler.h"


/*
 * This function controls the program proper. It takes command-
 * line-style user input, checks for superficial errors, calls
 * the relevant functions in the handler, and then waits for
 * more input.
 */
void InputLoop(Logger *log, FtpHandler *handler) {

	std::string input;
	std::string cmd;
	std::string word;
	
	CURLcode res = CURLE_OK;

	log->Log(MAIN, "Entering main input loop");

	do {
		// The command prompt for the user.
		printf("[%s]> ", handler->GetPath().c_str());

		// This will hold the list of parameters after the
		// command itself
		std::vector<std::string> params;

		getline(std::cin, input);
		log->Log(MAIN, "Command entered: \"" + input + "\"");

		// Get the base command
		std::stringstream iss(input);
		iss >> cmd;

		// Fill the list of parameters
		while (iss >> word) {
			params.push_back(word);
		}

		// A listing command
		if (cmd == "list" || cmd == "ls") {
			if (params.size() == 0) {
				handler->FtpList("");
			} else if (params.size() == 1) {
				handler->FtpList(params[0]);
			} else {
				printf("Too many arguments for 'list' command\n");
			}

		// A command to download a file
		} else if (cmd == "retr" || cmd == "get") {
			if (params.size() == 2) {
				handler->FtpRetrieve(params[0], params[1]);
			} else if (params.size() == 1) {
				handler->FtpRetrieve(params[0], params[0]);
			} else {
				printf("Too many arguments for 'retr/get' command\n"); 
			}

		// A command to upload a file
		} else if (cmd == "stor" || cmd == "put") {
			printf("Sorry uploading is not currently available\n");

		} else if (cmd == "cd"   || cmd == "cwd") {
			if (params.size() == 0) {
				handler->FtpChangeDir("");
			} else if (params.size() == 1) {
				handler->FtpChangeDir(params[0]);
			} else {
				printf("Too many arguments for 'cd' command\n");
			}
		
		// Exit ftpclient
		} else if (cmd == "exit" || cmd == "quit") {
			log->Log(MAIN, "Exiting...");
			break;

		// The help card
		} else{
			printf("\"%s\" is not an ftpclient command.\n"
				"Try:\n"
				"'ls' or 'list':   List files in a directory\n"
				"'cd' or 'cwd':    Change directory\n"
				"'get' or 'retr':  Download a file\n"
				"'exit' or 'quit': Close out of ftpclient\n\n",
				cmd.c_str());
		}

	// Bad practice I know, but the exit condition is entirely in
	// the user's hands.
	} while (true);
}


/*
 * The entrypoint for the program. This funtion queries the user
 * for login credentials, creates the logger for the program, and
 * gets the FTP ball rolling
 */
int main(int argc, char *argv[]) {

	Logger log;
	CURLcode res;
	std::string username;
	std::string password;
	std::string hostname;

	// Make sure we have correct input
	if (argc !=2) {
		printf("Usage: ftpclient <hostname>\n");
		return 1; // The actual return value isn't important
	}

	// Get the login credentials for the FTP server.
	//
	// If this was production program, I would have the client make
	// the sure the server exists *before* querying for the
	// credentials, as is general practice. Since I don't want
	// to waste time dicking around with more libraries or trying
	// to torture CURL into doing it, I'm going to cheat a bit.
	printf("Enter your remote username: ");
	std::getline (std::cin, username);
	password = getpass("Enter your remote password: ");

	log.Log(MAIN, "Received login credentials for user " + username);

	hostname = argv[1];
	log.Log(MAIN, "Hostname for the FTP server is " + hostname);

	FtpHandler handler(hostname, username, password, &log);

	if (!handler.Initialize()) {
		// Couldn't start CURL for some reason. The actual 
		// number isn't important. Logged in the handler
		return 2; 
	}

	// This call is to make sure that we can connect and perform
	// ftp calls on the server, and also orient the user.
	res = handler.FtpList(""); 

	if (res != CURLE_OK) {
		// Something went wrong with the curl call. It'll be
		// logged by deeper functions
		return res; 
	}

	// Here we enter the program proper.
	InputLoop(&log, &handler);
}
