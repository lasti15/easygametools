
#include "Logger.h"
#include "RollingFileAppender.h"

#include <time.h>


using namespace ROG;

OCArray<Logger*>* Logger::allLoggers = NULL;
Semaphore Logger::allLoggersSemaphore;
LogCreatorListener* Logger::logCreationListener = NULL;

static char* getDateForLog() {
	char* timeStr = new char[25];
	time_t system_time = time(NULL);
	
	ltoa((long)system_time, timeStr, 10);

	return timeStr;
}





const OCArray<Logger*>* Logger::getAllLoggers() {
	return allLoggers;
}



Logger::~Logger() {
	while (appenders.length() > 0) {
		Appender* appender = appenders.removeAt(0);
		delete appender;
	}
}


Logger::Logger(const char* name, LogLevel level) {
	this->name = name;
	this->currentLevel = level;
}


const Logger* Logger::getLogger(const char* name) {
	return getLogger(name, LVL_INFO);
}

const Logger* Logger::getLogger(const char* name, LogLevel level) {
	allLoggersSemaphore.lock();
	if (allLoggers == NULL) {
		allLoggers = new OCArray<Logger*>();
	}
	allLoggersSemaphore.unlock();

	//see if we have a log level override
	if (logCreationListener != NULL) {
		level = logCreationListener->getDefaultLogLevel(name);
	}
	Logger* newLogger = new Logger(name, level);

	RollingFileAppender* fileAppender = new RollingFileAppender();

	newLogger->addAppender(fileAppender);

	allLoggersSemaphore.lock();
	allLoggers->append(newLogger);
	allLoggersSemaphore.unlock();

	return newLogger;
}

void Logger::addAppender(Appender* appender) {
	appender->init(name);
	appenderSem.lock();
	appenders.append(appender);
	appenderSem.unlock();
}

const void Logger::log(const char* message, LogLevel level) const {

	//optimize: only prepare the message if there are some appenders
	if (appenders.length() > 0) {
		char msgbuf[1024];

		char* dateStr = getDateForLog();

		_snprintf(msgbuf, 1024, "%s : %s : %s : %s", dateStr, getLogLevelStr(level), name, message);

		delete [] dateStr;

		//cast away from const..
		((Logger*)this)->appenderSem.lock();
		for (unsigned int i=0; i<appenders.length(); i++) {
			Appender* appender  = appenders.at(i);
			appender->append(msgbuf);
		}
		((Logger*)this)->appenderSem.unlock();
	}
}



void Logger::CollectRemainingLoggers() {
	allLoggersSemaphore.lock();
	if (allLoggers != NULL) {

		while (allLoggers->length() > 0) {
			Logger* logger = allLoggers->removeAt(0);
			delete logger;
		}

		delete allLoggers;
	}
	allLoggersSemaphore.unlock();

	RollingFileAppender::cleanup();
}






