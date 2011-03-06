package com.mar.website.server.model.merge.impl;

import com.mar.website.server.model.User;
import com.mar.website.server.model.merge.Merge;

public class UncheckedModelMerger implements Merge {

	@Override
	public void merge(User kept, User thrownAway) {
		//cant change username or password here
		//kept.setPassword((String) safeSet(kept.getPassword(), thrownAway.getPassword()));
		kept.setEmail((String) safeSet(kept.getEmail(), thrownAway.getEmail()));
	}
	
	private static final Object safeSet(Object original, Object toSet) {
		 return (toSet == null) ? original : toSet;
	}

}
