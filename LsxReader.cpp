#include "LsxReader.h"
#include <boost/algorithm/string.hpp>
#include <tinyxml/tinyxml.h>
#include <boost/lexical_cast.hpp>

LsxReader::LsxReader()
{
}

HEADER_LSB *LsxReader::getHeader() {
	if (headerCached) {
		return &cachedHeader;
	}
	return 0;
}

TAG_LSB * LsxReader::getTagByIndex(long &index, std::vector<TAG_LSB *>& tagList) {
	for (int i=0; i<tagList.size(); ++i) {
		if (tagList[i]->index == index) {
			return tagList[i];
		}
	}
	return 0;
}

LsbObject *LsxReader::addChildToCurrentDirectory(LsbObject *child, LsbObject *directory, TAG_LSB *tagPtr, std::stack<LsbObject *>& directoryStack) {
	directory->addChild(child);
	long arraySize = 0;
	if (child->isDirectory()) {
		directory->decrementDirectoriesLeft();
	}
	else {
		directory->decrementItemsLeft();
	}
	arraySize = directory->getDirectoriesLeft() + directory->getItemsLeft();
	if (directory->getTotalNodesRemaining() == 0) {
		if (!directory->isEntity()) {
			directoryStack.pop();
			//std::cout<<"Popping dir "<<directory->getName()<<", dirstack size = "<<directoryStack.size()<<'\n';
		}
	}
	else {
		//std::cout<<"Decremented dir "<<directory->getName()<<" to size = d:"<<directory->getDirectoriesLeft()<<" i:"<<directory->getItemsLeft()<<'\n';
	}
	return child;
}

void LsxReader::addItemListToDirectory(long& entitiesBuilt, long& builtEntityTagCount, long& readTagCount, std::vector<LsbObject *>& arrayEntity, LsbObject *array, TAG_LSB *tagPtr, std::stack<LsbObject *>& arrayStack, bool& doneBuilding, bool& readFirst) {
	++entitiesBuilt;
	builtEntityTagCount = arrayEntity.size();
	readTagCount = 1;
	if (arrayEntity.size() == 0) {
		////std::cout<<"Attempted to add empty item list!\n";
	}
	else {
		LsbObject *objPtr = arrayEntity[0];
		for (int i=1; i<arrayEntity.size(); ++i) {
			objPtr->addChild(arrayEntity[i]);
		}
		addChildToCurrentDirectory(objPtr, array, tagPtr, arrayStack);
		arrayEntity.clear();
		doneBuilding = false;
		readFirst = true;
	}
}

std::vector<TiXmlElement *> getAllElementsByName(TiXmlNode *parent, const char *name) {
	std::vector<TiXmlElement *> elements;
	
	TiXmlElement *node = 0;
	for (;;) {
		if (node == 0) {
			node = parent->FirstChildElement(name);
		} else {
			node = node->NextSiblingElement(name);
		}
		if (node == 0) {
			break;
		}
		elements.push_back(node);
	}
	
	return elements;
}

bool LsxReader::readTagData(TiXmlElement *node, long readSize, std::stack<LsbObject *>& directoryStack, int& bytesLeft, std::vector<TAG_LSB *>& tagList, char * alloc) {
	TiXmlNode *currentDirectory = node;
	long bytesStart = bytesLeft;
	long bytesRead = 0;
	int dataSize;
	
	bool currentNodeIsDirectory = false;
	
	std::vector<TiXmlElement *> attributes = getAllElementsByName(currentDirectory, "attribute");
	std::vector<TiXmlElement *> nodes;
	TiXmlElement *childrenElement = currentDirectory->FirstChildElement("children");
	if (childrenElement != 0) {
		nodes = getAllElementsByName(childrenElement, "node");
	} else {
		nodes = getAllElementsByName(currentDirectory, "node");
	}
	std::vector<TiXmlElement *> allChildren;
	for (int i=0; i<attributes.size(); ++i) {
		allChildren.push_back(attributes[i]);
	}
	for (int i=0; i<nodes.size(); ++i) {
		allChildren.push_back(nodes[i]);
	}
	
	for (int i=0; i<allChildren.size(); ++i) {
		TiXmlElement *child = allChildren[i];
		currentNodeIsDirectory = !(i < attributes.size());

		const char *id;
		unsigned long type;
		unsigned long value;
		const char *typeText;
		const char *valueText;
		id = child->Attribute("id");
		if (id == 0) {
			return false;
		}
		if (!currentNodeIsDirectory) {
			typeText = child->Attribute("type");
			valueText = child->Attribute("value");
			if (valueText == 0 || typeText == 0) {
				return false;
			}
			try {
				type = boost::lexical_cast<unsigned long>(typeText);
			} catch (const boost::bad_lexical_cast& e) {
				
			}
			try {
				value = boost::lexical_cast<unsigned long>(valueText);
			} catch (const boost::bad_lexical_cast& e) {
				
			}
		} else {
			std::vector<TiXmlElement *> attributes = getAllElementsByName(child, "attribute");
			std::vector<TiXmlElement *> nodes;
			TiXmlElement *childrenElement = child->FirstChildElement("children");
			if (childrenElement != 0) {
				nodes = getAllElementsByName(childrenElement, "node");
			}
			type = attributes.size();
			value = nodes.size();
			//std::cout<<id<<": "<<type<<' '<<value<<'\n';
		}
		
		TAG_LSB *tagPtr = LsbObject::createTagIfNeeded(id, &tagList);
		
		LsbObject *currentDirectoryObject = directoryStack.top();
		if (currentDirectoryObject == 0) {
			////std::cout<<"Warning! Invalid array on top of stack!\n";
		}
		
		bool isStr = false;
		bool isUniStr = false;
		if (currentNodeIsDirectory) {
			dataSize = 4;
		}
		else {
			switch(type) {
			case 0: //array
				dataSize = 4;
				break;
			case 1: //byte
				dataSize = 1;
				break;
			case 2: //timestamp?
				dataSize = 2; //used to be 4
				break;
			case 3: //short
				dataSize = 2;
				break;
			case 4: //integer
				dataSize = 4;
				break;
			case 5: //unsigned int
				dataSize = 4;
				break;
			case 6: //float
				dataSize = 4;
				break;
			case 7: //double
				dataSize = 8;
				break;
			case 8:
				dataSize = 8;
				break;
			case 9:
				dataSize = 12;
				break;
			case 0x0A:
				dataSize = 16;
				break;
			case 0x0B:
				dataSize = 8;
				break;
			case 0x0C:
				dataSize = 12;
				break;
			case 0x0D:
				dataSize = 16;
				break;
			case 0x0E:
				dataSize = 16;
				break;
			case 0x0F: //rotation
				dataSize = 36;
				break;
			case 0x10:
				dataSize = 48;
				break;
			case 0x11:
				dataSize = 48;
				break;
			case 0x12:
				dataSize = 64;
				break;
			case 0x13: //boolean
				dataSize = 1;
				break;
			case 0x14:
				isStr = true;
				break;
			case 0x15:
				isStr = true;
				break;
			case 0x16: //string
				isStr = true;
				break;
			case 0x17: //hash string type
				isStr = true;
				break;
			case 0x18:
				isStr = true;
				break;
			case 0x19: //binary data block
				isStr = true;
				break;
			case 0x1A:
				dataSize = 8;
				break;
			case 0x1B: //boolean
				dataSize = 1;
				break;
			case 0x1C: //many strings
				isStr = true;
				break;
			case 0x1D:
				isStr = true;
				isUniStr = true;
				break;
			case 0x1E: //unicode string
				isStr = true;
				isUniStr = true;
				break;
			default:
				////std::cout<<"Unknown type ("<<(void*)type<<")!"<<'\n';
				return false;
				break;
			}
		}
		if (isStr) {
			dataSize = strlen(valueText) + 1;
		}
	
		char *readBuf = new char[dataSize];
		if (isStr) {
			strcpy(readBuf, valueText);
		} else {
			memcpy(readBuf, &value, sizeof(value));
		}
		//////std::cout<<":BLb:"<<bytesLeft<<"s:"<<dataSize<<":";
		//input.read(readBuf, dataSize);
		//////std::cout<<":BLa:"<<bytesLeft<<":";
		bytesLeft -= dataSize;
		
	//					currentDirectory->
	//					if (itemsLeft > 0) {
	//						--itemsLeft;
	//					}
	//					else if (directoriesLeft > 0) {
	//						--directoriesLeft;
	//					}
		
		if (tagPtr != 0) {
			LsbObject *object = new LsbObject();
			if (currentNodeIsDirectory) {
				object->setIsDirectory(true);
				object->setItemsLeft(type);
				object->setDirectoriesLeft(value);
				object->setChildrenCount(type + value);
			}
			//set object properties
			object->setIndex(tagPtr->index);
			object->setName(tagPtr->tag);
			object->setType(type);
			object->setData(readBuf, dataSize);
			object->setParent(currentDirectoryObject);
			object->setChildId(currentDirectoryObject->getChildrenCount() - currentDirectoryObject->getTotalNodesRemaining());
			object->setTagList(&tagList);
			delete []readBuf;
			
			if (currentDirectoryObject->getItemsLeft() >= 0) {
				addChildToCurrentDirectory(object, currentDirectoryObject, tagPtr, directoryStack);
			}
			
			if (currentNodeIsDirectory) {
				if (object->getTotalNodesRemaining() != 0) {
					directoryStack.push(object);
					//std::cout<<"Pushing dir "<<object->getName()<<", dirstack size = "<<directoryStack.size()<<'\n';
				}
				//if ((int)alloc < 3) {
					//std::cout<<"Looking at dir "<<id<<'\n';
					bool ret = readTagData(child, 0, directoryStack, bytesLeft, tagList, ++alloc);
					if (!ret) {
						return false;
					}
				//}
			}
		}
	}
	return true;
}

void LsxReader::readDataHeader(std::istream& input, int& bytesLeft, char *alloc) {
	for (int i=0; i<entityCount; ++i) {
		if (input && bytesLeft >= sizeof(ENTITY_HEADER_LSB)) {
			input.read(alloc, sizeof(ENTITY_HEADER_LSB));
			bytesLeft -= sizeof(ENTITY_HEADER_LSB);
			entityHeaderList.push_back(new ENTITY_HEADER_LSB);
			*(entityHeaderList[entityHeaderList.size() - 1]) = *((ENTITY_HEADER_LSB*)alloc);
		}
		else {
			////std::cout<<"error\n";
		}
	}
	dataHeaderCached = true;
}

void LsxReader::cleanup() {
	for (int i=0; i<entityHeaderList.size(); ++i) {
		delete entityHeaderList[i];
	}
	entityHeaderList.clear();
	if (!loanedTagList) {
		for (int i=0; i<tagList.size(); ++i) {
			delete tagList[i];
		}
	}
	tagList.clear();
}
#include <windows.h>
std::vector<LsbObject *> LsxReader::loadFile(std::istream& input) {
	cleanup();
	std::vector<LsbObject *> objects;
	std::vector<LsbObject *> empty;
	int startPos = input.tellg();
	input.seekg(0, std::ios::end);
	int bytesTotal = input.tellg() - startPos;
	input.seekg(0, std::ios::beg);
	int bytesLeft = bytesTotal;
	
	char *xmlBuf = new char[bytesTotal];
	input.read(xmlBuf, bytesTotal);
	TiXmlDocument doc;
	doc.Parse(xmlBuf);
	delete[] xmlBuf;
	
	TiXmlElement *saveElement = doc.FirstChildElement("save");
	if (saveElement == 0) {
		return empty;
	}
	
	TiXmlElement *headerElement = saveElement->FirstChildElement("header");
	if (headerElement != 0) {
		const char *version = headerElement->Attribute("version");
		const char *time = headerElement->Attribute("time");
		if (version == 0 || time == 0) {
			return empty;
		}
		cachedHeader.endianness = 0;
		cachedHeader.fileLength = bytesTotal;
		cachedHeader.fileTimestamp = 0;
		try {
			cachedHeader.fileTimestamp = boost::lexical_cast<long>(time);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		try {
			cachedHeader.magic = boost::lexical_cast<long>(version);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
	}
	
	TiXmlElement *versionElement = saveElement->FirstChildElement("version");
	if (versionElement != 0) {
		const char *major = versionElement->Attribute("major");
		const char *minor = versionElement->Attribute("minor");
		const char *revision = versionElement->Attribute("revision");
		const char *build = versionElement->Attribute("build");
		if (major == 0 || minor == 0 || revision == 0 || build == 0) {
			return empty;
		}
		try {
			cachedHeader.verMajor = boost::lexical_cast<long>(major);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		try {
			cachedHeader.verMinor = boost::lexical_cast<long>(minor);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		try {
			cachedHeader.verRevision = boost::lexical_cast<long>(revision);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
		try {
			cachedHeader.verBuild = boost::lexical_cast<long>(build);
		} catch (const boost::bad_lexical_cast& e) {
			
		}
	}
	
	if (headerElement != 0 || versionElement != 0) {
		headerCached = true;
	}
	
	std::vector<TiXmlElement *> regions = getAllElementsByName(saveElement, "region");
	if (readerProgressCallback != 0) {
		readerProgressCallback->onLoadBegin(regions.size());
	}
	//std::cout<<"Region count = "<<regions.size()<<'\n';
	for (int i=0; i<regions.size(); ++i) {
		TiXmlElement *regionNode = regions[i];
		const char *id = regionNode->Attribute("id");
		if (id == 0) {
			return empty;
		}
		TAG_LSB *tag = LsbObject::createTagIfNeeded(id, &tagList);
		objects.push_back(new LsbObject());
		LsbObject *obj = objects[objects.size() - 1];
		obj->setIndex(tag->index);
		obj->setType(0x0);
		obj->setName(id);
		obj->setDirectoriesLeft(1);
		obj->setEntityId(i + 1);
		obj->setEntitySize(0);
		std::stack<LsbObject *> arrayStack;
		arrayStack.push(obj);
		
		//std::cout<<"Processing region "<<id<<'\n';
		if (!readTagData(regionNode, 0, arrayStack, bytesLeft, tagList, 0)) {
			return empty;
		}
		
		if (readerProgressCallback != 0) {
			readerProgressCallback->onLoadUpdate(regions.size() - (i + 1));
		}
	}
	if (readerProgressCallback != 0) {
		readerProgressCallback->onLoadEnd();
	}
	
//	static const int BLOCK_SIZE = 2048;
//	char alloc[BLOCK_SIZE];
//	char allocHeader[sizeof(HEADER_LSB)];
//	if (input && bytesLeft >= sizeof(HEADER_LSB)) {
//		input.read(allocHeader, sizeof(HEADER_LSB));
//		bytesLeft -= sizeof(HEADER_LSB);
//		*(&cachedHeader) = *((HEADER_LSB*)allocHeader);
//		headerCached = true;
		
//		bool good = readTags(input, cachedHeader.tagCount, bytesLeft, tagList, alloc);
//		if (!good) {
//			return empty;
//		}
//		if (input && bytesLeft >= sizeof(long)) {
//			input.read(alloc, sizeof(long));
//			bytesLeft -= sizeof(long);
//			*(&entityCount) = *((long*)alloc);
//		}
//		else {
//			return empty;
//		}
//		readDataHeader(input, bytesLeft, alloc);
//		//        for (int i=0; i<entityHeaderList.size(); ++i) {
//		//            ////std::cout<<"Offset "<<(i+1)<<": "<<(void*)entityHeaderList[i].dataOffset<<
//		//                       "   id="<<entityHeaderList[i].id<<'\n';
//		//        }
//		if (readerProgressCallback != 0) {
//			readerProgressCallback->onLoadBegin(entityHeaderList.size());
//		}
//		for (int i=0; i<entityHeaderList.size(); ++i) {
//			ENTITY_HEADER_LSB *entityHeaderPtr = 0;
//			if ((i+1) < entityHeaderList.size()) {
//				entityHeaderPtr = entityHeaderList[i+1];
//			}
//			int bytesNext = (entityHeaderPtr == 0 ?
//								 bytesTotal - entityHeaderList[i]->dataOffset :
//								 entityHeaderPtr->dataOffset - entityHeaderList[i]->dataOffset);
//			input.seekg(entityHeaderList[i]->dataOffset);
//			TAG_LSB *tagPtr = getTagByIndex(entityHeaderList[i]->id, tagList);
//			char *tag = "?";
//			if (tagPtr != 0) {
//				tag = tagPtr->tag;
//			}
//			////std::cout<<"Entity "<<(i + 1)<<"------------------ Offset:"
////					<<(void*) entityHeaderList[i].dataOffset<<" Id: "
//	//			   <<(void*) entityHeaderList[i].id<<" ("<<tag<<")"<<'\n';
//			objects.push_back(new LsbObject());
//			LsbObject *obj = objects[objects.size() - 1];
//			obj->setIndex(entityHeaderList[i]->id);
//			obj->setType(0x0);
//			obj->setName(tag);
//			obj->setDirectoriesLeft(1);
//			obj->setEntityId(i + 1);
//			obj->setEntitySize(bytesNext);
//			std::stack<LsbObject *> arrayStack;
//			arrayStack.push(obj);
			
//			//if ((i+1) == 15)
//				//std::system("pause");
//			readTagData(input, bytesNext, arrayStack, cachedHeader.tagCount, bytesLeft, tagList, alloc);
//			if (readerProgressCallback != 0) {
//				readerProgressCallback->onLoadUpdate(entityHeaderList.size() - (i + 1));
//			}
//		}
//		if (bytesLeft > 0) {
//			////std::cout<<"Warning: extra bytes leftover: "<<bytesLeft<<'\n';
//		}
//		////freeTagList(tagList);
//	}
//	else {
//		if (readerProgressCallback != 0) {
//			readerProgressCallback->onLoadEnd();
//		}
//		return std::vector<LsbObject *>();
//	}
//	if (readerProgressCallback != 0) {
//		readerProgressCallback->onLoadEnd();
//	}
	return objects;
}
