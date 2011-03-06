package com.mar.website.server.model.merge.impl;

import java.util.Map;

import com.mar.website.server.model.exceptions.ValidationException;
import com.mar.website.server.model.merge.ValidationManager;
import com.mar.website.server.model.merge.Validator;

public class ValidationManagerImpl implements ValidationManager {
	
	private Map<String, Validator<?>> validators;

	@Override
	public Object validate(Object value, String validatorName) throws ValidationException {
		Validator<?> validator = validators.get(validatorName);
		if (validator == null) {
			 throw new ValidationException("missing validator");
		}
		
		return validator.validate(value);
	}

	public void setValidators(Map<String, Validator<?>> validators) {
		this.validators = validators;
	}

	public Map<String, Validator<?>> getValidators() {
		return validators;
	}
	
	
	
}
