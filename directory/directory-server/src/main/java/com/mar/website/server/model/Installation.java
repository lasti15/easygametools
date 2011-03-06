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


@Entity
@Table(name="install")
@XmlRootElement
public class Installation {

	@Id
	@Column(name="id")
	@GeneratedValue(strategy=GenerationType.AUTO)
	@XmlAttribute
	private Long id;
	
	@Column(name="pin", nullable=false)
	private String pin;
	
	@Column(name="installDate", nullable=false)
	private Date installDate;
	
	@Column(name="in_use", nullable=false)
	private boolean inUse;

	public void setId(Long id) {
		this.id = id;
	}

	public Long getId() {
		return id;
	}

	public void setPin(String pin) {
		this.pin = pin;
	}

	public String getPin() {
		return pin;
	}

	public void setInstallDate(Date installDate) {
		this.installDate = installDate;
	}

	public Date getInstallDate() {
		return installDate;
	}

	public void setInUse(boolean inUse) {
		this.inUse = inUse;
	}

	public boolean isInUse() {
		return inUse;
	}
	
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((id == null) ? 0 : id.hashCode());
		result = prime * result + (inUse ? 1231 : 1237);
		result = prime * result
				+ ((installDate == null) ? 0 : installDate.hashCode());
		result = prime * result + ((pin == null) ? 0 : pin.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Installation other = (Installation) obj;
		if (id == null) {
			if (other.id != null)
				return false;
		} else if (!id.equals(other.id))
			return false;
		if (inUse != other.inUse)
			return false;
		if (installDate == null) {
			if (other.installDate != null)
				return false;
		} else if (!installDate.equals(other.installDate))
			return false;
		if (pin == null) {
			if (other.pin != null)
				return false;
		} else if (!pin.equals(other.pin))
			return false;
		return true;
	}
}
