package com.mar.website.server.model;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;


@Entity
@Table(name="game",
	uniqueConstraints = {@UniqueConstraint(columnNames={"hostName", "hostPort"}),
						 @UniqueConstraint(columnNames={"hostUser_id"})
						}
)
@XmlRootElement
public class Game {
	
	@Id
	@Column(name="id")
	@GeneratedValue(strategy=GenerationType.AUTO)
	@XmlAttribute
	private Long id;
	
	@OneToOne(fetch=FetchType.EAGER, optional=false)
	private User hostUser;
	
	@Column(name="hostName", nullable=false)
	private String hostName;
	
	@Column(name="hostPort", nullable=false)
	private Integer hostPort;
	
	@Column(name="maxPlayers", nullable=false)
	private Integer maxPlayers;
	
	@Column(name="lastUpdate", nullable=false)
	private Long lastUpdate;
	
	@Column(name="hostGuid")
	private String hostGuid;
	
	@ManyToMany(targetEntity=User.class, fetch=FetchType.EAGER)
	@JoinTable(
	    name="User_X_Game",
	    joinColumns=@JoinColumn(name="userId"),
	    inverseJoinColumns=@JoinColumn(name="gameId")
	)
	private Collection<User> players;

	public void setHostUser(User hostUser) {
		this.hostUser = hostUser;
	}

	public User getHostUser() {
		return hostUser;
	}

	public void setHostName(String hostName) {
		this.hostName = hostName;
	}

	public String getHostName() {
		return hostName;
	}

	public void setHostPort(Integer hostPort) {
		this.hostPort = hostPort;
	}

	public Integer getHostPort() {
		return hostPort;
	}

	public void setMaxPlayers(Integer maxPlayers) {
		this.maxPlayers = maxPlayers;
	}

	public Integer getMaxPlayers() {
		return maxPlayers;
	}

	public void setLastUpdate(Long lastUpdate) {
		this.lastUpdate = lastUpdate;
	}

	public Long getLastUpdate() {
		return lastUpdate;
	}

	@XmlTransient
	public Long getId() {
		return id;
	}

	public void setHostGuid(String hostGuid) {
		this.hostGuid = hostGuid;
	}

	public String getHostGuid() {
		return hostGuid;
	}

	public void setPlayers(Collection<User> players) {
		this.players = players;
	}

	@XmlTransient
	public Collection<User> getPlayers() {
		return players;
	}
	
	@XmlElement(name="players")
	public UserList getPlayerList() {
		return new UserList(Arrays.asList(players.toArray(new User[0])));
	}
}
