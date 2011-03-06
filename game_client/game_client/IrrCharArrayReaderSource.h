
#ifndef __IRRCHARARRAYREADERSOURCE_H
#define __IRRCHARARRAYREADERSOURCE_H


#include <irrXML.h>

#include "Reference.h"


namespace ROG {

	class IrrCharArrayReaderSource : public irr::io::IFileReadCallBack, public Reference {
	private:
		char* sourceBuffer;
		int sourceBufferLen;
		char* currentPtr;
		int consumed;

	public:

		IrrCharArrayReaderSource(const char* source, const int len);

		long getSize() const;

		int read(void * buffer,	int sizeToRead);
	};

}



#endif

