package com.mar.website.server.model.merge.impl;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang.StringEscapeUtils;

import com.mar.website.server.model.exceptions.ValidationException;
import com.mar.website.server.model.merge.Validator;

public class InputValidator implements Validator<String> {
	
	private Class<?> type;
	
	private boolean required = true;
	
	private String regex = null;
	
	
	public Class<?> getType() {
		return type;
	}

	public void setType(Class<?> type) {
		this.type = type;
	}

	public boolean isRequired() {
		return required;
	}

	public void setRequired(boolean required) {
		this.required = required;
	}

	public String getRegex() {
		return regex;
	}

	public void setRegex(String regex) {
		this.regex = regex;
	}
	

	@Override
	public String escape(String o) throws ValidationException {
		return StringEscapeUtils.escapeSql(o);
	}

	@Override
	public String validate(Object o) throws ValidationException {
		//first check for bad nulls
		if (required) {
			if (o == null) {
				throw new ValidationException("Cannot be null");
			}
		}
		//if its null and optional its fine
		if (o == null) {
			return null;
		}
		
		String value = (String)o;
		
		String clean = escape(value);
		if (type != null) {
			try {
				type.getMethod("valueOf", String.class).invoke(null, o);
			} catch (RuntimeException e) {
				throw new ValidationException(e);
			} catch (Throwable t) {
				throw new ValidationException(t);
			}
		}
		
		if (regex != null) {
			Pattern p = Pattern.compile(regex);
			Matcher matcher = p.matcher(clean);
			if (!matcher.matches()) {
				throw new ValidationException(regex);
			}
		}

		return clean;
	}

}
