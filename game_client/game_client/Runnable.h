#ifndef __RUNNABLE_H
#define __RUNNABLE_H


#include "Reference.h"
#include "Logger.h"

namespace ROG {

	class Runnable : public Reference {
	private:

	protected:
		int signalFlag;

		Runnable() : signalFlag(0) {}

	public:
		virtual void run() = 0;

		inline virtual void signal(int signalcode);
		inline int getSignalled();

	};

	inline int Runnable::getSignalled()	{
		return signalFlag;
	}

	inline void Runnable::signal(int signalcode)	{
		signalFlag = signalcode;
	}

}



#endif

