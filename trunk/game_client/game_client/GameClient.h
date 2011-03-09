#ifndef __GAMECLIENT_H
#define __GAMECLIENT_H

//raknet
#include "SecureHandshake.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "FullyConnectedMesh2.h"
#include "ConnectionGraph2.h"
#include "RakNetTypes.h"
//end raknet

//our headers need to come after the raknet headers so that it can properly define some types
#include "Reference.h"
#include "DirectoryClient.h"
#include "Thread.h"
#include "GameClientEventListener.h"
#include "ClientFuture.h"
#include "ThreadPool.h"
#include "Player.h"

namespace ROG {

	#define DIRECTORYCLIENT_UPDATE_DELAY 45000

	enum NetworkErrorState {ERROR_OK=0, ERROR_CANT_REGISTER_CLIENT=900, ERROR_CANT_GENERATE_SECURITY_KEYS=901, ERROR_INVALID_ARGUMENT=902, ERROR_STARTUP_FAILED=903};
	enum ProtocolMessageIds {PROTOCOL_OBJECT_MESSAGE=ID_USER_PACKET_ENUM};

	class GameClient : public Runnable, public Semaphore {
	private:

		const static Logger* logger;

		//the lobby client
		Object<DirectoryClient> directoryClient;

		Object<ServerGame> theGameImPlaying;
		Object<User> myUser;

		void initGameClient();
		void initDirectoryClient(const char* username, const char* password);
		void startRakNet(const char* hostName, const int port);

		//game client
		RakNet::RakPeerInterface *rakPeer;
		RakNet::FullyConnectedMesh2* fcm2;
		RakNet::ConnectionGraph2* cg2;

		long lastUpdated;

		NetworkErrorState errorState;

		//for speed we will only allow one game client event listener
		Object<GameClientEventListener> eventListener;

		//the clientfuture when initially starting the network thread.
		Object<ClientFuture> startupFuture;

		Object<Thread> dispatcherThread;

		Object<ThreadPool> dispatchPool;

		//my hostname
		char* hostname;
		//my port
		int port;

		Semaphore myPlayerSem;
		Semaphore gamePlayersSem;

		Object<Player> myPlayer;
		AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM> gamePlayers;
		bool creator;

	protected:
		const void stopDirectoryClient(int numPlayers);

	public:

		GameClient() : rakPeer(NULL), fcm2(NULL), cg2(NULL), lastUpdated(0), errorState(ERROR_OK), hostname(NULL), port(0), creator(false) {
			initGameClient();
		}

		void run();

		void setEventListener(Object<GameClientEventListener> listener);
		Object<GameClientEventListener> getEventListener();

		bool isCurrentGame(Object<ServerGame> game);

		const NetworkErrorState getError() const;

		const void clearErrorState();

		void handleProtocolMessage(RakNet::Packet* packet);
		void sendProtocolObject(protocol::ProtocolMessage* message, RakNet::RakNetGUID guid);
		void sendProtocolMessage(std::string* message, RakNet::AddressOrGUID* addr);

		std::string* createProtocolObjectMessage(protocol::ProtocolMessage* message);
		protocol::ProtocolMessage* decodeProtocolObjectMessage(const unsigned char* message, int len);

		void updateGame(Object<ServerGame> game);

		//api
		void startClient(const char* username, const char* password);
		const void stopClient();
		const void killClient();

		Object<ClientFuture> startGame(const char* hostName, int desiredPort);
		Object<ClientFuture> joinGame(const char* hostName, int port, Object<ServerGame> gameInfo);

		OCArray<Object<ServerGame>> getAvailableGames();
		void bootPlayer(Object<Player> player);
		void addPlayer(Object<Player> player);

		void broadcast(const char* msg);

		bool isHost();

		Object<ServerGame> getTheGameImPlaying() {
			return theGameImPlaying;
		}

		Object<Player> getMyPlayer() {
			return myPlayer;
		}

	};


}



#endif

