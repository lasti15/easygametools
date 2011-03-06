#ifndef __DIRECTORYADOPTTASK_H
#define __DIRECTORYADOPTTASK_H

#include "RakPeerInterface.h"
#include "Logger.h"
#include "Runnable.h"
#include "DirectoryClient.h"


namespace ROG {


	class DirectoryAdoptTask : public Runnable {
	private:
		Object<DirectoryClient> directoryClient;
		Object<ServerGame> gameImPlaying;
		long gameId;
		char* hostName;
		int port;
		RakNet::RakPeerInterface* rakPeer;

		const static Logger* logger;

	public:
		DirectoryAdoptTask(Object<DirectoryClient> directoryClient, Object<ServerGame> gameImPlaying, const long gameId, const char* hostName, const int port, RakNet::RakPeerInterface* rakPeer) {
			this->directoryClient = directoryClient;
			this->gameImPlaying = gameImPlaying;
			this->gameId = gameId;
			this->hostName = (char*)hostName;
			this->port = port;
			this->rakPeer = rakPeer;
		}

		void run() {
			logger->debug("Adopting host game!");
			this->gameImPlaying = directoryClient->adoptGame(this->gameImPlaying->getGameId(), this->hostName, this->port,
				rakPeer->GetGuidFromSystemAddress(RakNet::SystemAddress(this->hostName, this->port)).ToString());
		}
	};


	const Logger* DirectoryAdoptTask::logger = Logger::getLogger("DirectoryAdoptTask");

}


#endif

