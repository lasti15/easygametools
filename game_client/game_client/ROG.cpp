
#include "ROG.h"

using namespace ROG;



const static void startWinsock() {
	//start up winsock
	WSADATA wsadata;

	int error = WSAStartup(0x0202, &wsadata);

	//Did something happen?

	if (error)
		throw new FatalException("WSAStartup Failed");

	//Did we get the right Winsock version?

	if (wsadata.wVersion != 0x0202)
	{
		WSACleanup(); //Clean up Winsock

		throw new FatalException("Wrong WSA version");
	}
}


LogLevel LogConfigurator::getDefaultLogLevel(const char* newLogger) {

	char nameBuf[255];
	_snprintf(nameBuf, 254, "%s.logLevel", newLogger);

	char* variable = (char*)(config->get(nameBuf));
	if (variable != NULL) {
		LogLevel newLevel = getLogLevel(variable);
		return newLevel;
	}
	variable = (char*)(config->get("logLevel"));
	if (variable != NULL) {
		LogLevel newLevel = getLogLevel(variable);
		return newLevel;
	}
	return LVL_INFO;
}

//configure the specific log levels on different components. 
//Abstract this out of Logger because the config is in a Reference and i took great pains to not have
//any dependencies on reference inside logger
//not thread safe
//if a logger got removed halfway through the operation wed blow up here
const static void configureLoggers(Object<ConfigLoader> config, LogConfigurator* logConfig) {
	OCArray<Logger*>* allLoggers = (OCArray<Logger*>*)Logger::getAllLoggers();
	for (unsigned int i=0; i<allLoggers->length(); i++) {

		Logger* logger = allLoggers->at(i);
		logger->debug("Configuring");
		logger->setLogLevel(logConfig->getDefaultLogLevel(logger->getName()));
	}
}




const void ROGInit() {

	//startWinsock();

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	Reference::Reference_initialize();

	//load the config file
	Object<ConfigLoader> config = ConfigLoader::getConfigLoader();

	LogConfigurator* logConfig = new LogConfigurator(config);
	Logger::setLogCreationListener(logConfig);

	//set the logging configuration
	configureLoggers(config, logConfig);
}


const void ROGDestroy() {
	Thread::CollectRemainingThreads();
	Reference::CollectRemainingObjects();
	Logger::CollectRemainingLoggers();

	google::protobuf::ShutdownProtobufLibrary();

}