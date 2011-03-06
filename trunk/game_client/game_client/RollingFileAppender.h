
#ifndef __ROLLINGFILEAPPENDER_H
#define __ROLLINGFILEAPPENDER_H


#include <iostream>
#include <fstream>

#include "Appender.h"
#include "Semaphore.h"

#pragma warning(disable : 4812)
#pragma warning(disable : 4244)
#pragma warning(disable : 4554)
#pragma warning(disable : 4018)
#pragma warning(disable : 4800)

#define OC_NEW_STYLE_INCLUDES
#define OC_BYTES_IN_POINTER 4
#include "opencontainers/opencontainers.h"

#pragma warning(default : 4812)
#pragma warning(default : 4244)
#pragma warning(default : 4554)
#pragma warning(default : 4018)
#pragma warning(default : 4800)

namespace ROG {

	class RollingFileAppender : public Appender {

	private:

		static OCArray<const char*> messageQueue;
		static Semaphore queueSemaphore;

		static bool isConfigured;

		static std::ofstream* outputFile;

	public:

		RollingFileAppender() {
		}

		~RollingFileAppender() {
			queueSemaphore.lock();
			if (outputFile != NULL) {
				outputFile->close();
				delete outputFile;
				outputFile = NULL;
				isConfigured = false;
			}
			queueSemaphore.unlock();
		}
	
		void init(const char* appenderName) {
			//setup the file handle if it hasnt been already
			queueSemaphore.lock();
			if (!isConfigured) {
				outputFile = new ofstream("rog.log", std::ios_base::app);
				//(*outputFile) << std::endl << std::endl;

				isConfigured = true;
			}
			queueSemaphore.unlock();
		}


		void append(const char* msg) {
			queueSemaphore.lock();
			if (isConfigured) {
				(*outputFile) << msg << std::endl;
				std::cout << msg << std::endl;
			}

			queueSemaphore.unlock();
		}


		void close() {

		}

		static void cleanup() {
			messageQueue.clear();
			outputFile->close();
		}
	};


	std::ofstream* RollingFileAppender::outputFile = NULL;
	bool RollingFileAppender::isConfigured = false;
	Semaphore RollingFileAppender::queueSemaphore;
	OCArray<const char*> RollingFileAppender::messageQueue;															

};

#endif

