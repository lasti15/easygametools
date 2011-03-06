#ifndef __SERVERGAME_H
#define __SERVERGAME_H


#include "XMLRootElement.h"
#include "User.h"

namespace ROG {

	class ServerGame : public Reference, public XMLRootElement {
	private:

		long gameId;

		char* hostName;
		int hostPort;
		int numberOfPlayers;
		int maxPlayers;
		long lastUpdate;
		char* hostGuid;

		Object<User> hostUser;

		OCArray<Object<User>> players;

	public:
		ServerGame() {
			this->gameId = 0;
			this->hostName = NULL;
			this->hostPort = 0;
			this->numberOfPlayers = 0;
			this->maxPlayers = 0;
			this->lastUpdate = 0;
			this->hostUser = NULL;
			this->hostGuid = NULL;
		};

		ServerGame(long gameId, char* hostName, int hostPort, int numberOfPlayers, int maxPlayers, long lastUpdate, Object<User> hostUser, char* hostGuid) {
			this->gameId = gameId;
			this->hostName = _strdup(hostName);
			this->hostPort = hostPort;
			this->numberOfPlayers = numberOfPlayers;
			this->maxPlayers = maxPlayers;
			this->lastUpdate = lastUpdate;
			this->hostUser = hostUser;
			this->hostGuid = _strdup(hostGuid);
		}

		~ServerGame() {
			if (hostName != NULL) delete [] hostName;
			if (hostGuid != NULL) delete [] hostGuid;
		}

		long getGameId() {
			return this->gameId;
		}

		char* getHostName() {
			return this->hostName;
		}

		int getHostPort() {
			return this->hostPort;
		}

		int getNumberOfPlayers() {
			return this->numberOfPlayers;
		}

		void setNumberOfPlayers(int numberOfPlayers) {
			this->numberOfPlayers = numberOfPlayers;
		}

		int getMaxPlayers() {
			return this->maxPlayers;
		}

		long getLastUpdate() {
			return lastUpdate;
		}

		Object<User> getHostUser() {
			return hostUser;
		}

		char* getHostGuid() {
			return hostGuid;
		}

		OCArray<Object<User>> getPlayers() {
			return players;
		}

		void loadFromReader(irr::io::IrrXMLReader* reader);
	};

}


#endif