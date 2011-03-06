package com.mar.website.server.model;

import java.util.List;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement(name="games")
public class GameList {
	
	private List<Game> games;
	
	public GameList() {}
	
	public GameList(List<Game> games) {
		this.games = games;
	}

	public void setGames(List<Game> games) {
		this.games = games;
	}

	@XmlElement(name="game")
	public List<Game> getGames() {
		return games;
	}

}
