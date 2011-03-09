
#include "GameClient.h"
//raknet
#include "RakSleep.h"
#include "PacketLogger.h"
#include "BitStream.h"
#include "SocketLayer.h"
#include "MessageIdentifiers.h"
#include "GetTime.h"
#include "SocketLayer.h"
//end raknet
#include "DateTime.h"
#include "LoginFailedException.h"
#include "Config.h"
#include "GameClientEventTask.h"
#include "GameClientEventEmitter.h"
#include "DirectoryUpdateTask.h"
#include "DirectoryAdoptTask.h"
#include "ProtocolException.h"
#include "ProtocolFactory.h"
#include "MessageHandler.h"

#include <string>

using namespace ROG;

#define MIN_GAME_NAME_LEN 5
#define MAX_GAME_NAME_LEN 50

const Logger* GameClient::logger = Logger::getLogger("GameClient");

const static char PROTOCOL_OBJECT_MESSAGE_BUF[] = {PROTOCOL_OBJECT_MESSAGE, 0};



class ClientMessageHandler : public MessageHandler {
private:
	
protected:
	Object<GameClient> client;

	ClientMessageHandler(Object<GameClient> client, protocol::MessageId messageId) : MessageHandler(messageId) {
		this->client = client;
	}
public:

	Object<GameClient> getClient() {
		return client;
	}

	void setClient(Object<GameClient> client) {
		this->client = client;
	}

};







class ChallengeAcceptedMessageHandler : public ClientMessageHandler {
private:
	const static Logger* logger;

	Object<DirectoryClient> directory;
	RakNet::RakPeerInterface* rakPeer;
public:
	ChallengeAcceptedMessageHandler(Object<GameClient> client, Object<DirectoryClient> directory, RakNet::RakPeerInterface* rakPeer) : ClientMessageHandler(client, protocol::CHALLENGEACCEPTED) {
		this->directory = directory;
		this->rakPeer = rakPeer;
	}

	void handleMessage(protocol::ProtocolMessage* message, Object<Player> me, Object<Player> from, AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* currentPlayers) {
		//Challenge accepted, we are allowed to join this person's game and they have sent us their user
		Object<User> user = new User();
		logger->debug("handleMessage!");

		protocol::ProtoChallengeAccepted response = message->GetExtension(protocol::ProtoChallengeAccepted::challengeAccepted);
		protocol::User protoUser = response.hostdetails();
		user->setUserId((long)protoUser.userid());

		//copy the username
		std::string username = protoUser.username();
		char usernameBuf[255];
		username.copy(usernameBuf, username.length());
		usernameBuf[username.length()] = 0;
		user->setUsername(usernameBuf);

		//add this player to our list of current players
		from->lock();

		from->setUser(user);
		from->setConnectionState(AUTHENTICATED);
		currentPlayers->insertKeyAndValue(OCString(from->getClientGuid().ToString()), from);

		from->unlock();

		delete message;
	}
};

const Logger* ChallengeAcceptedMessageHandler::logger = Logger::getLogger("ChallengeAcceptedMessageHandler");


class ChallengeResponseMessageHandler : public ClientMessageHandler {
private:
	const static Logger* logger;

	Object<DirectoryClient> directory;
	RakNet::RakPeerInterface* rakPeer;
public:
	ChallengeResponseMessageHandler(Object<GameClient> client, Object<DirectoryClient> directory, RakNet::RakPeerInterface* rakPeer) : ClientMessageHandler(client, protocol::CHALLENGERESPONSE) {
		this->directory = directory;
		this->rakPeer = rakPeer;
	}

	void handleMessage(protocol::ProtocolMessage* message, Object<Player> me, Object<Player> from, AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* currentPlayers) {
		//we got the challenge response, get the user info and ask the directory if this user is allowed to join our game
		Object<User> user = new User();
		logger->debug("handleMessage!");

		protocol::ProtoChallengeResponse response = message->GetExtension(protocol::ProtoChallengeResponse::challengeResponse);
		protocol::User protoUser = response.userdetails();
		user->setUserId((long)protoUser.userid());

		//copy the username
		std::string username = protoUser.username();
		char usernameBuf[255];
		username.copy(usernameBuf, username.length());
		usernameBuf[username.length()] = 0;
		user->setUsername(usernameBuf);

		logger->debug("validating this user is allowed to join");
		Object<User> directoryUser = directory->validateUser(user->getUserId(), client->getTheGameImPlaying()->getGameId());
		if (directoryUser == NULL) {
			//TODO:make booting players work
			//invalid connectionAttempt
			logger->error("Invalid player, booting.");
			client->bootPlayer(from);
			return;
		}

		if (strcmp(directoryUser->getUsername(), user->getUsername()) != 0) {
			//TODO:make booting players work
			//invalid connectionAttempt
			logger->error("Invalid player, booting.");
			client->bootPlayer(from);
			return;
		}

		//add this player to gamePlayers
		from->lock();

		from->setUser(directoryUser);
		from->setConnectionState(AUTHENTICATED);
		currentPlayers->insertKeyAndValue(OCString(from->getClientGuid().ToString()), from);
		client->addPlayer(from);

		from->unlock();

		logger->debug("User was allowed to join");

		//respond to this message
		{
			//send our user info to the client
			protocol::ProtocolMessage* challengeMessage = (protocol::ProtocolMessage*)ProtocolFactory::getProtoChallengeAcceptedMessage(me->getUser());
			client->sendProtocolObject(challengeMessage, from->getClientGuid());
		}

		delete message;
	}
};

const Logger* ChallengeResponseMessageHandler::logger = Logger::getLogger("ChallengeResponseMessageHandler");



class ChallengeMessageHandler : public ClientMessageHandler {
public:
	ChallengeMessageHandler(Object<GameClient> client) : ClientMessageHandler(client, protocol::CHALLENGE) {}

	//send the challenge response message
	void handleMessage(protocol::ProtocolMessage* message, Object<Player> me, Object<Player> from, AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* currentPlayers) {
		protocol::ProtocolMessage* response = (protocol::ProtocolMessage*)ProtocolFactory::getProtoChallengeResponseMessage(me->getUser());
		client->sendProtocolObject(response, from->getClientGuid());

		delete message;
	}
};




class InternalClientEventTask : public Runnable {
private:
	Object<MessageHandler> handler;
	GameClientEventType eventType;
	protocol::ProtocolMessage* message;
	Object<Player> myPlayer;
	Object<Player> from;
	AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* players;

public:
	InternalClientEventTask(Object<MessageHandler> handler, protocol::ProtocolMessage* message, Object<Player> me, Object<Player> from, AVLHashT<OCString, Object<Player>, AVL_CHUNK_SIZE_MEM>* players) {
		this->handler = handler;
		this->eventType = eventType;
		this->message = message;
		this->myPlayer = me;
		this->from = from;
		this->players = players;
	}

	void run() {
		handler->handleMessage(message, myPlayer, from, players);
	}

};


class ClientEventEmitter : public GameClientEventEmitter {
	Object<GameClientEventListener> listener;
public:
	ClientEventEmitter(Object<GameClientEventListener> listener) {
		this->listener = listener;
	}

	void fireEvent(GameClientEventType eventType, RakNet::Packet* eventPacket) {
		if (listener != NULL) {
			listener->onGameClientEvent(new GameClientEvent(eventType, eventPacket));
		}
	}
};



void GameClient::initGameClient() {
	logger->debug("Loading Game Client");
	//load the config file
	Object<ConfigLoader> config = ConfigLoader::getConfigLoader();
	if (config == NULL) {
		throw new FatalException("Can't Open Config");
	}
}



void GameClient::initDirectoryClient(const char* username, const char* password) {
	
	directoryClient = new DirectoryClient();
	myUser = directoryClient->login(username, password);
	if (myUser == NULL) {
		char msg[255];
		_snprintf(msg, 254, "Login failed for user [%s]", username);
		throw new LoginFailedException(msg);
	}

	this->myPlayer = new Player();
	this->myPlayer->setUser(myUser);

	(new ClientEventEmitter(this->eventListener))->fireEvent(EVENT_DIRECTORY_CLIENT_CONNECTED, NULL);
}


void GameClient::startClient(const char* username, const char* password) {
	//initdirectoryclient will throw an exception if it fails for any reason
	//dont clal it from an object constructor. unless you wrap it in try->catch for FatalException*
	initDirectoryClient(username, password);
}


const void GameClient::stopDirectoryClient(int numPlayers) {
	if (theGameImPlaying != NULL) {
		logger->info("Unregistering from directory server");

		if (strcmp(this->myPlayer->getClientGuid().ToString(), this->theGameImPlaying->getHostGuid()) == 0) {
			//i own the current game
			//im going ot delete the existing game record and let the new host create a new one
			directoryClient->deleteGame(theGameImPlaying->getGameId());
		}
		theGameImPlaying = NULL;
	}
	myUser = NULL;
}


const void GameClient::killClient() {
	if (dispatcherThread != NULL) {
		logger->info("Waiting for game client thread to exit");
		dispatcherThread->signal(1);
		dispatcherThread->waitfor();
	}

	stopClient();
}


const void GameClient::stopClient() {

	int numPlayers = 0;

	//grab some info form the mesh before we sever our connection
	if (fcm2 != NULL) {
		numPlayers = fcm2->GetParticipantCount();
	}
	stopDirectoryClient(gamePlayers.entries());

	//returnt he game client back to its original state
	if (rakPeer != NULL) {
		rakPeer->CloseConnection(RakNet::UNASSIGNED_SYSTEM_ADDRESS, false);
		RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	}
	if (fcm2 != NULL) {
		RakNet::FullyConnectedMesh2::DestroyInstance(fcm2);
	}
	if (cg2 != NULL) {
		RakNet::ConnectionGraph2::DestroyInstance(cg2);
	}

	if (hostname != NULL) {
		delete [] hostname;
	}
	hostname = NULL;
	port = 0;

	rakPeer = NULL;
	fcm2 = NULL;
	cg2 = NULL;

	directoryClient = NULL;
	theGameImPlaying = NULL;
	myUser = NULL;

	lastUpdated = 0;

	errorState = ERROR_OK;

	eventListener = NULL;
	//dont kill startupfuture in case we are exiting as a result of an error on startup
	//startupFuture = NULL;
	dispatcherThread = NULL;
	dispatchPool = NULL;

	myPlayer = NULL;
	gamePlayers.clear();
}

void GameClient::run() {
	Object<DateTime> clock = new DateTime();
	lastUpdated = clock->Update();

	RakNet::Packet* packet = NULL;

	logger->info("Client started");


	//start up our thread pool
	dispatchPool = new ThreadPool(10, 15);
	try {
		dispatchPool->start();
	} catch (FatalException* ex) {
		logger->error(ex->getMsg());
		delete ex;

		//at this point we are inside the dispatch thread, we've hit some fatal exception here, stop the client to make sure we unregister from the directory server
		//we cant call stopclient though, cause it will infinitely block waiting for this thread to exit form inside this thread
		stopClient();

		this->errorState = ERROR_STARTUP_FAILED;

		startupFuture->lock();
		startupFuture->setCode(FUTURE_ERROR);
		startupFuture->setComplete(true);
		startupFuture->unlock();

		throw new FatalException("Error creating thread pool");
	}


	startupFuture->setComplete(true);
	startupFuture->setCode(FUTURE_OK);

	try {

		//the main netowrking loop
		while (!dispatcherThread->getSignalled())	{
			if (this->rakPeer != NULL) {

				//do raknet stuff here
				for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive()) {
					switch (packet->data[0]) {
					// Connection lost normally
					case ID_DISCONNECTION_NOTIFICATION:
						logger->debug("ID_DISCONNECTION_NOTIFICATION");

						gamePlayers.remove(OCString(packet->guid.ToString()));

						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_CONNECTION_LOST, NULL));

						//everytime a client disconnects reduce our lastupdate time to make it sooner that we update the directory record
						lastUpdated -= ((8 - this->theGameImPlaying->getNumberOfPlayers())*5) * 1000;
						break;

					//A new client connected to us
					case ID_NEW_INCOMING_CONNECTION:
						logger->debug("ID_NEW_INCOMING_CONNECTION");

						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_NEW_CONNECTION, NULL));

						//everytime a client connects reduce our lastupdate time to make it sooner that we update the directory record
						lastUpdated -= ((8 - this->theGameImPlaying->getNumberOfPlayers())*5) * 1000;

						{
							//challenge this client to identify themselves
							protocol::ProtocolMessage* challengeMessage = (protocol::ProtocolMessage*)ProtocolFactory::getProtoChallengeMessage(this->myUser->getUserId());
							sendProtocolObject(challengeMessage, packet->guid);
						}
						break;

					//We connected to another client
					case ID_CONNECTION_REQUEST_ACCEPTED:
						logger->debug("ID_CONNECTION_REQUEST_ACCEPTED");

						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_CONNECTION_ACCEPTED, NULL));
						break;

					case ID_NO_FREE_INCOMING_CONNECTIONS:
						logger->debug("ID_NO_FREE_INCOMING_CONNECTIONS");

						this->theGameImPlaying = NULL;
						break;

					//connection lost for some bad reason
					case ID_CONNECTION_LOST:
						logger->debug("ID_CONNECTION_LOST");

						gamePlayers.remove(OCString(packet->guid.ToString()));

						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_CONNECTION_TERMINATED, NULL));

						//everytime a client disconnects reduce our lastupdate time to make it sooner that we update the directory record
						lastUpdated -= ((8 - this->theGameImPlaying->getNumberOfPlayers())*5) * 1000;
						break;

					case ID_FCM2_NEW_HOST:
						logger->debug("ID_FCM2_NEW_HOST");
						if (fcm2->IsHostSystem()) {
							dispatchPool->execute(new DirectoryAdoptTask(this->directoryClient, this, this->theGameImPlaying->getGameId(), this->hostname, this->port, this->rakPeer, creator));
						}
						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_HOST_CHANGED, NULL));
						break;

					//deprecate chat messages in favour of protocol messages
					/*case PROTOCOL_CHAT_MSG:
						logger->debug("PROTOCOL_CHAT_MSG");
						dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_CHAT_MESSAGE, packet));
						break;*/

					case PROTOCOL_OBJECT_MESSAGE:
						logger->debug("PROTOCOL_OBJECT_MESSAGE");
						handleProtocolMessage(packet);
						break;

					}
				}
			
			}

			//update the directory client every 45 seconds if necessary
			if (clock->Update() - lastUpdated > DIRECTORYCLIENT_UPDATE_DELAY) {
				if (this->theGameImPlaying != NULL) {
					if (this->myUser != NULL) {
						if (this->theGameImPlaying->getHostUser() != NULL) {
							//either update the game record or get the updated game record.
							if (strcmp(this->theGameImPlaying->getHostGuid(), this->myPlayer->getClientGuid().ToString()) == 0) {
								//if the current games host user id and mine are the same then update
								//edge case. When you have multiple clients for the same user in the same game they will all fight to be the directory updater
								//make sure I'm also the host of this game.
								Object<Runnable> updateTask = new DirectoryUpdateTask(this->theGameImPlaying, this->directoryClient, true);
								//otherwise just ask for any changes to the current game record
								dispatchPool->execute(updateTask);
								
							} else {
								Object<Runnable> updateTask = new DirectoryUpdateTask(this->theGameImPlaying, this->directoryClient, false);
								//otherwise just ask for any changes to the current game record
								dispatchPool->execute(updateTask);
							}

							lastUpdated = clock->Update();
						} else {
							//the hostuser is null, that means the game must be in transition
							//update again a bit sooner than normal
							lastUpdated = clock->Update() - (30 * 1000);
						}
					}
				}
			}
			RakSleep(30);
		}

	} catch (FatalException* ex) {
		logger->error(ex->getMsg());
		delete ex;

		logger->error("Stopping game client");

		dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_DIRECTORY_CLIENT_ERROR, NULL));

		//at this point we are inside the dispatch thread, we've hit some fatal exception here, stop the client to make sure we unregister from the directory server
		stopClient();
	}
}


void GameClient::updateGame(Object<ServerGame> game) {
	if (game == NULL) {
		//if game is NULL it could be because the host didnt exit cleanly, or I am hosting another game already
		//let it spin one cycle trying to figure itself out
		throw new ProtocolException("UpdateGame(): game is null");
	}

	this->theGameImPlaying = game;
}



bool GameClient::isHost() {
	if (this->myPlayer == NULL || this->theGameImPlaying == NULL || this->cg2 == NULL || this->fcm2 == NULL || this->directoryClient == NULL || this->myUser == NULL) {
		return false;
	}

	return fcm2->IsHostSystem();
}


void GameClient::handleProtocolMessage(RakNet::Packet* packet) {

	//deserialize and store the message somehow int eh event

	protocol::ProtocolMessage* message = decodeProtocolObjectMessage(packet->data, packet->length);
	if (message == NULL) {
		throw new ProtocolException("Could not decode Message");
	}

	logger->debug("Got new protocol message!");

	protocol::MessageId messageId = message->messageid();
	//based on the connectionstate, only allow access to certain messages
	Object<Player> messageFrom = gamePlayers[OCString(packet->guid.ToString())];
	if (messageFrom == NULL) {
		//this could be a new client authenticating or we are a client who received a challenge from another client int he mesh
		//check the messageId

		Object<Player> tempPlayer = new Player();
		tempPlayer->setConnectionState(CONNECTED);
		tempPlayer->setClientGuid(packet->guid);
		tempPlayer->setHost(packet->systemAddress.ToString(false));
		tempPlayer->setPort(packet->systemAddress.port);
		
		if (messageId == protocol::CHALLENGEACCEPTED) {
			//the challenge was accepted and the remote host has sent us their user info

			logger->debug("Challenge Accepted!!");

			dispatchPool->execute(new InternalClientEventTask(new ChallengeAcceptedMessageHandler(this, directoryClient, rakPeer), message, myPlayer, tempPlayer, &this->gamePlayers));

		} else if (messageId == protocol::CHALLENGERESPONSE) {
			//this player is not yet authenticated
			//they can only have sent us a challenge response

			logger->debug("Got Challenge Response!!!");

			dispatchPool->execute(new InternalClientEventTask(new ChallengeResponseMessageHandler(this, directoryClient, rakPeer), message, myPlayer, tempPlayer, &this->gamePlayers));

		} else if (messageId == protocol::CHALLENGE) {
			//we arent the host but we dont know who this client is
			//this can only be a challenge from another player
			//prepare the challenge response

			logger->debug("Sending Challenge Response!!!");

			//create a temp player to marshal this message through, since this client isnt authorized into the game yet
			//we got a challenge but we have no info yet about the client who sent it
			
			dispatchPool->execute(new InternalClientEventTask(new ChallengeMessageHandler(this), message, myPlayer, tempPlayer, &this->gamePlayers));

		} else {
			logger->warn("Client sent an invalid message");
		}
	} else {
		//handle authenticated clients

		//only authenticated clients should handle the new host message
		if (messageId == protocol::NEWHOST) {
			logger->debug("New host message!");
			Object<Runnable> updateTask = new DirectoryUpdateTask(this->theGameImPlaying, this->directoryClient, false);
			//otherwise just ask for any changes to the current game record
			dispatchPool->execute(updateTask);
		}

		//attach the protocol object to the event instead of the raw packet
		dispatchPool->execute(new GameClientEventTask(new ClientEventEmitter(this->eventListener), EVENT_OBJECT_MESSAGE, packet));
	}

	//dont delete the message because we are using it int he threads. Delete it when you are done with it
	//delete message;
}


void GameClient::sendProtocolObject(protocol::ProtocolMessage* message, RakNet::RakNetGUID guid) {
	std::string* msgData = this->createProtocolObjectMessage(message);
	this->sendProtocolMessage(msgData, &RakNet::AddressOrGUID(guid));
	delete msgData;
	delete message;
}

void GameClient::sendProtocolMessage(std::string* message, RakNet::AddressOrGUID* addr) {
	const static RakNet::AddressOrGUID sendAddr(RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	logger->debug("Sending Protocol Message!");

	if (addr != NULL) {
		logger->debug("Send to single host");
		//directsend
		rakPeer->Send((char*)message->c_str(), message->length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, *addr, false);
		return;
	}
	logger->debug("Send broadcast");
	//broadcast
	rakPeer->Send((char*)message->c_str(), message->length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, sendAddr, true);
}

std::string* GameClient::createProtocolObjectMessage(protocol::ProtocolMessage* message) {
	//TODO: make this more efficient
	std::string* outputStr = new std::string();

	message->SerializeToString(outputStr);


	std::string* packedStr = new std::string();
	packedStr->append(PROTOCOL_OBJECT_MESSAGE_BUF);
	packedStr->append(*outputStr);

	delete outputStr;

	return packedStr;
}

protocol::ProtocolMessage* GameClient::decodeProtocolObjectMessage(const unsigned char* message, int len) {
	if (message == NULL) {
		throw new FatalException("DecodeMessage: message is NULL");
	}
	char protocolMessageType = message[0];
	if (protocolMessageType != PROTOCOL_OBJECT_MESSAGE) {
		throw new FatalException("DecodeMessage: message has wrong protocol type");
	}
	
	protocol::ProtocolMessage* object = new protocol::ProtocolMessage();
	if (object->ParseFromArray(message+1, len-1) == false) {
		throw new ProtocolException("Can't parse protocol message");
	}

	return object;
}



const NetworkErrorState GameClient::getError() const {
	return this->errorState;
}


const void GameClient::clearErrorState() {
	this->errorState = ERROR_OK;
}





void GameClient::broadcast(const char* msg) {
	//rakPeer->Send(msg, (int) strlen(msg), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}




void GameClient::startRakNet(const char* hostName, const int port) {
	RakNet::RakPeerInterface *newPeer;

	newPeer = RakNet::RakPeerInterface::GetInstance();

	fcm2 = RakNet::FullyConnectedMesh2::GetInstance();
	cg2 = RakNet::ConnectionGraph2::GetInstance();

	newPeer->AttachPlugin(fcm2);
	newPeer->AttachPlugin(cg2);

	fcm2->SetAutoparticipateConnections(false);
	fcm2->SetConnectOnNewRemoteConnection(false, RakNet::RakString::emptyString.c_str);

	RakNet::SocketDescriptor sd;
	sd.port = port;


	RakNet::SystemAddress hostAddr(hostName, port);

	newPeer->SetMaximumIncomingConnections(7);
	newPeer->SetTimeoutTime(1000, hostAddr);

	startupFuture = new ClientFuture();

	RakNet::StartupResult result = newPeer->Startup(8, &sd,1);

	if (result != RakNet::RAKNET_STARTED) {
		stopClient();

		this->errorState = ERROR_CANT_REGISTER_CLIENT;

		startupFuture->lock();
		startupFuture->setCode(FUTURE_ERROR);
		startupFuture->setComplete(true);
		startupFuture->unlock();

		throw new FatalException("Error starting raknet");
	}

	fcm2->AddParticipant(newPeer->GetGuidFromSystemAddress(hostAddr));
	this->hostname = strdup(hostName);
	this->port = port;

	this->rakPeer = newPeer;
}



OCArray<Object<ServerGame>> GameClient::getAvailableGames() {
	Object<GameList> gameList = directoryClient->getGameList();
	if (gameList == NULL) {
		return NULL;
	}
	return gameList->getGameList();
}



Object<ClientFuture> GameClient::startGame(const char* hostName, int desiredPort) {
	if (hostName == NULL) {
		hostName = directoryClient->getExternalIp();
		if (hostName == NULL) {
			stopClient();

			throw new FatalException("Can't determine external ip");
		}
	}

	while (RakNet::SocketLayer::IsPortInUse(desiredPort)==true)
		desiredPort++;

	startRakNet(hostName, desiredPort);

	RakNet::RakNetGUID playerGuid = rakPeer->GetGuidFromSystemAddress(RakNet::SystemAddress(hostName, (unsigned short)desiredPort));

	Object<ServerGame> newGame = directoryClient->createGame(hostName, desiredPort, playerGuid.ToString());

	if (newGame == NULL) {
		stopClient();

		this->errorState = ERROR_CANT_REGISTER_CLIENT;

		startupFuture->lock();
		startupFuture->setCode(FUTURE_ERROR);
		startupFuture->setComplete(true);
		startupFuture->unlock();

		throw new FatalException("Can't create game");
	}

	this->theGameImPlaying = newGame;
	creator = true;

	//start the networking thread
	dispatcherThread = new Thread(this);
	dispatcherThread->start();

	//sett he connection state to authenticated, because we are implicitly authenticated into our own game
	this->myPlayer->setHost(hostName);
	this->myPlayer->setPort(desiredPort);
	this->myPlayer->setConnectionState(AUTHENTICATED);
	this->myPlayer->setClientGuid(playerGuid);
	this->gamePlayers.insertKeyAndValue(OCString(playerGuid.ToString()), this->myPlayer);
	fcm2->AddParticipant(playerGuid);

	return startupFuture;
}



Object<ClientFuture> GameClient::joinGame(const char* hostName, int desiredPort, Object<ServerGame> gameInfo) {
	if (hostName == NULL) {
		hostName = directoryClient->getExternalIp();
		if (hostName == NULL) {
			stopClient();

			throw new FatalException("Can't determine external ip");
		}
	}

	while (RakNet::SocketLayer::IsPortInUse(desiredPort)==true)
		desiredPort++;

	//startGame will throw an exception if it fails, dont call it from an object constructor
	startRakNet(hostName, desiredPort);

	RakNet::RakNetGUID playerGuid = rakPeer->GetGuidFromSystemAddress(RakNet::SystemAddress(hostName, (unsigned short)desiredPort));

	short gamePort = (short)gameInfo->getHostPort();

	RakNet::ConnectionAttemptResult result = rakPeer->Connect(gameInfo->getHostName(), gamePort,0,0);
	if (result != RakNet::CONNECTION_ATTEMPT_STARTED) {
		logger->error("ConnectionAttempt failed.");

		stopClient();

		this->errorState = ERROR_CANT_REGISTER_CLIENT;

		startupFuture->lock();
		startupFuture->setCode(FUTURE_ERROR);
		startupFuture->setComplete(true);
		startupFuture->unlock();

		return startupFuture;
	}

	this->theGameImPlaying = gameInfo;

	//start the networking thread
	dispatcherThread = new Thread(this);
	dispatcherThread->start();

	//sett he connection state to authenticated, because we are implicitly authenticated into our own game
	this->myPlayer->setHost(hostName);
	this->myPlayer->setPort(desiredPort);
	this->myPlayer->setConnectionState(CONNECTED);
	this->myPlayer->setClientGuid(playerGuid);
	this->gamePlayers.insertKeyAndValue(OCString(playerGuid.ToString()), this->myPlayer);
	fcm2->AddParticipant(playerGuid);

	return startupFuture;
}

bool GameClient::isCurrentGame(Object<ServerGame> game) {
	if (this->theGameImPlaying == NULL) {
		return false;
	}
	return game->getGameId() == this->theGameImPlaying->getGameId();
}



void GameClient::setEventListener(Object<GameClientEventListener> listener) {
	this->eventListener = listener;
}


Object<GameClientEventListener> GameClient::getEventListener() {
	return this->eventListener;
}



void GameClient::bootPlayer(Object<Player> player) {
	logger->warn("Booting player");

	char guidStr[255];
	player->getClientGuid().ToString(guidStr);

	//TODO: this is super inefficient
	Object<Player> gamePlayer = gamePlayers[guidStr];
	if (gamePlayer != NULL) {
		//player isnt in this game, try just booting the guid
		gamePlayers.remove(guidStr);
	}

	rakPeer->AddToBanList(rakPeer->GetSystemAddressFromGuid(player->getClientGuid()).ToString(false), 0);
	rakPeer->CancelConnectionAttempt(rakPeer->GetSystemAddressFromGuid(player->getClientGuid()));
	rakPeer->CloseConnection(player->getClientGuid(), false);
}


void GameClient::addPlayer(Object<Player> player) {
	logger->warn("Adding player");
	fcm2->AddParticipant(player->getClientGuid());
}






