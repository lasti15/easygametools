package com.mar.website.server.managers.impl;

import java.util.ArrayList;
import java.util.List;

import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import javax.persistence.PersistenceContext;
import javax.persistence.Query;

import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserActivation;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;


public class UserManagerImpl implements UserManager {
	public static final List<UserRole> defaultUserRoles = new ArrayList<UserRole>();

	@PersistenceContext
    private EntityManager em;
	
	static {
		defaultUserRoles.add(UserRole.ROLE_SUPERUSER);
	}

	@Override
	public void delete(Long userId) throws WebServiceException {

			Query query = em.createQuery("from "+ User.class.getName() +" where id=:id");
			query.setParameter("id", userId);
			
			em.remove((User) query.getSingleResult());
	}
	
	@Override
	public User find(Long userId) throws WebServiceException {
		Query query = em.createQuery("from "+ User.class.getName() +" where id=:id");
		query.setParameter("id", userId);

		User result = null;
		try {
			result = (User) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}

	@Override
	public User find(String username) throws WebServiceException {
		Query query = em.createQuery("from "+ User.class.getName() +" where username=:username");
		query.setParameter("username", username);

		User result = null;
		try {
			result = (User) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}
	
	@Override
	public User findByEmail(String emailAddress) throws WebServiceException {
		Query query = em.createQuery("from "+ User.class.getName() +" where email=:emailAddress");
		query.setParameter("emailAddress", emailAddress);

		User result = null;
		try {
			result = (User) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}

	@SuppressWarnings("unchecked")
	@Override
	public List<User> list() throws WebServiceException {
		Query query = em.createQuery("from "+ User.class.getName());
        return query.getResultList();
	}

	@Override
	public void save(User u) throws WebServiceException {
		em.persist(u);
	}
	
	@Override
	public void merge(User u) throws WebServiceException {
		em.merge(u);
	}

	
	public void saveActivation(UserActivation activate) throws WebServiceException {
		em.persist(activate);
	}
	
	public UserActivation getActivation(String activationCode) throws WebServiceException {
		Query query = em.createQuery("from "+ UserActivation.class.getName() +" where activationCode=:code");
		query.setParameter("code", activationCode);
		
		UserActivation result = null;
		try {
			result = (UserActivation) query.getSingleResult();
		} catch (NoResultException e) {
			//allow this to fall through
		}
		
		return result;
	}
	
	public void removeActivation(UserActivation activation) throws WebServiceException {
		em.remove(activation);
	}

	@SuppressWarnings("unchecked")
	@Override
	public List<User> findOnline(Long threshold) {
		Query query = em.createQuery("from "+ User.class.getName() + " WHERE lastUpdated > :updateTime");
		
		Long updateTime = System.currentTimeMillis() - threshold;
		query.setParameter("updateTime", updateTime);
		
        return query.getResultList();
	}

}
