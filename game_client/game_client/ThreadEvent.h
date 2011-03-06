#ifndef __THREADEVENT_H
#define __THREADEVENT_H

#include "Reference.h"



namespace ROG {

	typedef enum {THR_STARTED, THR_STOPPED, THR_DESTROYED, THR_EXCEPTION} ThreadEventType;


	class ThreadEvent : public Reference {
	private:
		ThreadEventType eventType;

	public:
		ThreadEvent(ThreadEventType eventType) {
			this->eventType = eventType;
		}

		ThreadEventType getEventType() {
			return eventType;
		}
	};


}


#endif

