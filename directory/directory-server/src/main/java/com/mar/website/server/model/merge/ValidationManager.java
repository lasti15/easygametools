package com.mar.website.server.model.merge;

import com.mar.website.server.model.exceptions.ValidationException;

public interface ValidationManager {
	
	Object validate(Object value, String validatorName) throws ValidationException;

}
