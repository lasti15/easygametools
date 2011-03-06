
#if defined(OC_FACTOR_INTO_H_AND_CC)
# include "ocval.h"
  // Explicit instantiations
#define OC_INST_PRINT(T) template ostream& PrintArray<T>(ostream&, const OCArray<T>& a);

OC_INST_PRINT(int_1)
OC_INST_PRINT(int_u1)
OC_INST_PRINT(int_2)
OC_INST_PRINT(int_u2)
OC_INST_PRINT(int_4)
OC_INST_PRINT(int_u4)
OC_INST_PRINT(int_8)
OC_INST_PRINT(int_u8)
OC_INST_PRINT(real_4)
OC_INST_PRINT(real_8)
OC_INST_PRINT(complex_8)
OC_INST_PRINT(complex_16)
OC_INST_PRINT(bool)
OC_INST_PRINT(Str)
OC_INST_PRINT(Tab)

#define OC_INST_VAL(T) template Val::Val (const OCArray<T>&);

OC_INST_VAL(int_1)
OC_INST_VAL(int_u1)
OC_INST_VAL(int_2)
OC_INST_VAL(int_u2)
OC_INST_VAL(int_4)
OC_INST_VAL(int_u4)
OC_INST_VAL(int_8)
OC_INST_VAL(int_u8)
OC_INST_VAL(real_4)
OC_INST_VAL(real_8)
OC_INST_VAL(complex_8)
OC_INST_VAL(complex_16)
OC_INST_VAL(bool)
OC_INST_VAL(Str)
OC_INST_VAL(Tab)
OC_INST_VAL(Val)

#endif

///////////////////////////////////////////////////////////////////////////
// Most of the useful interface is in the .h file, below be
// implementation.  If you are worried about code bloat, #define
// OC_FACTOR_INTO_H_AND_CC and you can compile this .cc into an object
// file.  Otherwise, this .cc file is just "included" and handled as
// if it were inlined code.

// Discussion: if you look at the implementation of Val, you may notice
// that I explicitly break rules [9] from Exceptional C++.  For
// scalability and performance, two of the fundamental rules of Midas
// 2k were "stay out of the heap" and "use a lookaside cache for
// locality".  So, you'll notice that the Str and Tab are constructed
// in-place in the 32 bytes of the Val.  This keeps most small Strs
// from going to the Heap, it makes one less call to the heap for
// Tabs, and keeps all numeric types in 32 bytes of val so they don't
// go the heap at all!  Herb complains that this is a bad idea, but
// (a) it works on 3 different platforms (b) I am using it with a
// real_8 in a union so the alignments are not problematic (c) there
// are no polymorphic types, so shouldn't be object slicing (d) it's
// great for performance: stay in a lookaside cache for scalability,
// locality.  And honestly, the code isn't that bad.  I break the rule
// on purpose, for performance and scalabilty.

OC_INLINE int_u4 HashFunction (const Val& v) 
{
  int_u4 retval;
  if (v.tag=='a') {
    Str* sp = (Str*)v.u.a;
    retval = HashFunction(*sp);
  } else {
    retval = v;
  }
  return retval;
}

OC_INLINE bool operator== (const Val& v1, const Val& v2);
OC_INLINE bool operator!= (const Val& v1, const Val& v2) { return !(v1==v2); }
OC_INLINE bool operator< (const Val& v1, const Val& v2); 
OC_INLINE bool operator<= (const Val&v1, const Val&v2)
                                                 { return (v1==v2)||(v1<v2); }
OC_INLINE bool operator> (const Val& v1, const Val& v2)  { return !(v1<=v2); }
OC_INLINE bool operator>= (const Val&v1, const Val& v2)  { return !(v1<v2); }

// Output: Note that default constructed ('Z') comes out as "None".
OC_INLINE ostream& operator<< (ostream& os, const Val& v);
OC_INLINE ostream& operator<< (ostream& os, const Tab& v);
//template <class T>
//OC_INLINE ostream& operator<< (ostream& os, const Array<T>& a);
template <> OC_INLINE ostream& operator<< <Val>(ostream& os, const OCArray<Val>& a);

OC_INLINE size_t Tab::total_elements () const
{
  size_t surface_entries = 0;
  for (It ii(*this); ii(); ) {
    Val& val(ii.value());
    if (val.tag=='t') {
      Tab& t = val; // Tab converts out as a reference, so no copying
      // +1 if we want to include this entry in the count
      surface_entries += t.total_elements() + 1;  
    } else {
      surface_entries++;
    }
  }
  return surface_entries;
}


OC_INLINE void Val::test ()
{
  // Make sure Tab can fit into union
  if (sizeof(Tab)>VALTAB) {
    Val sz = int_u4(sizeof(Tab));
    Str mesg="Tab is too big to fit in the union: VALTAB should be:"+Str(sz);
    cerr << mesg << endl;
    throw logic_error(mesg.c_str());
  }
  // Make Strs can fit in union
  if (sizeof(Str)>VALSTR) {
    Val sz = int_u4(sizeof(Str));
    Str mesg="Tab is too big to fit in the union: VALSTR should be:"+Str(sz);
    cerr << mesg << endl;
    throw logic_error(mesg.c_str());
  }
  // Make Arrs can fit in union
  if (sizeof(OCArray<Val>)>VALARR) {
    Val sz = int_u4(sizeof(Arr));
    Str mesg="Arr is too big to fit in the union: VALSTR should be:"+Str(sz);
    cerr << mesg << endl;
    throw logic_error(mesg.c_str());
  }
}


// For unknown types, throw same exception
OC_INLINE void unknownType_ (const char* routine, char tag)
{
  char name[2]; name[0] = tag; name[1] = '\0';
  Str mesg = "Unknown type:"+ Str(name)+ " in routine:" + Str(routine);
  throw logic_error(mesg.c_str());
}

      
OC_INLINE Val::Val (const Tab& t) : tag('t') { new (&u.t) Tab(t); }

template <class T> 
OC_INLINE Val::Val (const OCArray<T>& a) : tag('n') 
{
  Val tmp = T(); // construct a temporary "unused" of this type to get subtype
  subtype = tmp.tag;
  if (subtype=='n') {
    throw logic_error("Arrays of Arrays not currently supported");
  }
  new (&u.n) OCArray<T>(a);
}

#define VALDESTR(T) { OCArray<T>*ap=(OCArray<T>*)&u.n;ap->~OCArray<T>(); }
OC_INLINE Val::~Val ()
{
  switch(tag) { 
  case 'a': { Str* sp = (Str*)&u.a; sp->~Str(); break; }
  case 't': { Tab* tp = (Tab*)&u.t; tp->~Tab(); break; }
  case 'n': { 
    switch(subtype) { 
    case 's': VALDESTR(int_1);  break;
    case 'S': VALDESTR(int_u1); break;
    case 'i': VALDESTR(int_2);  break;
    case 'I': VALDESTR(int_u2); break;
    case 'l': VALDESTR(int_4);  break;
    case 'L': VALDESTR(int_u4); break;
    case 'x': VALDESTR(int_8);  break;
    case 'X': VALDESTR(int_u8); break;
    case 'b': VALDESTR(bool);   break;
    case 'f': VALDESTR(real_4); break;
    case 'd': VALDESTR(real_8); break;
    case 'F': VALDESTR(complex_8); break;
    case 'D': VALDESTR(complex_16); break;
    case 'a': VALDESTR(Str);    break;
    case 't': VALDESTR(Tab);    break;
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': VALDESTR(Val);    break;
    default: unknownType_("destructor", subtype);
    }
  }
  }
}

#define VALCOPYCONS(T) {subtype=r.subtype;OCArray<T>*ap=(OCArray<T>*)&r.u.n;new(&u.n)OCArray<T>(*ap);}
OC_INLINE Val::Val (const Val& r) : 
  tag(r.tag) 
{ 
  // Copy constructor: Have to write because of Str and table cases.
  // Although we could copy in less code, we want to be purify clean
  // (for now).
  switch(tag) { 
  case 's': u.s = r.u.s; break;
  case 'S': u.S = r.u.S; break;
  case 'i': u.i = r.u.i; break;
  case 'I': u.I = r.u.I; break;
  case 'l': u.l = r.u.l; break;
  case 'L': u.L = r.u.L; break;
  case 'x': u.x = r.u.x; break;
  case 'X': u.X = r.u.X; break;
  case 'b': u.b = r.u.b; break;
  case 'f': u.f = r.u.f; break;
  case 'd': u.d = r.u.d; break;
  case 'F': u.F = r.u.F; break;
  case 'D': u.D = r.u.D; break;
  case 'a': { Str* sp=(Str*)&r.u.a; new (&u.a) Str(*sp); break; }
  case 't': { Tab* tp=(Tab*)&r.u.t; new (&u.t) Tab(*tp); break; }
  case 'n': {
    switch(r.subtype) { 
    case 's': VALCOPYCONS(int_1);  break;
    case 'S': VALCOPYCONS(int_u1); break;
    case 'i': VALCOPYCONS(int_2);  break;
    case 'I': VALCOPYCONS(int_u2); break;
    case 'l': VALCOPYCONS(int_4);  break;
    case 'L': VALCOPYCONS(int_u4); break;
    case 'x': VALCOPYCONS(int_8);  break;
    case 'X': VALCOPYCONS(int_u8); break;
    case 'b': VALCOPYCONS(bool);   break;
    case 'f': VALCOPYCONS(real_4); break;
    case 'd': VALCOPYCONS(real_8); break;
    case 'F': VALCOPYCONS(complex_8); break;
    case 'D': VALCOPYCONS(complex_16); break;
    case 'a': VALCOPYCONS(Str);    break;
    case 't': VALCOPYCONS(Tab);    break;
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': VALCOPYCONS(Val);    break;
    default: unknownType_("copy constructor", subtype);
    }
  }
  case 'Z': break; // stay uninitialized
  default: unknownType_("copy constructor", tag);
  }
}

// For all the "convert out" operations: Since they are so similar,
// use macros to generate the code to avoid multiple maintenance.

// Code block to get the proper union member and cast it out correctly
#define VALSWITCHME(T,M) switch(tag) { case's':return T(u.s);\
case'S':return T(u.S);case'i':return T(u.i);case'I':return T(u.I);\
case'l':return T(u.l);case'L':return T(u.L);case'x':return T(u.x);case'X':return T(u.X);\
case 'b':return T(u.b); case'f':return T(u.f);case'd':return T(u.d); case'F':return T(M(u.F)); case'D':return T(M(u.D));}

#define VALSWITCHME2(T) switch(tag) { case's':return T(u.s);\
case'S':return T(u.S);case'i':return T(u.i);case'I':return T(u.I);\
case'l':return T(u.l);case'L':return T(u.L);case'x':return T(u.x);case'X':return T(u.X);\
case 'b':return T(u.b); case'f':return T(u.f);case'd':return T(u.d); case'F':return T(u.F.re, u.F.im); case'D':return T(u.D.re, u.D.im);}

// Function tempplate for convert outs for the numeric types
#define CREATEVALOPBODY(T) \
{ if (tag=='a') { Str* sp=(Str*)&u.a;T tmp=0;istrstream is(sp->c_str());is.precision(15);is>>tmp;return tmp;} \
else if (tag=='t'){Tab*tp=(Tab*)&u.t;return T(tp->entries());} else if (tag=='n'){Arr*np=(Arr*)&u.n;return T(np->length());} return T(0);}

#define CREATEVALOP(T) \
OC_INLINE Val::operator T () const { VALSWITCHME(T,mag2); CREATEVALOPBODY(T) }

#define CREATEVALOP2(T) \
OC_INLINE Val::operator T () const { VALSWITCHME2(T); CREATEVALOPBODY(T) }

// CREATEVALOP(int_1)  op>> doesn't seem to work for this?
OC_INLINE Val::operator int_1 () const 
{ 
  VALSWITCHME(int_1,mag2); 
  CREATEVALOPBODY(int_u1);  // Same bits of int_u1 and int_1
}

CREATEVALOP(int_u1)
CREATEVALOP(int_2)
CREATEVALOP(int_u2)
CREATEVALOP(int_4)
CREATEVALOP(int_u4)
CREATEVALOP(int_8)
CREATEVALOP(int_u8)
CREATEVALOP(bool)
CREATEVALOP(real_4)
CREATEVALOP(real_8)

CREATEVALOP2(complex_8)
CREATEVALOP2(complex_16)

OC_INLINE Val::operator Str () const
{
  if (tag=='a') {
    Str* sp = (Str*)&u.a;
    return *sp;
  } else {
    return Stringize(*this);
  }
}

OC_INLINE Val::operator Tab& () const
{
  if (tag!='t') {
    char s[2]; s[0] = tag; s[1] = '\0';
    Str mesg = "No conversion from:"+Str(*this)+" to table.";
    throw logic_error(mesg.c_str());
  }
  Tab* t= (Tab*)&u.t;
  return *t;
}

OC_INLINE Val::operator Arr& () const
{
  if (tag!='n' && subtype!='Z') {
    char s[2]; s[0] = tag; s[1] = '\0';
    Str mesg = "No conversion from:"+Str(*this)+" to table.";
    throw logic_error(mesg.c_str());
  }
  Arr* t= (Arr*)&u.n;
  return *t;
}


// You might wonder why we instatioate so many copies rather than
// having operator[](const Val& v).  Because this way we can pay most
// cost at compile time ... otherwise, we'd have to do it at runtime
// and slow us down a little more (not much, but every little bit
// helps).

template <class LOOKUP> Val& ValLookUpBody_ (const Val& v, LOOKUP ii)
{ 
  if (v.tag=='n') {
    if (v.subtype=='Z') { // For Array<Val>, we can return a reference
      OCArray<Val>& a = v;
      return a[ii];
    }
    // ... but for Arrays of other types, can't return a reference:
    // Force someone to get a contig array, thus force an exception  
    throw logic_error("Only Array<Val> can subscript, Array<T> cannot");
  } else { // Note that this will throw an exception if v is NOT a Tab
    Tab& t = v;
    return t[ii];
  }
}

#if defined(OC_FACTOR_INTO_H_AND_CC)
// Explicit instantiation
#define OC_INST_VALLOOKUP(T) template Val& ValLookUpBody_<T>(const Val&, T);
OC_INST_VALLOOKUP(int_1) 
OC_INST_VALLOOKUP(int_u1) 
OC_INST_VALLOOKUP(int_2) 
OC_INST_VALLOOKUP(int_u2) 
OC_INST_VALLOOKUP(int_4) 
OC_INST_VALLOOKUP(int_u4) 
OC_INST_VALLOOKUP(int_8) 
OC_INST_VALLOOKUP(int_u8) 
OC_INST_VALLOOKUP(bool) 
#endif


// Tab and Arr lookups
OC_INLINE Val& Val::operator[] (int_1  ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_u1 ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_2  ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_u2 ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_4  ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_u4 ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_8  ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (int_u8 ii) { return ValLookUpBody_(*this,ii);}
OC_INLINE Val& Val::operator[] (bool ii)   { return ValLookUpBody_(*this,ii);}

// Tab only lookups
OC_INLINE Val& Val::operator[] (real_4 v) { Tab& t = *this; return t[v]; }
OC_INLINE Val& Val::operator[] (real_8 v) { Tab& t = *this; return t[v]; }
//OC_INLINE Val& Val::operator[] (complex_8 v) { Tab& t=*this; return t[v]; }
//OC_INLINE Val& Val::operator[] (complex_16 v) { Tab& t=*this; return t[v]; }
OC_INLINE Val& Val::operator[] (const char* cc) { Tab& t=*this;return t[cc]; }
OC_INLINE Val& Val::operator[] (const Str& s)   { Tab& t=*this;return t[s]; }


template <class LOOKUP> Val& ValLookUpBodyThrow_ (const Val& v, LOOKUP ii)
{ 
  if (v.tag=='n') {
    if (v.subtype=='Z') { // For Array<Val>, we can return a reference
      OCArray<Val>& a = v;
      return a[ii];  // Possibly should use (), but makes more sense to use the version that throws an exception if there's a problem
    }
    // ... but for Arrays of other types, can't return a reference:
    // Force someone to get a contig array, thus force an exception  
    throw logic_error("Only Array<Val> can subscript, Array<T> cannot");
  } else { // Note that this will throw an exception if v is NOT a Tab
    Tab& t = v;
    return t(ii);
  }
}

#if defined(OC_FACTOR_INTO_H_AND_CC)
// Explicit instantiation
#define OC_INST_VALLOOKUP_THROW(T) template Val& ValLookUpBodyThrow_<T>(const Val&, T);
OC_INST_VALLOOKUP_THROW(int_1) 
OC_INST_VALLOOKUP_THROW(int_u1) 
OC_INST_VALLOOKUP_THROW(int_2) 
OC_INST_VALLOOKUP_THROW(int_u2) 
OC_INST_VALLOOKUP_THROW(int_4) 
OC_INST_VALLOOKUP_THROW(int_u4) 
OC_INST_VALLOOKUP_THROW(int_8) 
OC_INST_VALLOOKUP_THROW(int_u8) 
OC_INST_VALLOOKUP_THROW(bool) 
#endif


// Tab and Arr lookups
OC_INLINE Val& Val::operator() (int_1  ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_u1 ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_2  ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_u2 ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_4  ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_u4 ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_8  ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (int_u8 ii) { return ValLookUpBodyThrow_(*this,ii);}
OC_INLINE Val& Val::operator() (bool ii)   { return ValLookUpBodyThrow_(*this,ii);}

// Tab only lookups
OC_INLINE Val& Val::operator() (real_4 v) { Tab& t = *this; return t(v); }
OC_INLINE Val& Val::operator() (real_8 v) { Tab& t = *this; return t(v); }
//OC_INLINE Val& Val::operator() (complex_8 v) { Tab& t=*this; return t(v); }
//OC_INLINE Val& Val::operator() (complex_16 v) { Tab& t=*this; return t(v); }
OC_INLINE Val& Val::operator() (const char* cc) { Tab& t=*this;return t(cc); }
OC_INLINE Val& Val::operator() (const Str& s)   { Tab& t=*this;return t(s); }



#define OC_IS_CX(v)   ((((v.tag) ^ 0x40)>>3)==0)       // FD
#define OC_IS_REAL(v) ((((v.tag) ^ 0x60)& 0xfc)==0x04) //'fd'
static char is_int_type[16] = { 0,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0 };
#define OC_TAG_SOME_INT(tag) is_int_type[(((tag)>>6)^(tag))&0x0f] // silxSILX
#define OC_IS_INT(v)  (OC_TAG_SOME_INT(v.tag)&((v.tag)>0x60)) // silx
#define OC_IS_UINT(v) (OC_TAG_SOME_INT(v.tag)&((v.tag)<0x60)) // SILX
#define OC_IS_NUMERIC(v) !(v.tag=='a' | v.tag=='t'| v.tag=='n')

#define ARRAYEQUALITY(T) { OCArray<T>*vp1,*vp2;vp1=(OCArray<T>*)&v1.u.n;vp2=(OCArray<T>*)&v2.u.n; return *vp1==*vp2; }
OC_INLINE bool same_type_equal (const Val& v1, const Val& v2)
{ 
  if (v1.tag != v2.tag) return false; 
  switch (v1.tag) {
  case 's': return v1.u.s==v2.u.s;
  case 'S': return v1.u.S==v2.u.S;
  case 'i': return v1.u.i==v2.u.i;
  case 'I': return v1.u.I==v2.u.I;
  case 'l': return v1.u.l==v2.u.l;
  case 'L': return v1.u.L==v2.u.L;
  case 'x': return v1.u.x==v2.u.x;
  case 'X': return v1.u.X==v2.u.X;
  case 'b': return v1.u.b==v2.u.b;
  case 'f': return v1.u.f==v2.u.f;
  case 'd': return v1.u.d==v2.u.d;
  case 'F': return v1.u.F==v2.u.F;
  case 'D': return v1.u.D==v2.u.D;
  case 'a': { Str* s1=(Str*)&v1.u.a; Str* s2=(Str*)&v2.u.a; return *s1==*s2; }
  case 't': { Tab* t1=(Tab*)&v1.u.t; Tab* t2=(Tab*)&v2.u.t; return *t1==*t2; }
  case 'n': {
    // Two arrays of different types always have to compare (< and ==)
    // as strings so they are totally-ordered.
    if (v1.subtype != v2.subtype) return Str(v1)==Str(v2); // false; 
    switch(v1.subtype) { 
    case 's': ARRAYEQUALITY(int_1);  
    case 'S': ARRAYEQUALITY(int_u1); 
    case 'i': ARRAYEQUALITY(int_2);  
    case 'I': ARRAYEQUALITY(int_u2); 
    case 'l': ARRAYEQUALITY(int_4);  
    case 'L': ARRAYEQUALITY(int_u4); 
    case 'x': ARRAYEQUALITY(int_8);  
    case 'X': ARRAYEQUALITY(int_u8); 
    case 'b': ARRAYEQUALITY(bool); 
    case 'f': ARRAYEQUALITY(real_4); 
    case 'd': ARRAYEQUALITY(real_8); 
    case 'F': ARRAYEQUALITY(complex_8); 
    case 'D': ARRAYEQUALITY(complex_16); 
    case 'a': ARRAYEQUALITY(Str);    
    case 't': ARRAYEQUALITY(Tab);    
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': ARRAYEQUALITY(Val);    
    default:  unknownType_("operator==", v1.subtype);
    }
  }
  case 'Z': return true;
  default:  unknownType_("operator==", v1.tag);
  }
  return false;
}


OC_INLINE bool operator== (const Val& v1, const Val& v2)
{
  if (v1.tag==v2.tag) 
    return same_type_equal(v1,v2);

  // If they are both Numeric types, and they are equal when cast up
  // to a bigger type, then they are equal.  If they are different
  // types and NOT Numeric types, they are not equal.  Note that two
  // arrays of different types will NOT be equal BUT two arrays of
  // Vals can be equal if their Numeric types are the same.  BTW, this
  // is basically how Python works: we are doing what they do.
  if (OC_IS_INT(v1)) {         // int types
    if      (OC_IS_INT(v2))    { return int_8(v1)==int_8(v2); }
    else if (OC_IS_UINT(v2))   { return int_8(v1)==int_8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)==real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (v2.tag=='b')      { return int_8(v1)==int_8(v2); }
    else                       { return false; }
  } else if (OC_IS_UINT(v1)) { // unsigned int types
    if      (OC_IS_INT(v2))    { return int_8(v1)==int_8(v2); }
    else if (OC_IS_UINT(v2))   { return int_u8(v1)==int_u8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)==real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (v2.tag=='b')      { return int_u8(v1)==int_u8(v2); }
    else                       { return false; }
  } else if (OC_IS_REAL(v1)) { // real types
    if      (OC_IS_INT(v2))    { return real_8(v1)==real_8(v2); }
    else if (OC_IS_UINT(v2))   { return real_8(v1)==real_8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)==real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (v2.tag=='b')      { return int_8(v1)==int_8(v2); }
    else                       { return false; }
  } else if (OC_IS_CX(v1)) {   // cx types
    if      (OC_IS_INT(v2))    { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (OC_IS_UINT(v2))   { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (OC_IS_REAL(v2))   { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else if (v2.tag=='b')      { complex_16 t1=v1; complex_16 t2=v2; return t1==t2; }
    else                       { return false; }
  } else if (v1.tag=='b') {    // bool types
    if      (OC_IS_INT(v2))    { return bool(v1)==bool(v2); }
    else if (OC_IS_UINT(v2))   { return bool(v1)==bool(v2); }
    else if (OC_IS_REAL(v2))   { return bool(v1)==bool(v2); }
    else if (OC_IS_CX(v2))     { return bool(v1)==bool(v2); }
    else if (v2.tag=='b')      { return bool(v1)==bool(v2); }
    else                       { return false; }
  } else {
    return false; 
  }
}


#define ARRAYLESSTHAN(T) { OCArray<T>*vp1,*vp2;vp1=(OCArray<T>*)&v1.u.n;vp2=(OCArray<T>*)&v2.u.n; return *vp1<*vp2; }
OC_INLINE bool same_type_lt (const Val& v1, const Val& v2)
{   
  if (v1.tag != v2.tag) return Str(v1)<Str(v2); // false;
  switch (v1.tag) {
  case 's': return v1.u.s<v2.u.s;
  case 'S': return v1.u.S<v2.u.S;
  case 'i': return v1.u.i<v2.u.i;
  case 'I': return v1.u.I<v2.u.I;
  case 'l': return v1.u.l<v2.u.l;
  case 'L': return v1.u.L<v2.u.L;
  case 'x': return v1.u.x<v2.u.x;
  case 'X': return v1.u.X<v2.u.X;
  case 'b': return v1.u.b<v2.u.b;
  case 'f': return v1.u.f<v2.u.f;
  case 'd': return v1.u.d<v2.u.d;
  case 'F': return complex_8(v1.u.F.re,v1.u.F.im) < 
	      complex_8(v2.u.F.re,v2.u.F.im);
  case 'D': return complex_16(v1.u.D.re,v1.u.D.im) < 
	      complex_16(v2.u.D.re,v2.u.D.im);
  case 'a': { Str* s1=(Str*)&v1.u.a; Str* s2=(Str*)&v2.u.a; return *s1<*s2; }
  case 't': { Tab* t1=(Tab*)&v1.u.t; Tab* t2=(Tab*)&v2.u.t; return *t1<*t2; }
  case 'n': {
    if (v1.subtype != v2.subtype) return Str(v1)<Str(v2); // false;
    switch(v1.subtype) { 
    case 's': ARRAYLESSTHAN(int_1);  
    case 'S': ARRAYLESSTHAN(int_u1); 
    case 'i': ARRAYLESSTHAN(int_2);  
    case 'I': ARRAYLESSTHAN(int_u2); 
    case 'l': ARRAYLESSTHAN(int_4);  
    case 'L': ARRAYLESSTHAN(int_u4); 
    case 'x': ARRAYLESSTHAN(int_8);  
    case 'X': ARRAYLESSTHAN(int_u8); 
    case 'b': ARRAYLESSTHAN(bool); 
    case 'f': ARRAYLESSTHAN(real_4); 
    case 'd': ARRAYLESSTHAN(real_8); 
    case 'F': ARRAYLESSTHAN(complex_8); 
    case 'D': ARRAYLESSTHAN(complex_16); 
    case 'a': ARRAYLESSTHAN(Str);    
    case 't': ARRAYLESSTHAN(Tab);    
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': ARRAYLESSTHAN(Val);    
    default:  unknownType_("operator<", v1.subtype);
    }
  }
  case 'Z': return false;
  default:  unknownType_("operator<", v1.tag);
  }
  return false;
}

OC_INLINE bool operator< (const Val& v1, const Val& v2)
{
  if (v1.tag == v2.tag) 
    return same_type_lt(v1,v2);
  else if (v1.tag == 'Z') 
    return true;
  else if (v2.tag == 'Z') // None is less than everything
    return false;

  // Numeric types are always less than string types, which is what
  // things get converted to if they have no obvious comparison.
  // This seems to be what Python does.
  if (OC_IS_INT(v1)) {         // int types
    if      (OC_IS_INT(v2))    { return int_8(v1)<int_8(v2); }
    else if (OC_IS_UINT(v2))   { return int_8(v1)<int_8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)<real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (v2.tag=='b')      { return int_8(v1)<int_8(v2); }
    else                       { return true; }
  } else if (OC_IS_UINT(v1)) { // unsigned int types
    if      (OC_IS_INT(v2))    { return int_8(v1)<int_8(v2); }
    else if (OC_IS_UINT(v2))   { return int_u8(v1)<int_u8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)<real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (v2.tag=='b')      { return int_u8(v1)<int_u8(v2); }
    else                       { return true; }
  } else if (OC_IS_REAL(v1)) { // real types
    if      (OC_IS_INT(v2))    { return real_8(v1)<real_8(v2); }
    else if (OC_IS_UINT(v2))   { return real_8(v1)<real_8(v2); }
    else if (OC_IS_REAL(v2))   { return real_8(v1)<real_8(v2); }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (v2.tag=='b')      { return real_8(v1)<real_8(v2); }
    else                       { return true; }
  } else if (OC_IS_CX(v1)) {   // cx types
    if      (OC_IS_INT(v2))    { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (OC_IS_UINT(v2))   { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (OC_IS_REAL(v2))   { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (OC_IS_CX(v2))     { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else if (v2.tag=='b')      { complex_16 t1=v1; complex_16 t2=v2; return t1<t2; }
    else                       { return true; }
  } else if (v1.tag == 'b') { // bool
    if      (OC_IS_INT(v2))    { return bool(v1)<bool(v2); }
    else if (OC_IS_UINT(v2))   { return bool(v1)<bool(v2); }
    else if (OC_IS_REAL(v2))   { return bool(v1)<bool(v2); }
    else if (OC_IS_CX(v2))     { return bool(v1)<bool(v2); }
    else if (v2.tag=='b')      { return bool(v1)<bool(v2); }
    else                       { return true; }
  } else if (OC_IS_NUMERIC(v2)) {
    return false; // Numeric types always less-than string types
  } else {
    return Str(v1)<Str(v2);  // Compare as string types if no obvious compare
  }
}

#include "ocstringtools.h"


// Make Array<T> look like Python Numeric arrays
template <class T> 
OC_INLINE ostream& PrintArray (ostream& os, const OCArray<T>& a)
{
  const int len = a.length();
  os << "array([";
  for (int ii=0; ii<len-1; ++ii) {
    os << Val(a[ii]) << " ";
  }
  if (len) os << Val(a[len-1]);
  return os << "])";
}

// Specialization for Array<Val>: Make them look like pythons lists
template <>
OC_INLINE ostream& PrintArray <Val> (ostream& os, const OCArray<Val>& a)
{
  const int len = a.length();
  os << "[";
  for (int ii=0; ii<len-1; ii++) {
    os << a[ii] << ", ";
  }
  if (len)  {
    os << a[len-1];
  }
  return os << "]";
}

// Specialization for Array<Val>: Make them look like pythons lists
template <>
OC_INLINE ostream& operator<< <Val> (ostream& os, const OCArray<Val>& a)
{ return PrintArray(os,a); }

// Make this look similar to python
#define PRINTARRAY(T) {OCArray<T>*ap=(OCArray<T>*)&v.u.n; return PrintArray(os, *ap);}
OC_INLINE ostream& operator<< (ostream& os, const Val& v)
{
  switch(v.tag) {
  case 's': os << int_4(v.u.s); break;  // because python doesn't have char
  case 'S': os << int_u4(v.u.S); break; // because python doesn't have char
  case 'i': os << v.u.i; break;
  case 'I': os << v.u.I; break;
  case 'l': os << v.u.l; break;
  case 'L': os << v.u.L; break;
  case 'x': os << v.u.x; break;
  case 'X': os << v.u.X; break;
  case 'b': { Str res = "False"; if (v.u.b) res="True"; os << res; break; }
  case 'f': os.precision(7);  os << v.u.f; break;
  case 'd': os.precision(15); os << v.u.d; break;
#if defined(OC_SUPPORT_XM)
  case 'F': os.precision(7);  os<<"("<<v.u.F.re<<((v.u.F.im<0)?"":"+")<<v.u.F.im<<"j)"; break;
  case 'D': os.precision(15); os<<"("<<v.u.D.re<<((v.u.D.im<0)?"":"+")<<v.u.D.im<<"j)"; break;
#else
  case 'F': os << complex_8(v.u.F.re, v.u.F.im); break;
  case 'D': os << complex_16(v.u.D.re, v.u.D.im); break;
#endif
  case 'a': { Str *s = (Str*)&v.u.a; os << Image(*s, true); break; }
  case 't': { Tab *t = (Tab*)&v.u.t; os << *t; break; }
  case 'n': { 
    switch(v.subtype) { 
    case 's': PRINTARRAY(int_1);  
    case 'S': PRINTARRAY(int_u1); 
    case 'i': PRINTARRAY(int_2);  
    case 'I': PRINTARRAY(int_u2); 
    case 'l': PRINTARRAY(int_4);  
    case 'L': PRINTARRAY(int_u4); 
    case 'x': PRINTARRAY(int_8);  
    case 'X': PRINTARRAY(int_u8); 
    case 'b': PRINTARRAY(bool);   
    case 'f': PRINTARRAY(real_4); 
    case 'd': PRINTARRAY(real_8); 
    case 'F': PRINTARRAY(complex_8); 
    case 'D': PRINTARRAY(complex_16); 
    case 'a': PRINTARRAY(Str);    
    case 't': PRINTARRAY(Tab);    
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': PRINTARRAY(Val);    
    default:  unknownType_("operator<<", v.subtype);
    }
  }
  case 'Z': os << "None"; break;
  default: unknownType_("operator<<", v.tag);
  }
  return os;
}

// Make this look like pythons dictionaries
OC_INLINE ostream& operator<< (ostream& os, const Tab& t)
{
  os << "{";
  for (It it(t); it(); ) {
    // os << it.key() << ": " << it.value();
    const Val& key = it.key();
    const Val& value = it.value();
    os << key << ": " << value;
 
    // See if we want to output a comma
    It next(it); 
    if (next()) { 
      os << ", ";
    }
  }
  return os << "}";
}

// Due to crazy include dependencies, we do this last.
#include "ocvalreader.h"
OC_INLINE Tab::Tab (const char* cc) {ValReader r(cc); r.expectTab(*this);} 
OC_INLINE Tab::Tab (const Str& s) {ValReader r(s.c_str());r.expectTab(*this);}
OC_INLINE Tab::Tab () { }

OC_INLINE void Tab::append (const Val& v) 
{ 
  Val key=entries(); 
  this->insertKeyAndValue(key,v); // crappy dependent names 
}

OC_INLINE void Tab::appendStr (const Val& v) 
{ 
  Val key=Stringize(entries()); 
  this->insertKeyAndValue(key,v); // crappy dependent names 
}

OC_INLINE Tab& Tab::operator+= (const Tab& rhs)
{ 
  for (It ii(rhs); ii();) { 
    this->insertKeyAndValue(ii.key(), ii.value()); 
  }
  return *this; 
}

OC_INLINE ostream& Tab::prettyPrintHelper_ (ostream& os, int indent, 
					    bool pretty) const
{
  // Base case, empty table
  if (entries()==0) return os << "{ }"; 

  // Recursive case
  os << "{";
  if (pretty) os << endl;

  // Iterate through, printing out each element
  Sit sii(*this);
  for (int ii=0; sii(); ii++) {
    const Val& key = sii.key();
    const Val& value = sii.value();
    
    if (pretty) indentOut_(os, indent+4);
    os << key << ":";
    
    // For most values, use default output method
    switch (value.tag) {
    case 'a': { 
      Str* ap = (Str*)&value.u.a; 
      os << Image(*ap, true);
      break; 
    }
    case 't': { 
      Tab* tp = (Tab*)&value.u.t; 
      tp->prettyPrintHelper_(os, pretty ? indent+4 : 0, pretty);
      break; 
    }
    case 'n': { 
      if (value.subtype=='Z') {
	Arr* np = (Arr*)&value.u.n;
	np->prettyPrintHelper_(os, pretty ? indent+4 : 0, pretty);
	break;
      } // else fall thru for other array types
    }
    default: os << value; break;
    }

    if (entries()>1 && ii!=entries()-1) os << ","; // commas on all but last
    if (pretty) os << endl;
  }

  if (pretty) indentOut_(os, indent);
  return os << "}";
}

OC_INLINE void Tab::prettyPrint (ostream& os, int indent) const
{
  indentOut_(os, indent);
  prettyPrintHelper_(os, indent, true) << endl;
}


OC_INLINE Arr::Arr (const char* cc) {ValReader r(cc); r.expectArr(*this);} 
OC_INLINE Arr::Arr (const Str& s) {ValReader r(s.c_str());r.expectArr(*this);}
OC_INLINE Arr::Arr () { }
OC_INLINE Arr::Arr (int len) : OCArray<Val>(len) { }

OC_INLINE ostream& Arr::prettyPrintHelper_ (ostream& os, int indent, 
					    bool pretty) const
{
  // Base case, empty 
  if (entries()==0) return os << "[ ]"; 

  // Recursive case
  os << "[";
  if (pretty) os << endl;

  // Iterate through, printing out each element
  for (unsigned int ii=0; ii<entries(); ++ii) {
    const Val& value = (*this)[ii];
    
    if (pretty) indentOut_(os, indent+4);
    
    // For most values, use default output method
    switch (value.tag) {
    case 'a': { 
      Str* ap = (Str*)&value.u.a; 
      os << Image(*ap, true);
      break; 
    }
    case 't': { 
      Tab* tp = (Tab*)&value.u.t; 
      tp->prettyPrintHelper_(os, pretty ? indent+4 : 0, pretty);
      break; 
    }
    case 'n': {
      if (value.subtype=='Z') {
	Arr* np = (Arr*)&value.u.n;
	np->prettyPrintHelper_(os, pretty ? indent+4 : 0, pretty);
	break;
      } // else fall thru for other array types
    }
    default: os << value; break;
    }
    
    if (entries()>1 && ii!=entries()-1) os << ","; // commas on all but last
    if (pretty) os << endl;
  }

  if (pretty) indentOut_(os, indent);
  return os << "]";
}

OC_INLINE void Arr::prettyPrint (ostream& os, int indent) const
{
  indentOut_(os, indent);
  prettyPrintHelper_(os, indent, true) << endl;
}

OC_INLINE void Val::swap (Val& rhs)
{ 
  ::swap(this->tag,     rhs.tag);
  ::swap(this->subtype, rhs.subtype);
  ::swap(this->u,       rhs.u); 
}


OC_INLINE void Val::prettyPrint (ostream& os, int indent) const
{
  if (tag=='t') {
    Tab* tp = (Tab*)&(u.t);
    tp->prettyPrint(os, indent);
  } else if (tag=='n' && subtype=='Z') {
    Arr* np = (Arr*)&(u.a);
    np->prettyPrint(os, indent);
  } else {
    os << (*this);
  }
}


#define OC_APPEND(T) {OCArray<T>*np=(OCArray<T>*)&u.n;np->append(v); break;}
OC_INLINE void Val::append (const Val& v) 
{ 
  if (tag=='t') {
    Tab* tp = (Tab*)&(u.t);
    tp->append(v);
  } else if (tag=='n') {
    switch (subtype) {
    case 's': OC_APPEND(int_1);  
    case 'S': OC_APPEND(int_u1); 
    case 'i': OC_APPEND(int_2);  
    case 'I': OC_APPEND(int_u2); 
    case 'l': OC_APPEND(int_4);  
    case 'L': OC_APPEND(int_u4); 
    case 'x': OC_APPEND(int_8);  
    case 'X': OC_APPEND(int_u8); 
    case 'b': OC_APPEND(bool);   
    case 'f': OC_APPEND(real_4); 
    case 'd': OC_APPEND(real_8); 
    case 'F': OC_APPEND(complex_8); 
    case 'D': OC_APPEND(complex_16); 
    case 'a': OC_APPEND(Str);    
    case 't': OC_APPEND(Tab);    
    case 'n': throw logic_error("Arrays of Arrays not currently supported");
    case 'Z': OC_APPEND(Val);    
    default:  unknownType_("append", subtype);
    }
  } else {
    throw logic_error("Only Array<T>, Arr and Tab can append");
  }
}

OC_INLINE void Val::appendStr (const Val& v)
{
  if (tag=='t') {
    Tab* tp = (Tab*)&(u.t);
    tp->appendStr(v);
  } else {
    append(v); // call the normal append
  }
}

