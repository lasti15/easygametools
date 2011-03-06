package com.mar.website.server.managers.impl;

import java.util.List;

import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.PersistenceException;
import javax.persistence.Query;

import org.apache.log4j.Logger;

import com.mar.website.server.managers.InstallManager;
import com.mar.website.server.model.Installation;
import com.mar.website.server.model.exceptions.NotAllowedException;

public class InstallManagerImpl implements InstallManager {
	
	@PersistenceContext
    private EntityManager em;
	
	private static final Logger log = Logger.getLogger(InstallManagerImpl.class);

	@Override
	@SuppressWarnings("unchecked")
	public Installation getActiveInstall() throws NotAllowedException {
		
		List<Installation> installs = null;
		try {
			Query query = em.createQuery("from "+ Installation.class.getName()+" where inUse=true");
			installs = query.getResultList();
		} catch (PersistenceException ex) {
			//the table doesnt yet exist
			return null;
		}
		
        //this must meen something weird is going on
        //get a db/sys admin asap
        int size = installs.size();
        if (size > 1 || size < 0) {
        	log.error("#############################BADBADBAD: Too many/Too few installs........BADBADBAD##############################");
        	throw new NotAllowedException();
        }
        
        //return the only install or none if it has not been installed yet
        if (installs.size() > 0) {
        	return installs.get(0);
        } else {
        	return null;
        }
        
	}

	@Override
	public void save(Installation install) throws NotAllowedException {
		em.persist(install);
	}

	@Override
	public void deactivateAllInstalls() throws NotAllowedException {
		Query query = em.createQuery("from "+ Installation.class.getName());
        @SuppressWarnings("unchecked")
		List<Installation> installs = query.getResultList();
        
        for (Installation install : installs) {
        	install.setInUse(false);
        	em.persist(install);
        }
        
        em.clear();
	}

}
