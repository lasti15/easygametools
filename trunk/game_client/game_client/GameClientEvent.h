#ifndef __GAMECLIENTEVENT_H
#define __GAMECLIENTEVENT_H

#include "RakNetTypes.h"
#include "Reference.h"

namespace ROG {

	typedef enum GameClientEventType {EVENT_CLIENT_STARTED, EVENT_CONNECTION_ACCEPTED, EVENT_CONNECTION_REQUESTED, 
					EVENT_NEW_CONNECTION, EVENT_CONNECTION_LOST, EVENT_CONNECTION_TERMINATED, 
					EVENT_HOST_CHANGED, EVENT_CHAT_MESSAGE, EVENT_OBJECT_MESSAGE, EVENT_DIRECTORY_CLIENT_CONNECTED, EVENT_DIRECTORY_CLIENT_ERROR};

	class GameClientEvent : public Reference {
	private:

		GameClientEventType eventType;

		RakNet::Packet* eventPacket;

	public:

		GameClientEvent(GameClientEventType eventType, RakNet::Packet* eventPacket) {
			this->eventType = eventType;
			this->eventPacket = eventPacket;
		}

		GameClientEventType getEventType() {
			return eventType;
		}

		RakNet::Packet* getEventPacket() {
			return eventPacket;
		}
	};

}




#endif