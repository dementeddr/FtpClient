#include <stdio.h>
#include <ctime>
#include <string>

#ifndef LOGGER_H
#define LOGGER_H

// This could ideally be replaced by reflection, but I'm not
// going to get into that.
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

		void Log(Origin from, const std::string text);
		void Print(Origin from, const std::string text);

	private:
		
		void CreateLogger(const std::string filename);
		void Output(Origin from, const std::string text, bool print);
};

#endif
