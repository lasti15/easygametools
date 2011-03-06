
#include "UserList.h"

using namespace ROG;

const static Logger* logger = Logger::getLogger("UserList");

void UserList::loadFromReader(irr::io::IrrXMLReader* reader) {
	logger->debug("load XMl from reader");

	char* nodeName = NULL;
	char nodeNameBuf[255];

	while (reader->read()) {

		irr::io::EXML_NODE nodeType = reader->getNodeType();

		switch (nodeType) {
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

				if (strncmp(nodeName, "user", strlen(nodeName)) == 0) {
					logger->debug("Found USER node");
					Object<User> user = new User();
					user->loadFromReader(reader);
					userList.append(user);

					continue;
				}
				break;
		}
	}
}







