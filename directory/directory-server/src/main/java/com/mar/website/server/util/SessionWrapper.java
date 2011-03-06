package com.mar.website.server.util;

import java.io.Serializable;

import org.springframework.beans.factory.annotation.Autowired;

public class SessionWrapper implements Serializable {
	private static final long serialVersionUID = -1678221645373860205L;
	
	
	@Autowired
	private SessionState session;

	public void setSession(SessionState session) {
		this.session = session;
	}

	public SessionState getSession() {
		return session;
	}
}
