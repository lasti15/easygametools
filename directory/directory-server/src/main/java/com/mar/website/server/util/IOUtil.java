package com.mar.website.server.util;

import java.io.IOException;
import java.io.InputStream;

public class IOUtil {
	
	public static final String loadTextFile(String resource) throws IOException {
		InputStream input = null;
		StringBuilder stringBuilder = null;
		
		try {
			input = IOUtil.class.getResourceAsStream(resource);
			if (input == null) {
				throw new IOException("Cant open resource");
			}
			
			stringBuilder = new StringBuilder();
			String line = null;
			
			byte[] buffer = new byte[1024];
			int read = 0;
			while ((read = input.read(buffer, 0, 1023)) != -1) {
				buffer[read] = 0;
				line = new String(buffer,0, read);
				stringBuilder.append(line);
			}
		} finally {
			input.close();
		}
		
		return stringBuilder.toString();
	}
}

