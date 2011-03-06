#ifndef __THREADEVENTLISTENER_H
#define __THREADEVENTLISTENER_H


#include "ThreadEvent.h"

namespace ROG {

	class ThreadEventListener : public Reference {
	public:
		virtual void threadEvent(Object<ThreadEvent> evt) = 0;
	};

}


#endif


