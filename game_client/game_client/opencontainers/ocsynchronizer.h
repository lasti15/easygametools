#ifndef SYNCHRONIZER_H_

// $Id: synchronizer.h,v 1.4 2004/05/10 21:39:57 dtj Exp $

// ///////////////////////////////////////////// Synchronizer

extern "C" {
#undef __PURE_CNAME  // some craziness for Tru64, doesn't hurt anyone
#include <errno.h>
}


#include "ocport.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>


// Wrapper for POSIX Mutex
struct Mutex {
  // Initialize a Mutex
  Mutex (bool shared_across_processes=false) 
  {
    // Initialize Mutex: Because the mutex is probably allocated in
    // shared memory, explicitly inititialize.
    {
      pthread_mutexattr_t mutex_attr;
      if (pthread_mutexattr_init(&mutex_attr)) {
	perror("pthread_mutexattr_init");
	exit(1);
      }
	
      // Necessary so mutexes can be shared between processes.
      if (shared_across_processes) {
#ifndef _POSIX_THREAD_PROCESS_SHARED

# ifndef PTHREAD_PROCESS_SHARED
        // Can't compile this because mutexes/cvs can't be shared 
	errout_("Mutexes can't be shared across processes");
# endif 
	
#endif
	if (pthread_mutexattr_setpshared(&mutex_attr, 
					 PTHREAD_PROCESS_SHARED)) {
	  perror("pthread_mutexattr_setpshared");
	  exit(1);
	}
      }
      // Initialize mutex with these attributes
      if (pthread_mutex_init(&lock_, &mutex_attr)) {
	perror("pthread_mutex_init");
	exit(1);
      }
      // Clean up attribute object
      if (pthread_mutexattr_destroy(&mutex_attr)) {
	perror("pthread_mutexattr_destroy");
	exit(1);
      }
    }
  }
  
  void lock ()        { pthread_mutex_lock(&lock_); }
  void unlock ()      { pthread_mutex_unlock(&lock_); }

  ~Mutex ()
  {
    // Clean up mutex
    if (pthread_mutex_destroy(&lock_)) {
      perror("pthread_mutex_destroy");
      exit(1);
    }
  }

  pthread_mutex_t lock_;  
}; // Mutex


// This is a helper class to lock and unlock Mutexes automatically so
// thrown exceptions will automatically destruct the ProtectScope and
// unlock the Mutex.
struct ProtectScope {
  ProtectScope (Mutex& m) : mp_(m) { mp_.lock(); }
  ~ProtectScope () { mp_.unlock(); }
  Mutex& mp_;
}; // ProtectScope


// A wrapper for POSIX CondVars.  Note that this is an inplace class:
// none of the data members allocate memory on the heap.
struct CondVar {

    CondVar (bool shared_across_processes=false) 
    { init(shared_across_processes); }
    
    ~CondVar ()
    {
      // Make sure everyone out.  TODO:  make this more robust.
      lock();
      condition_ = true;
      broadcast();
      unlock();

      // Clean up CondVar
      if (pthread_cond_destroy(&cv_)) {
	perror("pthread_cond_destroy");
	exit(1);
      }
      // Clean up mutex
      if (pthread_mutex_destroy(&lock_)) {
	perror("pthread_mutex_destroy");
	exit(1);
      }
    }
    
    
    // Initialize the locks and condvars.
    void init (bool shared_across_processes=false)
    {
      // Initialize Mutex: Because the mutex is probably allocated in
      // shared memory, explicitly inititialize.
      {
	pthread_mutexattr_t mutex_attr;
	if (pthread_mutexattr_init(&mutex_attr)) {
	  perror("pthread_mutexattr_init");
	  exit(1);
	}
	
	if (shared_across_processes) {
#ifndef _POSIX_THREAD_PROCESS_SHARED

# ifndef PTHREAD_PROCESS_SHARED
      // Can't compile this because mutexes/cvs can't be shared 
	  errout_("Mutexes can't be shared across process boundaries");
# endif 

#endif
	  // Necessary so mutexes can be shared between processes.
	  if (pthread_mutexattr_setpshared(&mutex_attr, 
					   PTHREAD_PROCESS_SHARED)) {
	    perror("pthread_mutexattr_setpshared");
	    exit(1);
	  }
	}
	// Initialize mutex with these attributes
	if (pthread_mutex_init(&lock_, &mutex_attr)) {
	  perror("pthread_mutex_init");
	  exit(1);
	}
	// Clean up attribute object
	if (pthread_mutexattr_destroy(&mutex_attr)) {
	  perror("pthread_mutexattr_destroy");
	  exit(1);
	}
      }
      
      // CVs: Becase the CV is probably allocated in shared memory,
      // explicitly initialize
      {
	pthread_condattr_t cv_attr;
	if (pthread_condattr_init(&cv_attr)) {
	  perror("pthread_condattr_init");
	  exit(1);
	}
	// Necessary so cvs can be shared between processes.
	if (pthread_condattr_setpshared(&cv_attr, PTHREAD_PROCESS_SHARED)) {
	  perror("pthread_condattr_setpshared");
	  exit(1);
	}
	// Initialize cv with these attributes
	if (pthread_cond_init(&cv_, &cv_attr)) {
	  perror("pthread_mutex_init");
	  exit(1);
	}
	// Clean up attribute object
	if (pthread_condattr_destroy(&cv_attr)) {
	  perror("pthread_condattr_destroy");
	  exit(1);
	}
      }
      condition_ = false;
    }

    void lock ()        { pthread_mutex_lock(&lock_); }
    void unlock ()      { pthread_mutex_unlock(&lock_); }
    void signal ()      { pthread_cond_signal(&cv_); }
    void broadcast ()   { pthread_cond_broadcast(&cv_); }
    void wait ()        { pthread_cond_wait(&cv_, &lock_); }


    // Returns true if a timeout occurred, false if a normal wakeup.
    // TODO: This needs a lot of work, don't use yet.
#define NANOSECONDS_IN_SECOND 1000000000UL
    bool timedwait (int_u8 microseconds = 1000) // .001 of a second by default
    {
      struct timeval now;
      struct timespec timeout;
      int retcode;

      gettimeofday(&now, 0);
      timeout.tv_sec = now.tv_sec;
      timeout.tv_nsec = (now.tv_usec + microseconds) * 1000;
//      if (timeout.tv_nsec>NANOSECONDS_IN_SECOND) {
//	timeout.tv_sec  += 1;
//	timeout.tv_nsec %= NANOSECONDS_IN_SECOND;
//      }
      retcode = 0;
      retcode = pthread_cond_timedwait(&cv_, &lock_, &timeout);
      return (retcode == ETIMEDOUT );
    }

    volatile bool& operator() () { return condition_; }

    pthread_mutex_t lock_;
    pthread_cond_t  cv_;
    volatile bool   condition_;

}; // CondVar 


#define SYNCHRONIZER_H_
#endif // SYNCHRONIZER_H_
