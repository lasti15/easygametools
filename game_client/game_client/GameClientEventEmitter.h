#ifndef __GAMECLIENTEVENTEMITTER_H
#define __GAMECLIENTEVENTEMITTER_H

#include "RakNetTypes.h"
#include "Reference.h"
#include "GameClientEvent.h"

namespace ROG {

	class GameClientEventEmitter : public Reference {
	public:
		virtual void fireEvent(GameClientEventType eventType, RakNet::Packet* eventPacket) = 0;
	};

}

#endif

