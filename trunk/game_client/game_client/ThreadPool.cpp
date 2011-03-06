
#include "ThreadPool.h"
#include "RunQueue.h"


using namespace ROG;


const Logger* ThreadPool::logger = Logger::getLogger("ThreadPool");


ThreadPool::ThreadPool(int minSize, int maxSize) : started(false) {
	logger->debug("Creating Thread Pool");
	this->minSize = minSize;
	this->currentSize = minSize;
	this->maxSize = maxSize;

	logger->debug("Initializing threads");
	threadPoolSem.lock();
	for (int i=0; i<minSize; i++) {
		Object<RunQueue> runQueue = new RunQueue();
		runQueue->setQueueListener(this);
		//gross casting...
		Object<Thread> threadRunner = new Thread(&(*runQueue));
		threadPool.append(threadRunner);
	}
	threadPoolSem.unlock();
}


ThreadPool::~ThreadPool() {
	taskQueue.clear();
	threadPool.clear();
}


void ThreadPool::start() {
	logger->debug("Starting initial threads");
	threadPoolSem.lock();
	for (unsigned int i=0; i<threadPool.length(); i++) {
		Object<Thread> t = threadPool.removeAt(0);
		t->start();
		threadPool.append(t);
	}

	started = true;
	threadPoolSem.unlock();
}


void ThreadPool::execute(Object<Runnable> task) {
	if (!started) {
		throw new FatalException("Thread pool is not started yet.");
	}

	logger->debug("Executing task");

	if (this->isclosing) {
		return;
	}

	//dont submit tasks if the system is shutting down
	threadPoolSem.lock();

	if (threadPool.length() <= 0) {
		if (currentSize < maxSize) {
			logger->debug("Adding new thread to pool");

			//put a new thread into the pool
			Object<Runnable> runQueue = new RunQueue();
			Object<Thread> threadRunner = new Thread(runQueue);
			threadPool.append(threadRunner);

			threadRunner->start();
		} else {
			logger->debug("Queuing task");

			//queue this task
			taskQueueSem.lock();
			taskQueue.append(task);
			taskQueueSem.unlock();
			threadPoolSem.unlock();
			return;
		}
	}

	logger->debug("Getting thread to perform task");
	//get a thread to execute this task in
	Object<Thread> thread = threadPool.removeAt(0);
	if (thread == NULL) {
		throw new FatalException("Weird... no threads available");
	}
	//nasty casting... i wonder how i could improve that
	Object<RunQueue> queue = (RunQueue*)(&(*(thread->getRunnable())));
	queue->offer(task);

	threadPoolSem.unlock();
}


void ThreadPool::runQueueEvent(Object<RunQueueEvent> evt) {
	logger->info("run queue event");
	taskQueueSem.lock();
	
	Object<RunQueue> queue = (RunQueue*)(&(*(evt->getRunQueue())));

	if (taskQueue.length() > 0) {
		Object<Runnable> task = taskQueue.removeAt(0);

		//unlock the semaphore first in case the task we offer to the queue executes really quickly
		taskQueueSem.unlock();

		queue->offer(task);
	} else {
		taskQueueSem.unlock();

		threadPoolSem.lock();

		//return the thread to the pool
		threadPool.append(evt->getThread());

		threadPoolSem.unlock();
	}
}

