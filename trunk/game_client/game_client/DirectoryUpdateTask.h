#ifndef __DIRECTORYUPDATETASK_H
#define __DIRECTORYUPDATETASK_H

#include "Logger.h"
#include "Runnable.h"


namespace ROG {


	class DirectoryUpdateTask : public Runnable {
	private:
		Object<ServerGame> gameImPlaying;
		Object<DirectoryClient> directoryClient;
		bool isHost;

		const static Logger* logger;

	public:
		DirectoryUpdateTask(Object<ServerGame> gameImPlaying, Object<DirectoryClient> directoryClient, bool isHost) {
			this->gameImPlaying = gameImPlaying;
			this->directoryClient = directoryClient;
			this->isHost = isHost;
		}

		void run() {
			if (isHost) {
				logger->info("Updating directory client");
				this->gameImPlaying = directoryClient->updateGame(this->gameImPlaying);
			} else {
				//otherwise just ask for any changes to the current game record
				logger->info("Checking directory client for updates");
				this->gameImPlaying = directoryClient->getUpdatedGame(this->gameImPlaying->getGameId());
			}
		}
	};


	const Logger* DirectoryUpdateTask::logger = Logger::getLogger("DirectoryUpdateTask");

}


#endif

