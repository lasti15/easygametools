#ifndef __TASKREJECTEDEXCEPTION_H
#define __TASKREJECTEDEXCEPTION_H

#include "FatalException.h"

namespace ROG {

	class TaskRejectedException : public FatalException {
	public:
		TaskRejectedException() {}
		TaskRejectedException(int code) : FatalException(code) {}
		TaskRejectedException(char* msg) : FatalException(msg) {}
		TaskRejectedException(int code, char* msg) : FatalException(code, msg) {}
	};

}

#endif

