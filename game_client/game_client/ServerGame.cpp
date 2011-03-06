
#include "ServerGame.h"
#include "UserList.h"


using namespace ROG;

const static Logger* logger = Logger::getLogger("ServerGame");

void ServerGame::loadFromReader(irr::io::IrrXMLReader* reader) {
	logger->info("load XMl from reader");

	char* nodeName = NULL;
	char nodeNameBuf[255];

	do {

		irr::io::EXML_NODE nodeType = reader->getNodeType();

		switch (nodeType) {
			case irr::io::EXN_COMMENT:
			case irr::io::EXN_ELEMENT_END:
			case irr::io::EXN_NONE:
				break;
			case irr::io::EXN_TEXT:
				if (nodeName == NULL || nodeName[0] < 33) {
					nodeName = NULL;
					continue;
				}
				
				if (strncmp(nodeName, "hostName", 8) == 0) {
					this->hostName = strdup((char*)reader->getNodeData());
					logger->debug("found hostName");
					continue;
				}

				if (strncmp(nodeName, "hostPort", 8) == 0) {
					this->hostPort = atoi((char*)reader->getNodeData());
					logger->debug("found hostPort");
					continue;
				}

				if (strncmp(nodeName, "lastUpdate", 10) == 0) {
					this->lastUpdate = atol((char*)reader->getNodeData());
					logger->debug("found lastUpdate");
					continue;
				}

				if (strncmp(nodeName, "numberOfPlayers", 15) == 0) {
					this->numberOfPlayers = atoi((char*)reader->getNodeData());
					logger->debug("found numberOfPlayers");
					continue;
				}

				if (strncmp(nodeName, "maxPlayers", 10) == 0) {
					this->maxPlayers = atoi((char*)reader->getNodeData());
					logger->debug("found maxPlayers");
					continue;
				}

				if (strncmp(nodeName, "hostGuid", 10) == 0) {
					this->hostGuid = strdup((char*)reader->getNodeData());
					logger->debug("found hostGuid");
					continue;
				}
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
					//get the user id attribute
					for (unsigned int i=0; i<reader->getAttributeCount(); i++) {
						const char* attrName = reader->getAttributeName(i);
						if (attrName != NULL || attrName[0] < 33) {
							if (strncmp("id", attrName, 2) == 0) {
								//we found the right attribute
								this->gameId = atol((char*)reader->getAttributeValue(attrName));
								logger->debug("found GameId");
							}
						}
					}
					continue;
				}

				if (strncmp(nodeName, "hostUser", strlen(nodeName)) == 0) {
					logger->debug("Found User node");
					Object<User> user = new User();
					user->loadFromReader(reader);
					
					this->hostUser = user;

					continue;
				}

				if (strncmp(nodeName, "players", strlen(nodeName)) == 0) {
					logger->debug("Found PLAYERS node");
					
					Object<UserList> list = new UserList();
					list->loadFromReader(reader);

					this->players.clear();
					this->players = list->getUserList();

					continue;
				}
				break;
		}
	} while (reader->read());
}