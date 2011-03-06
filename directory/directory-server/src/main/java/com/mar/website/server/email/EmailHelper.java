package com.mar.website.server.email;

import java.io.IOException;
import java.io.InputStream;
import java.util.Collections;
import java.util.Map;

import javax.activation.DataSource;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;

import org.apache.commons.mail.ByteArrayDataSource;
import org.apache.commons.mail.EmailException;
import org.apache.commons.mail.HtmlEmail;

import com.mar.website.server.model.exceptions.WebServiceException;

public class EmailHelper {

	private String smtpHostName;
	private Boolean smtpNeedsAuth;
	private Boolean smtpNeedsTls;
	private String smtpUsername;
	private String smtpPassword;
	private String defaultFromAddress;
	private String serverHost;
	
	public void sendMessage(String to, String subject, String msgTemplate, Map<String, Object> properties) throws WebServiceException {
		sendMessage(to, defaultFromAddress, subject, msgTemplate, properties);
	}
	
	public void sendMessage(String to, String from, String subject, String msgTemplate, Map<String, Object> properties) throws WebServiceException {
		if (to == null || from == null || subject == null || msgTemplate == null) {
			throw new WebServiceException("Missing required parameters");
		}
		
		//load the template
		String template = null;

		InputStream is = this.getClass().getResourceAsStream(msgTemplate);
		StringBuffer buffer = new StringBuffer();
		
		byte[] intermediate = new byte[1024];
		int count = 0;
		try {
			while ((count = is.read(intermediate, 0, 1023)) != -1) {
				String input = new String(intermediate, 0, count, "UTF-8");
				buffer.append(input);
			}
		} catch (IOException e) {
			throw new WebServiceException(e);
		}
		template = buffer.toString();
		
		properties.put("serverUrl", serverHost);
		//to template replacement here
		for (String key : properties.keySet()) {
			Object property = properties.get(key);
			//anythign that is not a string or number is an attachment
			if (property instanceof String || property instanceof Number) {
				template = template.replace("{"+key+"}", (CharSequence) properties.get(key));
			}
		}
		
		
		HtmlEmail email = new HtmlEmail();
		try {
			email.setFrom(from);
			email.setSubject(subject);
			email.setTo(Collections.nCopies(1, new InternetAddress(to)));
			email.setHtmlMsg(template);
			email.setHostName(smtpHostName);

			if (smtpNeedsAuth) {
				email.setAuthentication(smtpUsername, smtpPassword);
			}
			
			email.setSSL(true);
			
			//add all of the attachments here
			for (String key : properties.keySet()) {
				Object property = properties.get(key);
				//attachments
				if (property instanceof EmailAttachment) {
					EmailAttachment attachment = (EmailAttachment) property;
					DataSource dataSource = new ByteArrayDataSource(attachment.getData(), attachment.getContentType());
					email.attach(dataSource, key, attachment.getContentType());
				}
			}
			
			email.send();
		} catch (EmailException e) {
			throw new WebServiceException(e);
		} catch (AddressException e) {
			throw new WebServiceException(e);
		} catch (IOException e) {
			throw new WebServiceException(e);
		}
	}
	
	public void setSmtpHostName(String smtpHostName) {
		this.smtpHostName = smtpHostName;
	}
	
	public String getSmtpHostName() {
		return smtpHostName;
	}

	public void setSmtpNeedsAuth(Boolean smtpNeedsAuth) {
		this.smtpNeedsAuth = smtpNeedsAuth;
	}

	public Boolean getSmtpNeedsAuth() {
		return smtpNeedsAuth;
	}

	public void setSmtpUsername(String smtpUsername) {
		this.smtpUsername = smtpUsername;
	}

	public String getSmtpUsername() {
		return smtpUsername;
	}

	public void setSmtpPassword(String smtpPassword) {
		this.smtpPassword = smtpPassword;
	}

	public String getSmtpPassword() {
		return smtpPassword;
	}

	public void setDefaultFromAddress(String defaultFromAddress) {
		this.defaultFromAddress = defaultFromAddress;
	}

	public String getDefaultFromAddress() {
		return defaultFromAddress;
	}

	public void setSmtpNeedsTls(Boolean smtpNeedsTls) {
		this.smtpNeedsTls = smtpNeedsTls;
	}

	public Boolean getSmtpNeedsTls() {
		return smtpNeedsTls;
	}

	public void setServerHost(String serverHost) {
		this.serverHost = serverHost;
	}

	public String getServerHost() {
		return serverHost;
	}
}
