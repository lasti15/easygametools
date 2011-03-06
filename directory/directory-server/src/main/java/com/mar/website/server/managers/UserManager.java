package com.mar.website.server.managers;

import java.util.List;

import com.mar.website.server.model.User;
import com.mar.website.server.model.UserActivation;
import com.mar.website.server.model.exceptions.WebServiceException;

public interface UserManager {
	User find(String username) throws WebServiceException;
	
	User find(Long userId) throws WebServiceException;
	
	User findByEmail(String emailAddress) throws WebServiceException;
	
	void delete(Long userId) throws WebServiceException;

	List<User> list() throws WebServiceException;
	
	List<User> findOnline(Long thresholdMillis);

	void save(User u) throws WebServiceException;
	
	void merge(User u) throws WebServiceException;
	
	UserActivation getActivation(String activationCode) throws WebServiceException;
	
	void saveActivation(UserActivation activate) throws WebServiceException;
	
	void removeActivation(UserActivation activation) throws WebServiceException;
}
