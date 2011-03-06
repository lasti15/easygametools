
#include "DirectoryClient.h"
#include "Web.h"
#include "Config.h"
#include "IrrCharArrayReaderSource.h"



using namespace ROG;

const static Logger* logger = Logger::getLogger("DirectoryClient");



static const openutils::WebForm* createWebRequest(const char* host, const int port, const char* script, const char* method) {
	logger->trace("Creating web request");
	openutils::WebForm* out = new openutils::WebForm();
	out->setHost(host);
	out->setScriptFile(script);
	out->setScriptMethod(method);
	out->setPort(port);

	return out;
}


DirectoryClient::DirectoryClient() {
	logger->debug("Loading Directory Client");
	//load the config file
	Object<ConfigLoader> config = ConfigLoader::getConfigLoader();
	if (config == NULL) {
		throw new FatalException("Can't Open Config");
	}

	char* hostName = (char*)config->get("DirectoryClient.hostName");
	if (hostName == NULL) {
		this->directoryHostName = "localhost";
	} else {
		this->directoryHostName = hostName;
	}

	if (logger->isDebugEnabled()) {
		char buf[100];
		_snprintf(buf, 99, "%s%s", "Hostname: ", hostName);
		logger->debug(buf);
	}

	int hostPort = config->getInt("DirectoryClient.hostPort");
	if (hostPort <= 1024) {
		this->directoryPort = 11223;
	} else {
		this->directoryPort = hostPort;
	}

	if (logger->isDebugEnabled()) {
		char buf[100];
		_snprintf(buf, 99, "%s%d", "Hostport: ", hostPort);
		logger->debug(buf);
	}

	char* context = (char*)config->get("DirectoryClient.context");
	if (context == NULL) {
		this->directoryContext = "/";
	} else {
		this->directoryContext = context;
	}

	if (logger->isDebugEnabled()) {
		char buf[100];
		_snprintf(buf, 99, "%s%d", "Context: ", context);
		logger->debug(buf);
	}

	sessionId = NULL;
}



Object<User> DirectoryClient::login(const char* username, const char* password) {

	logger->debug("Login!");

	//get the path to the login method
	char loginPath[255];
	_snprintf(loginPath, 254, "%s%s", this->directoryContext, "/login/login");
	logger->debug(loginPath);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, loginPath, "POST");
	wf->putVariable("username",username);
	wf->putVariable("password",password);

	wf->sendRequest("ROG");

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		return NULL;
	}

	if (sessionId != NULL) {
		delete [] sessionId;
	}
	//save the session id for this user
	sessionId = strdup(wf->getResponseCookies());

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open config file.");
	}

	Object<User> user = new User();
	user->loadFromReader(xmlReader);

	delete wf;

	return user;
}


Object<User> DirectoryClient::validateUser(long userId, long gameId) {
	logger->debug("validate!");

	//get the path to the login method
	char loginPath[255];
	_snprintf(loginPath, 254, "%s%s", this->directoryContext, "/gamer/validateUser");
	logger->debug(loginPath);

	char userIdStr[50];
	char gameIdStr[50];

	itoa(userId, userIdStr, 10);
	itoa(gameId, gameIdStr, 10);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, loginPath, "POST");
	wf->putVariable("userId",userIdStr);
	wf->putVariable("gameId",gameIdStr);

	wf->sendRequest("ROG");

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		return NULL;
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open config file.");
	}

	Object<User> user = new User();
	user->loadFromReader(xmlReader);

	delete wf;

	return user;
}



Object<GameList> DirectoryClient::getGameList() {

	logger->debug("GetGameList!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/findGames");
	logger->debug(gameListPath);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("page", "1");

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		return NULL;
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open config file.");
	}

	Object<GameList> gameList = new GameList();
	gameList->loadFromReader(xmlReader);

	delete wf;

	return gameList;
}


Object<ServerGame> DirectoryClient::createGame(const char* hostName, const int port, const char* hostGuid) {
	logger->debug("createGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/startGame");
	logger->debug(gameListPath);

	char portBuf[25];
	_snprintf(portBuf, 24, "%d", port);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("host", hostName);
	wf->putVariable("port", portBuf);
	wf->putVariable("guid", hostGuid);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		return NULL;
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open XML stream.");
	}

	Object<ServerGame> game = new ServerGame();
	game->loadFromReader(xmlReader);

	delete wf;

	return game;
}





Object<ServerGame> DirectoryClient::updateGame(Object<ServerGame> gameToUpdate) {
	logger->debug("updateGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/update");
	logger->debug(gameListPath);

	char gameBuf[255];
	_snprintf(gameBuf, 254, "<game id=\"%d\"> <numberOfPlayers>%d</numberOfPlayers> </game>", gameToUpdate->getGameId(), gameToUpdate->getNumberOfPlayers());

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("game", gameBuf);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		throw new FatalException("Could not update server game");
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open XML stream.");
	}

	Object<ServerGame> game = new ServerGame();
	game->loadFromReader(xmlReader);

	delete wf;

	return game;
}

Object<ServerGame> DirectoryClient::getUpdatedGame(long gameId) {
	logger->debug("getGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/get");
	logger->debug(gameListPath);

	char gameBuf[255];
	_snprintf(gameBuf, 254, "%d", gameId);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("gameId", gameBuf);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		throw new FatalException("Could not update server game");
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open XML stream.");
	}

	Object<ServerGame> game = new ServerGame();
	game->loadFromReader(xmlReader);

	delete wf;

	return game;
}


void DirectoryClient::releaseGame(long gameId, long userId, const char* nextHostGuid) {
	logger->debug("releaseGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/release");
	logger->debug(gameListPath);

	char gameBuf[255];
	_snprintf(gameBuf, 254, "%d", gameId);

	char userBuf[255];
	_snprintf(userBuf, 254, "%d", userId);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("gameId", gameBuf);
	wf->putVariable("newHostUserId", userBuf);
	wf->putVariable("newHostGuid", nextHostGuid);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		throw new FatalException("Could not release server game");
	}

	delete wf;
}


Object<ServerGame> DirectoryClient::adoptGame(long gameId, const char* hostName, const int port, const char* hostGuid) {
	logger->debug("adoptGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/adopt");
	logger->debug(gameListPath);

	char gameBuf[255];
	_snprintf(gameBuf, 254, "%d", gameId);

	char portBuf[25];
	_snprintf(portBuf, 24, "%d", port);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("gameId", gameBuf);
	wf->putVariable("host", hostName);
	wf->putVariable("port", portBuf);
	wf->putVariable("guid", hostGuid);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader((irr::io::IFileReadCallBack*)new IrrCharArrayReaderSource((const char*)wf->getResponseData(), wf->getResponseLength()), false);
	if (xmlReader == NULL) {
		delete wf;
		throw new FatalException("Cannot open XML stream.");
	}

	Object<ServerGame> game = new ServerGame();
	game->loadFromReader(xmlReader);

	delete wf;

	return game;
}



void DirectoryClient::deleteGame(long gameId) {
	logger->debug("deleteGame!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/stopGame");
	logger->debug(gameListPath);

	char gameBuf[255];
	_snprintf(gameBuf, 254, "%d", gameId);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "POST");
	wf->putVariable("gameId", gameBuf);

	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		throw new FatalException("Could not remove server game");
	}

	delete wf;
}


const char* DirectoryClient::getExternalIp() {
	logger->debug("getExternalIp!");

	//get the path to the login method
	char gameListPath[255];
	_snprintf(gameListPath, 254, "%s%s", this->directoryContext, "/gamer/getHostIp");
	logger->debug(gameListPath);

	openutils::WebForm* wf = (openutils::WebForm*)createWebRequest(this->directoryHostName, this->directoryPort, gameListPath, "GET");
	wf->sendRequest("ROG", this->sessionId);

	int response = wf->getResponseStatus();

	if (logger->isDebugEnabled()) {
		char responseCode[255];
		_snprintf(responseCode, 254, "HTTP: %d", response);
		logger->debug(responseCode);
	}

	if (response != 200) {
		delete wf;
		throw new FatalException("Could not remove server game");
	}

	if (wf->getResponseLength() > 1023) {
		delete wf;
		throw new FatalException("Could not remove server game");
	}

	int responseLen = wf->getResponseLength();
	char* externalIp = new char[responseLen + 1];
	memcpy((void*)externalIp, wf->getResponseData(), responseLen);
	externalIp[responseLen] = 0;

	delete wf;

	return externalIp;
}




