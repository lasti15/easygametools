#ifndef __GAMELIST_H
#define __GAMELIST_H

#pragma warning(disable : 4812)
#pragma warning(disable : 4244)
#pragma warning(disable : 4554)
#pragma warning(disable : 4018)
#pragma warning(disable : 4800)

#define OC_NEW_STYLE_INCLUDES
#define OC_BYTES_IN_POINTER 4
#include "opencontainers/opencontainers.h"

#pragma warning(default : 4812)
#pragma warning(default : 4244)
#pragma warning(default : 4554)
#pragma warning(default : 4018)
#pragma warning(default : 4800)

#include "XMLRootElement.h"
#include "Reference.h"

#include "ServerGame.h"

namespace ROG {


	class GameList : public Reference, public XMLRootElement {
	private:

		OCArray<Object<ServerGame>> gameList;

	public:

		void loadFromReader(irr::io::IrrXMLReader* reader);

		//hmm the copy consrtructors for these dont just copy a reference to the array....
		OCArray<Object<ServerGame>> getGameList() {
			return this->gameList;
		}

	};

}

#endif

