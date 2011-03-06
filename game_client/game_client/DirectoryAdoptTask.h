#ifndef __DIRECTORYADOPTTASK_H
#define __DIRECTORYADOPTTASK_H

#include "RakPeerInterface.h"
#include "Logger.h"
#include "Runnable.h"
#include "DirectoryClient.h"
#include "ProtocolFactory.h"


namespace ROG {


	class DirectoryAdoptTask : public Runnable {
	private:
		Object<DirectoryClient> directoryClient;
		Object<GameClient> gameClient;
		long gameId;
		char* hostName;
		int port;
		RakNet::RakPeerInterface* rakPeer;

		bool creator;

		const static Logger* logger;

	public:
		DirectoryAdoptTask(Object<DirectoryClient> directoryClient, Object<GameClient> gameClient, const long gameId, const char* hostName, const int port, RakNet::RakPeerInterface* rakPeer, bool creator) {
			this->directoryClient = directoryClient;
			this->gameClient = gameClient;
			this->gameId = gameId;
			this->hostName = (char*)hostName;
			this->port = port;
			this->rakPeer = rakPeer;
			this->creator = creator;
		}

		void run() {
			logger->debug("Adopting host game!");
			//dont make the creator adopt the game.
			if (!creator) {
				this->gameClient->updateGame(directoryClient->adoptGame(gameClient->getTheGameImPlaying()->getGameId(), this->hostName, this->port,
					rakPeer->GetGuidFromSystemAddress(RakNet::SystemAddress(this->hostName, this->port)).ToString()));
			}

			//tell everyone there is a new host
			protocol::ProtocolMessage* challengeMessage = (protocol::ProtocolMessage*)ProtocolFactory::getProtoNewHostMessage(gameClient->getTheGameImPlaying()->getGameId());
			std::string* msgData = gameClient->createProtocolObjectMessage(challengeMessage);
			gameClient->sendProtocolMessage(msgData, NULL);
			delete msgData;
			delete challengeMessage;
		}
	};


	const Logger* DirectoryAdoptTask::logger = Logger::getLogger("DirectoryAdoptTask");

}


#endif

