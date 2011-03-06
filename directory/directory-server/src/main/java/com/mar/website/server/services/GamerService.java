package com.mar.website.server.services;

import javax.servlet.http.HttpServletRequest;

import org.springframework.security.annotation.Secured;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.mar.website.server.model.Game;
import com.mar.website.server.model.GameList;
import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;


@Controller
@RequestMapping("/gamer")
public interface GamerService extends MarService {

	@ResponseBody
	@RequestMapping(value="findGames", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	GameList getAvailableGames(@RequestParam("page") int pageNum) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="startGame", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	Game startGame(@RequestParam("host") String host, @RequestParam("port") int port, @RequestParam("guid") String hostGuid) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="update", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	Game updateGame(@RequestParam("game") Game gameToUpdate) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="get", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	Game getUpdatedGame(@RequestParam("gameId") Long gameToFind) throws WebServiceException;

	@ResponseBody
	@RequestMapping(value="adopt", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	Game adoptGame(@RequestParam("gameId") Long gameToAdopt, @RequestParam("host") String host, @RequestParam("port") int port, @RequestParam("guid") String hostGuid) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="stopGame", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	void stopGame(@RequestParam("gameId") Long gameToRemove) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="validateUser", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	User validateUser(@RequestParam("userId") Long userId, @RequestParam("gameId") Long gameId) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="getHostIp", method=RequestMethod.GET)
	@Secured({ UserRole.ROLE_USER_STR })
	String getHostIp(HttpServletRequest request);
}
