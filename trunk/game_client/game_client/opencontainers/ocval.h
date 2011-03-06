#ifndef OCVAL_H_

// Val is a recursive, heterogeneuous container class, without virtual
// function overhead.  Vals can contains ints and reals, as well as
// arrays (Arr or OCArray<int_type> or OCArray<real_type) and Tabs
// (AVLHashT tables).  For example:

// Val v = 123.567890123456;  // v contains a real_8 (tag='d' for double)
// v = 56;                    // v contains an int   (tag='l' for int)
// v = Tab();                 // now a table         (tag='t')
// v[10] = 100;               // insert key:10 with value 100 into table
// Val result = v[10];        // Get value out
// int_4 ii = result;         // COnvert result into int 

// $Id: val.h,v 1.1 2004/04/06 00:28:08 dtj Exp $

#include "ocavlhasht.h"
#include "occomplex.h"

#if defined(OC_USE_OC_STRING)
# include "ocstring.h"
  typedef OCString Str; // choose to use Str because of speed
# define VALSTR 32      // should be sizeof(Str)
#else
# include "ocstring.h"    // To pick up "HashFunction"
# include <string>
  typedef std::string Str;     // choose STL string for generality
# define VALSTR sizeof(Str)
#endif 

#ifndef VALTAB
# define VALTAB 32 // should be sizeof(Tab)
#endif
#ifndef VALSTR
# define VALSTR 32 // should be sizeof(Str)
#endif
#ifndef VALARR 
# define VALARR 32 // should be sizeof(Arr)
#endif

// Correspondance between tags and types
static char* ValTags [] = { 
  "s", "S", // int_1, int_u1 
  "i", "I", // int_2, int_u2
  "l", "L", // int_4, int_u4
  "x", "X", // int_8, int_u8
  "b",      // bool
  "f",      // real_4
  "d",      // real_8
  "F",      // complex_8 (2 real_4)
  "D",      // complex_16 (2 real_8)
  "a",      // ascii Str, like strings
  "t",      // table
  "n",      // Array of Vals or primitives
  "Z",      // None, the empty value
  0
}; // ValTags

// ///// Forwards
class Arr;
struct Tab; 

// A value container for heterogeneous types: Note that we can
// recursively contain other Vals.
struct Val {

    char tag; char subtype; char pad[2];

    // Constructors for every explicit type
    Val () : tag('Z') { }   // None value
    Val (int_1 v)  : tag('s') { u.s = v; } 
    Val (int_u1 v) : tag('S') { u.S = v; }
    Val (int_2 v)  : tag('i') { u.i = v; }
    Val (int_u2 v) : tag('I') { u.I = v; }
    Val (int_4 v)  : tag('l') { u.l = v; }
    Val (int_u4 v) : tag('L') { u.L = v; }
    Val (int_8 v)  : tag('x') { u.x = v; }
    Val (int_u8 v) : tag('X') { u.X = v; }
    Val (bool v)   : tag('b') { u.b = v; }
    Val (real_4 v) : tag('f') { u.f = v; }
    Val (real_8 v) : tag('d') { u.d = v; }
    Val (complex_8 v) : tag('F') { new (&u.F) complex_8(v); }
    Val (complex_16 v) : tag('D') { new (&u.D) complex_16(v); }
    Val (const char* cc) : tag('a') { new (&u.a) Str(cc); }
    Val (const Str& s)   : tag('a') { new (&u.a) Str(s); }
    OC_INLINE Val (const Tab& v);

    // Construct an Array of any primitive type Val can hold,
    // Array<int_4>, Array<real_8>, Array<Val> (aka Arr).  No Arrays
    // of Arrays.
    template <class T> OC_INLINE Val (const OCArray<T>& a);

    // Copy constructor:  Have to write because of string, table, array cases
    OC_INLINE Val (const Val& r);

    // Copy: Have to write for string, table, array classes
    Val& operator= (const Val& v) {this->~Val();new(this)Val(v);return*this;}

    // Destructor: Have to write for string, table, array classes
    OC_INLINE ~Val ();

    // Useful for cascading lookups s["key"][2], etc.  Like python,
    // you can only use integer types for indices to arrays, but Tabs
    // can use others type as lookup keys. Str currently don't support
    // lookup (TODO: Should they? No. We return Val&)
    OC_INLINE Val& operator [] (int_1  v);
    OC_INLINE Val& operator [] (int_u1 v);
    OC_INLINE Val& operator [] (int_2  v);
    OC_INLINE Val& operator [] (int_u2 v);
    OC_INLINE Val& operator [] (int_4  v);
    OC_INLINE Val& operator [] (int_u4 v);
    OC_INLINE Val& operator [] (int_8  v);
    OC_INLINE Val& operator [] (int_u8 v);
    OC_INLINE Val& operator [] (bool v);
    OC_INLINE Val& operator [] (real_4 v);
    OC_INLINE Val& operator [] (real_8 v);
    //inline Val& operator [] (complex_8 v);
    //inline Val& operator [] (complex_16 v);
    OC_INLINE Val& operator [] (const char* cc);
    OC_INLINE Val& operator [] (const Str& s);
    //OC_INLINE Val& operator [] (Tab& t);      // doesn't make sense
    //OC_INLINE Val& operator [] (Array<T>& t); // doesn't make sense

    // Like operator[] above, but the () operation WILL throw an
    // runtime_error if the key os not in the table.
    OC_INLINE Val& operator () (int_1  v);
    OC_INLINE Val& operator () (int_u1 v);
    OC_INLINE Val& operator () (int_2  v);
    OC_INLINE Val& operator () (int_u2 v);
    OC_INLINE Val& operator () (int_4  v);
    OC_INLINE Val& operator () (int_u4 v);
    OC_INLINE Val& operator () (int_8  v);
    OC_INLINE Val& operator () (int_u8 v);
    OC_INLINE Val& operator () (bool v);
    OC_INLINE Val& operator () (real_4 v);
    OC_INLINE Val& operator () (real_8 v);
    //inline Val& operator () (complex_8 v);
    //inline Val& operator () (complex_16 v);
    OC_INLINE Val& operator () (const char* cc);
    OC_INLINE Val& operator () (const Str& s);
    //OC_INLINE Val& operator () (Tab& t);      // doesn't make sense
    //OC_INLINE Val& operator () (Array<T>& t); // doesn't make sense

    // Out converters for every type: so you can ask for a value
    // easily: 
    // Val somevalue = ...;
    // int get_value_out = somevalue; // Get out as int if possible
    OC_INLINE operator int_1 ()  const;
    OC_INLINE operator int_u1 () const;
    OC_INLINE operator int_2 ()  const;
    OC_INLINE operator int_u2 () const;
    OC_INLINE operator int_4 ()  const;
    OC_INLINE operator int_u4 () const;
    OC_INLINE operator int_8 ()  const;
    OC_INLINE operator int_u8 () const;
    OC_INLINE operator bool () const;
    OC_INLINE operator real_4 () const;
    OC_INLINE operator real_8 () const;
    OC_INLINE operator complex_8 () const;
    OC_INLINE operator complex_16 () const;
    OC_INLINE operator Str ()    const;
    OC_INLINE operator Tab& () const; // Throws logic_error if not really a table
    OC_INLINE operator Arr& () const; // Throws logic_error if not really an Arr

#if defined(TEMPLATE_OUTCONVERSION_SUPPORTED)
    // Interestingly, this "feature" to outcast isn't supported by all
    // compilers: if that is the case, you may have to revert to OC1.3.4
    template <class T> operator OCArray<T>& () const 
    {
      Val tmp = T(); // Only construct types
      if (tag!='n' || tmp.tag != subtype) {
	Str mesg = "No conversion from:"+Str(*this)+" to array.";
	throw logic_error(mesg.c_str());
      } 
      OCArray<T>* n = (OCArray<T>*)&u.n;
      return *n;
    }
#else

// Same code as above ... squiched to fit in macro
#  define TEMPLATE_OUTCONV(T) operator OCArray<T>& () const { \
Val tmp=T(); if (tag!='n' || tmp.tag != subtype) { Str mesg = "No conversion from:"+Str(*this)+" to array.";throw logic_error(mesg.c_str()); } \
OCArray<T>*n=(OCArray<T>*)&u.n;return *n;}

    TEMPLATE_OUTCONV(int_1)
    TEMPLATE_OUTCONV(int_u1)
    TEMPLATE_OUTCONV(int_2)
    TEMPLATE_OUTCONV(int_u2)
    TEMPLATE_OUTCONV(int_4)
    TEMPLATE_OUTCONV(int_u4)
    TEMPLATE_OUTCONV(int_8)
    TEMPLATE_OUTCONV(int_u8)
    TEMPLATE_OUTCONV(bool)
    TEMPLATE_OUTCONV(real_4)
    TEMPLATE_OUTCONV(real_8)
    TEMPLATE_OUTCONV(complex_8)
    TEMPLATE_OUTCONV(complex_16)
    TEMPLATE_OUTCONV(Str)
       //TEMPLATE_OUTCONV(Tab)
    TEMPLATE_OUTCONV(Val)

#endif // TEMPLATE_OUTCONVERSION_SUPPORTED

    union valunion {
	int_1  s;
	int_u1 S;
	int_2  i;
	int_u2 I;
	int_4  l;
	int_u4 L;
	int_8  x;
	int_u8 X;
        bool   b;
	real_4 f;
	real_8 d;       // in a union with a real_8, so no need to worry 
        cx_union_t<real_4> F;
        cx_union_t<real_8> D;
	char a[VALSTR]; // sizeof(Str)
	char t[VALTAB]; // sizeof(Tab) 
        char n[VALARR]; // sizeof(OCArray<T>): Note, the Array container always same size regardless of type
    } u;

    // Swap in O(1) time.
    OC_INLINE void swap (Val& rhs);

    // Print this Val with nesting
    OC_INLINE void prettyPrint (ostream& os, int indent=0) const;

    // Append to the end of the Tab or Arr.
    OC_INLINE void append (const Val& v);
    OC_INLINE void appendStr (const Val& v);

    // Run this to make sure everything is compiled and working okay
    OC_INLINE void test ();

}; // Val

static const Val None;  // None constant, default constructed

// Table of Vals: same interface as HashTable, AVLHash, etc.  An
// AVLHash is an extendible HashTable: performs just about as well as
// a HashTable, but can grow without having to rehash or reduce
// performance.
struct Tab : public AVLHashT<Val, Val, 8> {
  
  // Return the total number of elements: entries only returns the
  // total number of entries at the surface level.  In fact, most of
  // the routines only operate at the "surface" of the table.
  OC_INLINE size_t total_elements() const;

  // Constructor allows easier way to build Tabs from literal strings
  // Tab t = " { 'a':1, 'b':2 }"
  OC_INLINE Tab (const char* s); 
  OC_INLINE Tab (const Str& s);

  // Empty Table
  OC_INLINE Tab ();
  
  // Merge right table into this table
  OC_INLINE Tab& operator+= (const Tab& rhs);

  // Append a new entry, based on the current number of elements
  OC_INLINE void append (const Val& v);     // int version
  OC_INLINE void appendStr (const Val& v);  // string version

  // Swap in O(1) time.
  inline void swap (Tab& rhs) { this->swap(rhs); }

  // Pretty print the table (with indentation)
  OC_INLINE void prettyPrint (ostream& os, int indent=0) const;
  ostream& prettyPrintHelper_ (ostream& os, int indent, bool pretty=true) const;

}; // Tab


// ///////////////////////////////////////////// Class It

// Iterator through Tables.  Syntax:
// Tab some_table = ..
// for (It it(some_table); it(); ) {
//   const Val& key = it.key();      
//   Val& val = it.value();    
//   cerr << key << " " << value << endl;
// }
// NOTE: Keys and values are give to you as references so no copying 
// is required unless you specifically request it.
struct It : public AVLHashTIterator<Val, Val, 8> {
    It (const Tab& t) : AVLHashTIterator<Val, Val, 8>(t) { }
}; // It

// Sorted iterator WILL give you the keys in sorted order
struct Sit : public AVLHashTSortedIterator<Val, Val, 8> {
    Sit (const Tab& t) : AVLHashTSortedIterator<Val, Val, 8>(t) { }
}; // Sit

// ///// An array of Vals.  Very similar to Python List
class Arr : public OCArray<Val> {
 public:
  // Constructor for Arrs that allows easy literal construction
  // Arr a = "[1,'two',3.0, [1,2,3]]";
  OC_INLINE Arr (const char* s);
  OC_INLINE Arr (const Str& s);
  
  Arr (const OCArray<Val>& a) : OCArray<Val>(a) { } 

  // Constructor for expected length.
  OC_INLINE Arr (int expected_len);
  
  // Empty List
  OC_INLINE Arr ();
  
  // Swap in O(1) time
  inline void swap (Arr& rhs) { this->swap(rhs); }

  // Print a more nested structure
  OC_INLINE void prettyPrint (ostream& os, int indent = 0) const;
  OC_INLINE ostream& prettyPrintHelper_ (ostream& os, int indent, bool pretty=true) const;
}; // Arr


// ///////////////////////////////////////////// Class Tab

// HashFunction on Vals: If a string, use Pearson's hash, otherwise
// just return the value as an int_u4.
OC_INLINE int_u4 HashFunction (const Val& v);

OC_INLINE void unknownType_ (const char* routine, char tag);

// If two items are "essentially" the same type, they are converted to
// the bigger type and compared in the bigger type.  If they are
// "incomparable", then they are both converted to strings and
// compared that way.
OC_INLINE bool operator== (const Val& v1, const Val& v2);
OC_INLINE bool operator!= (const Val& v1, const Val& v2);
OC_INLINE bool operator< (const Val& v1, const Val& v2);
OC_INLINE bool operator<= (const Val&v1, const Val&v2);
OC_INLINE bool operator> (const Val& v1, const Val& v2);
OC_INLINE bool operator>= (const Val&v1, const Val&v2);

// Output: Note that default constructed ('Z') comes out as "None".
OC_INLINE ostream& operator<< (ostream& os, const Val& v);
OC_INLINE ostream& operator<< (ostream& os, const Tab& v);
//template <class T>
//OC_INLINE ostream& operator<< (ostream& os, const Array<T>& a);
template <> OC_INLINE ostream& operator<< <Val>(ostream& os, const OCArray<Val>& a);

template <class T> 
OC_INLINE ostream& PrintArray (ostream& os, const OCArray<T>& a);
template <>
OC_INLINE ostream& PrintArray <Val> (ostream& os, const OCArray<Val>& a);

inline void swap (Arr& lhs, Arr& rhs) { lhs.swap(rhs); }
inline void swap (Val& lhs, Val& rhs) { lhs.swap(rhs); }
inline void swap (Tab& lhs, Tab& rhs) { lhs.swap(rhs); }

// The implementation: can be put into a .o if you don't want
// everything inlined.
#if !defined(OC_FACTOR_INTO_H_AND_CC)
# include "ocval.cc"
#endif 


#define OCVAL_H_
#endif // OCVAL_H_
