package com.mar.website.server.services.impl;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.providers.encoding.PasswordEncoder;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.InstallManager;
import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.Installation;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.NotAllowedException;
import com.mar.website.server.model.exceptions.ValidationException;
import com.mar.website.server.model.exceptions.WebServiceException;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.impl.Validators;
import com.mar.website.server.services.AbstractMarService;
import com.mar.website.server.services.InstallService;
import com.mar.website.server.util.HibernateHelper;

public class InstallServiceImpl extends AbstractMarService implements InstallService {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 5630803670156333016L;
	
	@Autowired
	private final UserManager userManager = null;
	
	@Autowired
	private final InstallManager installManager = null;
	
	@Autowired
	private final PasswordEncoder passwordEncoder = null;
	
	@Autowired
	private final ValidationManager valid = null;
	
	private String applicationPinNumber;
	
	private String installerDbPassword;
	
	private void checkInstalled(String pin) throws NotAllowedException, ValidationException {
		Installation install = installManager.getActiveInstall();
		if (install != null) {
			//check to see if it we have the correct pin number
			pin = (String) valid.validate(pin, Validators.PIN_VALIDATOR);
			if (pin.trim().equals(install.getPin())) {
				installManager.deactivateAllInstalls();
				return;
			}
			throw new NotAllowedException();
		}
	}

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public void install(String pin) throws WebServiceException {
		checkInstalled(pin);

		HibernateHelper.exportDbSchema(installerDbPassword);
		
		System.out.println("Creating admin accounts");
		
		List<UserRole> roles = new ArrayList<UserRole>();
		roles.add(UserRole.ROLE_SUPERUSER);
		roles.add(UserRole.ROLE_USER);
		
		
		User matt = new User();
		matt.setUsername("mattadmin");
		matt.setPassword(passwordEncoder.encodePassword("p@55W0rd", "mattadmin"));
		matt.setEmail("mattrochon@gmail.com");
		matt.setActive(true);
		matt.setRole(UserRole.ROLE_USER);
		
		userManager.save(matt);

		System.out.println("Admins Created, persisting any application data");
		
		Installation installed = new Installation();
		installed.setPin((pin != null) ? pin : applicationPinNumber);
		installed.setInUse(true);
		installed.setInstallDate(new Date());
		
		installManager.save(installed);
		
		System.out.println("Install completed");
	}

	public void setApplicationPinNumber(String applicationPinNumber) {
		this.applicationPinNumber = applicationPinNumber;
	}

	public String getApplicationPinNumber() {
		return applicationPinNumber;
	}

	public void setInstallerDbPassword(String installerDbPassword) {
		this.installerDbPassword = installerDbPassword;
	}

	public String getInstallerDbPassword() {
		return installerDbPassword;
	}

	@Override
	@Transactional(rollbackFor={WebServiceException.class, RuntimeException.class})
	public void resetAdmin() throws WebServiceException {
		User matt = new User();
		matt.setUsername("mattadmin");
		matt.setPassword(passwordEncoder.encodePassword("p@55W0rd", "mattadmin"));
		matt.setEmail("mattrochon@gmail.com");
		matt.setActive(true);
		matt.setRole(UserRole.ROLE_USER);
		
		userManager.save(matt);
	}
}
