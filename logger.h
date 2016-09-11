#include <stdio.h>
#include <ctime>
#include <string>

#ifndef LOGGER_H
#define LOGGER_H

enum Origin {
	MAIN,
	LOGGER,
	HANDLER
};

class Logger {
	public:
		Logger();
		Logger(const std::string filename);
		~Logger();

		void Out(Origin from, const std::string text);

	private:
		FILE *logfile;

		void CreateLogger(const std::string filename);
};

#endif
