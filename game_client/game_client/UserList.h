#ifndef __USERLIST_H
#define __USERLIST_H


#include "XMLRootElement.h"
#include "Reference.h"

#include "User.h"

namespace ROG {


	class UserList : public Reference, public XMLRootElement {
	private:

		OCArray<Object<User>> userList;

	public:

		void loadFromReader(irr::io::IrrXMLReader* reader);

		//hmm the copy consrtructors for these dont just copy a reference to the array....
		OCArray<Object<User>> getUserList() {
			return this->userList;
		}

	};

}






#endif

