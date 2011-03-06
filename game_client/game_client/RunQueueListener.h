#ifndef __RUNQUEUELISTENER_H
#define __RUNQUEUELISTENER_H

#include "Reference.h"
#include "RunQueueEvent.h"

namespace ROG {

	class RunQueueListener : public Reference {
	public:
		virtual void runQueueEvent(Object<RunQueueEvent> evt) = 0;
	};

}


#endif

