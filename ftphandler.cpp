#include "ftphandler.h"

/*
 * The default constructor for the FtpHandler class.
 * This constructor is not actually supported.
 */
FtpHandler::FtpHandler() :
	hostname("localhost"),
	username("anonymous"),
	password(""),
	log(NULL) {
	
	pathPieces.push_back("~");
}

/*
 * The constructor you should be using.
 */
FtpHandler::FtpHandler(std::string hostname, std::string username, std::string password, Logger *log) : 
	hostname(hostname),
	username(username),
	password(password),
	log(log) {
	
	pathPieces.push_back("~");
}


/*
 * Initializes curl and sets the common curl parameters for the
 * program.
 * Ideally this would just be in the constructor, but you need to
 * check the curl_easy_init return and kill the program if it
 * failed.
 */
bool FtpHandler::Initialize() {

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	// I've never actually seen this happen, but you never know.
	if (curl == NULL) {
		log->Log(HANDLER, "ERROR: CURL could not initialize");
		return false;
	}

	log->Log(HANDLER, "CURL initialized");

	// These lines tell curl to record it's --verbose output in
	// the loggers logfile. It's the only way to log problems with
	// curl itself.
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_STDERR, log->logfile);

	// Setting the user credentials for the session
	curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str()); 
	
	return true;
}


/*
 * Destructor for the handler. Not much to do here, since pretty
 * much everything is stack allocated. Curl needs some cleanup
 * though.
 */
FtpHandler::~FtpHandler() {
	curl_global_cleanup();
}


/*
 * Callback function used by curl when we are downloading a file
 * from the server. Takes the file data and writes it to a file
 * named by the user.
 */
static size_t retrieveCallback(void *buffer, size_t size, size_t nmemb, void *stream) {

	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Log(HANDLER, "Received callback for Retrieve call.");

	if (output && !output->stream) {
		output->stream = fopen(output->filename.c_str(), "wb");
		
		if (!output->stream) {
			output->log->Print(HANDLER, "Could not open file " + output->filename + " for writing.");
			return -1;
		}

		output->log->Print(HANDLER, "Wrting " + std::to_string(nmemb) + " bytes to file " + output->filename);
		return fwrite(buffer, size, nmemb, output->stream);
	}
}


/*
 * Callback function used by curl when we are just listing the
 * contents of a directory on the server. Takes the data and
 * writes it straight to the console. Ideally, it would also
 * format the directory listing to make it less verbose, but I
 * don't really care right now
 */
static size_t listCallback(void *buffer, size_t size, size_t nmemb, void *stream) {
	
	struct FtpFile *output = (struct FtpFile *)stream;

	output->log->Log(HANDLER, "Received callback for Listing call");

	if (output && !output->stream) {
		return fwrite(buffer, size, nmemb, stdout);
	}
}


/*
 * Assembles the list of path components into the relative path of
 * the "working directory" we should be in on the server.
 */
std::string FtpHandler::GetPath() {

	std::string path = "";

	for(std::string &part : pathPieces) {
		path.append(part);

		// Make sure we have path seperators
		if (part.back() != '/') {
			path.push_back('/');
		}
	}

	return path;
}


/*
 * Returns the base URL for the ftp session
 */
std::string FtpHandler::GetUrl() {
	return "ftp://" + hostname + "/";
}


/*
 * Sets up the ftp command to list the contents of a directory on
 * the server, calls the executor, and cleans up afterward.
 */
CURLcode FtpHandler::FtpList(std::string input) {
	
	// Used to pass data to and from the callback
	struct FtpFile ftpfile = {
		"unused", // Normally the output filename
		NULL,
		log
	};

	// Assembling the command the server will execute
	std::string request = "LIST " + GetPath() + input;

	curl_easy_setopt(curl, CURLOPT_URL, GetUrl().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, listCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());

	log->Log(HANDLER, "Sending command \"" + request + "\"");

	// Execute the ftp call
	CURLcode res = ExecuteFtp();
	printf("\n");

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

	return res;
}


/*
 * Changes the "working directory" that we are tracking on
 * the server, calls the list command on the new directory, 
 * and cleans up afterward.
 *
 * FTP actually has a "Change Working Directory" command, but for
 * the life of me I can't get it to do anything useful, so I
 * tracked the directory myself. Note that the current cd behavior
 * is *very* primitive.
 */
CURLcode FtpHandler::FtpChangeDir(std::string input) {

	std::vector<std::string> backupPath = pathPieces;

	// Go up a directory
	if (input == "..") {
		if (pathPieces.size() > 1) {
			pathPieces.pop_back();
		} else {
			// This is to prevent weird pathing issues that
			// happen in this implementation if you try to make
			// a relative path with ".." in it.
			log->Print(HANDLER, "Already at home directory");
			return CURLE_OK;
		}

	// Equivalent to "cd " on a normal shell. Brings you back to
	// the home dir.
	} else if (input == "") {
		while (pathPieces.size() > 1) {
			pathPieces.pop_back();
		}

	} else if (input == ".") {
		// Do nothing.

	// Making the assumption that the user is only inputting one
	// directory change at a time, adds the new element to the
	// working directory path. Very primitive.
	} else {
		pathPieces.push_back(input);
	}

	log->Print(HANDLER, "Changing working directory to " + GetPath());

	// List the files in the new directory
	CURLcode res = FtpList("");

	// If something went wrong, don't change directories.
	if (res != CURLE_OK) {
		log->Print(HANDLER, "Attempt to change directory failed with error: " + std::to_string(res));
		pathPieces = backupPath;
	}

	return res;
}


/*
 * Sets up the ftp command to download a file residing on
 * the server, calls the executor, and cleans up afterward.
 */
CURLcode FtpHandler::FtpRetrieve(std::string target, std::string destination) {
	
	// Used to pass data to and from the callback function
	struct FtpFile ftpfile = {
		destination,
		NULL,
		log
	};

	// Set up the actual command the server will execute
	std::string request = "RETR " + GetPath() + target;

	curl_easy_setopt(curl, CURLOPT_URL, GetUrl().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, retrieveCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());

	log->Log(HANDLER, "Sending command \"" + request + "\"");

	// Execute the call. With extreme prejudice.
	CURLcode res = ExecuteFtp();

	printf("\n");

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

	return res;
}


/*
 * Does the actual execution of the ftp calls.
 */
CURLcode FtpHandler::ExecuteFtp() {

	log->Log(HANDLER, "Attempting FTP call");

	if (curl) {

		// This is where curl actually excutes the call
		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			log->Print(HANDLER, "ERROR: FTP call failed with curl error: " + std::to_string((unsigned int) res)); 
		} else {
			log->Log(HANDLER, "FTP call succeeded");
		}
		
	} else {
		log->Print(HANDLER, "ERROR: Curl was never initialized");
		return CURLE_FAILED_INIT;
	}

	return res;
}

