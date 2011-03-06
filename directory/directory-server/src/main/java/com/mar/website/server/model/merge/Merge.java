package com.mar.website.server.model.merge;

import com.mar.website.server.model.User;

public interface Merge {
	
	void merge(User kept, User thrownAway);
}
