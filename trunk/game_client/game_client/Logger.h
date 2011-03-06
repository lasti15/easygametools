#ifndef __LOGGER_H
#define __LOGGER_H

#include <list>
#include <memory>
#include <string>

#include "FatalException.h"
#include "Semaphore.h"
#include "Appender.h"

#pragma warning(disable : 4812)
#pragma warning(disable : 4244)
#pragma warning(disable : 4554)
#pragma warning(disable : 4018)
#pragma warning(disable : 4800)

#define OC_NEW_STYLE_INCLUDES
#define OC_BYTES_IN_POINTER 4
#include "opencontainers/opencontainers.h"

#pragma warning(default : 4812)
#pragma warning(default : 4244)
#pragma warning(default : 4554)
#pragma warning(default : 4018)
#pragma warning(default : 4800)

namespace ROG {

	static char CURRENT_DIRECTORY[1024];


	typedef enum {LVL_TRACE, LVL_DEBUG, LVL_INFO, LVL_WARN, LVL_ERROR, LVL_FATAL, LVL_OFF} LogLevel;

	static char* getLogLevelStr(LogLevel level) {
		switch (level) {
		case LVL_TRACE:
			return "TRACE";
		case LVL_DEBUG:
			return "DEBUG";
		case LVL_INFO:
			return "INFO";
		case LVL_WARN:
			return "WARN";
		case LVL_ERROR:
			return "ERROR";
		case LVL_FATAL:
			return "FATAL";
		default:
			return "UNKNOWN";
		}
	}
	static LogLevel getLogLevel(const char* level) {
		if (strncmp("TRACE", level, 5) == 0) {
			return LVL_TRACE;
		}
		if (strncmp("DEBUG", level, 5) == 0) {
			return LVL_DEBUG;
		}
		if (strncmp("INFO", level, 4) == 0) {
			return LVL_INFO;
		}
		if (strncmp("WARN", level, 4) == 0) {
			return LVL_WARN;
		}
		if (strncmp("ERROR", level, 5) == 0) {
			return LVL_ERROR;
		}
		if (strncmp("FATAL", level, 5) == 0) {
			return LVL_FATAL;
		}
		return LVL_INFO;
	}


	class LogCreatorListener {
	public:
		virtual LogLevel getDefaultLogLevel(const char* newLogger) = 0;
	};


	//logger class like log4j
	class Logger {
	private:
		Logger(const char* name, LogLevel level);

		const char* name;
		LogLevel currentLevel;

		Semaphore appenderSem;
		OCArray<Appender*> appenders;

		static OCArray<Logger*>* allLoggers;
		static Semaphore allLoggersSemaphore;

		static LogCreatorListener* logCreationListener;

	protected:

		void addAppender(Appender* appender);
		const void log(const char* message, LogLevel level) const;

	public:

		static void setLogCreationListener(LogCreatorListener* listener) {
			logCreationListener = listener;
		}

		const static OCArray<Logger*>* getAllLoggers();

		const static Logger* getLogger(const char* name);
		const static Logger* getLogger(const char* name, LogLevel level);

		//TODO: give a copy of the name so its immutable
		const char* getName() const {
			return this->name;
		}

		const LogLevel getLevel() const {
			return this->currentLevel;
		}

		const void setLogLevel(LogLevel level) {
			this->currentLevel = level;
		}

		~Logger();

		inline void trace(const char* msg) const {
			if (isTraceEnabled()) {
				log(msg, LVL_TRACE);
			}
		}

		inline bool isTraceEnabled() const {
			return this->currentLevel <= LVL_TRACE;
		}

		inline void debug(const char* msg) const {
			if (isDebugEnabled()) {
				log(msg, LVL_DEBUG);
			}
		}

		inline bool isDebugEnabled() const {
			return this->currentLevel <= LVL_DEBUG;
		}

		inline void info(const char* msg) const {
			if (isInfoEnabled()) {
				log(msg, LVL_INFO);
			}
		}

		inline bool isInfoEnabled() const {
			return this->currentLevel <= LVL_INFO;
		}

		inline void warn(const char* msg) const {
			if (isWarnEnabled()) {
				log(msg, LVL_WARN);
			}
		}

		inline bool isWarnEnabled() const {
			return this->currentLevel <= LVL_WARN;
		}

		inline void error(const char* msg) const {
			if (isErrorEnabled()) {
				log(msg, LVL_ERROR);
			}
		}

		inline bool isErrorEnabled() const {
			return this->currentLevel <= LVL_ERROR;
		}

		inline void fatal(const char* msg) const {
			if (isFatalEnabled()) {
				log(msg, LVL_FATAL);
			}
		}

		inline bool isFatalEnabled() const {
			return this->currentLevel <= LVL_FATAL;
		}
		

		static void CollectRemainingLoggers();
	};

}

#endif

