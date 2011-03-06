package com.mar.website.server.managers;

import java.util.List;

import com.mar.website.server.model.Game;

public interface GameManager {
	Game find(Long gameId);
	
	Game findByUserId(Long userId);
	
	List<Game> findGames(Long gameId, Long userId, String hostLike, Integer port);
	
	Game createGame(Game gameToCreate);
	
	void deleteGame(Long gameToDelete);
	
	Game updateGame(Game gameToUpdate);
	
	List<Long> getAllGameIds(Long timeoutMillis);
	
	Boolean isPlaying(Long gameId, Long userId);
}
