
#include "User.h"


using namespace ROG;


const static Logger* logger = Logger::getLogger("User");

void User::loadFromReader(irr::io::IrrXMLReader* reader) {
	logger->info("Load user xml");

	char* nodeName = NULL;
	char nodeNameBuf[255];

	do {

		irr::io::EXML_NODE nodeType = reader->getNodeType();

		switch (nodeType) {
			case irr::io::EXN_ELEMENT_END:
			case irr::io::EXN_COMMENT:
			case irr::io::EXN_NONE:
				break;
			case irr::io::EXN_TEXT:
				if (nodeName == NULL || nodeName[0] < 33) {
					nodeName = NULL;
					continue;
				}
				if (strncmp(nodeName, "username", 8) == 0) {
					this->username = strdup((char*)reader->getNodeData());
					logger->debug("found Username");
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

				if (strncmp(nodeName, "hostUser", strlen(nodeName)) == 0 || strncmp(nodeName, "user", strlen(nodeName)) == 0) {
					//get the user id attribute
					for (unsigned int i=0; i<reader->getAttributeCount(); i++) {
						const char* attrName = reader->getAttributeName(i);
						if (attrName != NULL || attrName[0] < 33) {
							if (strncmp("id", attrName, 2) == 0) {
								//we found the right attribute
								this->userId = atol((char*)reader->getAttributeValue(attrName));
								logger->debug("found UserId");
							}
						}
					}
					continue;
				}
				break;
		}

		//bah i need better breakout logic
		if (this->username != NULL) {
			break;
		}
	} while (reader->read());
}

