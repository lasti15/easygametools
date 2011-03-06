package com.mar.website.server.util;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.GameManager;
import com.mar.website.server.model.Game;

/**
 * Do the game delete in a very small transaction
 * @author Matt
 *
 */
public class GameRemover {
	
	@Autowired
	private final GameManager gameManager = null;
	
	@Transactional
	public void removeGame(Game gameToRemove) {
		gameManager.deleteGame(gameToRemove.getId());
	}

}
