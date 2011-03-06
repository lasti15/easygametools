
#if defined(OC_FACTOR_INTO_H_AND_CC)
# include "ocserialize.h"
  // Explicit instantiations
#define OC_INST_BTS(T) template size_t BytesToSerialize<T>(const OCArray<T>& a);

OC_INST_BTS(int_1)
OC_INST_BTS(int_u1)
OC_INST_BTS(int_2)
OC_INST_BTS(int_u2)
OC_INST_BTS(int_4)
OC_INST_BTS(int_u4)
OC_INST_BTS(int_8)
OC_INST_BTS(int_u8)
OC_INST_BTS(bool)
OC_INST_BTS(real_4)
OC_INST_BTS(real_8)
OC_INST_BTS(complex_8)
OC_INST_BTS(complex_16)

#define OC_INST_SER(T) template char* Serialize<T>(const OCArray<T>&, char*);

OC_INST_SER(int_1)
OC_INST_SER(int_u1)
OC_INST_SER(int_2)
OC_INST_SER(int_u2)
OC_INST_SER(int_4)
OC_INST_SER(int_u4)
OC_INST_SER(int_8)
OC_INST_SER(int_u8)
OC_INST_SER(bool)
OC_INST_SER(real_4)
OC_INST_SER(real_8)
OC_INST_SER(complex_8)
OC_INST_SER(complex_16)

#endif

#define VALCOMPUTE(T,N) bytes+=sizeof(T)
#define VALARRCOMPUTE(T) {OCArray<T>*ap=(OCArray<T>*)&v.u.n;bytes=BytesToSerialize(*ap);}
OC_INLINE size_t BytesToSerialize (const Val& v) 
{ 
  size_t bytes = 1; // At least one for tag
  switch(v.tag) {
  case 's': VALCOMPUTE(int_1,  v.u.s); break;
  case 'S': VALCOMPUTE(int_u1, v.u.S); break;
  case 'i': VALCOMPUTE(int_2,  v.u.i); break;
  case 'I': VALCOMPUTE(int_u2, v.u.I); break;
  case 'l': VALCOMPUTE(int_4,  v.u.l); break;
  case 'L': VALCOMPUTE(int_u4, v.u.L); break;
  case 'x': VALCOMPUTE(int_8,  v.u.x); break;
  case 'X': VALCOMPUTE(int_u8, v.u.X); break;
  case 'b': VALCOMPUTE(bool,   v.u.b); break;
  case 'f': VALCOMPUTE(real_4, v.u.f); break;
  case 'd': VALCOMPUTE(real_8, v.u.d); break;
  case 'F': VALCOMPUTE(complex_8, v.u.F); break;
  case 'D': VALCOMPUTE(complex_16, v.u.D); break;
  case 'a': { Str*sp=(Str*)&v.u.a; bytes=BytesToSerialize(*sp); break;}
  case 't': { Tab*tp=(Tab*)&v.u.t; bytes=BytesToSerialize(*tp); break;}
  case 'n': { 
    switch (v.subtype) {
    case 's': VALARRCOMPUTE(int_1);  break;
    case 'S': VALARRCOMPUTE(int_u1); break;
    case 'i': VALARRCOMPUTE(int_2);  break;
    case 'I': VALARRCOMPUTE(int_u2); break;
    case 'l': VALARRCOMPUTE(int_4);  break;
    case 'L': VALARRCOMPUTE(int_u4); break;
    case 'x': VALARRCOMPUTE(int_8);  break;
    case 'X': VALARRCOMPUTE(int_u8); break;
    case 'b': VALARRCOMPUTE(bool);   break;
    case 'f': VALARRCOMPUTE(real_4); break;
    case 'd': VALARRCOMPUTE(real_8); break;
    case 'F': VALARRCOMPUTE(complex_8); break;
    case 'D': VALARRCOMPUTE(complex_16); break;
    case 'a': 
    case 't': throw logic_error("Can't have arrays of tables orstrings");
    case 'n': throw logic_error("Can't have arrays of arrays");
    case 'Z': {Arr*ap=(Arr*)&v.u.n;bytes=BytesToSerialize(*ap); break;}
    }
  }
  case 'Z': break; // Just a tag
  default: unknownType_("BytesToSerialize", v.tag);
  }
  return bytes;
}

OC_INLINE size_t BytesToSerialize (const Str& s)
{ return 1 + sizeof(int_u4) + s.length(); } // 'a' tag, then length, then data

OC_INLINE size_t BytesToSerialize (const Tab& t)
{
  // A 't' marker (actually single byte, not the full Val) starts, plus len
  size_t bytes = 1 + sizeof(int_u4);
  // ... then key/value pairs.  When we look for a key and see a None
  // marker, then we know we are at the end of the table.
  for (It ii(t); ii(); ) {
    bytes += BytesToSerialize(ii.key()) + BytesToSerialize(ii.value());
  }
  return bytes;
}


OC_INLINE size_t BytesToSerialize (const Arr& a)
{
  // An 'n' marker (actually single byte, not the full Val) starts,
  // the the subtype (a Z for Vals) 
  // then the length ....
  size_t bytes = 1 + 1 + sizeof(int_u4);
 
  // ... then n vals. 
  const int len = a.length();
  for (int ii=0; ii<len; ii++) { 
    bytes += BytesToSerialize(a[ii]);
  }
  return bytes;
}


template <class T>
OC_INLINE size_t BytesToSerialize (const OCArray<T>& a)
{
  // An 'n' marker (actually single byte, not the full Val) starts,
  // then the subtype of the array, then the length
  size_t bytes = 1 + 1 + sizeof(int_u4);
 
  // ... then n vals. 
  const int len = a.length() * sizeof(T);
  return bytes + len;
}


// Macro for copying into buffer with right types/fields
#define VALCOPY(T,N) { memcpy(mem,&N,sizeof(T)); mem+=sizeof(T); }
#define VALARRCOPY(T) { OCArray<T>*ap=(OCArray<T>*)&v.u.n; mem=Serialize(*ap,mem-1);}
char* Serialize (const Val& v, char* mem)
{
  *mem++ = v.tag; // Always need tag
  switch(v.tag) {
  case 's': VALCOPY(int_1,  v.u.s); break;
  case 'S': VALCOPY(int_u1, v.u.S); break;
  case 'i': VALCOPY(int_2,  v.u.i); break;
  case 'I': VALCOPY(int_u2, v.u.I); break;
  case 'l': VALCOPY(int_4,  v.u.l); break;
  case 'L': VALCOPY(int_u4, v.u.L); break;
  case 'x': VALCOPY(int_8,  v.u.x); break;
  case 'X': VALCOPY(int_u8, v.u.X); break;
  case 'b': VALCOPY(bool, v.u.b); break;
  case 'f': VALCOPY(real_4, v.u.f); break;
  case 'd': VALCOPY(real_8, v.u.d); break;
  case 'F': VALCOPY(complex_8, v.u.F); break;
  case 'D': VALCOPY(complex_16, v.u.D); break;
  case 'a': { Str*sp=(Str*)&v.u.a; mem=Serialize(*sp,mem-1);break; }
  case 't': { Tab*tp=(Tab*)&v.u.t; mem=Serialize(*tp,mem-1);break; }
  case 'n': { 
    switch (v.subtype) {
    case 's': VALARRCOPY(int_1);  break;
    case 'S': VALARRCOPY(int_u1); break;
    case 'i': VALARRCOPY(int_2);  break;
    case 'I': VALARRCOPY(int_u2); break;
    case 'l': VALARRCOPY(int_4);  break;
    case 'L': VALARRCOPY(int_u4); break;
    case 'x': VALARRCOPY(int_8);  break;
    case 'X': VALARRCOPY(int_u8); break;
    case 'b': VALARRCOPY(bool); break;
    case 'f': VALARRCOPY(real_4); break;
    case 'd': VALARRCOPY(real_8); break;
    case 'F': VALARRCOPY(complex_8); break;
    case 'D': VALARRCOPY(complex_16); break;
    case 'a': 
    case 't': throw logic_error("Can't have arrays of strings or tables");
    case 'n': throw logic_error("Can't have arrays of arrays");
    case 'Z': { Arr*ap=(Arr*)&v.u.n;mem=Serialize(*ap,mem-1); break;}
    }
  }
  case 'Z': break;
  default: unknownType_("Serialize into circular buffer", v.tag);
  }
  return mem;  
}

OC_INLINE char* Serialize (const Str& s, char* mem)
{
  *mem++ = 'a'; // Always need tag
  // Strings: 'a', int_u4 length, (length) bytes of data
  const int_u4 len=s.length(); 
  VALCOPY(int_u4, len);
  memcpy(mem, s.c_str(), len);
  mem += len;
  return mem;
}

OC_INLINE char* Serialize (const Tab& t, char* mem)
{
  *mem++ = 't'; // Always need tag
  // Tables: 't', int_u4 length, (length) Key/Value pairs
  const int_u4 len = t.entries();
  VALCOPY(int_u4, len);
  for (It ii(t); ii(); ) {
    const Val& key = ii.key();
    const Val& value = ii.value();
    mem = Serialize(key, mem);
    mem = Serialize(value, mem);
  }
  return mem;
}


// Specialization because Arrays of Vals serialize differently
OC_INLINE char* Serialize (const Arr& a, char* mem)
{
  // Arrays: 'n', subtype, int_u4 length, (length) vals

  *mem++ = 'n'; // Always need tag
  *mem++ = 'Z'; // subtype

  const int_u4 len = a.length();
  VALCOPY(int_u4, len);

  for (int ii=0; ii<len; ii++) {
    mem = Serialize(a[ii], mem);
  }
  return mem;
}


template <class T>
OC_INLINE char* Serialize (const OCArray<T>& a, char* mem)
{
  // Arrays: 'n', subtype, int_u4 length, (length) vals
  *mem++ = 'n'; // Always need tag

  Val sub_type = T();
  *mem++ = sub_type.tag; // subtype

  const int_u4 len = a.length(); // int_u4 len 
  VALCOPY(int_u4, len);

  OCArray<T>& a_non = const_cast<OCArray<T>&>(a);
  const int_u4 byte_len = sizeof(T)*len;
  const T* a_data = a_non.data();

  memcpy(mem, a_data, byte_len);
  mem += byte_len;  

  return mem;
}



#define VALDECOPY(T,N) { memcpy(&N,mem,sizeof(T)); mem+=sizeof(T); }
#define VALDECOPY2(T) { OCArray<T>*ap = (OCArray<T>*)&v.u.n; new (ap) OCArray<T>(len); ap->expandTo(len); memcpy(ap->data(),mem,sizeof(T)*len); mem+=sizeof(T)*len; }
#define VALDECOPY3(T) {OCArray<T>*ap=(OCArray<T>*)&v.u.n;new(ap)OCArray<T>(len); for(int ii=0;ii<len;ii++){ap->append(T());mem=Deserialize((*ap)[ii], mem);}}
#define VALDECOPY4(T) {OCArray<T>*ap=(OCArray<T>*)&v.u.n;new(ap)OCArray<T>(len); for(int ii=0;ii<len;ii++){ap->append(T());Val temp;mem=Deserialize(temp, mem); (*ap)[ii]=temp;}}
char* Deserialize (Val& v, char* mem)
{
  if (v.tag!='Z') { // Don't let anything be serialized EXCEPT empty Val
    throw logic_error("You can only deserialize into an empty Val.");
  }
  
  v.tag = *mem++; // Grab tag 
  switch(v.tag) {
  case 's': VALDECOPY(int_1,  v.u.s); break;
  case 'S': VALDECOPY(int_u1, v.u.S); break;
  case 'i': VALDECOPY(int_2,  v.u.i); break;
  case 'I': VALDECOPY(int_u2, v.u.I); break;
  case 'l': VALDECOPY(int_4,  v.u.l); break;
  case 'L': VALDECOPY(int_u4, v.u.L); break;
  case 'x': VALDECOPY(int_8,  v.u.x); break;
  case 'X': VALDECOPY(int_u8, v.u.X); break;
  case 'b': VALDECOPY(bool,   v.u.b); break;
  case 'f': VALDECOPY(real_4, v.u.f); break;
  case 'd': VALDECOPY(real_8, v.u.d); break;
  case 'F': VALDECOPY(complex_8, v.u.F); break;
  case 'D': VALDECOPY(complex_16, v.u.D); break;
  case 'a':
    { // Strs: 'a' then int_u4 len, then len bytes
      int_u4 len; VALDECOPY(int_u4, len);
      Str* sp = (Str*)&v.u.a; new (sp) Str(mem, len);
      mem += len;
      break;
    } 
  case 't':
    { // Tables: 't', then int_u4 length, then length ley/value pairs
      int_u4 len; VALDECOPY(int_u4, len);
      Tab* tp = (Tab*)&v.u.t; new (tp) Tab(); 
      for (int ii=0; ii<len; ii++) {
	Val key;
	mem = Deserialize(key, mem);    // Get the key
	Val& value = (*tp)[key];        // Insert it with a default Val
	mem = Deserialize(value, mem);  // ... copy in the REAL value
      }
      break;
    }
  case 'n': 
    { // Arrays: 'n', then subtype, then int_u4 len, then number of
      // values following)
      v.subtype = *mem++;
      int_u4 len; VALDECOPY(int_u4, len); 
      switch(v.subtype) {
      case 's': VALDECOPY2(int_1);  break;
      case 'S': VALDECOPY2(int_u1); break;
      case 'i': VALDECOPY2(int_2);  break;
      case 'I': VALDECOPY2(int_u2); break;
      case 'l': VALDECOPY2(int_4);  break;
      case 'L': VALDECOPY2(int_u4); break;
      case 'x': VALDECOPY2(int_8);  break;
      case 'X': VALDECOPY2(int_u8); break;
      case 'b': VALDECOPY2(bool); break;
      case 'f': VALDECOPY2(real_4); break;
      case 'd': VALDECOPY2(real_8); break;
      case 'F': VALDECOPY2(complex_8); break;
      case 'D': VALDECOPY2(complex_16); break;
      case 'a': {
	OCArray<Str>*ap=(OCArray<Str>*)&v.u.n;
	new (ap) OCArray<Str>(len); 
	for(int ii=0;ii<len;ii++) {
	  ap->append("");
	  // Strs: 'a' then int_u4 len, then len bytes
	  int_u4 len; VALDECOPY(int_u4, len);
	  (*ap)[ii] = Str(mem, len);
	  mem += sizeof(Str);
	}
	break;
      }
      case 't': {
	OCArray<Tab>*ap=(OCArray<Tab>*)&v.u.n;
	new (ap) OCArray<Tab>(len); 
	for(int ii=0;ii<len;ii++) {
	  ap->append(Tab());
	  // Tables: 't', then int_u4 length, then length ley/value pairs
	  int_u4 len; VALDECOPY(int_u4, len);
	  Tab& tp = (*ap)[ii]; 
	  for (int ii=0; ii<len; ii++) {
	    Val key;
	    mem = Deserialize(key, mem);    // Get the key
	    Val& value = tp[key];        // Insert it with a default Val
	    mem = Deserialize(value, mem);  // ... copy in the REAL value
	  }
	}
	break;
      }
      case 'n': throw logic_error("Can't have arrays of arrays");
      case 'Z': {
	Arr*ap=(Arr*)&v.u.n;
	new (ap) Arr(len); 
	for(int ii=0; ii<len; ii++) {
	  ap->append(Val());
	  mem=Deserialize((*ap)[ii], mem);
	}
	break;
      }
      default: unknownType_("Deserialize Array", v.subtype);
      }
      break;
    }
  case 'Z': break; // Already copied the tag out, nothing else
  default: unknownType_("Deserialize", v.tag);
  }
  return mem;
}
