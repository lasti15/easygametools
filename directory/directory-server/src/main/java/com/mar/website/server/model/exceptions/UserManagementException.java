package com.mar.website.server.model.exceptions;

public class UserManagementException extends WebServiceException {

	private static final long serialVersionUID = 229059697206952119L;

	public UserManagementException() {
	}

	public UserManagementException(String message) {
		super(message);
	}

	public UserManagementException(Throwable t) {
		super(t);
	}

	public UserManagementException(String message, Throwable t) {
		super(message, t);
	}

}
