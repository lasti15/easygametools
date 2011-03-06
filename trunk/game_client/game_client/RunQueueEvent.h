#ifndef __RUNQUEUEEVENT_H
#define __RUNQUEUEEVENT_H


#include "Reference.h"

namespace ROG {

	typedef enum {EVT_SUBMITTED, EVT_QUEUED, EVT_COMPLETED, EVT_REJECTED} RunQueueEventType;

	class RunQueueEvent : public Reference {
	private:
		RunQueueEventType eventType;
		Object<Runnable> runQueue;
		Object<Thread> thread;

	public:
		RunQueueEvent(RunQueueEventType eventType, Object<Runnable> runQueue, Object<Thread> thread) {
			this->eventType = eventType;
			this->runQueue = runQueue;
			this->thread = thread;
		}

		RunQueueEventType getEventType() {
			return eventType;
		}

		Object<Runnable> getRunQueue() {
			return runQueue;
		}

		Object<Thread> getThread() {
			return thread;
		}
	};

}

#endif

