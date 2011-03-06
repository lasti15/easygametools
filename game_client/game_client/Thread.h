#ifndef __THREAD_H
#define __THREAD_H


#include <process.h>
#include "Reference.h"
#include "Logger.h"
#include "ThreadEventListener.h"
#include "Runnable.h"


namespace ROG {

	#define AVL_CHUNK_SIZE_THR 32

	class Thread : public Reference	{
	private:

		HANDLE _thread;
		unsigned int _thrid;

		static AVLTreeT<unsigned long, Object<Thread>, AVL_CHUNK_SIZE_THR> runningThreads;
		static Semaphore runningSem;
		static unsigned long engine_threadid_master;

		static Semaphore eventListenerSem;
		static OCArray<Object<ThreadEventListener>> threadEventListeners;
		static void fireThreadEvent(Object<ThreadEvent> evt);

		const static Logger* logger;

		unsigned long engine_threadid;

		int signalFlag;

		Object<Runnable> runnable;
	public:
		Thread();
		Thread(Object<Runnable> runnable);
		~Thread();

		void start();

		void run();

		static void CollectRemainingThreads();

		static Object<Thread> getCurrentThread();
		static void addThreadEventListener(Object<ThreadEventListener> listener);
		static void removeThreadEventListener(Object<ThreadEventListener> listener);

		void cleanup();

		void signal(int signalcode);
		inline int getSignalled();

		inline Object<Runnable> getRunnable();

		void waitfor();
	};


	inline int Thread::getSignalled()	{
		return signalFlag;
	}

	inline Object<Runnable> Thread::getRunnable() {
		return runnable;
	}

}

#endif
