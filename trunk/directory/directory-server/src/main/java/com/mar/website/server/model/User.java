package com.mar.website.server.model;

import java.util.Date;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;



@Entity
@Table(name="user")
@XmlRootElement
public class User {
	
	@Id
	@Column(name="id")
	@GeneratedValue(strategy=GenerationType.AUTO)
	@XmlAttribute
	private Long id;
	
	@Column(name="username", nullable=false, unique=true)
	private String username;

	@Column(name="password", nullable=false)
	private String password;

	@Column(name="email", nullable=false, unique=true)
	private String email;

	@Column(name="lastLogin")
	private Date lastLogin;
	
	@Column(name="role", nullable=false)
	private UserRole role;
	
	@Column(name="active", nullable=false)
	private boolean active = false;
	
	//so we know if theyre online or not
	@Column(name="lastUpdated", nullable=true)
	private Long lastUpdated;
	
	public User() {}
	public User(Long userId) {
		this.id = userId;
	}

	public Long getId() {
		return id;
	}

	public void setUsername(String username) {
		this.username = username;
	}

	public String getUsername() {
		return username;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	@XmlTransient
	public String getPassword() {
		return password;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getEmail() {
		return email;
	}

	public void setLastLogin(Date lastLogin) {
		this.lastLogin = lastLogin;
	}

	public Date getLastLogin() {
		return lastLogin;
	}
	
	public void setRole(UserRole role) {
		this.role = role;
	}
	
	@XmlTransient
	public UserRole getRole() {
		return role;
	}
	
	public void setActive(boolean active) {
		this.active = active;
	}
	
	@XmlTransient
	public boolean isActive() {
		return active;
	}
	
	public void setLastUpdated(Long lastUpdated) {
		this.lastUpdated = lastUpdated;
	}
	
	public Long getLastUpdated() {
		return lastUpdated;
	}
}
