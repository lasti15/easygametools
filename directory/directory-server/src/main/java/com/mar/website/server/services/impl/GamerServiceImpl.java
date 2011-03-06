package com.mar.website.server.services.impl;

import java.util.Date;
import java.util.HashSet;

import javax.servlet.http.HttpServletRequest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.GameManager;
import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.Game;
import com.mar.website.server.model.GameList;
import com.mar.website.server.model.User;
import com.mar.website.server.model.exceptions.WebServiceException;
import com.mar.website.server.services.AbstractMarService;
import com.mar.website.server.services.GamerService;
import com.mar.website.server.util.GameRemover;
import com.mar.website.server.util.SessionWrapper;

public class GamerServiceImpl extends AbstractMarService implements	GamerService {
	private static final long serialVersionUID = -7317985274296288812L;
	
	@Autowired
	private final UserManager userManager = null;

	@Autowired
	private final GameManager gameManager = null;
	
	@Autowired
	private final SessionWrapper sessionContext = null;
	
	@Autowired
	private final GameRemover gameRemover = null;
	
	
	@Override
	@Transactional
	public GameList getAvailableGames(int pageNum) {
		GameList gameList = new GameList(gameManager.findGames(null, null, null, null));
		return gameList;
	}

	@Override
	@Transactional
	public Game startGame(String host, int port, String hostGuid) throws WebServiceException {
		Game newGame = new Game();
		newGame.setHostName(host);
		newGame.setHostPort(port);
		newGame.setHostUser(sessionContext.getSession().getUser());
		newGame.setMaxPlayers(8);
		newGame.setLastUpdate(new Date().getTime());
		newGame.setHostGuid(hostGuid);
		
		newGame.setPlayers(new HashSet<User>());
		newGame.getPlayers().add(userManager.find(sessionContext.getSession().getUser().getId()));
		
		newGame = gameManager.createGame(newGame);
		
		sessionContext.getSession().setHostGameId(newGame.getId());
		
		return newGame;
	}

	@Override
	//this is so hacky, and will bottleneck
	@Transactional
	public Game updateGame(Game gameToUpdate) throws WebServiceException {
		synchronized(gameRemover) {
			Game game = gameManager.find(gameToUpdate.getId());
			//if we foudn the game then we beat the synchronizer to deleting it
			if (game != null) {
				//make sure the updater is actually the owner of this game
				//this is dicey when we have one player with 2 or more characters in a game
				if (sessionContext.getSession().getHostGameId() != null && sessionContext.getSession().getHostGameId().equals(gameToUpdate.getId())) {
					if (game.getHostUser().getId().equals(sessionContext.getSession().getUser().getId())) {
						game.setLastUpdate(new Date().getTime());
						game.setHostUser(userManager.find(sessionContext.getSession().getUser().getId()));
						
						return gameManager.updateGame(game);
					}
				}
			}
		}
		throw new WebServiceException("Cannot update game");
	}

	@Override
	@Transactional
	public void stopGame(Long gameToRemove) throws WebServiceException {
		synchronized(gameRemover) {
			Game game = gameManager.find(gameToRemove);
			//if we found the game then we beat the synchronizer to deleting it
			if (game != null) {
				//make sure the deleter is actually the owner of this game
				if (sessionContext.getSession().getHostGameId() != null && sessionContext.getSession().getHostGameId().equals(gameToRemove)) {
					if (game.getHostUser().getId().equals(sessionContext.getSession().getUser().getId())) {
						gameManager.deleteGame(gameToRemove);
					}
				}
			}
		}
	}

	@Override
	public String getHostIp(HttpServletRequest request) {
		return request.getRemoteHost();
	}


	@Override
	@Transactional
	public Game adoptGame(Long gameToAdopt, String host, int port,	String hostGuid) throws WebServiceException {
		synchronized(gameRemover) {
			Game game = gameManager.find(gameToAdopt);
			//if we foudn the game then the host didnt exit cleanly
			if (game != null) {
				//if we were the previous host too theres nothing to do, and in fact allowing any changes here would create a security hole
				//basically I could create a game as host, join it with another character of mine, then spoof the game
				//into letting my 2nd character become the host even though the first is still in the game and thinks it is host
				//then we would get intoa situation where 2 of the clients are actually technically valid game hosts and i just dont
				//want to deal with that
				if (game.getHostUser().getId().equals(sessionContext.getSession().getUser().getId())) {
					/*game.setLastUpdate(new Date().getTime());
					
					//setting this here is a huge security hole
					//we have no idea if this is actually the new host, or if
					//it is someone pretending to be the new host who's user was also the previous host
					sessionContext.getSession().setHostGameId(gameToAdopt);
					
					return gameManager.updateGame(game);*/
					
					//for now we are going to take a huge dump if the host did not exit cleanly but 
					//another of his characters wants to become the host
					//make sure the host exits properly if you want a child of theirs to be able 
					//to become the new host
					throw new WebServiceException("Can't validate new host");
				}
			}
			//create the new game
			Game newGame = new Game();
			newGame.setHostName(host);
			newGame.setHostPort(port);
			newGame.setHostUser(sessionContext.getSession().getUser());
			newGame.setMaxPlayers(8);
			newGame.setLastUpdate(new Date().getTime());
			newGame.setHostGuid(hostGuid);
			
			newGame.setPlayers(new HashSet<User>());
			newGame.getPlayers().add(userManager.find(sessionContext.getSession().getUser().getId()));
			
			newGame = gameManager.createGame(newGame);
			
			sessionContext.getSession().setHostGameId(newGame.getId());
			
			return newGame;
		}
	}

	@Override
	public Game getUpdatedGame(Long gameToFind) throws WebServiceException {
		synchronized(gameRemover) {
			Game game = gameManager.find(gameToFind);
			//if we foudn the game then we beat the synchronizer to deleting it
			if (game != null) {
				return game;
			}
			
			throw new WebServiceException("Invalid game");
		}
	}

	@Override
	@Transactional
	public User validateUser(Long userId, Long gameId) throws WebServiceException {
		//make sure this user is who they say they are and can actually join this game
		if (userId == null) {
			throw new WebServiceException("Invalid user");
		}
		
		User user = userManager.find(userId);
		if (user == null) {
			throw new WebServiceException("Invalid user");
		}
		
		//make sure we are actually the host of this game.
		Game game = gameManager.find(gameId);
		if (game == null) {
			throw new WebServiceException("Invalid game");
		}
		
		//amke sure only the host validates users
		if (sessionContext.getSession().getHostGameId() != null && sessionContext.getSession().getHostGameId().equals(gameId)) {
			//this player is validated, add them to the list of players
			game.getPlayers().add(user);
		}
		
		gameManager.updateGame(game);

		return user;
	}

}
