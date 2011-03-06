package com.mar.website.server.model.merge.impl;

import org.springframework.beans.factory.annotation.Autowired;

import com.mar.website.server.model.User;
import com.mar.website.server.model.exceptions.ValidationException;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.Validator;

public class UserValidator implements Validator<User> {
	
	@Autowired
	private final ValidationManager valid = null;

	@Override
	public String escape(User o) throws ValidationException {
		return null;
	}

	@Override
	public User validate(Object object) throws ValidationException {
		if (object ==  null) {
			throw new ValidationException("Cannot be null");
		}
		User u = null;
		try {
			u = (User)object;
		} catch (ClassCastException ce) {
			throw new ValidationException(ce);
		}
		
		//validate the individual properties
		if (u.getUsername() != null) u.setUsername((String) valid.validate(u.getUsername().trim(), Validators.USERNAME_VALIDATOR));
		if (u.getPassword() != null) u.setPassword((String) valid.validate(u.getPassword().trim(), Validators.PASSWORD_VALIDATOR));
		if (u.getEmail() != null) u.setEmail((String) valid.validate(u.getEmail().trim(), Validators.EMAIL_VALIDATOR));

		return u;
	}

}
