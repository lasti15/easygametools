#ifndef __PLAYER_H
#define __PLAYER_H


#include "Reference.h"
#include "User.h"
#include "RakNetTypes.h"

namespace ROG {

	typedef enum {CONNECTED, AUTHENTICATED, READY, DISCONNECTED} ConnectionState;

	class Player : public Reference, public Semaphore {
	private:

		Object<User> user;
		ConnectionState connectionState;
		RakNet::RakNetGUID clientGuid;
		char* host;
		int port;

	public:
		Player() {
			connectionState = DISCONNECTED;
			host = NULL;
			port = 0;
		}

		~Player() {
			if (host != NULL) {
				delete [] host;
			}
		}


		Object<User> getUser() {
			return user;
		}

		void setUser(Object<User> user) {
			this->user = user;
		}

		ConnectionState getConnectionState() {
			return connectionState;
		}

		void setConnectionState(ConnectionState connectionState) {
			this->connectionState = connectionState;
		}

		RakNet::RakNetGUID getClientGuid() {
			return clientGuid;
		}

		void setClientGuid(RakNet::RakNetGUID guid) {
			this->clientGuid = guid;
		}

		char* getHost() {
			return host;
		}

		void setHost(const char* host) {
			if (this->host != NULL) {
				delete [] this->host;
			}
			this->host = strdup(host);
		}

		int getPort() {
			return port;
		}

		void setPort(int port) {
			this->port = port;
		}

	};

}




#endif


