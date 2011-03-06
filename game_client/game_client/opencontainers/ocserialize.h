#ifndef OCSERIALIZE_H_

#include "ocval.h"

// Routines for a very fast serialization routines. These are not
// compatible with Python Pickling: there are routines available that
// do that.  These routines also do not support cross-platform
// serialializations.

// The number of bytes it will take to serialize the given.  Note that
// there are special signatures for Tabs and strings to avoid extra
// copies (so it won't get converted to a Val first!).
OC_INLINE size_t BytesToSerialize (const Val& v);
OC_INLINE size_t BytesToSerialize (const Tab& t);
OC_INLINE size_t BytesToSerialize (const Str& s);
OC_INLINE size_t BytesToSerialize (const Arr& a);
template <class T>
OC_INLINE size_t BytesToSerialize (const OCArray<T>& a);

// Serialize into a memory buffer: this assumes mem is big enough to
// hold the data (it can be computed with BytesToSerialize).  Note
// that we have extra signatures for Tabs and strings to avoid extra
// construction that may not be necessary. This returns one past where
// the next serialize would begin (so that the total bytes serialized
// can be computed from mem-Serialize(...))
OC_INLINE char* Serialize (const Val& v, char* mem);
OC_INLINE char* Serialize (const Tab& t, char* mem);
OC_INLINE char* Serialize (const Str& s, char* mem);
OC_INLINE char* Serialize (const Arr& s, char* mem);
template <class T>
OC_INLINE char* Serialize (const OCArray<T>& s, char* mem);

// Deserialize into the given memory.  It assumes there is enough
// memory (computed with BytesToSerialize) to deserialize from. This
// returns one byte beyond where it serialized (so mem-return value is
// how many bytes it serialized).  The into val has to be an "empty"
// Val or it throws a logic_error exception.
OC_INLINE char* Deserialize (Val& into, char* mem);


// The implementation: can be put into a .o if you don't want
// everything inlined.
#if !defined(OC_FACTOR_INTO_H_AND_CC)
# include "ocserialize.cc"
#endif


#define OCSERIALIZE_H_
#endif // OCSERIALIZE_H_
