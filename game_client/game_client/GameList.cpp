
#include "GameList.h"

using namespace ROG;

const static Logger* logger = Logger::getLogger("GameList");

void GameList::loadFromReader(irr::io::IrrXMLReader* reader) {
	logger->debug("load XMl from reader");

	char* nodeName = NULL;
	char nodeNameBuf[255];

	while (reader->read()) {

		irr::io::EXML_NODE nodeType = reader->getNodeType();

		switch (nodeType) {
			case irr::io::EXN_COMMENT:
			case irr::io::EXN_ELEMENT_END:
			case irr::io::EXN_NONE:
			case irr::io::EXN_TEXT:
				break;
			case irr::io::EXN_ELEMENT:
				nodeName = (char*)reader->getNodeName();

				if (nodeName == NULL || nodeName[0] < 33) {
					nodeName = NULL;
					continue;
				}

				int nodeNameLen = strlen(nodeName);
				strncpy(nodeNameBuf, nodeName, nodeNameLen);
				nodeNameBuf[nodeNameLen] = 0;
				nodeName = nodeNameBuf;

				if (strncmp(nodeName, "game", strlen(nodeName)) == 0) {
					logger->debug("Found game node");
					Object<ServerGame> serverGame = new ServerGame();
					serverGame->loadFromReader(reader);
					gameList.append(serverGame);

					continue;
				}
				break;
		}
	}
}







