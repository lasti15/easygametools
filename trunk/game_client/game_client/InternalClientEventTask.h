#ifndef __INTERNALCLIENTEVENTTASK_H
#define __INTERNALCLIENTEVENTTASK_H

#include "Runnable.h"
#include "GameClientEventEmitter.h"
#include "GameClientEvent.h"
#include "Protocol.pb.h"

namespace ROG {

	class InternalClientEventTask : public Runnable {
	private:
		Object<GameClientEventEmitter> gameClient;
		GameClientEventType eventType;
		protocol::ProtocolMessage* message;

	public:
		InternalClientEventTask(Object<GameClientEventEmitter> gameClient, GameClientEventType eventType, protocol::ProtocolMessage* message) {
			this->gameClient = gameClient;
			this->eventType = eventType;
			this->message = message;
		}

		void run() {
			
		}

	};


}



#endif

