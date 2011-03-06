#ifndef __DIRECTORY_CLIENT_H
#define __DIRECTORY_CLIENT_H



#include "Reference.h"
#include "ServerGame.h"
#include "GameList.h"

namespace ROG {

	class DirectoryClient : public Reference {
	private:
		//if i am in a game this is it
		Object<ServerGame> currentGame;
		
		char* directoryHostName;
		int directoryPort;
		char* directoryContext;

		char* sessionId;

	public:

		DirectoryClient();

		//api methods
		Object<User> login(const char* username, const char* password);
		Object<GameList> getGameList();
		Object<ServerGame> createGame(const char* hostName, const int port, const char* hostGuid);
		Object<ServerGame> updateGame(Object<ServerGame> gameToUpdate);
		Object<ServerGame> getUpdatedGame(long gameId);
		void deleteGame(long gameId);
		void releaseGame(long gameId, long userId, const char* nextHostGuid);
		Object<ServerGame> adoptGame(long gameId, const char* hostName, const int port, const char* hostGuid);
		const char* getExternalIp();
		Object<User> validateUser(long userId, long gameId);

	};

}



#endif
