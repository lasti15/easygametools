package com.mar.website.server.services;

import java.io.IOException;

import javax.servlet.http.HttpServletResponse;

import org.springframework.security.annotation.Secured;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.mar.website.server.model.User;
import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;

@Controller
@RequestMapping("/user")
public interface UserService extends MarService {

	@ResponseBody
	@RequestMapping(value="save", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	public String saveMyAccount(@RequestParam("u") User u) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="register", method=RequestMethod.POST)
	public String registerAccount(@RequestParam("u") User u) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="activate", method=RequestMethod.GET)
	public String activate(HttpServletResponse response, @RequestParam("activationCode") String activationCode) throws WebServiceException, IOException;
	
	@ResponseBody
	@RequestMapping(value="forgotUsername", method=RequestMethod.POST)
	public String forgotUsername(@RequestParam("email") String email) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="forgotPassword", method=RequestMethod.POST)
	public String forgotPassword(@RequestParam("username") String username, @RequestParam("email") String email) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping(value="resetPassword", method=RequestMethod.POST)
	public String resetPassword(HttpServletResponse response, @RequestParam("activationCode") String activationCode, @RequestParam("newPassword")String newPassword, @RequestParam("newPasswordConfirm")String newPasswordConfirm) throws WebServiceException, IOException;
	
	@ResponseBody
	@RequestMapping(value="unregister", method=RequestMethod.POST)
	@Secured({ UserRole.ROLE_USER_STR })
	public String unregisterAccount() throws WebServiceException;
}
