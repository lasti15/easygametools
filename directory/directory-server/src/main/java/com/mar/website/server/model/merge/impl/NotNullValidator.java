package com.mar.website.server.model.merge.impl;

import com.mar.website.server.model.exceptions.ValidationException;
import com.mar.website.server.model.merge.Validator;

public class NotNullValidator implements Validator<Object> {

	@Override
	public String escape(Object o) throws ValidationException {
		return null;
	}

	@Override
	public Object validate(Object object) throws ValidationException {
		if (object == null) {
			throw new ValidationException("Cannot be null");
		}
		return object;
	}

}
