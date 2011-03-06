package com.mar.website.server.services;

import java.beans.PropertyEditorSupport;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;

import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.oxm.jaxb.Jaxb2Marshaller;
import org.springframework.web.bind.WebDataBinder;

import com.mar.website.server.model.Game;
import com.mar.website.server.model.User;
import com.mar.website.server.util.IOUtil;

public abstract class AbstractMarService implements MarService {

	private static final long serialVersionUID = -2986518614701609254L;
	
	@Autowired
	private final Jaxb2Marshaller xmlConverter = null;
	
	private String errorPagePath;
	
	private String errorPage = new String();  
	
	protected final class XmlParamBinder extends PropertyEditorSupport {
		
		@Override
		public Object getValue() {
			return value;
		}

		@Override
		public void setValue(Object value) {
			this.value = value;
		}

		private Object value;
		
		@Override
		public String getAsText() {
			StringWriter writer = new StringWriter();
			xmlConverter.marshal(value, new StreamResult(writer));
			return writer.toString();
		}

		@Override
		public void setAsText(String text) throws IllegalArgumentException {
			value = xmlConverter.unmarshal(new StreamSource(new StringReader(text)));
		}
		
	}
	
	@Override
	public void initBinder(WebDataBinder binder) {
	    binder.registerCustomEditor(User.class, new XmlParamBinder());
	    binder.registerCustomEditor(Game.class, new XmlParamBinder());
	}
	
	public String error(int errorCode) throws IOException {
		
		synchronized(errorPage) {
			if (errorPage.isEmpty()) {
				String test = IOUtil.loadTextFile("/error.jsp");
				test = test.replaceAll("\\{errorPagePath\\}", errorPagePath);
				errorPage = test;
			}
		}
		
		
		
		return errorPage;
	}

	public void setErrorPagePath(String errorPagePath) {
		this.errorPagePath = errorPagePath;
	}

	public String getErrorPagePath() {
		return errorPagePath;
	}

}
