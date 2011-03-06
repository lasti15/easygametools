#ifndef __ABSTRACTAPPLICATION_H
#define __ABSTRACTAPPLICATION_H


#include "Logger.h"
#include "Config.h"
#include "Reference.h"
#include "Thread.h"
#include "ThreadEventListener.h"

namespace ROG {

	class AbstractApplication : public ThreadEventListener {

	private:
		const static Logger* log;

		char appName[255];

	protected:
		AbstractApplication(const char* appName = "Unknown");

		virtual const void run() = 0;

	public:

		const void runGame();

		virtual ~AbstractApplication();

		void threadEvent(Object<ThreadEvent> evt);

	};

}


#endif

