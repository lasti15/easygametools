package com.mar.website.server.services;

import java.io.IOException;
import java.io.Serializable;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.WebDataBinder;
import org.springframework.web.bind.annotation.InitBinder;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
public interface MarService extends Serializable {
	
	@InitBinder
	void initBinder(WebDataBinder binder);
	
	@ResponseBody
	@RequestMapping(value="error", method={RequestMethod.GET, RequestMethod.POST})
	String error(@RequestParam(value="errorCode",required=false) int errorCode) throws IOException;
}
