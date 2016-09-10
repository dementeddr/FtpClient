#include <stdio.h>
#include <ctime>
#include "logger.h"

Logger::Logger() {
	CreateLogger("logfile.txt");
}

Logger::Logger(char *filename) {
	CreateLogger(filename);
}

void Logger::CreateLogger(char *filename) {

	// Going for the efficient if somewhat less memory-safe option.
	// The safer option would be to open and close it on each
	// logging. We'll see.
	logfile = fopen(filename, "a");

	time_t now = time(0);
	//time(&timev);

	fprintf(logfile, "\n---- LOG FILE OPENED ----\n");
	fprintf(logfile, "Log opened at %d\n", ctime(&now));
}

Logger::~Logger() {
	fclose(logfile);
}

void Logger::Out(const char* text) {
	// Get time at some point
	time_t now = time(0);

	// Logging to a file? Logging to stdout? WHY NOT BOTH!?
	fprintf(logfile, "[%d]: %s\n", ctime(&now), text);
	fprintf(stdout, "%s\n", ctime(&now), text);
}

