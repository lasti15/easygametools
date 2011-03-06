#ifndef __PROTOCOLEXCEPTION_H
#define __PROTOCOLEXCEPTION_H


#include "FatalException.h"

namespace ROG {

	class ProtocolException : public FatalException {
	public:
		ProtocolException() {}
		ProtocolException(int code) : FatalException(code) {}
		ProtocolException(char* msg) : FatalException(msg) {}
		ProtocolException(int code, char* msg) : FatalException(code, msg) {}
	};

}



#endif


