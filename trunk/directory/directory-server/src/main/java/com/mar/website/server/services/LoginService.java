package com.mar.website.server.services;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.security.annotation.Secured;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;

@Controller
@RequestMapping("/login")
public interface LoginService extends MarService {
	
	@ResponseBody
	@RequestMapping(value="login", method=RequestMethod.POST)
	public String login(HttpServletRequest request, HttpServletResponse response, @RequestParam("username") String username, @RequestParam("password") String password) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping("isLoggedIn")
	@Secured({ UserRole.ROLE_SUPERUSER_STR, UserRole.ROLE_USER_STR })
	public String isLoggedIn(HttpServletResponse response) throws WebServiceException;
}
