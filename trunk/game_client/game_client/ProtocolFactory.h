#ifndef __PROTOCOLFACTORY_H
#define __PROTOCOLFACTORY_H

#include "User.h"

#include "Protocol.pb.h"
#include "ProtoChallenge.pb.h"
#include "User.pb.h"



namespace ROG {


	class ProtocolFactory {

	public:

		inline const static protocol::ProtocolMessage* getProtoChallengeMessage(const long challengerId) {
			protocol::ProtocolMessage* message = new protocol::ProtocolMessage();

			message->set_messageid(protocol::CHALLENGE);

			protocol::ProtoChallenge* instance = message->MutableExtension(protocol::ProtoChallenge::challenge);

			instance->set_challengerid(challengerId);

			return message;
		}

		inline const static protocol::ProtocolMessage* getUserMessage(const long userId, const char* username) {
			protocol::ProtocolMessage* message = new protocol::ProtocolMessage();
			message->set_messageid(protocol::USER);

			protocol::User* response = message->MutableExtension(protocol::User::user);

			response->set_userid(userId);
			response->set_username(username);

			return message;
		}


		inline const static protocol::ProtocolMessage* getProtoChallengeResponseMessage(Object<User> user) {

			protocol::ProtocolMessage* message = new protocol::ProtocolMessage();
			message->set_messageid(protocol::CHALLENGERESPONSE);

			protocol::ProtoChallengeResponse* response = message->MutableExtension(protocol::ProtoChallengeResponse::challengeResponse);
			
			protocol::User* userP = response->mutable_userdetails();
			userP->set_userid(user->getUserId());
			userP->set_username(user->getUsername());

			return message;
		}

		inline const static protocol::ProtocolMessage* getProtoChallengeAcceptedMessage(Object<User> host) {

			protocol::ProtocolMessage* message = new protocol::ProtocolMessage();
			message->set_messageid(protocol::CHALLENGEACCEPTED);

			protocol::ProtoChallengeAccepted* response = message->MutableExtension(protocol::ProtoChallengeAccepted::challengeAccepted);
			
			protocol::User* userP = response->mutable_hostdetails();
			userP->set_userid(host->getUserId());
			userP->set_username(host->getUsername());

			return message;
		}


	};



}



#endif

