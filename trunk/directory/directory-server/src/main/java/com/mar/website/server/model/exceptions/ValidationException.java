package com.mar.website.server.model.exceptions;

public class ValidationException extends WebServiceException {

	private static final long serialVersionUID = 4283342148201649428L;

	public ValidationException() {}
	public ValidationException(String msg) {
		super(msg);
	}
	public ValidationException(Throwable t) {
		super(t);
	}
	public ValidationException(String msg, Throwable t) {
		super(msg, t);
	}
}
