package com.mar.website.server.model.merge;

import com.mar.website.server.model.exceptions.ValidationException;

public interface Validator<T> {
	public String escape(T o) throws ValidationException;
	public T validate(Object object) throws ValidationException;
}
