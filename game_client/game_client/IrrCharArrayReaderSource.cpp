
#include "IrrCharArrayReaderSource.h"


#include <cstring>
#include <algorithm>

using namespace ROG;

IrrCharArrayReaderSource::IrrCharArrayReaderSource(const char* source, const int len){
	this->sourceBuffer = currentPtr = (char*)source;
	this->sourceBufferLen = len;
	this->consumed = 0;
}

long IrrCharArrayReaderSource::getSize() const {
	return sourceBufferLen - consumed;
}

int IrrCharArrayReaderSource::read(void * buffer,	int sizeToRead) {
	int size = min(getSize(), sizeToRead);
	memcpy(buffer, this->currentPtr, size);

	this->consumed += size;
	this->currentPtr = this->sourceBuffer + this->consumed;

	return size;
}



