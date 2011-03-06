//this is work based on
//http://www.gamedev.net/reference/articles/article1954.asp
#ifndef __REFERENCE_H
#define __REFERENCE_H

#define LTRUE 1
#define LFALSE 0

//disable some annoying warning messages from the librarywe are about to include
#pragma warning(disable : 4812)
#pragma warning(disable : 4244)
#pragma warning(disable : 4554)
#pragma warning(disable : 4018)
#pragma warning(disable : 4800)

//opencontainers is a nice compact data structure library
#define OC_NEW_STYLE_INCLUDES
#define OC_BYTES_IN_POINTER 4
#include "opencontainers/opencontainers.h"

//put these warnings back in place
#pragma warning(default : 4812)
#pragma warning(default : 4244)
#pragma warning(default : 4554)
#pragma warning(default : 4018)
#pragma warning(default : 4800)

//import some libraries (arent I a terrible person?)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

//import some ROG objects
//FatalException is the kind of exception we throw for like, null pointer exceptions etc.
#include "FatalException.h"
//A semaphore is, well, a semaphore. If you dont know what it is google it.
//My (gasp) win32 implementation uses system critical sections, which i think is cool.
#include "Semaphore.h"
//Ah logger, make many a logging task so simple
#include "Logger.h"


namespace ROG {

LPCTSTR ErrorMessage( DWORD error );

//the default mem chunk size in our hash tree
#define AVL_CHUNK_SIZE_MEM 64

//A Reference is the base unit of memory allocation in the framework.
//A Reference instance has several characteristics which make them more 
//useful than using standard pointers or objects.
//1. A Reference pointer is reference tracked while it is in use by 
//   any part of you application. You dont need to worry about freeing 
//   any memory allocated as a direct result of calling 
//		ex: new Object<? extends Reference>()
//   with 2 exceptions, mentioned below.
//2. References that are no longer in use are reused as raw memory 
//   space for new objects being allocated.
//3. References can be garbage collected at known intervals or when 
//   total usage exceeds some threashold.
//
//**Because the lifecycle of the Reference object is tied to some 
//  static instance management at the framework level, when you extend 
//  Reference you must always use heap allocation and a Dynamic Pointer.
//		ex: Object<? extends Reference> myObject = new MyObject(constructorArgs);
//
//***Any memory you allocate is yours to manage, unless it is allocated through
//   the invocation of the reference constructor of some object.
//		ex: 
//      class MyObject : public Reference {
//      public:
//			char* someText;
//			Object<SomeOtherReferenceObject> myOtherObject;
//			MyObject() {
//				someText = new char[100];   //this memory is yours to manage because new char[] is not a Reference
//				myOtherObject = new SomeOtherReferenceObject(); // this memory is automatically managed by the framework
//			}
//      };
//
class Reference	{
	private:

		//A semaphore to protect our tree of live objects
		static Semaphore liveSem;
		//A semaphore to protect our tree of dead objects
		static Semaphore deadSem;

		//A sorted AVL tree of all dead References.
		//A Reference becomes dead when it has no Object<> referenceing it.
		static AVLHashT2<Val, OCArray<Reference*>, AVL_CHUNK_SIZE_MEM> deadObjects;

		//A sorted AVL tree of all live References.
		//A Reference becomes live when its Reference() constructor runs.
		//Garbage collection does not happen on live objects until the application is exiting
		static AVLHashT<Val, Reference*, AVL_CHUNK_SIZE_MEM> liveObjects;

		//Add a reference to the list of dead objects
		static void addDeadObject(Reference* deadObj);

		//Our trusty logger
		const static Logger* log;

		//The size in bytes of this Reference instance. This is set in the new() method,
		//and will always reflect t he correct size in bytes of the object.
		long _size;
		//Set the value of the size in bytes. This is called from the new() method.
		void setSize(long size) {
			this->_size = size;
		}

		//The count of currently referencing Object<>s.
		int_8 refCount;

		//The interenal ID of this Reference.
		int_u8 _mem_id;

		//The master internal id
		static int_u8 _mem_id_master;

		//The total number of References allocated. 
		static int_u8 _mem_objects_total;
		//The total number of live objects (should be same as overall total)
		static int_u8 _live_objects_total;
		//The total number of dead objects
		static int_u8 _dead_objects_total;
		//The total number of bytes allocated.
		static int_u8 _total_allocated;
		//The total number of bytes requested.
		static int_u8 _total_used;

		//Print the memory usage stats at the end of application run.
		static void printStats();

	protected:
		//The default reference constructor.
		Reference();

	public:
		//The virtual destructor means all of our inheriting destructors can be called internally even from a Reference*.
		virtual ~Reference();

		//A hint to the allocator that the system is shutting down.
		static bool isclosing;

		//Add to this Reference's refCount.
		void AddRef();
		//Subtract from this Reference's refCount.
		void Release();

		//Initialize the Reference subsystem.
		static void Reference_initialize();

		//Collect kb KB from the deadObjects repository.
		//If kb is not available, delete all of the objects in the repo.
		//pass 0 to 
		static void CollectGarbage(const long kb);
		//Collect all remaining objects in the system
		static void CollectRemainingObjects();
		//Get the syze in bytes of this objects
		const unsigned long size() { return _size; }

		//The overloaded new Operator. Allocates memory from our pool.
		void *operator new(unsigned int num_bytes);
		//The equality operator.
		bool operator==(const Reference &other) const;
};

//The Object<T> is the only way to reference a Reference. Pun intended.
//It contains all of the smart pointer logic to add and remove references.
//This is why you always need to use an Object<T> pointer to access your
//References or you are going to have a bad time.
//You don't want to french fry when you're supposed to pizza.
//Objects are used just like normal pointers. They support the assignment 
//and indirection operators int he same way as regular pointers.
//	ex: Object<MyObject> myObject = new MyObject();
//		myObject->someMethod();
//		myObject = someOtherObject;
//		(*myObject).someMethod();
//
template<class T>
class Object	{
protected:
	T* obj;
public:

	//Constructors - basic
	Object()	{
		obj=0;
	}

	//Constructing with a pointer
	Object(T *o)	{
		obj=0;
		*this=o;
	}

	//Constructing with another smart pointer (copy constructor)
	Object(const Object<T> &p)	{
		obj=0;
		*this=p;
	}

	//Destructor
	~Object()	{
		if (!Reference::isclosing)
		if(obj)obj->Release();
	}

	//Assignement operators - assigning a plain pointer
	inline T* operator =(T *o)	{
		if (!Reference::isclosing)	{
			if(obj)obj->Release();
			obj=o;
			if(obj)obj->AddRef();
		}	else	{
			obj=o;
		}
	  return o;
	}

	//Assigning another smart pointer
	inline T* operator =(const Object<T> &p)	{
		if (!Reference::isclosing)	{
			if(obj)obj->Release();
			obj=p.obj;
			if(obj)obj->AddRef();
		}	else	{
			obj=p.obj;
		}
	  return obj;
	}

	//Access as a reference
	inline T& operator *() const	{
	  if (obj == 0)	{
			throw new FatalException("NullPointerException");
		}
		return *obj;
	}

	//Access as a pointer
	inline T* operator ->() const	{
		if (obj == 0)	{
			throw new FatalException("NullPointerException");
		}
		return obj;
	}

	//Conversion - allow the smart pointer to be automatically
	//converted to type T*
	inline operator T*() const	{
		return obj;
	}


	inline operator void*() const	{
		return (void*)obj;
	}


	inline bool isValid() const	{
		return (obj!=0);
	}

	inline bool operator !()	{
		return !(obj);
	}

	inline bool operator ==(const Object<T> &p) const	{
		return (obj==p.obj);
	}

	inline bool operator !=(const Object<T> &p) const	{
		return (obj!=p.obj);
	}

	inline bool operator ==(const T* o) const	{
		return (obj==o);
	}

	inline bool operator !=(const T* o) const	{
		return (obj!=o);
	}
};

 
}

#endif

