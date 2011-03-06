#ifndef __DATETIME_H
#define __DATETIME_H

#include "Reference.h"
#include "Semaphore.h"
#include <time.h>

namespace ROG {

#define CLOCK_ACCURACY	1000 //1 millisecond clock accuracy



	class DateTime : public Reference	{
	private:
		//date and time
		time_t currentDate;

		Semaphore updateSem;

	#ifdef _WIN32
		LARGE_INTEGER tick_accuracy;
		LARGE_INTEGER tick_count;

		LARGE_INTEGER timer;
	
	#else
		unsigned long tick_accuracy;
		unsigned long tick_count;

		unsigned long timer;

	#endif

	public:
		unsigned long system_time;

		DateTime();

		int TimeInit();

		//int DateStr(char* buf, int maxlen);

		inline unsigned long Update();

	};


	inline unsigned long DateTime::Update()	{
		if (!updateSem.test())	{
			updateSem.lock();
			updateSem.unlock();
			return system_time;
		}	else	{
			#ifdef _WIN32
				QueryPerformanceCounter(&tick_count);

				system_time = (unsigned long)(tick_count.QuadPart / (tick_accuracy.QuadPart / CLOCK_ACCURACY));
			#else
				system_time = time(NULL);
			#endif
			updateSem.unlock();
			return system_time;
		}
	}


}

#endif


