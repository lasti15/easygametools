package com.mar.website.server.model.exceptions;

public class LoginFailedException extends WebServiceException {

	public LoginFailedException() {
		super();
	}

	public LoginFailedException(String message, Throwable t) {
		super(message, t);
	}

	public LoginFailedException(String message) {
		super(message);
	}

	public LoginFailedException(Throwable t) {
		super(t);
	}

	private static final long serialVersionUID = -8110791221431029317L;

}
