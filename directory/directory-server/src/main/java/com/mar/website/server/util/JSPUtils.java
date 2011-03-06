package com.mar.website.server.util;

import java.util.List;

import javax.servlet.ServletContext;

import org.springframework.web.context.WebApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

public class JSPUtils {
	
	public static final Object getBean(String name, ServletContext servletContext) {
		WebApplicationContext ctx = WebApplicationContextUtils.getRequiredWebApplicationContext(servletContext);
		return ctx.getBean(name);
	}
	
	public static final <T> List<T> XmlList(List<T> list) {
		
		return null;
	}
}
