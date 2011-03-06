#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include "Thread.h"
#include "Semaphore.h"
#include "RunQueueListener.h"

namespace ROG {

	
	class ThreadPool : public RunQueueListener {
	private:
		const static Logger* logger;

		Semaphore taskQueueSem;
		OCArray<Object<Runnable>> taskQueue;

		Semaphore threadPoolSem;
		OCArray<Object<Thread>> threadPool;

		int minSize;
		int currentSize;
		int maxSize;

		bool started;

	public:

		ThreadPool(int minSize, int maxSize);
		~ThreadPool();

		void start();

		void execute(Object<Runnable> task);

		void runQueueEvent(Object<RunQueueEvent> evt);
	};




}




#endif

