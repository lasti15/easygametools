package com.mar.website.server.managers;

import com.mar.website.server.model.Installation;
import com.mar.website.server.model.exceptions.NotAllowedException;

public interface InstallManager {

	Installation getActiveInstall() throws NotAllowedException;
	
	void save(Installation install) throws NotAllowedException;
	
	void deactivateAllInstalls() throws NotAllowedException;
}
