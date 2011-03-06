#ifndef __CONFIG_H
#define __CONFIG_H


#include <string>

#include <irrXML.h>


#include "Reference.h"


namespace ROG {

#define VALUE_ATTR_NAME "value"
#define VALUE_ATTR_LEN 5


	class ConfigLoader : public Reference {
	private:
		static Semaphore instanceSem;
		static Object<ConfigLoader> instance;

		const static Logger* logger;

		Semaphore varSem;
		AVLHashT<std::string, char*, AVL_CHUNK_SIZE_MEM> configVariables;

		ConfigLoader(irr::io::IrrXMLReader* reader) {
			loadXmlReader(reader);
		}


		void loadXmlReader(irr::io::IrrXMLReader* reader) {
			logger->debug("Loading config file");
			while (reader->read()) {

				irr::io::EXML_NODE nodeType = reader->getNodeType();

				switch (nodeType) {
					case irr::io::EXN_COMMENT:
					case irr::io::EXN_ELEMENT_END:
					case irr::io::EXN_NONE:
					case irr::io::EXN_TEXT:
						break;
					case irr::io::EXN_ELEMENT: {
						char* nodeName = (char*)reader->getNodeName();
						if (nodeName == NULL || nodeName[0] < 33) {
							continue;
						}

						char* value = NULL;
						//we found a variable name, now look for a value
						for (unsigned int i=0; i<reader->getAttributeCount(); i++) {
							const char* attrName = reader->getAttributeName(i);
							if (attrName != NULL || attrName[0] < 33) {
								if (strncmp(VALUE_ATTR_NAME, attrName, VALUE_ATTR_LEN) == 0) {
									//we found the right attribute
									value = (char*)reader->getAttributeValue(attrName);
								}
							}
						}

						if (value == NULL || value[0] < 33) {
							//look for the value of this tag
							if (!reader->isEmptyElement()) {
								value = (char*)reader->getNodeData();
							}
						}

						//make sure we have enough data to construct this variable
						if (value == NULL || value[0] == 0) {
							char buf[50];
							_snprintf(buf, 49, "Missing value for %s", nodeName);
							logger->warn(buf);
							continue;
						}
				
						logger->debug("Found variable");
						set(nodeName, value);
						break;
					}

					default:
						logger->warn("Invalid tag in config file");
				}
			}
			delete reader;
		}

	public:

		~ConfigLoader() {
			configVariables.clearWithDelete();
		}

		static Object<ConfigLoader> getConfigLoader() {

			instanceSem.lock();
			if (instance == NULL) {
				irr::io::IrrXMLReader* xmlReader = irr::io::createIrrXMLReader("config.xml");
				if (xmlReader == NULL) {
					throw new FatalException("Cannot open config file.");
				}

				instance = new ConfigLoader(xmlReader);
			}
			instanceSem.unlock();

			return instance;
		}

		inline const char* get(const char* name) {
			return configVariables[std::string(name)];
		}

		inline void set(const char* name, const char* value) {
			varSem.lock();
			if (configVariables.contains(name)) {
				//delete the old value
				delete configVariables[name];
			}
			configVariables.insertKeyAndValue(std::string(name), strdup(value));
			varSem.unlock();
		}

		/*
			Return the value as an int or 0 if it doesnt exist
		*/
		inline const int getInt(const char* name) {
			const char* value = get(name);
			if (value != NULL) {
				return atoi(value);
			}
			return 0;
		}

		/*
			Return the value as an float or 0 if it doesnt exist
		*/
		inline const double getFloat(const char* name) {
			const char* value = get(name);
			if (value != NULL) {
				return atof(value);
			}
			return 0.0;
		}

		/*
			Return the value as an bool or false if it doesnt exist
		*/
		inline const int getBool(const char* name) {
			const char* value = get(name);
			if (value != NULL) {
				if (strncmp("true", strlwr((char*)value), 4) == 0) {
					return true;
				}
			}
			return false;
		}

	};

};



#endif


