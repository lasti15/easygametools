package com.mar.website.server.util;

import java.io.Serializable;

import com.mar.website.server.model.User;


public class SessionState implements Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = 4319012864492831418L;
	
	private String username = null;
	
	private User user;

	public void setUsername(String username) {
		this.username = username;
	}

	public String getUsername() {
		return username;
	}

	public void setUser(User user) {
		this.user = user;
	}

	public User getUser() {
		return user;
	}
}
