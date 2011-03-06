package com.mar.website.server.model.exceptions;

public class UnavailableException extends WebServiceException {
	private static final long serialVersionUID = 4924950354134526014L;

	public UnavailableException() {
	}

	public UnavailableException(String message) {
		super(message);
	}

	public UnavailableException(Throwable t) {
		super(t);
	}

	public UnavailableException(String message, Throwable t) {
		super(message, t);
	}

}
