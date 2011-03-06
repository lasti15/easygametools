#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif


#include <winsock2.h>

namespace ROG {

	class Semaphore	{
		CRITICAL_SECTION semaphore;

	public:
		Semaphore()	{
			InitializeCriticalSection(&semaphore);
		}

		~Semaphore()	{
			DeleteCriticalSection(&semaphore);
		}

		inline void lock()	{
			if (ready()) {
				EnterCriticalSection(&semaphore);
			}
		}

		inline void unlock()	{
			if (ready()) {
				LeaveCriticalSection(&semaphore);
			}
		}

		inline int test()	{
			if (ready()) {
				return TryEnterCriticalSection(&semaphore);
			}
			return 0;
		}

		inline bool ready() {
			return semaphore.DebugInfo != NULL;
		}
	};

}


#endif