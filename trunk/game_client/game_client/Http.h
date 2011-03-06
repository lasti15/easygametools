#ifndef __HTTP_H
#define __HTTP_H


#include <cstring>
#include <cctype>

namespace ROG {

	typedef enum {HTTP_OK = 200, HTTP_ERR = 500} HttpStatus;

	class Http {

	public:
		const static int getStatus(char* response);
	
		const static char* getData(char* response);
	};

}

#endif
