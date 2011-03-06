/**
 * copyright 2009 MarSoftware.
 */

package com.mar.website.server.util;

import java.util.Date;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.dao.DataAccessException;
import org.springframework.security.GrantedAuthority;
import org.springframework.security.GrantedAuthorityImpl;
import org.springframework.security.providers.encoding.PasswordEncoder;
import org.springframework.security.userdetails.UserDetails;
import org.springframework.security.userdetails.UserDetailsService;
import org.springframework.security.userdetails.UsernameNotFoundException;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;

public class MarUserDetailsService implements UserDetailsService {
	
	@Autowired
	private SessionWrapper sessionState;
	
	@Autowired
	private UserManager userManager;
	
	@Autowired
	private final PasswordEncoder passwordEncoder = null;
	
	private String installerUsername;
	private String installerPassword;
	private Boolean useInstaller;

	@Override
	@Transactional
	public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException, DataAccessException {
	
		if (username == null) {
			SessionState session = sessionState.getSession();
			if (session == null) {
				throw new UsernameNotFoundException("null");
			}
			username = session.getUsername();
			if (username == null) {
				throw new UsernameNotFoundException("null");
			}
		}
		
		if (username.equals(this.installerUsername) && useInstaller) {
			UserDetails userDetails = new org.springframework.security.userdetails.User(
					this.installerUsername, passwordEncoder.encodePassword(this.installerPassword, this.installerUsername), true, true, true, true, 
					new GrantedAuthorityImpl[] {new GrantedAuthorityImpl(UserRole.ROLE_INSTALLER.toString())});
			return userDetails;
		}
		
		User user = null;
		try {
			user = userManager.find(username);
		} catch (WebServiceException e) {
			throw new UsernameNotFoundException(e.getMessage());
		}
		
		if (user == null) {
			throw new UsernameNotFoundException(username);
		}
		
		if (!user.isActive()) {
			throw new UsernameNotFoundException("Account has not been activated");
		}
		
		//set the last login date
		user.setLastLogin(new Date());
		
		//set the session info before storing the last login date
		@SuppressWarnings("unused")
		SessionState oldSession = sessionState.getSession();
		sessionState.setSession(new SessionState());
		sessionState.getSession().setUsername(username);
		sessionState.getSession().setUser(user);
		
		//save the last login date
		try {
			userManager.save(user);
		} catch (WebServiceException e) {
			throw new DataAccessException(e.getMessage()) {
				private static final long serialVersionUID = -2077826631228371832L;};
		}
		
		UserDetails userDetails = new org.springframework.security.userdetails.User(
				user.getUsername(), user.getPassword(), true, true, true, true, convertRoles(user.getRole()));
		
		return userDetails;
	}
	
	private GrantedAuthority [] convertRoles(UserRole roles) {
		GrantedAuthority [] auths = new GrantedAuthority[1];
		
		int i = 0;
		//for (UserRole role : roles) {
			auths[i] = new GrantedAuthorityImpl(roles.toString());
			//i++;
		//}
		return auths;
	}

	public void setInstallerUsername(String installerUsername) {
		this.installerUsername = installerUsername;
	}

	public String getInstallerUsername() {
		return installerUsername;
	}

	public void setInstallerPassword(String installerPassword) {
		this.installerPassword = installerPassword;
	}

	public String getInstallerPassword() {
		return installerPassword;
	}

	public void setUseInstaller(Boolean useInstaller) {
		this.useInstaller = useInstaller;
	}

	public Boolean getUseInstaller() {
		return useInstaller;
	}

}
