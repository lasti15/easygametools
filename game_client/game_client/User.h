#ifndef __USER_H
#define __USER_H

#include "XMLRootElement.h"
#include "Reference.h"
#include "Protocol.pb.h"
#include "User.pb.h"

namespace ROG {

	class User : public Reference, public XMLRootElement {
	private:

		long userId;

		char* username;

	public:

		User() {
			this->username = NULL;
		};

		User(const char* username) {
			if (username != NULL) {
				this->username = strdup(username);
			} else {
				this->username = NULL;
			}
		}

		User(int userId, const char* username) {
			this->userId = userId;

			if (username != NULL) {
				this->username = strdup(username);
			} else {
				this->username = NULL;
			}
		}

		User(int userId) {
			this->userId = userId;
			this->username = NULL;
		}

		~User() {
			if (username != NULL) {
				delete [] username;
			}
		}

		void setUserId(long id) {
			this->userId = id;
		}

		void setUsername(const char* username) {
			if (this->username != NULL) {
				delete [] this->username;
			}
			this->username = strdup(username);
		}

		long getUserId() {
			return this->userId;
		}

		const char* getUsername() {
			return this->username;
		}

		void loadFromReader(irr::io::IrrXMLReader* reader);
	};

}


#endif

