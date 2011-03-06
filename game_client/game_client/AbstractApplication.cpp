

#include "AbstractApplication.h"


using namespace ROG;

const Logger* AbstractApplication::log = Logger::getLogger("AbstractApplication");


AbstractApplication::AbstractApplication(const char* appName) {
	strncpy(this->appName, appName, 254);
	Thread::addThreadEventListener(this);
}


AbstractApplication::~AbstractApplication() {
	Thread::removeThreadEventListener(this);
}

//TODO: do something useful with this
void AbstractApplication::threadEvent(Object<ThreadEvent> evt) {
	switch (evt->getEventType()) {
	case THR_STARTED:
		log->info("Thread Started");
		break;
	case THR_STOPPED:
		log->info("Thread Stopped");
		break;
	case THR_DESTROYED:
		log->info("Thread Destroyed");
		break;
	case THR_EXCEPTION:
		log->info("Thread Exception");
		break;
	}
}




const void AbstractApplication::runGame() {
	run();
}








