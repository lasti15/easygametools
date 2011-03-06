#ifndef __LOGINFAILEDEXCEPTION_H
#define __LOGINFAILEDEXCEPTION_H

#include "FatalException.h"

namespace ROG {

	class LoginFailedException : public FatalException {
	public:
		LoginFailedException() {}
		LoginFailedException(int code) : FatalException(code) {}
		LoginFailedException(char* msg) : FatalException(msg) {}
		LoginFailedException(int code, char* msg) : FatalException(code, msg) {}
	};

}

#endif

