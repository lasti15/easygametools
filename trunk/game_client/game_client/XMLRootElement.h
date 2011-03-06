
#ifndef __XMLROOTELEMENT_H
#define __XMLROOTELEMENT_H

#include <irrXML.h>


namespace ROG {

	class XMLRootElement {
	public:

		virtual void loadFromReader(irr::io::IrrXMLReader* reader) = 0;

	};

}

#endif

