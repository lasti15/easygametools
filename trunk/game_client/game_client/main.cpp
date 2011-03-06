#ifndef __TEST


#include "ROG.h"



using namespace ROG;


class TestApp : public AbstractApplication {
private:
	const static Logger* logger2;

	Object<GameClient> client;

	void registerClient(const char* username, const char* password) {
		client->startClient(username, password);
		if (client->getError() != ERROR_OK) {
			throw new FatalException("Game Client Error");
		}
	}

	OCArray<Object<ServerGame>> displayGameList() {
		logger2->info("");
		logger2->info("Host List");
		logger2->info("----------");
		OCArray<Object<ServerGame>> gameList = client->getAvailableGames();
		for (unsigned int i=0; i<gameList.length(); i++) {
			Object<ServerGame> game = gameList.at(i);

			char gameInfo[1024];

			char* format = NULL;
			if (client->isCurrentGame(game)) {
				format = "%d)* %-15s | %7d | %-15s | %-5d";
			} else {
				format = "%d) %-15s | %7d | %-15s | %-5d";
			}

			_snprintf(gameInfo, 1023, format, i, game->getHostUser()->getUsername(), game->getGameId(), game->getHostName(), game->getHostPort());
			logger2->info(gameInfo);
		}

		return gameList;
	}

	void hostGame() {
		Object<ClientFuture> gameFuture = client->startGame("localhost", 11223);
		
		FutureCode result = gameFuture->waitFor();
		if (result != FUTURE_OK) {
			throw new FatalException("Error creating game");
		}
	}

	void joinGame(Object<ServerGame> game) {
		Object<ClientFuture> gameFuture = client->joinGame("localhost", 11223, game);
		
		FutureCode result = gameFuture->waitFor();
		if (result != FUTURE_OK) {
			throw new FatalException("Error joining game");
		}
	}

protected:
	const void run() {
		logger2->info("run()");

		client = new GameClient();

		char username[256];
		char password[256];

		char scanbuf[1000];

		/*logger2->info("Enter username:");
		scanf("%s", scanbuf);*/
		_snprintf(username, 255, "mattadmin");//, scanbuf);

		/*logger2->info("Enter password:");
		scanf("%s", scanbuf);*/
		_snprintf(password, 255, "p@55W0rd");//, scanbuf);

		registerClient(username, password);

		OCArray<Object<ServerGame>> games = displayGameList();


		logger2->info("'s' for Start Game, anything else is username of game to join:");
		scanf("%s", scanbuf);

		if (scanbuf[0] == 's' && scanbuf[1] == 0) {
			hostGame();

			displayGameList();
		} else {
			for (unsigned int i=0; i<games.length(); i++) {
				Object<ServerGame> game = games.at(i);
				if (strcmp(game->getHostUser()->getUsername(), scanbuf) == 0) {
					if (client->isCurrentGame(game)) {
						logger2->error("Already joined to that game");
					} else {
						joinGame(game);
					}
				}
			}
		}

		logger2->info("Enter anything to quit:");
		scanf("%s", scanbuf);

		client->killClient();
	}


};

const Logger* TestApp::logger2 = Logger::getLogger("TestApp");


int main(void) {

	const Logger* logger = Logger::getLogger("Main");

	try {

		logger->info("Starting up");

		ROGInit();

		logger->info("Creating Application");

		Object<TestApp> app = new TestApp();

		logger->info("Running Application");
		app->runGame();

		

	} catch (FatalException* ex) {
		//char buf[1024];
		//_snprintf(buf, 1023, "%d:%s", ex->getCode(), ex->getMsg());
		logger->error(ex->getMsg());
		delete ex;
	}
	
	logger->info("Shutting down");
	//maximum destructiveness
	ROGDestroy();


	char scanbuf[1000];
	scanf("%s", scanbuf);
	

	return 10;
}


#endif