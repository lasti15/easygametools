#ifndef __MESSAGEHANDLER_H
#define __MESSAGEHANDLER_H


#include "Reference.h"
#include "Player.h"
#include "Protocol.pb.h"


namespace ROG {


	class MessageHandler : public Reference {
	private:
		protocol::MessageId messageId;

	protected:
		MessageHandler(protocol::MessageId messageId) {
			this->messageId = messageId;
		}

	public:
		virtual void handleMessage(protocol::ProtocolMessage* message, Object<Player> me, Object<Player> from, AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* currentPlayers) = 0;
	};



}


#endif

