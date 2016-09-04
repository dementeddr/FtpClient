
public:
	void Logger();
	void ~Logger();
	
	void Out(const char* text);

private:
	FILE logfile;
