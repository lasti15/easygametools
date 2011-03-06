
#ifdef __TEST

//Include the main ROG header
#include "../game_client/ROG.h"

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
	//Make sure you protect you application with a try->catch block for the most evil of exceptions,
	//the ROG::FatalException. Beware! The FatalException is a terrible error in the application. 
	//The system can't recover and it needs to exit ASAP.
	//ROG code is guaranteed to NEVER trap an ROG::FatalException. No Exceptions!
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

#endif