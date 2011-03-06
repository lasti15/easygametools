#ifndef __CLIENTFUTURE_H
#define __CLIENTFUTURE_H


#include "Reference.h"

namespace ROG {

	typedef enum {FUTURE_OK, FUTURE_ERROR} FutureCode;

	class ClientFuture : public Reference, public Semaphore {
	private:
		bool complete;
		FutureCode code;

	public:

		ClientFuture() {
			complete = false;
			code = FUTURE_OK;
		}

		void setComplete(bool complete) {
			this->complete = complete;
		}

		bool isComplete() {
			return complete;
		}

		void setCode(const FutureCode code) {
			this->code = code;
		}

		FutureCode getCode() {
			return code;
		}

		FutureCode waitFor() {
			lock();
			while(!complete) {
				Sleep(10);
				unlock();
			}
			unlock();
			return code;
		}
	};

};



#endif

