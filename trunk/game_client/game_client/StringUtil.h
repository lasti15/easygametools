#ifndef __STRINGUTIL_H
#define __STRINGUTIL_H

namespace ROG {

	#define MAX_BUF_SIZE 255

	class StringUtil {
	public:
		//allocates its buffer
		static const char* valueOf(const int i) {
			char* buf = new char[MAX_BUF_SIZE];

			_itoa_s(i, buf, MAX_BUF_SIZE, 10);

			return buf;
		}
	};


}


#endif