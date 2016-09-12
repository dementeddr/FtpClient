#include "logger.h"

/*
 * Default Constructor
 */
Logger::Logger() {
	CreateLogger("logfile.txt");
}

/*
 * Optional constructor, if you want to define your own logfile.
 */
Logger::Logger(const std::string filename) {
	CreateLogger(filename);
}


/*
 * The secret initializer wrapped by the constructors, since other
 * than one string they are both identical.
 */
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

/*
 * Destructor for the Logger. Just need to close a file here.
 */
Logger::~Logger() {
	fclose(logfile);
}


/*
 * Logs to the logfile *and* prints to the console.
 */
void Logger::Print(Origin from, const std::string text) {
	Output(from, text, true);
}


/*
 * Logs to the logfile but does not print to the console.
 */
void Logger::Log(Origin from, const std::string text) {
	Output(from, text, false);
}


/*
 * Does the actual printing and logging. I did the function calls
 * like this to minimize the amount of stuff you have to type to
 * use the logging functions.
 */
void Logger::Output(Origin from, const std::string text, bool print) {

	time_t now = time(0);
	std::string prefix;

	// Ideally I could use some form of reflection to automatically
	// get more detailed information about the calling method.
	// This will have to do for now
	switch (from) {
		case MAIN:	  prefix = "main.cpp";	break;
		case LOGGER:  prefix = "logger.cpp";	break;
		case HANDLER: prefix = "ftphandler.cpp";	break;
		default:	  prefix = "UNKNOWN";	break;
	}

	// Fun fact, ctime() adds a newline character at the end which
	// kind of breaks how the log should look.
	std::string stime = ctime(&now);
	stime.pop_back();

	fprintf(logfile, "[%s] %s: %s\n", stime.c_str(), prefix.c_str(), text.c_str());
	
	if (print)
		fprintf(stdout, "%s\n", text.c_str());

}
