#ifndef __GAMECLIENTEVENTLISTENER_H
#define __GAMECLIENTEVENTLISTENER_H


#include "Reference.h"
#include "GameClientEvent.h"

namespace ROG {


	class GameClientEventListener : public Reference {

	public:

		virtual void onGameClientEvent(Object<GameClientEvent> e) = 0;
	};

}


#endif
