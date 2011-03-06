
#include "Config.h"

using namespace ROG;

Semaphore ConfigLoader::instanceSem;
Object<ConfigLoader> ConfigLoader::instance = NULL;
const Logger* ConfigLoader::logger = Logger::getLogger("ConfigLoader");