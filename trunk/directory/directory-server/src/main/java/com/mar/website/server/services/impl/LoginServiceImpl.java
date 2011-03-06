package com.mar.website.server.services.impl;

import java.io.ByteArrayOutputStream;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.xml.transform.stream.StreamResult;

import org.apache.log4j.Logger;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.oxm.jaxb.Jaxb2Marshaller;
import org.springframework.security.Authentication;
import org.springframework.security.AuthenticationException;
import org.springframework.security.context.SecurityContext;
import org.springframework.security.context.SecurityContextHolder;
import org.springframework.security.providers.ProviderManager;
import org.springframework.security.providers.UsernamePasswordAuthenticationToken;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.context.ServletContextAware;
import org.springframework.web.context.support.WebApplicationContextUtils;

import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.User;
import com.mar.website.server.model.exceptions.LoginFailedException;
import com.mar.website.server.model.exceptions.WebServiceException;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.impl.Validators;
import com.mar.website.server.services.AbstractMarService;
import com.mar.website.server.services.LoginService;
import com.mar.website.server.util.SessionState;
import com.mar.website.server.util.SessionWrapper;

public class LoginServiceImpl extends AbstractMarService implements LoginService, ServletContextAware {
	
	@SuppressWarnings("unused")
	private static final Logger log = Logger.getLogger(LoginServiceImpl.class);

	private static final long serialVersionUID = 8157702297410648222L;
	
	@Autowired
	private SessionWrapper sessionState;
	
	@Autowired
	private UserManager userManager;
	
	@Autowired
	private final ValidationManager valid = null;
	
	@Autowired
	private final Jaxb2Marshaller marshaller = null;

	private ServletContext servletContext;

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String isLoggedIn(HttpServletResponse response) throws WebServiceException {
		if (sessionState.getSession() == null ) {
			return null;
		}
		User u = userManager.find(sessionState.getSession().getUsername());
		if (u == null) {
			throw new LoginFailedException("Login failed");
		}
		
		ByteArrayOutputStream result = new ByteArrayOutputStream();
		StreamResult jaxbResult = new StreamResult(result);
		marshaller.marshal(u, jaxbResult);

		response.setContentType("application/xml");
		
		return result.toString();
	}

	@Override
	public String login(HttpServletRequest request, HttpServletResponse response, String username, String password) throws WebServiceException {
		username = (String) valid.validate(username.trim(), Validators.USERNAME_VALIDATOR);
		password = (String) valid.validate(password.trim(), Validators.PASSWORD_VALIDATOR);

		SecurityContext context = SecurityContextHolder.getContext();
		
		User u = null;
		
		try{
			ProviderManager pm = (ProviderManager)WebApplicationContextUtils.getWebApplicationContext(servletContext).getBean("authenticationManager");
			Authentication auth = new UsernamePasswordAuthenticationToken(username, password);
			
			pm.doAuthentication(auth);
			context.setAuthentication(auth);
			
			try {
			u = updateUserRecord(username);
			} catch (Exception ignored){};
			
		}catch(AuthenticationException e){
		    context.setAuthentication(null);
		    throw new LoginFailedException("Login failed");
		}catch(Exception e){
			context.setAuthentication(null);
			throw new WebServiceException(e);
		}
		
		String sessionId = request.getSession().getId();
		response.addHeader("Set-Cookie", sessionId);

		ByteArrayOutputStream result = new ByteArrayOutputStream();
		StreamResult jaxbResult = new StreamResult(result);
		marshaller.marshal(u, jaxbResult);
	    
	    response.setContentType("application/xml");

	    return result.toString();
	}
	
	/**
	 * Put this in a separate method so that the entire login method is not transactional
	 * @param username
	 * @return
	 * @throws WebServiceException
	 */
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	private User updateUserRecord(String username) throws WebServiceException {
		User u = userManager.find(username);
		
		if (u != null) {
			SessionState state = new SessionState();
			state.setUsername(u.getUsername());
			state.setUser(u);
			
			sessionState.setSession(state);
		}
		return u;
	}

	@Override
	public void setServletContext(ServletContext servletContext) {
		this.servletContext = servletContext;
	}

}
