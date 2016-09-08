class Logger {

	public:
		Logger();
		Logger(char *filename);
		~Logger();

		void Out(const char *text);

	private:
		FILE *logfile;

		void CreateLogger(char *filename);
};
