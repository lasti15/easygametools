#ifndef OCARRAY_H_

// A completely inlineable Array/ArrayPtr class.  These two classes
// essentially implement a sub set of the the Roguewave
// RWTValOrderedVector/RWTPtrOrderedVector.

// How to use the Array class: It is what you'd expect: A wrapper for
// C-style arrays with some error checking (if requested) with
// built-in management for resizing of the array.  It contains things
// by value.  In order for Array to contain elements of type T, T
// needs to support: (1) a copy constructor (2) an operator= (3) an
// operator== (4) a default constructor.

// How to use the ArrayPtr class: It is what you'd expect: A wrapper
// for C-style arrays where you contain things by pointer.
// In order to use ArrayPtr with your type T, T needs to support
// (1) op== only  

// One thing to note about how both Array/ArrayPtr manages memory:
// they actually holds an array of some CAPACITY (which is always
// greater than or equal to the actual LENGTH) number of elements.
// The elements are NOT constructed, but there is enough memory to
// hold them when they need to be constructed.

// ////////////////////////////////////////////////// Include Files

#include "ocport.h"        // Handles portability issues


///////////////////////////////////////////// The Array Class

template <class T>
class OCArray {

  public:

    // Why a default of 2?  Well these classes are used everywhere,
    // and frequently they are left completely empty.  A significant
    // amount of memory gets wasted to store nothing.
    enum {ARRAY_DEFAULT_CAPACITY = 2, ARRAY_NPOS = ~0};

  
    // ///// Methods

    // Constructor.  Create an empty ordered vector with capacity
    // capac.  Should the number of items inserted in exceed this
    // value, the vector will be resized autotmatically.  By default,
    // all memory allocation/deallocations will be done with new and
    // delete, but you can specify that the allocations/deallocations
    // will be done with malloc and free if necessary.
    OCArray (size_t capac = ARRAY_DEFAULT_CAPACITY, int use_new_and_delete = 0):
      length_(0),
      capac_(capac),
      useNewAndDelete_(use_new_and_delete),
      data_(allocate_(capac))  // dependency ... useMallocAndFree needs to be initialized before allocate_ is called
    { }


    // Copy constructor.  Constructs a new order vector as a copy of
    // c.  The copy constructor of all elements will be called.  The
    // new array will have the same capacity and number of elements as
    // the old array.
    OCArray (const OCArray<T>& c) : 
      length_(c.length_),
      capac_(c.capac_),
      useNewAndDelete_(c.useNewAndDelete_),
      data_(allocate_(c.capac_))
    {
      // Iterate through all legal elements, calling copy constructor.
      unsigned int ii;
      for (ii=0; ii<length_; ii++)
	(void)new (&data_[ii]) T(c[ii]);
    }


    // operator=.  Sets self to a copy of rhs. The new array will have
    // the same capacity and number of elements as the old array.  The
    // copy constructor of all elements will be called.  The new array
    // will have the same capacity and number of elements as the old
    // array.
    OCArray<T>& operator= (const OCArray<T>& rhs)
    {
      // Catch the assignment to myself
      if (&rhs!=this) {
     
	// Release lhs resources and then copy rhs resources.
	releaseResources_();
	(void)new (this) OCArray<T>(rhs);
      }
      return *this;
    }


    // Destructor.  Release resources.
    ~OCArray () { releaseResources_(); }


    // Returns the ith value in the array.  The first variant can be
    // used as an lvalue, the second cannot.  The index i must be
    // between zero and the number of items in the collection less 1.
    // No bounds checking is performed.
    T& operator() (size_t i)             { return data_[i]; }
    const T& operator() (size_t i) const { return data_[i]; }


    // Returns the ith value in the array.  The first variant can be
    // used as an lvalue, the second cannot.  The index i must be
    // between zero and the number of items in the collection less 1,
    // or an Exception of Type MidasException will be thrown.
    T& operator[] (size_t i) { return at(i); }
    const T& operator[] (size_t i) const { return at(i); }
    

    // Appends the value a to the end of the array.  The collection
    // will automatically be resized if this causes the number of
    // items in the collection to exceed the capacity.
    void append (const T& a) 
    {
      if (capac_==length_)
	resize(2*capac_);
      (void)new (&data_[length_]) T(a);
      length_++;
    }


    // Returns the ith value in the array.  The first variant can be
    // used as an lvalue, the second cannot.  The index i must be
    // between zero and the number of items in the collection less 1,
    // or an Exception of Type MidasException will be thrown.
    T& at (size_t i) 
    { if (i>=length()) arrayError_(i); return data_[i]; }
    const T& at (size_t i) const 
    { if (i>=length()) arrayError_(i); return data_[i]; }


    // Removes all items from the collection.  The clear() method
    // calls the destructor on each item.  If the Array is holding a
    // built-in type or a simple struct that need not be destructed,
    // clearFast() clears the items without calling the destructor on
    // each one.  Caveat emptor when using clearFast.
    void clear () 
    {
      for (unsigned int ii=0; ii<length(); ii++)
	(&data_[ii])->~T();
      length_ = 0;
    }
    void clearFast () { length_=0; }


    // Returns true if the collection contains an item equal to a.  A
    // linear search is done.  Equality is measured by T's operator==.
    bool contains (const T& a) const 
    {
      for (unsigned int ii=0; ii<length(); ii++)
	if (a==data_[ii])
	  return true;
      return false;
    }

    
    // Returns a pointer to the raw data of the array.  The contents
    // should not be changed.  Should be used with care.
    T* data () { return data_; }

    // Returns the number of items currently in the collection.
    size_t entries () const { return length_; }

    // Performs a linear search, returning the index of the first item
    // that is equal to a.  Returns ARRAY_NPOS if there is no such
    // item.  Equality is measured by T's operator==.
    size_t index (const T& a) const
    {
      for (size_t ii=0; ii<length(); ii++)
	if (a==data_[ii])
	  return ii;
      return ARRAY_NPOS;
    }

    
    // Performs a linear search and return trues if the array contains
    // an object that is equal to the object target and puts a copy of
    // the matching object into ret.  Returns false otherwise and does
    // not touch ret.  Equality is determined by T's operator==.
    bool find (const T& target, T& ret) const
    {
      size_t ii=index(target);
      if (ii==ARRAY_NPOS)
	return false;
      else {
	ret = data_[ii];
	return true;
      }
    }


    // Appends the value of a to the end of the array.  The collection
    // will automatically be resized if this causes the number of
    // items in the collection to exceed the capacity.
    void insert (const T& a) { append(a); }

    // Inserts the value a into the array at index i.  The item
    // previously at position i is moved to i+1, etc.  The collection
    // will automatically be resized if this causes the number of
    // items in the collection to exceed the capacity.  The index i
    // must be between 0 and the number of items in the array or an
    // exception of type out_of_range will be thrown.
    void insertAt (size_t i, const T& a)
    {
      if (i>length())
	arrayError_(i);
      if (length_==capac_)
	resize(2*capac_);
      
      // At this point, you know that the point to insert is valid and
      // that there is enough space to move everything over.
      
      // Default construct into the last space so it actually has been
      // constructed.  (right now, it's only guaranteed to have
      // memory, it has NOT been constructed).
      (void)new (&data_[length()]) T();

      for (int jj=length(); jj>i; jj--)
	data_[jj] = data_[jj-1];
      data_[i] = a;
      length_++;
    }

    // Returns true if there are no items in the collection,
    // false otherwise
    bool isEmpty () const { return length_==0; }

    // Returns the number of items currently in the collection.
    size_t length () const { return length_; }

    // Prepends the value a to the beginning of the array.  The
    // collection will automatically be resized if this causes the
    // number of items in the collection to exceed the capacity.
    void prepend (const T& a) { insertAt(0, a); }


    // Performs a linear search, removing the first object which is
    // equal to the object and returns true.  Returns false if there
    // is no such object.  Equality is measure by operator== for T.
    bool remove (const T& val)
    {
      for (int ii=0; ii<length(); ii++)
	if (val==data_[ii]) {
	  (void)removeAt(ii);
	  return true;
	}
      return false;
    }

    // Removes all items which are equal to a, returning the number
    // removed.
    size_t removeAll (const T& val) 
    {
      size_t count = 0;
      int ii=0;
      while (ii<length()) {
	if (val==data_[ii]) {
	  removeAt(ii);
	  count++;
	} else
	  ii++;
      }
      return count;
    }


    // Removes and returns the object at index i.  The array has to
    // shift down (by one) all the items above it to perform the
    // removal. An exception of type out_of_range will be thrown if the
    // list is empty.
    T removeAt (size_t i) 
    {
      if (i>=length())
	arrayError_(i); // Note:  This always throws an exception

      // Otherwise, things are okay
      T ret_val = data_[i];
    
      // Move em over
      int jj=i;
      for (; jj<length()-1; jj++)
	data_[jj] = data_[jj+1];
      
      (&data_[jj])->~T(); // Destruct the last
      length_--;          // Shrink
      
      return ret_val;  // All done, return ret_val
    }
    

    // Removes, starting at position i, the next run_length objects.
    // Note that we have to move all items above down by run_length.
    // This operation is essentially run_length removeAt operations,
    // except that this is more efficient.  If, during the process, we
    // run off the end of the array, an exception of type out_of_range
    // will be thrown.
    void removeRange (size_t i, size_t run_length)
    {
      // Three checks to avoid accidental -1s
      if (i>=length() || i+run_length>length() || run_length>length()) 
	arrayError_(i); // Note:  This always throws an exception

      // Otherwise, things are okay: Move em over (
      int jj=i;
      for (; jj+run_length<length(); jj++) {
	data_[jj] = data_[jj+run_length];
      }
      
      // Destruct last bit of objects
      for (int kk=0; kk<run_length; kk++) {
	(&data_[jj+kk])->~T(); // Destruct the last run_length
      }
      
      // And shrink
      length_-= run_length;         
    }


    // Removes and returns the first object in the collection.  An
    // exception of type out_of_range will be thrown if the array is
    // empty.
    T removeFirst () { return removeAt(0); }

    // Removes and returns the last object in the collection.  An
    // exception of type out_of_range will be thrown if the array is
    // empty.
    T removeLast () { return removeAt(length_==0 ? 0 : length_-1); }

    // Changes the capacity of the collection to n.  Note that the
    // number of objects in the collection does not change, just the
    // capacity.    
    void resize (size_t new_capacity)
    {
      // Zero capac becomes 1
      if (!new_capacity)
	new_capacity = 1;

      // The new capacity has to be at least as big as the length.  If
      // not, no need to do any work.
      if (new_capacity<=length()) {
	capac_ = length();
	return;
      }
      
      T* new_data = allocate_(new_capacity);

      // Copy the real elements first
      unsigned int ii=0;
      for (; ii<length(); ii++)
	(void)new (&new_data[ii]) T(data_[ii]);

      // Clean up old resources and install new resources.
      releaseResources_();
      data_ = new_data;
      capac_ = new_capacity;
    }

    // Swap with another Array: This is a O(1) operation since we only
    // swap pointers (no copying of full arrays).
    void swap (OCArray<T>& rhs) 
    {
      ::swap(length_, rhs.length_);
      ::swap(capac_,  rhs.capac_);
      ::swap(useNewAndDelete_, rhs.useNewAndDelete_);
      ::swap(data_,   rhs.data_);
    }

    // Inspect the current capacity (useful if you decide you want to
    // resize or not)
    size_t capacity () const { return capac_; }
    
    // Expand the array to the given size.  Caveat Emptor when using
    // this method, as it will NOT call the destructors when setting
    // the length.  This is really most useful when using POD (data
    // with out constructors/destructors).
    
    void expandTo (size_t l) 
    {
      if (l>capac_) {
	resize(l);
      }
      length_ = l;
    }

  protected:

    // ///// Data Members

    // The number of items currently in the array.
    size_t length_;

    // The number of spaces allocated in the array.  This is
    // necessarily larger than length.
    size_t capac_;

    // Indicate whether you want to use malloc/free or op new/op
    // delete for managing memory.  We don't use a bool because of
    // problems (on some platforms) with static bools.
    int useNewAndDelete_;

    // The pointer to the actual data
    T* data_;


    // ///// Methods

    // Centralize the throw out_of_range 
    void arrayError_ (size_t i) const
    { 
      throw out_of_range("array: attempting to access element:"+
			 Stringize(i)+" in array of length:"+
			 Stringize(length()));
      // throw out_of_range("array");
    }

    // Allocate the memory resources.  This does not call the
    // constructors
    T* allocate_ (size_t amt) const 
    {
      if (useNewAndDelete_) 
	return (T*) operator new(sizeof(T)*amt);
      else
	return (T*) malloc(sizeof(T)*amt);
    }

    // Deleting the resources.  This involves calling the destructor
    // on each legal element.
    void releaseResources_ ()
    {
      // Force destructor call on this memory
      for (unsigned int ii=0; ii<length(); ii++)
	(&data_[ii])->~T();

      // Delete the memory
      if (useNewAndDelete_)
	operator delete(data_);
      else
	free(data_);
    }

   
}; // Array


// ///////////////////////////////////////////// The ArrayPtr Class

// The ArrayPtr: essentially a drop-in replacement for the Roguewave
// RWTPtrOrderedVector class.  Most of the implementation comes from
// the Array class, but we are templatized on T* rather than T.
template <class T>
class ArrayPtr {

  public:

    // Why a default of 2?  Well these classes are used everywhere,
    // and frequently they are left completely empty.  A significant
    // amount of memory gets wasted to store nothing.
    enum {ARRAY_DEFAULT_CAPACITY = 2, ARRAY_NPOS = ~0};

    // ///// Methods

    // Constructor.  Constructs an empty ordered vector with capacity
    // capac.  Should the number of items exceed this value, the
    // vector will ber resized automatically. By default, all memory
    // allocation/deallocations will be done with new and delete, but
    // you can specify that the allocations/deallocations will be done
    // with malloc and free if necessary.
    ArrayPtr (size_t capac=ARRAY_DEFAULT_CAPACITY, int use_new_and_delete = 1):
      a_(capac, use_new_and_delete)
    { }

    // Copy constructor.  Constructs self to be a shallow copy of
    // c.  Afterwards, pointers will be shared between the two
    // collections.
    ArrayPtr (const ArrayPtr<T>& c) :
      a_(c.a_)
    { }
    
    // Destructor.  Removes all pointers but does not delete what they
    // point to.
    ~ArrayPtr () { }


    // Operator=.  Constructs self to be a shallow copy of c.
    // Afterwards, pointers will be shared between the two
    // collections.
    ArrayPtr<T>& operator= (const ArrayPtr<T>& c) { a_=c.a_; return *this; }


    // Returns a pointer to the ith value of the array.  The first
    // variant can be used as an l-value, the second cannot.  The
    // index i must be between zero and the number of items in the
    // collection less one.  No bounds checking is performed.
    T*& operator() (size_t i) { return a_(i); }
    T* const& operator() (size_t i) const { return a_(i); }


    // Returns a pointer to the ith value of the array.  The first
    // variant can be used as an l-value, the second cannot.  The
    // index i must be between zero and the number of items in the
    // collection less one, or an exception of type out_of_range will be
    // thrown.
    T*& operator[] (size_t i) { return a_[i]; }
    T* const& operator[] (size_t i) const { return a_[i]; }


    // Appends the item pointed to by a to the end of the array.  The
    // collection will automatically be resized if this causes the
    // number of items in the collection to exceed the capacity.
    void append (T* a) { a_.append((T*)a); }


    // Returns a pointer to the ith value of the array.  The first
    // variant can be used as an l-value, the second cannot.  The
    // index i must be between zero and the number of items in the
    // collection less one, or an exception of type out_of_range will be
    // thrown.
    T*& at (size_t i) { return a_[i]; }
    T* const& at (size_t i) const { return a_[i]; }


    // Removes all items from the collection without deleting them.
    void clear () { a_.clearFast(); }
    void clearFast () { a_.clearFast(); }
    
    // Removes and deletes all items from the collection.
    void clearAndDestroy () 
    {
      // Delete old stuff
      for (int ii=0; ii<length(); ii++)
	delete a_(ii);
      clear();
    }

    // Returns TRUE if the collection contains an item equal to the
    // object pointed to by a, false otherwise.  A linear search is
    // done.  Equality is measured by T's operator==.
    bool contains (const T* a) const { return a_.contains((T*)a); }
    
    // Returns a pointer to the raw memory of the array ptr.  The
    // contents should not be changed.  Should be used with care.
    T** data () const { return a_.data(); }

    // Returns the number of items currently in the collection.
    size_t entries () const { return length(); }

    // Returns a pointer to the first object encountered which is
    // equal to the object pointed to by target, or nil is no such
    // object can be found.  Equality is measured by the class-defined
    // equality operator for type T.
    T* find (const T* target) const 
    {
      size_t ind = index(target);
      if (ind==ARRAY_NPOS)
	return 0;
      else
	return a_(ind);
    }

    // Performs a linear search, returning the index of the first
    // object that is equal to the object pointed to by target, or
    // ARRAY_NPOS if there is no object.  Equality is measured by the
    // class-defined equality operator for type T.
    size_t index (const T* target) const 
    {
      for (int ii=0; ii<length(); ii++) 
	if (*target==*a_(ii))
	  return ii;
      return ARRAY_NPOS;
    }

    // Adds the object pointed to by a to the end of the array.  The
    // collection will be resized automatically if this causes the
    // number of items to exceed the capacity.
    void insert (T* a) { a_.insert(a); }

    // Inserts the value a into the array at index i.  The item
    // previously at position i is moved to i+1, etc.  The collection
    // will automatically be resized if this causes the number of
    // items in the collection to exceed the capacity.  The index i
    // must be between 0 and the number of items in the collection or
    // an exception of type out_of_range will be thrown.
    void insertAt (size_t i, T* a) { a_.insertAt(i,a); }

    // Returns true if there are no items in the collection,
    // false otherwise.
    bool isEmpty () const { return length()==0; }

    // Returns the number of items currently in the collection.
    size_t length () const { return a_.length(); }

    // Add the item pointed to by a to the beginning of the
    // collection.  The collection will be resized automatically if
    // this causes the number of items to exceed the capacity.
    void prepend (T* a) { a_.prepend(a); }

    // Performs a linear search, removing the first object which is
    // equal to the object pointed to by a and returns a pointer to
    // it, or NIL if no such object could be found.  Equality is
    // measured by T's operator==.
    T* remove (const T* a)
    {
      for (int ii=0; ii<length(); ii++) 
	if (*a==*a_(ii))
	  return removeAt(ii);
      return 0;
    }

    // Performs a linear search, removing all objects which are equal
    // to the object pointed to by a.  Returns the number of objects
    // removed.  Equality is measured by T's operator==.    
    size_t removeAll (const T* a) { return a_.removeAll((T*)a); }

    // Removes the object at index i and returns a pointer to it.  An
    // exception of type out_of_range will be thrown if it is not a
    // valid index.  Valid indices are from zero to the number of
    // items in the array less one.  Note, all the other indices will
    // then slide down.
    T* removeAt (size_t i) { return a_.removeAt(i); }

    // Removes run_length objects starting at index i. An exception of
    // type BadIndex will be thrown if the range is not valid.  Note
    // that all the other indices slide down.
    void removeRange (size_t i, size_t run_length) { return a_.removeRange(i, run_length); }

    // Removes the first item in the collection and returns a pointer
    // to it.  An exception of type out_of_range will be thrown if the
    // list is empty.
    T* removeFirst () { return removeAt(0); }

    // Removes the last item in the collection and returns a pointer
    // to it.  An exception of type out_of_range will be thrown if the
    // list is empty.
    T* removeLast () { return removeAt(length()-1); }

    // Changes the capacity of the collection to N.  Note that the
    // number of objects in the collection does not change, just the
    // capacity.
    void resize (size_t N) { a_.resize(N); }

    // Swap with another Array: This is a O(1) operation since we only
    // swap pointers (no copying of full arrays).
    void swap (ArrayPtr<T>& rhs) { a_.swap(rhs.a_); }

  protected:

    // ///// Data Members

    // The implementation: Holds an array of pointers.  Why don't we
    // inherit from Array directly? Because the interfaces between
    // Array and ArrayPtr are just different enough.  (In particular,
    // the const T&, when templatized on T* becomes T* const which
    // causes problems in the interface.  Also you have to worry about
    // how "==" is defined ... by derefencing or straight?)
    OCArray<T*> a_;

    
}; // ArrayPtr



// ///////////////////////////////////////////// Global Operators

template <class T>
inline bool operator== (const OCArray<T>& a1, const OCArray<T>& a2)
{
  if (a1.length()!=a2.length()) return false;
  for (int ii=0; ii<a1.length(); ii++) 
    if (a1[ii]!=a2[ii]) return false;
  return true;
}

template <class T>
inline bool operator!= (const OCArray<T>& a1, const OCArray<T>& a2)
{ return !(a1==a2); }

template <class T>
inline bool operator== (const ArrayPtr<T>& a1, const ArrayPtr<T>& a2)
{
  if (a1.length()!=a2.length()) return false;
  for (int ii=0; ii<a1.length(); ii++) 
    if (a1[ii]!=a2[ii]) return false;
  return true;
}

template <class T>
inline bool operator!= (const ArrayPtr<T>& a1, const ArrayPtr<T>& a2)
{ return !(a1==a2); }

template <class T>
inline bool operator< (const OCArray<T>& a1, const OCArray<T>& a2)
{
  const bool smaller = a1.length() < a2.length();
  const int m = smaller ? a1.length() : a2.length();
  for (int ii=0; ii<m; ii++) 
    if (a1[ii]<a2[ii]) return true;
  return smaller;
}

template <class T>
inline bool operator<= (const OCArray<T>& a1, const OCArray<T>& a2) 
{ return a1<a2 || a1==a2;}

template <class T>
inline bool operator> (const OCArray<T>& a1, const OCArray<T>& a2)
{ return !(a1<=a2); }

template <class T>
inline bool operator>= (const OCArray<T>& a1, const OCArray<T>& a2)
{ return !(a1<a2); }

template <class T>
inline ostream& operator<< (ostream& os, const OCArray<T>& a)
{
  for (int ii=0; ii<a.length(); ii++)
    os << a(ii) << " ";
  return os;
}

template <class T>
inline ostream& operator<< (ostream& os, const ArrayPtr<T>& a)
{
  for (int ii=0; ii<a.length(); ii++)
    os << *a(ii) << " ";
  return os;
}

// Alias for append.
template<class T>
inline OCArray<T>& operator<< (OCArray<T>& arr, const T& item)
{
  arr.append(item);

  return arr;
}

template <class T>
inline void swap (OCArray<T>& lhs, OCArray<T>& rhs)
{
  lhs.swap(rhs);
}


// Aliases for append.  Since only pointers can be added for arrays,
// the operator will work with pointers or references, but in both
// cases, quietly store only the pointer.
template<class T>
inline ArrayPtr<T>& operator<< (ArrayPtr<T>& arr, T& item)
{
  arr.append(&item);

  return arr;
}

template<class T>
inline ArrayPtr<T>& operator<< (ArrayPtr<T>& arr, T* item)
{
  arr.append(item);

  return arr;
}

// Array concatenation operator.  
template<class T>
inline OCArray<T>& operator+= (OCArray<T>& lhs, const OCArray<T>& rhs)
{
  for (int i = 0; i < rhs.length(); ++i) {
    lhs.append(rhs[i]);
  }
  return lhs;
}

template<class T>
inline ArrayPtr<T>& operator+= (ArrayPtr<T>& lhs, const ArrayPtr<T>& rhs)
{
  for (int i = 0; i < rhs.length(); ++i) {
    lhs.append(rhs[i]);
  }
  return lhs;
}

template <class T>
inline void swap (ArrayPtr<T>& lhs, ArrayPtr<T>& rhs)
{
  lhs.swap(rhs);
}

#define OCARRAY_H_
#endif // OCARRAY_H_



