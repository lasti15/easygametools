package com.mar.website.server.services.impl;

import java.util.Date;
import java.util.List;

import org.apache.log4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;

import com.mar.website.server.managers.GameManager;
import com.mar.website.server.model.Game;
import com.mar.website.server.services.InactiveGameCleaner;
import com.mar.website.server.util.GameRemover;

public class InactiveGameCleanerImpl implements InactiveGameCleaner {
	
	private static final Logger logger = Logger.getLogger(InactiveGameCleanerImpl.class);
	
	@Autowired
	private final GameManager gameManager = null;
	
	@Autowired
	private final GameRemover gameRemover = null;

	@Override
	public void cleanOldGames() {
		if (gameManager == null) {
			logger.error("GameManager is null!");
			return;
		}
		
		logger.info("Cleaning up old games");
		
		List<Long> games = gameManager.getAllGameIds(120000L);
		if (games.size() > 0) {
			logger.info("Found [" + games.size() + "] games to cleanup");
			
			//we arent locking the records, so for every game we found we need to re-select it from the database
			for (Long gameId : games) {
				//this is so hacky, and will bottleneck
				synchronized(gameRemover) {
					Game game = gameManager.find(gameId);
					if (new Date().getTime() - game.getLastUpdate() > 120000L) {
						gameRemover.removeGame(game);
					}
				}
			}
		}
	}

}
