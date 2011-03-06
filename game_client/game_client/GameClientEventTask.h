#ifndef __GAMECLIENTEVENTTASK_H
#define __GAMECLIENTEVENTTASK_H

#include "RakNetTypes.h"
#include "Runnable.h"
#include "GameClientEventEmitter.h"
#include "GameClientEvent.h"

namespace ROG {

	class GameClientEventTask : public Runnable {
	private:
		Object<GameClientEventEmitter> gameClient;
		GameClientEventType eventType;
		RakNet::Packet* eventPacket;

	public:
		GameClientEventTask(Object<GameClientEventEmitter> gameClient, GameClientEventType eventType, RakNet::Packet* eventPacket) {
			this->gameClient = gameClient;
			this->eventType = eventType;
			this->eventPacket = eventPacket;
		}

		void run() {
			gameClient->fireEvent(eventType, eventPacket);
		}

	};


}



#endif

