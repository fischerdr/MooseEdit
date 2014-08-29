#include "LsbObject.h"
#include <boost/algorithm/string.hpp>

/**
  Lookup the specified entity based on the given path.  Path is delimited by '/' characters.
  
 * @brief lookupByUniquePath
 * @param entity
 * @param path
 * @return 
 */

std::string& LsbObject::getLocalized1()
{
	return localized1;
}

void LsbObject::setLocalized1(const std::string &value)
{
	localized1 = value;
}

std::string& LsbObject::getLocalized2()
{
	return localized2;
}

void LsbObject::setLocalized2(const std::string &value)
{
	localized2 = value;
}
LsbObject *LsbObject::lookupByUniquePathEntity(LsbObject *entity, const char *path) {
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
  
 * @brief LsbObject::lookupByUniquePath
 * @param entities
 * @param path
 * @return 
 */
LsbObject *LsbObject::lookupByUniquePath(std::vector<LsbObject *>& entities, const char *path) {
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

std::vector<LsbObject *> LsbObject::lookupAllEntitiesWithName(LsbObject *object, const char *name) {
	std::vector<LsbObject *> entityList;
	for (int i=0; i<object->getChildren().size(); ++i) {
		if (object->getChildren()[i]->getName() == name) {
			entityList.push_back(object->getChildren()[i]);
		}
	}
	return entityList;
}

std::vector<LsbObject *> LsbObject::extractPropertyForEachListItem(std::vector<LsbObject *>& list, const char *propertyName) {
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

std::vector<LsbObject *> LsbObject::findItemsByAttribute(std::vector<LsbObject *>& list, const char *attributeName, const char *attributeValue, int valueLength) {
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

LsbObject *LsbObject::getObjectCreator(LsbObject *object) {
	long childId = object->getChildId();
	LsbObject *objectGroup = object->getParent();
	if (objectGroup != 0) {
		LsbObject *groupContainerObject = objectGroup->getParent();
		if (groupContainerObject != 0) {
			LsbObject *creatorsObject = LsbObject::lookupByUniquePathEntity(groupContainerObject, "Creators");
			if (childId < creatorsObject->getChildren().size()) {
				LsbObject *objectCreator = creatorsObject->getChildren()[childId];
				return objectCreator;
			}
		}
	}
	return 0;
}

LsbObject *LsbObject::getObjectFromCreator(LsbObject *creator, const char *objectGroupName) {
	long childId = creator->getChildId();
	LsbObject *creators = creator->getParent();
	if (creators != 0) {
		LsbObject *groupContainerObject = creators->getParent();
		if (groupContainerObject != 0) {
			LsbObject *objectGroup = LsbObject::lookupByUniquePathEntity(groupContainerObject, objectGroupName);
			if (childId < objectGroup->getChildren().size()) {
				LsbObject *object = objectGroup->getChildren()[childId];
				return object;
			}
		}
	}
	return 0;
}
