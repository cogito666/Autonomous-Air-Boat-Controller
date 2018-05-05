// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class IfaceLogger
{
public:
	virtual ~IfaceLogger(){

	}
	virtual void write(const char* msg) = 0;
	virtual void flush() = 0;
};

class LoggerManager{
	enum{
		MAX_NUM_LOGGERS = 5
	};
public:
	enum LogLevels{
		LOG_VERBOSE = 0,
		LOG_DEBUG,
		LOG_LOGGING,
		LOG_INFO,
		LOG_ERROR
	};

	LoggerManager():n(0){
		reset();
		setLogLevel(LOG_INFO);
	}

	~LoggerManager(){
		reset();
	}

	bool setLogLevel(const LogLevels& level){
		log_level = level;
		return true;
	}
	
	bool setLogLevel(const String& level) {
		if (level.compareTo("INFO") == 0) {
			setLogLevel(LOG_INFO);
		}
		else if (level.compareTo("LOGGING") == 0) {
			setLogLevel(LOG_LOGGING);
		}
		else if (level.compareTo("DEBUG") == 0) {
			setLogLevel(LOG_DEBUG);
		}
		else if (level.compareTo("VERBOSE") == 0) {
			setLogLevel(LOG_VERBOSE);
		}
		else if (level.compareTo("ERROR") == 0) {
			setLogLevel(LOG_ERROR);
		}
		else {
			//do nothing.
			return false;
		}
		return true;
	}

	void log(const char* msg, const LogLevels& level){
		if (level >= log_level){
			for (int i = 0; i < n; i++){
				loggers[i]->write(msg);
				if (level >= LOG_ERROR)
					loggers[i]->flush();
			}
		}
	}

	void logln(const char* msg, const LogLevels& level){
		if (level >= log_level) {
			for (int i = 0; i < n; i++) {
				loggers[i]->write(msg);
				loggers[i]->write("\n\r");
				loggers[i]->flush();
			}
			delay(7);
		}
	}

	void reset(){
		for (int i = 0; i < n; i++){
			loggers[i] = NULL;
		}
		n = 0;
	}

	int registerLogger(IfaceLogger* logger){
		if (n == MAX_NUM_LOGGERS){
			return -1;
		}
		loggers[n++] = logger;
		return n;
	}

	void unregisterLogger(int id){
		if (id <= n){
			for (int i = id-1; i < n-1; i++){
				loggers[i] = loggers[i + 1];
			}
			loggers[n-1] = NULL;
		}
	}

	void flush(){
		for (int i = 0; i < n; i++) {
			loggers[i]->flush();
		}
	}
protected:
	IfaceLogger* loggers[MAX_NUM_LOGGERS];
	int n;
	LogLevels log_level;
};

extern LoggerManager loggers;


#endif

