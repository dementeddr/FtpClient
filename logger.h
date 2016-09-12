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
		// Generally this should be private and only used
		// internally by the logger. Making it public means I can
		// get the CURL verbose stream logged. It's not ideal,
		// but it works for the scope of the project.
		FILE *logfile;
		
		Logger();
		Logger(const std::string filename);
		~Logger();

		void Out(Origin from, const std::string text);

	private:

		void CreateLogger(const std::string filename);
};

#endif
