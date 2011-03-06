package com.mar.website.server.services.impl;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.servlet.http.HttpServletResponse;

import junit.framework.Assert;

import org.apache.log4j.Logger;
import org.hsqldb.lib.MD5;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.context.SecurityContext;
import org.springframework.security.context.SecurityContextHolder;
import org.springframework.security.providers.encoding.PasswordEncoder;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.email.EmailHelper;
import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserActivation;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;
import com.mar.website.server.model.merge.Merge;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.impl.Validators;
import com.mar.website.server.services.AbstractMarService;
import com.mar.website.server.services.UserService;
import com.mar.website.server.util.SessionWrapper;

public class UserServiceImpl extends AbstractMarService implements UserService {
	
	@Autowired
	private final SessionWrapper state = null;

	@Autowired
	private final UserManager userManager = null;
	
	@Autowired
	private final Merge merge = null;
	
	@Autowired
	private final EmailHelper email = null;
	
	@Autowired
	private final PasswordEncoder passwordEncoder = null;
	
	@Autowired
	private final ValidationManager valid = null;

	private static final long serialVersionUID = -8586264685601532583L;
	
	@SuppressWarnings("unused")
	private static final Logger log = Logger.getLogger(UserServiceImpl.class);
	
	
	
	private String activateSuccessPath;
	private String activateFailPath;
	private String resetSuccessPath;
	private String resetFailPath;
	private String resetPath;
	
	

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String saveMyAccount(User u) throws WebServiceException {
		u = (User) valid.validate(u, Validators.USER_VALIDATOR);
		
		Assert.assertEquals(state.getSession().getUser().getId(), u.getId());
		Assert.assertEquals(state.getSession().getUser().getUsername(), u.getUsername());
		
		User exists = userManager.find(u.getUsername());
		if (exists == null) {
			throw new WebServiceException("Doesnt Exists");
		}

		merge.merge(exists, u);
		
		userManager.merge(exists);
		//if it didnt throw an exception it saved correctly
		return "true";
	}
	
	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String resetPassword(HttpServletResponse response, String activationCode, String newPassword, String newPasswordConfirm) throws WebServiceException, IOException {
		activationCode = (String) valid.validate(activationCode.trim(), Validators.ACTIVATION_VALIDATOR);
		newPassword = (String) valid.validate(newPassword.trim(), Validators.PASSWORD_VALIDATOR);
		newPasswordConfirm = (String) valid.validate(newPasswordConfirm.trim(), Validators.PASSWORD_VALIDATOR);

		Assert.assertTrue(newPassword.equals(newPasswordConfirm));
		
		UserActivation activation = userManager.getActivation(activationCode);
		if (activation == null) {
			response.sendRedirect(resetFailPath);
		}
		
		User u = activation.getUser();
		u.setActive(true);
		u.setPassword(passwordEncoder.encodePassword(newPassword, u.getUsername()));
		
		userManager.save(u);
		userManager.removeActivation(activation);
		
		response.sendRedirect(resetSuccessPath);
		
		return "true";
	}
	
	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String forgotPassword(String username, String emailAddress) throws WebServiceException {
		username = (String) valid.validate(username.trim(), Validators.USERNAME_VALIDATOR);
		emailAddress = (String) valid.validate(emailAddress.trim(), Validators.EMAIL_VALIDATOR);
		
		User u = userManager.find(username);
		if (u == null) {
			throw new WebServiceException("Invalid");
		}
		
		Assert.assertTrue(emailAddress.equals(u.getEmail()));
		
		//send the confirmation email
		UserActivation activation = new UserActivation();
		activation.setUser(u);
		activation.setPasswordReset(true);
		u.setActive(false);
		String activationCode = createRandomActivationCode();
		activation.setActivationCode(activationCode);
		userManager.saveActivation(activation);
		userManager.save(u);
		
		Map<String, Object> properties = new HashMap<String, Object>();
		properties.put("activationCode", activationCode);
		properties.put("username", u.getUsername());
		properties.put("resetPath", u.getUsername());
		
		email.sendMessage(u.getEmail(), "SeeMyTablet Forgotten Password", "/templates/forgotpassword.html", properties);
		
		return "true";
	}
	
	@Override
	public String forgotUsername(String emailAddress) throws WebServiceException {
		emailAddress = (String) valid.validate(emailAddress.trim(), Validators.EMAIL_VALIDATOR);
		
		User u = userManager.findByEmail(emailAddress);
		if (u == null) {
			throw new WebServiceException("Invalid");
		}
		
		String activationCode = createRandomActivationCode();

		Map<String, Object> properties = new HashMap<String, Object>();
		properties.put("activationCode", activationCode);
		properties.put("username", u.getUsername());
		
		email.sendMessage(u.getEmail(), "SeeMyTablet Forgotten Username", "/templates/forgotusername.html", properties);
		
		return "true";
	}

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String registerAccount(User u) throws WebServiceException {
		u = (User) valid.validate(u, Validators.USER_VALIDATOR);
		
		User exists = userManager.find(u.getUsername());
		if (exists != null) {
			throw new WebServiceException("Already Exists");
		}

		String hashed = passwordEncoder.encodePassword(u.getPassword().trim(), u.getUsername().trim());
		u.setPassword(hashed);
		u.setRole(UserRole.ROLE_USER);
		u.setActive(false);
		userManager.save(u);
		
		//send the confirmation email
		UserActivation activation = new UserActivation();
		activation.setUser(u);
		activation.setPasswordReset(false);
		String activationCode = createRandomActivationCode();
		activation.setActivationCode(activationCode);
		userManager.saveActivation(activation);
		
		Map<String, Object> properties = new HashMap<String, Object>();
		properties.put("activationCode", activationCode);
		properties.put("username", u.getUsername());
		
		email.sendMessage(u.getEmail(), "SeeMyTablet email confirmation", "/templates/confirmation.html", properties);
		
		return "true";
	}
	
	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String activate(HttpServletResponse response, String activationCode) throws WebServiceException, IOException {
		activationCode = (String) valid.validate(activationCode.trim(), Validators.ACTIVATION_VALIDATOR);
		
		UserActivation activation = userManager.getActivation(activationCode);
		if (activation == null) {
			response.sendRedirect(activateFailPath);
			return "false";
		}
		
		User u = activation.getUser();
		u.setActive(true);
		
		userManager.save(u);
		userManager.removeActivation(activation);
		
		response.sendRedirect(activateSuccessPath);

		return "true";
	}
	
	private String createRandomActivationCode() {
		Long rand1 = ((Double)(Math.random() * (Long.MAX_VALUE * Math.random()))).longValue();
		Long rand2 = ((Double)(Math.random() * (Long.MIN_VALUE * Math.random()))).longValue();
		String simpleCode = rand1.toString() + rand2.toString();
		return MD5.encodeString(simpleCode, null);
	}

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public String unregisterAccount() throws WebServiceException {
		User u = state.getSession().getUser();
		
		User exists = userManager.find(u.getUsername());
		if (exists == null) {
			throw new WebServiceException("Doesnt Exists");
		}
		
		userManager.delete(exists.getId());
		
		//logout this user who just unregistered
		SecurityContext context = SecurityContextHolder.getContext();
		context.setAuthentication(null);

		return "true";
	}

	public void setActivateSuccessPath(String activateSuccessPath) {
		this.activateSuccessPath = activateSuccessPath;
	}

	public String getActivateSuccessPath() {
		return activateSuccessPath;
	}

	public void setActivateFailPath(String activateFailPath) {
		this.activateFailPath = activateFailPath;
	}

	public String getActivateFailPath() {
		return activateFailPath;
	}

	public void setResetSuccessPath(String resetSuccessPath) {
		this.resetSuccessPath = resetSuccessPath;
	}

	public String getResetSuccessPath() {
		return resetSuccessPath;
	}

	public void setResetFailPath(String resetFailPath) {
		this.resetFailPath = resetFailPath;
	}

	public String getResetFailPath() {
		return resetFailPath;
	}

	public void setResetPath(String resetPath) {
		this.resetPath = resetPath;
	}

	public String getResetPath() {
		return resetPath;
	}

}
