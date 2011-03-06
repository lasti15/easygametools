package com.mar.website;

import java.util.Date;
import java.util.List;

import junit.framework.Assert;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.providers.encoding.PasswordEncoder;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;
import org.springframework.transaction.annotation.Transactional;

import com.mar.website.server.managers.GameManager;
import com.mar.website.server.managers.UserManager;
import com.mar.website.server.model.Game;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.merge.ValidationManager;

@RunWith(SpringJUnit4ClassRunner.class)
@ContextConfiguration(locations = {"classpath:test-spring-config.xml"})
@Transactional
public class GameServiceTest {

	@Autowired
	private UserManager userManager;
	
	@Autowired
	private final PasswordEncoder passwordEncoder = null;
	
	@Autowired
	private final ValidationManager valid = null;
	
	@Autowired
	private final GameManager gameManager = null;
	
	
	
	
	@Test
	public void password() {
		String pw = passwordEncoder.encodePassword("password", "test2");
		System.out.println(pw);
	}
	
	
	
	
	@Test
	public void testCreateRetrieveGame() throws Exception {
		User u = new User();
		u.setUsername("test");
		u.setPassword("password");
		u.setEmail("a1@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User foundU = userManager.find("test");
		Assert.assertNotNull(foundU);
		
		
		Game g = new Game();
		g.setHostUser(u);
		g.setHostName("localhost");
		g.setHostPort(11223);
		g.setMaxPlayers(8);
		g.setNumberOfPlayers(1);
		g.setLastUpdate(new Date().getTime());
		
		g = gameManager.createGame(g);
		Assert.assertNotNull(g);
		Assert.assertTrue(g.getId() > 0);
		
		Game found = gameManager.find(g.getId());
		Assert.assertNotNull(found);
		Assert.assertTrue(found.getId() > 0);
	}
	
	
	
	
	
	@Test
	public void testUpdateGame() throws Exception {
		User u = new User();
		u.setUsername("test");
		u.setPassword("password");
		u.setEmail("a1@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User foundU = userManager.find("test");
		Assert.assertNotNull(foundU);
		
		
		Game g = new Game();
		g.setHostUser(u);
		g.setHostName("localhost");
		g.setHostPort(11223);
		g.setMaxPlayers(8);
		g.setNumberOfPlayers(1);
		g.setLastUpdate(new Date().getTime());
		
		g = gameManager.createGame(g);
		Assert.assertNotNull(g);
		Assert.assertTrue(g.getId() > 0);
		
		Game found = gameManager.find(g.getId());
		Assert.assertNotNull(found);
		Assert.assertTrue(found.getId() > 0);
		
		found.setHostPort(999999);
		g = gameManager.updateGame(found);
		Assert.assertNotNull(g);
		Assert.assertTrue(g.getId() > 0);
		Assert.assertTrue(g.getHostPort() == 999999);
	}
	
	
	@Test
	public void testDeleteGame() throws Exception {
		User u = new User();
		u.setUsername("test");
		u.setPassword("password");
		u.setEmail("a1@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User foundU = userManager.find("test");
		Assert.assertNotNull(foundU);
		
		
		Game g = new Game();
		g.setHostUser(u);
		g.setHostName("localhost");
		g.setHostPort(11223);
		g.setMaxPlayers(8);
		g.setNumberOfPlayers(1);
		g.setLastUpdate(new Date().getTime());
		
		g = gameManager.createGame(g);
		Assert.assertNotNull(g);
		Assert.assertTrue(g.getId() > 0);
		
		Game found = gameManager.find(g.getId());
		Assert.assertNotNull(found);
		Assert.assertTrue(found.getId() > 0);
		
		
		gameManager.deleteGame(found.getId());
		
		found = gameManager.find(g.getId());
		Assert.assertNull(found);
	}
	
	
	
	@Test
	public void testFindGames() throws Exception {
		User u = new User();
		u.setUsername("test");
		u.setPassword("password");
		u.setEmail("a1@m.com");
		u.setRole(UserRole.ROLE_USER);
		
		userManager.save(u);
		
		User foundU = userManager.find("test");
		Assert.assertNotNull(foundU);
		
		
		Game g = new Game();
		g.setHostUser(u);
		g.setHostName("localhost");
		g.setHostPort(11223);
		g.setMaxPlayers(8);
		g.setNumberOfPlayers(1);
		g.setLastUpdate(new Date().getTime());
		
		g = gameManager.createGame(g);
		Assert.assertNotNull(g);
		Assert.assertTrue(g.getId() > 0);
		
		Game found = gameManager.find(g.getId());
		Assert.assertNotNull(found);
		Assert.assertTrue(found.getId() > 0);
		
		
		User u2 = new User();
		u2.setUsername("test2");
		u2.setPassword("password");
		u2.setEmail("a2@m.com");
		u2.setRole(UserRole.ROLE_USER);
		
		userManager.save(u2);
		
		User foundU2 = userManager.find("test2");
		Assert.assertNotNull(foundU2);
		
		Game g2 = new Game();
		g2.setHostUser(u2);
		g2.setHostName("localhost");
		g2.setHostPort(11224);
		g2.setMaxPlayers(8);
		g2.setNumberOfPlayers(1);
		g2.setLastUpdate(new Date().getTime());
		
		g2 = gameManager.createGame(g2);
		Assert.assertNotNull(g2);
		Assert.assertTrue(g2.getId() > 0);
		
		found = gameManager.find(g2.getId());
		Assert.assertNotNull(found);
		Assert.assertTrue(found.getId() > 0);
		
		Game game2 = gameManager.findByUserId(u2.getId());
		Assert.assertNotNull(game2);
		Assert.assertTrue(game2.getId().equals(g2.getId()));
		
		List<Game> games1 = gameManager.findGames(null, null, null, null);
		Assert.assertNotNull(games1);
		Assert.assertTrue(games1.size() == 2);
		
		List<Game> games2 = gameManager.findGames(g.getId(), null, null, null);
		Assert.assertNotNull(games2);
		Assert.assertTrue(games2.size() == 1);
		
		List<Game> games3 = gameManager.findGames(g2.getId(), null, null, null);
		Assert.assertNotNull(games3);
		Assert.assertTrue(games3.size() == 1);
		
		List<Game> games4 = gameManager.findGames(null, u.getId(), null, null);
		Assert.assertNotNull(games4);
		Assert.assertTrue(games4.size() == 1);
		
		List<Game> games5 = gameManager.findGames(null, u2.getId(), null, null);
		Assert.assertNotNull(games5);
		Assert.assertTrue(games5.size() == 1);
		
		List<Game> games6 = gameManager.findGames(null, null, "localhost", null);
		Assert.assertNotNull(games6);
		Assert.assertTrue(games6.size() == 2);
		
		List<Game> games7 = gameManager.findGames(null, null, null, 11223);
		Assert.assertNotNull(games7);
		Assert.assertTrue(games7.size() == 1);
		
		List<Game> games8 = gameManager.findGames(null, null, null, 11224);
		Assert.assertNotNull(games8);
		Assert.assertTrue(games8.size() == 1);
		
		List<Game> games9 = gameManager.findGames(g.getId(), null, null, 11223);
		Assert.assertNotNull(games9);
		Assert.assertTrue(games9.size() == 1);
		
		List<Game> games10 = gameManager.findGames(g.getId(), u.getId(), "localhost", 11223);
		Assert.assertNotNull(games10);
		Assert.assertTrue(games10.size() == 1);
		
		List<Game> games11 = gameManager.findGames(g2.getId(), u.getId(), "localhost", 11223);
		Assert.assertNotNull(games11);
		Assert.assertTrue(games11.size() == 0);
		
		List<Game> games12 = gameManager.findGames(g.getId(), u2.getId(), null, 11224);
		Assert.assertNotNull(games12);
		Assert.assertTrue(games12.size() == 0);
		
		List<Long> ids = gameManager.getAllGameIds(0L);
		Assert.assertNotNull(ids);
		Assert.assertTrue(ids.size() == 2);
	}
}
