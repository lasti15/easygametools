package com.mar.website;

import java.util.Date;
import java.util.List;

import junit.framework.Assert;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.providers.encoding.PasswordEncoder;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.impl.Validators;

@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(locations = {"classpath:test-spring-config.xml"})
@Transactional
public class UserServiceTest {

	@Autowired
	private UserManager userManager;
	
	@Autowired
	private final PasswordEncoder passwordEncoder = null;
	
	@Autowired
	private final ValidationManager valid = null;
	
	@Test
	public void password() {
		String pw = passwordEncoder.encodePassword("password", "test2");
		System.out.println(pw);
	}
	
	@Test
	public void testValidation() throws Exception {
		String validusername = "testusername";
		valid.validate(validusername, Validators.USERNAME_VALIDATOR);
		
		String invalidUsername = "!testusername!";
		try {
			valid.validate(invalidUsername, Validators.USERNAME_VALIDATOR);
			Assert.fail();
		} catch (Exception e) {
			//this should throw exception
		}
		
		String validPassword = "password123!@#$%^&*()";
		valid.validate(validPassword, Validators.PASSWORD_VALIDATOR);
		
		String invalidPassword = "this is an invalid password";
		try {
			valid.validate(invalidPassword, Validators.PASSWORD_VALIDATOR);
			Assert.fail();
		} catch (Exception e) {
			//this should throw exception
		}
		
		String validPin = "12345";
		valid.validate(validPin, Validators.PIN_VALIDATOR);
		
		String invalidPin = "123abc45";
		try {
			valid.validate(invalidPin, Validators.PIN_VALIDATOR);
			Assert.fail();
		} catch (Exception e) {
			//this should throw exception
		}
		
		String validEmail = "matt@test.com";
		valid.validate(validEmail, Validators.EMAIL_VALIDATOR);
		
		String invalidEmail = "123abc45";
		try {
			valid.validate(invalidEmail, Validators.EMAIL_VALIDATOR);
			Assert.fail();
		} catch (Exception e) {
			//this should throw exception
		}
	}
	
	
	@Test
	public void testCreateRetrieveUser() throws Exception {
		User u = new User();
		u.setUsername("test");
		u.setPassword("password");
		u.setEmail("a1@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User found = userManager.find("test");
		Assert.assertNotNull(found);
	}
	
	@Test
	public void testListUsers() throws Exception {
		User u = new User();
		u.setUsername("test3");
		u.setPassword("password");
		u.setEmail("a2@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		u = new User();
		u.setUsername("test2");
		u.setPassword("password");
		u.setEmail("a3@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		List<User> users = userManager.list();
		Assert.assertNotNull(users);
		Assert.assertTrue(users.size() == 2);
	}
	
	@Test
	public void testListOnlineUsers() throws Exception {
		User u = new User();
		u.setUsername("test3");
		u.setPassword("password");
		u.setEmail("a2@m.com");
		u.setLastUpdated(new Date().getTime());
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		u = new User();
		u.setUsername("test2");
		u.setPassword("password");
		u.setEmail("a3@m.com");
		u.setLastUpdated(new Date().getTime()-100000000L);
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		List<User> users = userManager.findOnline(100000L);
		Assert.assertNotNull(users);
		Assert.assertTrue(users.size() == 1);
	}
	
	
	@Test
	public void testDeleteUser() throws Exception {
		User u = new User();
		u.setUsername("test5");
		u.setPassword("password");
		u.setEmail("a5@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User found = userManager.find("test5");
		Assert.assertNotNull(found);
		
		userManager.delete(found.getId());
		found = userManager.find("test5");
		Assert.assertNull(found);
	}
	
}
