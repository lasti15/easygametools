package com.mar.website.server.model;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlRootElement;

@Entity
@Table(name="user_activation")
@XmlRootElement
public class UserActivation {
	@Id
	@Column(name="id")
	@GeneratedValue(strategy=GenerationType.AUTO)
	@XmlAttribute
	private Long id;
	
	@Column(name="activation_code", nullable=false, unique=true)
	private String activationCode;
	
	@OneToOne
	private User user;
	
	@Column(name="password_reset", nullable=false)
	private boolean passwordReset;

	public Long getId() {
		return id;
	}

	public void setActivationCode(String activation_code) {
		this.activationCode = activation_code;
	}

	public String getActivationCode() {
		return activationCode;
	}

	public void setUser(User user) {
		this.user = user;
	}

	public User getUser() {
		return user;
	}

	public void setPasswordReset(boolean passwordReset) {
		this.passwordReset = passwordReset;
	}

	public boolean isPasswordReset() {
		return passwordReset;
	}

}
