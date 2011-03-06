package com.mar.website.server.util;

import java.io.IOException;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;
import org.springframework.security.AccessDeniedException;
import org.springframework.web.servlet.HandlerExceptionResolver;
import org.springframework.web.servlet.ModelAndView;

import com.mar.website.server.model.exceptions.LoginFailedException;
import com.mar.website.server.model.exceptions.UnavailableException;
import com.mar.website.server.model.exceptions.ValidationException;

public class ExceptionMapper implements HandlerExceptionResolver {
	
	private static final Logger log = Logger.getLogger(ExceptionMapper.class);

	@Override
	public ModelAndView resolveException(HttpServletRequest request, HttpServletResponse response, Object handler, Exception ex) {

		//default error and message codes
		int errorCode = 599;
		String errorMessage = ex.getMessage();
		
		log.warn(ex.getMessage(), ex);
		
		String formLogin = request.getParameter("formLogin");
		//if its accessdenied lets let the framework handle it so we can still use the default login page
		//only let the framework handle it if we tell it to, the default behaviour of throwing an error
		//tot he client is desired when we are in normal operations
		if (ex instanceof AccessDeniedException && formLogin != null && formLogin.equals("true")) {
			return null;
		}
		
		if (ex instanceof UnavailableException) {
			errorCode = 549;
		} else
		if (ex instanceof LoginFailedException) {
			errorCode = 598;
		} else
		if (ex instanceof ValidationException) {
			errorCode = 597;
			errorMessage = "Validation Failed";
		}
		
		try {
			//response.addHeader("Location", "error?errorCode="+errorCode);
			response.sendError(errorCode, errorMessage);
		} catch (IOException e) {
			log.warn(e.getMessage(), e);
		}

		try {
		return new ModelAndView((String)null);
		} catch (Throwable t) {
			return null;
		}
	}

}
