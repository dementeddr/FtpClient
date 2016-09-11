#include "logger.h"

Logger::Logger() {
	CreateLogger("logfile.txt");
}

Logger::Logger(const std::string filename) {
	CreateLogger(filename);
}

void Logger::CreateLogger(const std::string filename) {

	// Going for the efficient if somewhat less memory-safe option.
	// The safer option would be to open and close it on each
	// logging. We'll see.
	logfile = fopen(filename.c_str(), "a");

	time_t now = time(0);

	// This doesn't have the same prefix as everthing else because
	// it should stand out as much as possible from the rest of
	// the log.
	fprintf(logfile, "\n  |---- LOG FILE OPENED ----|\n");
	fprintf(logfile, "Log opened at %s\n", ctime(&now));
}

Logger::~Logger() {
	fclose(logfile);
}

void Logger::Out(Origin from, const std::string text) {
	// Get time at some point
	time_t now = time(0);
	std::string prefix;

	switch (from) {
		case MAIN:	  prefix = "main.cpp";	break;
		case LOGGER:  prefix = "logger.cpp";	break;
		case HANDLER: prefix = "ftphandler.cpp";	break;
		default:	  prefix = "UNKNOWN";	break;
	}

	// Logging to a file? Logging to stdout? WHY NOT BOTH!?
	fprintf(logfile, "[%s] %s: %s\n", ctime(&now), prefix.c_str(), text.c_str());
	fprintf(stdout, "%s\n", text.c_str());
}

