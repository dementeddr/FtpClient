#include <stdio.h>
#include "logger.h"

void Logger() {
	logfile = fopen("logfile.txt", "a");
}

void ~Logger() {
	logfile.close();
}

void Logger::Out(const char* text) {
	// Get time at some point
	char *time = "--:--:--";

	fprintf(logfile, "[%s]: %s\n", time, text);
}

