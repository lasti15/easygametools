//this is work based on
//http://www.gamedev.net/reference/articles/article1954.asp

#include "Reference.h"
#include <iostream>
#include <sstream>

using namespace ROG;

const Logger* Reference::log = Logger::getLogger("Reference");;

//Initialize all of the static objects in Reference
Semaphore Reference::liveSem;
Semaphore Reference::deadSem;
AVLHashT<Val, Reference*, AVL_CHUNK_SIZE_MEM> Reference::liveObjects;
AVLHashT2<Val, OCArray<Reference*>, AVL_CHUNK_SIZE_MEM> Reference::deadObjects;
int_u8 Reference::_mem_id_master = 1;
int_u8 Reference::_mem_objects_total = 0;
int_u8 Reference::_live_objects_total;
int_u8 Reference::_dead_objects_total;
int_u8 Reference::_total_used;
int_u8 Reference::_total_allocated;




LPCTSTR ErrorMessage( DWORD error ) 

// Routine Description:
//      Retrieve the system error message for the last-error code.
{ 

    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    return((LPCTSTR)lpMsgBuf);
}


bool Reference::isclosing = false;

void Reference::Reference_initialize()	{
	
}


void Reference::printStats() {

	char buf[1024];

	_snprintf(buf, 1023, "Total Allocated: %d", _total_allocated);
	log->info(buf);

	_snprintf(buf, 1023, "Total Used: %d", _total_used);
	log->info(buf);

	_snprintf(buf, 1023, "Number Allocated: %d", _mem_objects_total);
	log->info(buf);

	_snprintf(buf, 1023, "Avg. Object Size: %d", _total_used / _mem_objects_total);
	log->info(buf);
}

//the new operator allocates from the memory pool
void *Reference::operator new(unsigned int num_bytes)	{
	log->debug("operator new()");
	void* ptr = NULL;

	//check deadobjects to see if we have an object of this size thats not in use right now.
	Val deadKey = (int_u8)num_bytes;

	//for thread safety
	deadSem.lock();
	_total_used+=num_bytes;

	//look to see if we have a dead object that fits our size :) alot of 
	//small objects get allocated and dumped and reallocated, so this will use those.
	//the key is the size of the object we are trying to allocate. This will return an Array
	//which we then need to check to see if it is empty.
	OCArray<Reference*>* arr = &(deadObjects[deadKey]);
	if (arr->length())	{

		log->debug("new() reusing old object");

		//remove this Reference fromt he list of dead objects.
		Reference* obj = arr->removeAt(0);

		//set the return value to the value of this pre-allocated ptr
		ptr = (void*)obj;

		//for thread safety
		deadSem.unlock();

		//explicitly call the destructor for our old object to 
		//make sure it ges the chance to do whatever cleanup it needs to
		obj->~Reference();

		//return the pre-allocated ptr
		return ptr;
	}

	_total_allocated+=num_bytes;

	//for thread safety
	deadSem.unlock();


	log->debug("new() malloc new object");

	//malloc this object on the heap
	ptr = malloc(num_bytes);

	//set the size field of our new object to the size we just allocated
	((Reference*)ptr)->setSize(num_bytes);

	return ptr;
}


Reference::Reference()	{
	//update the constructor to initialise refCount to zero
	refCount=0;
	//cout << "construct memobject" << endl;

	liveSem.lock();
	_mem_objects_total++;
	_live_objects_total++;
	_mem_id = _mem_id_master++;
	Val v = _mem_id;
	liveObjects.insertKeyAndValue(v, this);
	liveSem.unlock();

	log->debug("new reference constructed");
}

void Reference::AddRef()	{
	log->trace("reference added");
	refCount++;
}

void Reference::Release()	{
	refCount--;
	if (refCount==0) {
		log->debug("adding to deadobjects");
		liveSem.lock();
		Val liveKey = _mem_id;
		liveObjects.remove(liveKey);
		liveSem.unlock();
		Reference::addDeadObject(this);
	}
}


bool Reference::operator==(const Reference &other) const {
    if (&other == this) {
		return true;
	}
	return false;
}


void Reference::addDeadObject(Reference* deadObj)	{
	//the key is the size of the object, there are arrays in those buckets
	Val deadKey = (int_u8)deadObj->_size;
	deadSem.lock();

	//it really really never should be added here twice, 
	//it usually means you created a * pointer to a Reference object
	//As in: SomeObjectThatExtendsReference* myObject = new ...();
	//Or: SomeObjectThatExtendsReference myObjectInstance;
	//
	//
	//Dont do that.
	deadObjects[deadKey].append(deadObj);
	_dead_objects_total++;

	deadSem.unlock();
}



void Reference::CollectRemainingObjects()	{
	//this flag makes it so we dont try to insert or update the trees while shutting down
	isclosing = true;
	log->debug("collect remaining objects");
	Reference::CollectGarbage(0);

	liveSem.lock();
	AVLHashTIterator<Val, Reference*, AVL_CHUNK_SIZE_MEM> it(liveObjects);
	while(it())	{
		Reference* arr = it.value();

		//DEVELOPERS:if an exception was thrown in the constructor of a Reference Object, it will create a dead reference in the tree. 
		//this is because at the time the exception was thrown the Reference() constructor had already run, but since not all of the
		//constructors completed successfully, the memory allocation was rejected, and the memobject pointer was recovered by the runtime.
		//I seriously cant think of a solution to this problem.
		delete arr;
	}
	liveObjects.clear();
	liveSem.unlock();

	printStats();
}


Reference::~Reference()	{
	// We add an empty virtual destructor to make sure
	// that the destructors in derived classes work properly.
}



void Reference::CollectGarbage(const long kb)	{
	if (kb < 0)
		return;

	long deleted = 0;

	log->debug("collect garbage");
	//std::cout	<< "collectgarbage" << std::endl;
	deadSem.lock();
	AVLHashTIterator2<Val, OCArray<Reference*>, AVL_CHUNK_SIZE_MEM> it(deadObjects);
	while(it())	{
		OCArray<Reference*>* arr = &(it.value());
		while (arr->length() > 0)	{
			Reference* obj = arr->removeAt(0);
			deleted += obj->size();
			delete obj;
		}

		//leave if we've deleted as much as we thought we had
		if (kb > 0) {
			if ((deleted/1024) >= kb) {
				deadSem.unlock();
				return;
			}
		}
	}
	deadObjects.clear();
	deadSem.unlock();
}






