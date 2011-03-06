package com.mar.website.server.model;

public enum UserRole {
	
	ROLE_USER ("ROLE_USER"),
	ROLE_INSTALLER ("ROLE_INSTALLER"),
	ROLE_SUPERUSER ("ROLE_SUPERUSER");
	
	public static final String ROLE_USER_STR = "ROLE_USER";
	public static final String ROLE_INSTALLER_STR = "ROLE_INSTALLER";
	public static final String ROLE_SUPERUSER_STR = "ROLE_SUPERUSER";
	
	private final String value;
	
	UserRole(final String role) {
		this.value = role;
	}
	
	public final String toString() {
		return value;
	}
};
