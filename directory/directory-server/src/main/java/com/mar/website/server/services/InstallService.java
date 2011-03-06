package com.mar.website.server.services;

import org.springframework.security.annotation.Secured;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.mar.website.server.model.UserRole;
import com.mar.website.server.model.exceptions.WebServiceException;

@Controller
@RequestMapping("/install")
public interface InstallService extends MarService {

	@ResponseBody
	@RequestMapping("install")
	@Secured({ UserRole.ROLE_INSTALLER_STR })
	void install(@RequestParam(value="pin", required=false) String pin) throws WebServiceException;
	
	@ResponseBody
	@RequestMapping("resetadmin")
	@Secured({ UserRole.ROLE_INSTALLER_STR })
	void resetAdmin() throws WebServiceException;

}
