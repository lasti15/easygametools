#ifndef __FATALEXCEPTION_H
#define __FATALEXCEPTION_H

namespace ROG {

	class FatalException {

	private:
		int code;
		char* msg;

	public:
		FatalException() {
			code = 0;
			msg = 0;
		}

		FatalException(int code) {
			this->code = code;
			msg = 0;
		}

		FatalException(char* msg) {
			code = 0;
			this->msg = msg;
		}

		FatalException(int code, char* msg) {
			this->code = code;
			this->msg = msg;
		}

		int getCode() {
			return code;
		}

		char* getMsg() {
			return msg;
		}

	};

}



#endif

