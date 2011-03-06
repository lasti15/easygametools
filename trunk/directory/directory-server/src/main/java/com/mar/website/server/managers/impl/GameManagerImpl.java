package com.mar.website.server.managers.impl;

import java.util.Date;
import java.util.List;

import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;

import org.apache.log4j.Logger;

import com.mar.website.server.managers.GameManager;
import com.mar.website.server.model.Game;

public class GameManagerImpl implements GameManager {
	
	@SuppressWarnings("unused")
	private static final Logger log = Logger.getLogger(GameManagerImpl.class);
	
	@PersistenceContext
    private EntityManager em;

	@Override
	public Game find(Long gameId) {
		Query query = em.createQuery("from "+ Game.class.getName() +" where id=:id");
		query.setParameter("id", gameId);

		Game result = null;
		try {
			result = (Game) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}

	@Override
	public Game findByUserId(Long userId) {
		Query query = em.createQuery("from "+ Game.class.getName() +" g where g.hostUser.id=:id");
		query.setParameter("id", userId);

		Game result = null;
		try {
			result = (Game) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}
	
	
	private final Query filteredQuery(Long gameId, Long userId, String hostLike, Integer port) {
		StringBuffer queryStr = new StringBuffer("from "+ Game.class.getName() +" g");
		
		int clauseCount = 0;
		
		//i hate building queries like this
		if (gameId != null) {
			queryStr.append(" WHERE g.id=:gameId");
			
			clauseCount++;
		}
		
		if (userId != null) {
			if (clauseCount < 1) {
				queryStr.append(" WHERE ");
			} else {
				queryStr.append(" AND ");
			}
			
			queryStr.append("g.hostUser.id=:userId");
			
			clauseCount++;
		}
		
		if (hostLike != null) {
			if (clauseCount < 1) {
				queryStr.append(" WHERE ");
			} else {
				queryStr.append(" AND ");
			}
			
			queryStr.append("g.hostName LIKE '%"+hostLike+"%'");
			
			clauseCount++;
		}
		
		
		if (port != null) {
			if (clauseCount < 1) {
				queryStr.append(" WHERE ");
			} else {
				queryStr.append(" AND ");
			}
			
			queryStr.append("g.hostPort=:hostPort");
			
			clauseCount++;
		}
		
		
		if (clauseCount < 1) {
			queryStr.append(" WHERE ");
		} else {
			queryStr.append(" AND ");
		}
		queryStr.append("g.hostGuid is not null");

		//create the query
		Query query = em.createQuery(queryStr.toString());
		
		//set the parameters
		if (gameId != null) query.setParameter("gameId", gameId);
		if (userId != null) query.setParameter("userId", userId);
		if (port != null) query.setParameter("hostPort", port);
		
		return query;
	}

	@SuppressWarnings("unchecked")
	@Override
	public List<Game> findGames(Long gameId, Long userId, String hostLike, Integer port) {

		Query query = filteredQuery(gameId, userId, hostLike, port);
		//query.setParameter("id", userId);

		List<Game> result = null;
		try {
			result = (List<Game>) query.getResultList();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}

	@Override
	public Game createGame(Game gameToCreate) {
		em.persist(gameToCreate);
		return gameToCreate;
	}

	@Override
	public void deleteGame(Long gameToDelete) {
		Query query = em.createQuery("from "+ Game.class.getName() +" where id=:id");
		query.setParameter("id", gameToDelete);
		
		em.remove((Game) query.getSingleResult());
	}

	@Override
	public Game updateGame(Game gameToUpdate) {
		em.merge(gameToUpdate);
		return gameToUpdate;
	}

	@SuppressWarnings("unchecked")
	@Override
	public List<Long> getAllGameIds(Long timeoutMillis) {
		String querySql = "SELECT g.id FROM "+ Game.class.getName() +" g";
		
		if (timeoutMillis != null && timeoutMillis > 0) {
			Date now = new Date();
			Long timeoutTimeInMillis = now.getTime() - timeoutMillis;
			querySql += " WHERE g.lastUpdate < " + timeoutTimeInMillis;
		}
		
		Query query = em.createQuery(querySql);

		List<Long> result = null;
		try {
			result = (List<Long>) query.getResultList();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}

	@Override
	public Boolean isPlaying(Long gameId, Long userId) {
		String querySql = "SELECT true FROM User_X_Game where userId=:userId and gameId=:gameID";

		Query query = em.createQuery(querySql);
		query.setParameter("gameId", gameId);
		query.setParameter("userId", userId);
		
		return (Boolean) query.getSingleResult();
	}

}
