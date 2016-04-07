# EasyGameTools #
Easy game tools is not a game, or a game engine itself. I have only written some of the components personally, but I am working on integrating alot of really helpful tools into a platform that is useful for making games.

Current features provided by the C++ library are as follows:
  * Dynamic and reusable memory management
  * Configurable, per-module logging
  * Global XML config
  * Threading and ThreadPools
  * Date/Time handling
  * Semaphores/Mutex
  * Http Client
  * User/Player Directory server
  * Client API to coordinate/communicate with the directory server
  * Simple mesh networking framework (8 players, authenticated against directory server)
  * Server-side orphaned game cleaner

## 3rd Party Technologies ##

**(Game client)**
  * RakNet
  * Irrlicht
  * Protoc
  * OpenContainers

**(Directory Server)**
  * Java SE
  * Servlet
  * Spring
  * Hibernate
  * Mysql
  * Quartz Scheduling
  * AspectJ
  * Maven


## Hello, World! ##
```

//Include the main ROG header
#include "ROG.h"

//use the ROG namespace
using namespace ROG;


//All applications in the framwork extend from AbstractApplication.
//AbstractApplication gives you all the tools and flexibility to build any 
//application you want, but maintains some state information for the system.
//Extend AbstractApplication by implementing the following methods:
//
//AbstractApplication::run()
// -The main application method. This method is yours to keep, so do whatever 
//  you want inside of it. Once you exit this method, you are signalling to the 
//  framework that you are exiting the application.
//
//Refer to the various examples for implementations of this subclass that expose 
//much more functionality.
class TestApp : public AbstractApplication {
private:
	const static Logger* logger;
protected:

	//AbstractApplication implementation.
	//This is the greatest program ever :D
	const void run() {
		logger->info("Hello, World!");
	}


};

//Our nice TestApp logger. Look at rog.log for details.
const Logger* TestApp::logger = Logger::getLogger("TestApp");


//I love main methods
int main(void) {

	//OOh! another logger for us to use! This one has a different name, "Main"
	const Logger* logger = Logger::getLogger("Main");

	//Sometimes ROG can throw some nasty errors. You wouldnt want one to hit your user in the face.
	//Make sure you protect your application with a try->catch block for the most evil of exceptions,
	//the ROG::FatalException. Beware! The FatalException is a terrible error in the application. 
	//The system can't recover and it needs to exit ASAP.
	//ROG Main code is guaranteed to NEVER trap an ROG::FatalException. No Exceptions!
        //If you throw a FatalExcepion inside a threadpool task, we wont kill the entire 
        //thread on you, but we wont do anything useful witht he exception either.
        //It is up to you to make sur eyou check your threads and tasks for errors.
	try {

		logger->info("Starting up");

		//Initialize the framework.
		ROGInit();

		logger->info("Creating Application");

		//Instantiate our application.
		Object<TestApp> app = new TestApp();

		logger->info("Running Application");

		//Start the application.
		app->runGame();

	//Here we catch the infamous FatalException. Dont forget to delete the exception object for us please :)
	} catch (FatalException* ex) {
		//log a suitably sinister error
		logger->error(ex->getMsg());
		//Clean up after whatever messy error occurred.
		delete ex;
	}
	
	logger->info("Shutting down");
	//maximum destructiveness
	ROGDestroy();

	//wait for the user to type a key
	char scanbuf[1000];
	scanf("%s", scanbuf);
	
	//10 is a good return code.
	return 10;
}

```