#include "LsbReader.h"
#include <boost/algorithm/string.hpp>

LsbReader::LsbReader()
{
}

HEADER_LSB *LsbReader::getHeader() {
	if (headerCached) {
		return &cachedHeader;
	}
	return 0;
}

/**
  Lookup the specified entity based on the given path.  Path is delimited by '/' characters.
  
 * @brief lookupByUniquePath
 * @param entity
 * @param path
 * @return 
 */
LsbObject *LsbReader::lookupByUniquePathEntity(LsbObject *entity, const char *path) {
	char *pathCopy = new char[strlen(path) + 1];
	strcpy(pathCopy, path);
	LsbObject *current = entity;
	
	std::vector<std::string> tokens;
	boost::split(tokens, pathCopy, boost::is_any_of("/"));
	for (int i=0; i<tokens.size(); ++i) {
		std::string& tok = tokens[i];
		bool found = false;
		for (int j=0; j<current->getChildren().size(); ++j) {
			LsbObject *child = current->getChildren()[j];
			if (child->getName() == tok) {
				current = child;
				found = true;
				break;
			}
		}
		if (!found) {
			delete []pathCopy;
			return 0;
		}
	}
	delete []pathCopy;
	return current;
}

/**
  Lookup the specified entity based on the given path.  Path is delimited by '/' characters.
  
 * @brief LsbReader::lookupByUniquePath
 * @param entities
 * @param path
 * @return 
 */
LsbObject *LsbReader::lookupByUniquePath(std::vector<LsbObject *>& entities, const char *path) {
	char *pathCopy = new char[strlen(path) + 1];
	strcpy(pathCopy, path);
	
	std::vector<std::string> tokens;
	boost::split(tokens, pathCopy, boost::is_any_of("/"));
	for (int i=0; i<tokens.size(); ++i) {
		std::string& tok = tokens[i];
		for (int j=0; j<entities.size(); ++j) {
			if (entities[j]->getName() == tok) {
				delete []pathCopy;
				std::string finalPath = path;
				if (tokens.size() > 1) {
					int prefixLen = tok.length() + 1;
					finalPath = finalPath.substr(prefixLen, strlen(path) - prefixLen);
					return lookupByUniquePathEntity(entities[j], finalPath.c_str());
				}
				return entities[j];
			}
		}
	}
	delete []pathCopy;
	return 0;
}

std::vector<LsbObject *> LsbReader::lookupAllEntitiesWithName(LsbObject *object, const char *name) {
	std::vector<LsbObject *> entityList;
	for (int i=0; i<object->getChildren().size(); ++i) {
		if (object->getChildren()[i]->getName() == name) {
			entityList.push_back(object->getChildren()[i]);
		}
	}
	return entityList;
}

std::vector<LsbObject *> LsbReader::extractPropertyForEachListItem(std::vector<LsbObject *>& list, const char *propertyName) {
	std::vector<LsbObject *> propertyList;
	for (int i=0; i<list.size(); ++i) {
		LsbObject *object = list[i];
		for (int j=0; j<object->getChildren().size(); ++j) {
			LsbObject *child = object->getChildren()[j];
			if (child->getName() == propertyName) {
				propertyList.push_back(child);
			}
		}
	}
	return propertyList;
}

std::vector<LsbObject *> LsbReader::findItemsByAttribute(std::vector<LsbObject *>& list, const char *attributeName, const char *attributeValue, int valueLength) {
	std::vector<LsbObject *> matches;
	for (int i=0; i<list.size(); ++i) {
		LsbObject *object = list[i];
		for (int j=0; j<object->getChildren().size(); ++j) {
			LsbObject *child = object->getChildren()[j];
			if (child->getName() == attributeName && child->getDataSize() == valueLength && memcmp(child->getData(), attributeValue, valueLength) == 0) {
				matches.push_back(object);
			}
		}
	}
	return matches;
}

LsbObject *LsbReader::getObjectCreator(LsbObject *object) {
	long childId = object->getChildId();
	LsbObject *objectGroup = object->getParent();
	if (objectGroup != 0) {
		LsbObject *groupContainerObject = objectGroup->getParent();
		if (groupContainerObject != 0) {
			LsbObject *creatorsObject = LsbReader::lookupByUniquePathEntity(groupContainerObject, "Creators");
			if (childId < creatorsObject->getChildren().size()) {
				LsbObject *objectCreator = creatorsObject->getChildren()[childId];
				return objectCreator;
			}
		}
	}
	return 0;
}

LsbObject *LsbReader::getObjectFromCreator(LsbObject *creator, const char *objectGroupName) {
	long childId = creator->getChildId();
	LsbObject *creators = creator->getParent();
	if (creators != 0) {
		LsbObject *groupContainerObject = creators->getParent();
		if (groupContainerObject != 0) {
			LsbObject *objectGroup = LsbReader::lookupByUniquePathEntity(groupContainerObject, objectGroupName);
			if (childId < objectGroup->getChildren().size()) {
				LsbObject *object = objectGroup->getChildren()[childId];
				return object;
			}
		}
	}
	return 0;
}

bool LsbReader::readTags(std::istream& input, long tagCount, int& bytesLeft, std::vector<TAG_LSB *>& tagList, char * alloc) {
	for (int i=0; i<tagCount; ++i) {
		if (input && bytesLeft >= sizeof(long)) {
			input.read(alloc, sizeof(long));
			bytesLeft -= sizeof(long);
			long tagSize;
			*(&tagSize) = *((long*)alloc);
			TAG_LSB *tagLsb = (TAG_LSB *)malloc(sizeof(TAG_LSB));
			tagList.push_back(tagLsb);
			tagLsb->tagLength = tagSize;
			
			if (input && bytesLeft >= tagSize) {
				input.read(alloc, tagSize);
				bytesLeft -= tagSize;
				tagLsb->tag = (char *)malloc(tagSize + 1);
				memset(tagLsb->tag, 0, tagSize + 1);
				memcpy(tagLsb->tag, alloc, tagSize);
				
				if (input && bytesLeft >= sizeof(long)) {
					input.read(alloc, sizeof(long));
					bytesLeft -= sizeof(long);
					long index;
					*(&index) = *((long*)alloc);
					tagLsb->index = index;
					//std::cout<<"["<<(i+1)<<"]"<<"tag = "<<tagLsb->tag<<" = "<<(void*)tagLsb->index<<'\n';
				}
				else {
					//freeTagList(tagList);
					return false;
				}
			}
			else {
				//freeTagList(tagList);
				return false;
			}
		}
		else {
			return false;
		}
	}
	return true;
}

TAG_LSB * LsbReader::getTagByIndex(long &index, std::vector<TAG_LSB *>& tagList) {
	for (int i=0; i<tagList.size(); ++i) {
		if (tagList[i]->index == index) {
			return tagList[i];
		}
	}
	return 0;
}

LsbObject *LsbReader::addChildToCurrentDirectory(LsbObject *child, LsbObject *directory, TAG_LSB *tagPtr, std::stack<LsbObject *>& directoryStack) {
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

void LsbReader::addItemListToDirectory(long& entitiesBuilt, long& builtEntityTagCount, long& readTagCount, std::vector<LsbObject *>& arrayEntity, LsbObject *array, TAG_LSB *tagPtr, std::stack<LsbObject *>& arrayStack, bool& doneBuilding, bool& readFirst) {
	++entitiesBuilt;
	builtEntityTagCount = arrayEntity.size();
	readTagCount = 1;
	if (arrayEntity.size() == 0) {
		//std::cout<<"Attempted to add empty item list!\n";
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

bool LsbReader::readTagData(std::istream& input, long readSize, std::stack<LsbObject *>& directoryStack, long tagCount, int& bytesLeft, std::vector<TAG_LSB *>& tagList, char * alloc) {
	long bytesStart = bytesLeft;
	long bytesRead = 0;
	while (bytesRead < readSize) {
		if (input && bytesLeft >= sizeof(long)) {
			input.read(alloc, sizeof(long));
			bytesLeft -= sizeof(long);
			long index;
			*(&index) = *((long*)alloc);
			//std::cout<<"read index = "<<(void*)index<<", ";
			
			TAG_LSB *tagPtr = getTagByIndex(index, tagList);
			if (tagPtr == 0) {
				//std::cout<<"No tag found for index "<<(void*)index<<'\n';
			} else {
				//std::cout<<tagPtr->tag<<'\n';
			}
			
			LsbObject *currentDirectory = directoryStack.top();
			if (currentDirectory == 0) {
				//std::cout<<"Warning! Invalid array on top of stack!\n";
			}
			if (input && bytesLeft >= sizeof(long)) {
				input.read(alloc, sizeof(long));
				bytesLeft -= sizeof(long);
				long type;
				*(&type) = *((long*)alloc);
				
				int dataSize;
				bool isStr = false;
				bool isUniStr = false;
				bool currentNodeIsDirectory = false;
				if (currentDirectory->getItemsLeft() <= 0 && currentDirectory->getDirectoriesLeft() > 0) {
					dataSize = 4;
					currentNodeIsDirectory = true;
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
						//std::cout<<"Unknown type ("<<(void*)type<<")!"<<'\n';
						return false;
						break;
					}
				}

				if (isStr) {
					long strSize;
					if (input && bytesLeft >= sizeof(long)) {
						input.read(alloc, sizeof(long));
						bytesLeft -= sizeof(long);
						*(&strSize) = *((long*)alloc);
					}
					else {
						return false;
					}
					if (isUniStr) {
						dataSize = strSize * 2;
					}
					else {
						dataSize = strSize;
					}
				}
				if (input && bytesLeft >= dataSize) {
					char *readBuf = new char[dataSize];
					////std::cout<<":BLb:"<<bytesLeft<<"s:"<<dataSize<<":";
					input.read(readBuf, dataSize);
					////std::cout<<":BLa:"<<bytesLeft<<":";
					bytesLeft -= dataSize;
//					std::string textOut;
//					if (type == 4 || type == 1 || type == 0 || currentNodeIsDirectory) {
//						long value;
//						*(&value) = *((long*)readBuf);
//						std::ostringstream ss;
//						ss<<value;
//						textOut = ss.str();
//					}
//					else {
//						textOut = readBuf;
//					}
					//std::cout<<"type = "<<(void*)type<<", "<<"buffer = '"<<textOut<<"' ["<<(currentNodeIsDirectory ? "dir" : "item")<<"]"<<((void*)(long)input.tellg())<<"\n";
					
					long value = *((long*)readBuf);
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
						object->setIndex(index);
						object->setName(tagPtr->tag);
						object->setType(type);
						object->setData(readBuf, dataSize);
						object->setParent(currentDirectory);
						object->setChildId(currentDirectory->getChildrenCount() - currentDirectory->getTotalNodesRemaining());
						object->setTagList(&tagList);
						delete []readBuf;
						
						if (type == 0x1C) {
							long strSize;
							if (input && bytesLeft >= sizeof(long)) {
								input.read((char *)&strSize, sizeof(long));
								bytesLeft -= sizeof(long);
								char *pairedStr = new char[strSize];
								if (input && bytesLeft >= strSize) {
									input.read(pairedStr, strSize);
									bytesLeft -= strSize;
									long tempSize = object->getDataSize() + strSize;
									char *temp = new char[tempSize];
									char *data = object->getData();
									memcpy(temp, data, object->getDataSize());
									memcpy(temp + object->getDataSize(), pairedStr, strSize);
									object->setData(temp, tempSize);
									delete []temp;
								}
								delete []pairedStr;
							}
						}
						
						if (currentDirectory->getItemsLeft() >= 0) {
							addChildToCurrentDirectory(object, currentDirectory, tagPtr, directoryStack);
						}
						
						if (currentNodeIsDirectory) {
							if (object->getTotalNodesRemaining() != 0) {
								directoryStack.push(object);
								//std::cout<<"Pushing dir "<<object->getName()<<", dirstack size = "<<directoryStack.size()<<'\n';
							}
						}
					}
					
					
				}
			}
			else {
				return false;
			}
		}
		bytesRead = bytesStart - bytesLeft;
	}
	//std::cout<<"Entity done\n";
}

void LsbReader::readDataHeader(std::istream& input, int& bytesLeft, char *alloc) {
	for (int i=0; i<entityCount; ++i) {
		if (input && bytesLeft >= sizeof(ENTITY_HEADER_LSB)) {
			input.read(alloc, sizeof(ENTITY_HEADER_LSB));
			bytesLeft -= sizeof(ENTITY_HEADER_LSB);
			entityHeaderList.push_back(new ENTITY_HEADER_LSB);
			*(entityHeaderList[entityHeaderList.size() - 1]) = *((ENTITY_HEADER_LSB*)alloc);
		}
		else {
			//std::cout<<"error\n";
		}
	}
	dataHeaderCached = true;
}

void LsbReader::cleanup() {
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

std::vector<LsbObject *> LsbReader::loadFile(std::istream& input) {
	cleanup();
	std::vector<LsbObject *> objects;
	std::vector<LsbObject *> empty;
	int startPos = input.tellg();
	input.seekg(0, std::ios::end);
	int bytesTotal = input.tellg() - startPos;
	input.seekg(0, std::ios::beg);
	int bytesLeft = bytesTotal;
	static const int BLOCK_SIZE = 2048;
	char alloc[BLOCK_SIZE];
	char allocHeader[sizeof(HEADER_LSB)];
	if (input && bytesLeft >= sizeof(HEADER_LSB)) {
		input.read(allocHeader, sizeof(HEADER_LSB));
		bytesLeft -= sizeof(HEADER_LSB);
		*(&cachedHeader) = *((HEADER_LSB*)allocHeader);
		headerCached = true;
		
		bool good = readTags(input, cachedHeader.tagCount, bytesLeft, tagList, alloc);
		if (!good) {
			return empty;
		}
		if (input && bytesLeft >= sizeof(long)) {
			input.read(alloc, sizeof(long));
			bytesLeft -= sizeof(long);
			*(&entityCount) = *((long*)alloc);
		}
		else {
			return empty;
		}
		readDataHeader(input, bytesLeft, alloc);
		//        for (int i=0; i<entityHeaderList.size(); ++i) {
		//            //std::cout<<"Offset "<<(i+1)<<": "<<(void*)entityHeaderList[i].dataOffset<<
		//                       "   id="<<entityHeaderList[i].id<<'\n';
		//        }
		for (int i=0; i<entityHeaderList.size(); ++i) {
			ENTITY_HEADER_LSB *entityHeaderPtr = 0;
			if ((i+1) < entityHeaderList.size()) {
				entityHeaderPtr = entityHeaderList[i+1];
			}
			int bytesNext = (entityHeaderPtr == 0 ?
								 bytesTotal - entityHeaderList[i]->dataOffset :
								 entityHeaderPtr->dataOffset - entityHeaderList[i]->dataOffset);
			input.seekg(entityHeaderList[i]->dataOffset);
			TAG_LSB *tagPtr = getTagByIndex(entityHeaderList[i]->id, tagList);
			char *tag = "?";
			if (tagPtr != 0) {
				tag = tagPtr->tag;
			}
			//std::cout<<"Entity "<<(i + 1)<<"------------------ Offset:"
//					<<(void*) entityHeaderList[i].dataOffset<<" Id: "
	//			   <<(void*) entityHeaderList[i].id<<" ("<<tag<<")"<<'\n';
			objects.push_back(new LsbObject());
			LsbObject *obj = objects[objects.size() - 1];
			obj->setIndex(entityHeaderList[i]->id);
			obj->setType(0x0);
			obj->setName(tag);
			obj->setDirectoriesLeft(1);
			obj->setEntityId(i + 1);
			obj->setEntitySize(bytesNext);
			std::stack<LsbObject *> arrayStack;
			arrayStack.push(obj);
			
			//if ((i+1) == 15)
				//std::system("pause");
			readTagData(input, bytesNext, arrayStack, cachedHeader.tagCount, bytesLeft, tagList, alloc);
		}
		if (bytesLeft > 0) {
			//std::cout<<"Warning: extra bytes leftover: "<<bytesLeft<<'\n';
		}
		////freeTagList(tagList);
	}
	else {
		return std::vector<LsbObject *>();
	}
	return objects;
}
