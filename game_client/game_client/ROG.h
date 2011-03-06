
#include "SecureHandshake.h" // Include header for secure handshake

#include "FatalException.h"

#include "GameClient.h"
#include "base64.h"

#include "Logger.h"
#include "Config.h"

#include "AbstractApplication.h"

#include "User.pb.h"

const void ROGInit();
const void ROGDestroy();

namespace ROG {

	class LogConfigurator : public LogCreatorListener {
		Object<ConfigLoader> config;

	public:

		LogConfigurator(Object<ConfigLoader> config) {
			this->config = config;
		}

		LogLevel getDefaultLogLevel(const char* newLogger);
	};

}


#ifdef DEBUG
	#pragma comment(lib, "libprotobuf.lib")
	#pragma comment(lib, "libprotoc.lib")
#else
	#pragma comment(lib, "release/libprotobuf.lib")
	#pragma comment(lib, "release/libprotoc.lib")
#endif

