
#include <iostream>
#include "Thread.h"


using namespace ROG;

const Logger* Thread::logger = Logger::getLogger("Thread");


AVLTreeT<unsigned long, Object<Thread>, AVL_CHUNK_SIZE_THR> Thread::runningThreads;
Semaphore Thread::runningSem;
unsigned long Thread::engine_threadid_master = 0;

Semaphore Thread::eventListenerSem;
OCArray<Object<ThreadEventListener>> Thread::threadEventListeners;


unsigned int __stdcall _staticThreadFunc(void* data)	{
	Thread* thr = reinterpret_cast<Thread*>(data);
	thr->run();
	thr->cleanup();
	_endthreadex( 0 );
	return 0;
}




void Thread::addThreadEventListener(Object<ThreadEventListener> listener) {
	eventListenerSem.lock();
	threadEventListeners.append(listener);
	eventListenerSem.unlock();
}

void Thread::removeThreadEventListener(Object<ThreadEventListener> listener) {
	eventListenerSem.lock();
	threadEventListeners.remove(listener);
	eventListenerSem.unlock();
}

void Thread::fireThreadEvent(Object<ThreadEvent> evt) {
	eventListenerSem.lock();
	
	for (unsigned int i=0; i<threadEventListeners.length(); i++) {
		Object<ThreadEventListener> listener = threadEventListeners.at(i);
		listener->threadEvent(evt);
	}

	eventListenerSem.unlock();
}






void Thread::run() {
	try {
		runnable->run();
	} catch (FatalException* ex) {
		logger->error(ex->getMsg());
		fireThreadEvent(new ThreadEvent(THR_EXCEPTION));
		delete ex;
	}

	fireThreadEvent(new ThreadEvent(THR_STOPPED));
}



/*
HANDLE WINAPI CreateThread(
  __in_opt   LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in       SIZE_T dwStackSize,
  __in       LPTHREAD_START_ROUTINE lpStartAddress,
  __in_opt   LPVOID lpParameter,
  __in       DWORD dwCreationFlags,
  __out_opt  LPDWORD lpThreadId
);
*/

Thread::Thread() : signalFlag(0), runnable(NULL)	{
	_thrid = 0;
	signalFlag = 0;
	_thread = NULL;
}

Thread::Thread(Object<Runnable> runnable) : signalFlag(0)	{
	_thrid = 0;
	signalFlag = 0;
	_thread = NULL;

	this->runnable = runnable;
}

Object<Thread> Thread::getCurrentThread()	{
	runningSem.lock();
	DWORD h = GetCurrentThreadId();
	Object<Thread> mine = NULL;

	AVLTreeTIterator<unsigned long, Object<Thread>, AVL_CHUNK_SIZE_THR> it(runningThreads);
	while (it())	{
		Object<Thread> thr = it.value();
		if (thr->_thrid == h)	{
			mine = thr;
			break;
		}
	}

	runningSem.unlock();

	return mine;
}

Thread::~Thread()	{

}


void Thread::start()	{
	logger->debug("Thread::Start()");

	if (runnable == NULL) {
		throw new FatalException("No runnable specified");
	}

	logger->debug("Starting new thread");

	//a nice big stack :)
	//std::cout << "Creating thread\n";
	_thread = (HANDLE)_beginthreadex(NULL, 0, _staticThreadFunc, (LPVOID)this, CREATE_SUSPENDED, &_thrid);
	if (_thread == 0 || _thrid == 0) {
		int error = 0;
		_get_errno(&error);

		char* msg = NULL;

		switch (error) {
		case EAGAIN:
			msg = "Error creating thread pool: There are too many active threads";
			break;
		case EACCES:
		case ENOMEM:
			msg = "Error creating thread pool: Insufficient resources";
			break;
		default:
			msg = "Error creating thread pool.";
			break;
		}
		//some error occurred
		throw new FatalException(msg);
	}

	Object<Thread> me = this;

	runningSem.lock();
	engine_threadid = ++engine_threadid_master;
	runningThreads.insertKeyAndValue(engine_threadid, me);
	runningSem.unlock();

	if (((Logger*)logger)->isDebugEnabled()) {
		char buf[1024];
		sprintf(buf, "Thread [%d] has been started", engine_threadid);
		logger->debug(buf);
	}

	ResumeThread(_thread);
	fireThreadEvent(new ThreadEvent(THR_STARTED));
}

void Thread::cleanup()	{
	logger->debug("Thread::Cleanup()");
	if (_thrid)	{
		runningSem.lock();
		runningThreads.remove(engine_threadid);
		runningSem.unlock();

		fireThreadEvent(new ThreadEvent(THR_DESTROYED));

		engine_threadid = 0;
		_thrid = 0;
		_thread = NULL;
		//this has to be the last call
		_endthreadex(_thrid);
	}
}

void Thread::signal(int signalcode)	{
	logger->debug("Thread::signal()");
	signalFlag = signalcode;
	if (runnable != NULL) {
		runnable->signal(signalcode);
	}
}


void Thread::waitfor()	{
	logger->debug("Thread::waitfor()");
	while (_thrid != 0 && _thread != NULL)	{
		SleepEx(10, false);
	}
}



void Thread::CollectRemainingThreads()	{
	logger->debug("Thread::CollectRemainingThreads()");

	if (runningThreads.entries() > 0)	{

		Object<Thread> t;

		runningSem.lock();
		AVLTreeTIterator<unsigned long, Object<Thread>, AVL_CHUNK_SIZE_THR> i(runningThreads);
		while (i())	{
			t = i.value();
			logger->debug("Signalling thread");
			t->signal(1);
		}
		runningSem.unlock();

		logger->debug("Waiting for all threads to exit");
		//give the threads we just signalled some processor time to start exiting
		Sleep(100);

		//wait for any leftover threads
		//dont rely on any iterator here, since it could be invalidated by exiting threads
		logger->debug("Destroying threads");
		runningSem.lock();
		while (runningThreads.entries() > 0)	{
			AVLTreeTIterator<unsigned long, Object<Thread>, AVL_CHUNK_SIZE_THR> i(runningThreads);
			i();
			t = i.value();
			runningSem.unlock();
			t->waitfor();
			runningSem.lock();
		}
		runningSem.unlock();
		//give the threads we just signalled some processor time to start exiting
		Sleep(200);
		runningSem.lock();
		runningThreads.clear();
		runningSem.unlock();
		logger->debug("Thread::CollectRemainingThreads() completed");
	}
}


