package com.mar.website.server.model.exceptions;

import java.io.Serializable;

public class WebServiceException extends Exception implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 279681584592770943L;
	
	//private ServiceMethodDescriptor method = null;

	public WebServiceException() {
		super();
	}
	
	public WebServiceException(String message) {
		super(message);
	}
	
	public WebServiceException(Throwable t) {
		super(t);
	}
	
	public WebServiceException(String message, Throwable t) {
		super(message, t);
	}

	//public void setMethod(ServiceMethodDescriptor method) {
	//	this.method = method;
	//}

	//public ServiceMethodDescriptor getMethod() {
	//	return method;
	//}
}
