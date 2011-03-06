#ifndef __RUNQUEUE_H
#define __RUNQUEUE_H


#include "Runnable.h"
#include "TaskRejectedException.h"
#include "RunQueueListener.h"

namespace ROG {

	class RunQueue : public Runnable {
	private:
		Semaphore submittedSem;
		Object<Runnable> submittedTask;

		//the deliverylock lets us sleep on a critical section when waiting for a  new task to be submitted
		Semaphore deliveryLock;

		Object<RunQueueListener> queueListener;

		const static Logger* logger;

	public:

		RunQueue() {
			//lock on by default
			deliveryLock.lock();
		}

		void run() {
			while (!getSignalled()) {
				//lock the delivery lock until we have a task submitted
				deliveryLock.lock();
				submittedSem.lock();
				if (submittedTask != NULL) {
					if (queueListener != NULL) {
						queueListener->runQueueEvent(new RunQueueEvent(EVT_SUBMITTED, this, Thread::getCurrentThread()));
					}
					try {
						submittedTask->run();
					} catch (FatalException* ex) {
						logger->error(ex->getMsg());
						delete ex;
					}
					submittedTask = NULL;
					if (queueListener != NULL) {
						queueListener->runQueueEvent(new RunQueueEvent(EVT_COMPLETED, this, Thread::getCurrentThread()));
					}
				}
				submittedSem.unlock();
				deliveryLock.lock();
				Sleep(100);
			}
		}
		
		void offer(Object<Runnable> task) {
			submittedSem.lock();
			if (submittedTask == NULL && task != NULL) {
				submittedTask = task;
				//unlock the delivery lock to let the processor process this task
				deliveryLock.unlock();
			} else {
				submittedSem.unlock();
				queueListener->runQueueEvent(new RunQueueEvent(EVT_REJECTED, this, Thread::getCurrentThread()));
				throw new TaskRejectedException();
			}
			submittedSem.unlock();
		}

		inline void signal(int signalcode)	{
			signalFlag = signalcode;
			deliveryLock.unlock();
		}

		void setQueueListener(Object<RunQueueListener> listener) {
			this->queueListener = listener;
		}

		Object<RunQueueListener> getQueueListener() {
			return this->queueListener;
		}
	};

	const Logger* RunQueue::logger = Logger::getLogger("RunQueue");

}




#endif

