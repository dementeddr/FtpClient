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

	time_t time;

	fprintf(logfile, "\n---- LOG FILE OPENED ----\n");
	fprintf(logfile, "Log opened at %d", time);
}

Logger::~Logger() {
	fclose(logfile);
}

void Logger::Out(const char* text) {
	// Get time at some point
	char *time = "--:--:--";

	fprintf(logfile, "[%s]: %s\n", time, text);
}

