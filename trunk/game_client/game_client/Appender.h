#ifndef __APPENDER_H
#define __APPENDER_H



namespace ROG {

	class Appender {
	public:
		virtual void init(const char* appenderName) = 0;
		virtual void append(const char* msg) = 0;
		virtual void close() = 0;
	};

}


#endif

