#ifndef LSBOBJECT_H
#define LSBOBJECT_H

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stack>

#include <iostream>

#pragma pack(push,1)

struct HEADER_LSB {
	long magic;
	long fileLength;
	long endianness;
	long reserved1;
	long reserved2;
	long fileTimestamp;
	long verMajor;
	long verMinor;
	long verBuild;
	long verRevision;
	long tagCount;
};

struct TAG_LSB {
	long tagLength;
	char* tag = 0;
	long index;
	
	TAG_LSB() {
		;
	}
	
	~TAG_LSB() {
		delete tag;
	}
	
	TAG_LSB(const TAG_LSB& other) {
		this->tagLength = other.tagLength;
		long strLen = strlen(other.tag);
		this->tag = new char[strLen + 1];
		strcpy(this->tag, other.tag);
		this->index = other.index;
	}
};

struct ENTITY_HEADER_LSB {
    long id;
	long dataOffset;
};

#pragma pack(pop)

class LsbObject {
	std::string name;
	long index;
	long type;
	char *data = 0;
	long dataSize = 0;
	std::vector<LsbObject*> children;
	long childId = -1;
	long itemsLeft = 0;
	long directoriesLeft = 0;
	long childrenCount = 0;
	long entityId = 0;
	long entitySize = 0;
	bool bIsDirectory = false;
	LsbObject *parent = 0;
	std::vector<TAG_LSB *> *tagList = 0;
	std::string localized1;
	std::string localized2;
public:
	LsbObject *lookupByUniquePath(const char *path);
	static LsbObject *lookupByUniquePath(std::vector<LsbObject *>& entities, const char *path);
	static std::vector<LsbObject *> lookupAllEntitiesWithName(LsbObject *object, const char *name);
	static std::vector<LsbObject *> extractPropertyForEachListItem(std::vector<LsbObject *>& list, const char *propertyName);
	static std::vector<LsbObject *> findItemsByAttribute(std::vector<LsbObject *>& list, const char *attributeName, const char *attributeValue, int valueLength);
	static LsbObject *getObjectCreator(LsbObject *object);
	static LsbObject *getObjectFromCreator(LsbObject *creator, const char *objectGroupName);
	
	~LsbObject() {
		if (data != 0) {
			delete[] data;
		}
		for (int i=0; i<children.size(); ++i) {
			delete children[i];
		}
	}
	
	LsbObject() {
		;
	}
	
	LsbObject(bool isDirectory, long index, const char *tag, long type, LsbObject *parent, std::vector<TAG_LSB *> *tagList = 0) {
		this->setIsDirectory(isDirectory);
		this->setItemsLeft(0);
		this->setDirectoriesLeft(0);
		this->setChildrenCount(-1);
		this->setIndex(index);
		this->setName(tag);
		this->setType(type);
		this->setParent(parent);
		this->setTagList(tagList);
		if (parent != 0) {
			this->setChildId(parent->getChildren().size());
		}
	}
	
	LsbObject(const LsbObject& other) {
		this->name = other.name;
		this->index = other.index;
		this->type = other.type;
		this->data = new char[other.dataSize];
		memcpy(this->data, other.data, other.dataSize);
		this->dataSize = other.dataSize;
		for (int i=0; i<other.children.size(); ++i) {
			LsbObject *newChild = new LsbObject(*other.children[i]);
			this->addChild(newChild);
		}
		this->childId = other.childId;
		this->itemsLeft = other.itemsLeft;
		this->directoriesLeft = other.directoriesLeft;
		this->childrenCount = other.childrenCount;
		this->entityId = other.entityId;
		this->entitySize = other.entitySize;
		this->bIsDirectory = other.bIsDirectory;
		this->parent = other.parent;
		this->tagList = other.tagList;
		this->localized1 = other.localized1;
		this->localized2 = other.localized2;
	}
	
	static TAG_LSB *getTagByName(const char *name, std::vector<TAG_LSB *> *tagList)
	{
		std::string nameText = name;
		for (int i=0; i<tagList->size(); ++i) {
			if (tagList->at(i)->tag == nameText) {
				return tagList->at(i);
			}
		}
		return 0;
	}
	
	static TAG_LSB *createTagIfNeeded(const char *name, std::vector<TAG_LSB *> *tagList)
	{
		TAG_LSB *tag = getTagByName(name, tagList);
		if (tag != 0) {
			return tag;
		}
		tag = new TAG_LSB();
		tag->index = LsbObject::getNextFreeTagIndex(tagList);
		long tagLength = strlen(name) + 1;
		char *tagAlloc = new char[tagLength];
		strcpy(tagAlloc, name);
		tag->tag = tagAlloc;
		tag->tagLength = tagLength;
		tagList->push_back(tag);
		return tag;
	}
	
	void retag(std::vector<TAG_LSB *> *newTagList) {
		std::stack<LsbObject *> dirStack;
		dirStack.push(this);
		while (!dirStack.empty()) {
			LsbObject *currentDirectory = dirStack.top();
			dirStack.pop();
			std::string dirTag = currentDirectory->getName();
			TAG_LSB *newTag = LsbObject::createTagIfNeeded(dirTag.c_str(), newTagList);
			currentDirectory->setIndex(newTag->index);
			std::vector<LsbObject *> items = currentDirectory->getItemsOnly();
			std::vector<LsbObject *> dirs = currentDirectory->getDirectoriesOnly();
			for (int j=0; j<items.size(); ++j) {
				LsbObject *item = items[j];
				std::string itemTag = item->getName();
				TAG_LSB *newTag = LsbObject::createTagIfNeeded(itemTag.c_str(), newTagList);
				item->setIndex(newTag->index);
			}
			for (int j=dirs.size() - 1; j>=0; --j) {
				dirStack.push(dirs[j]);
			}
		}
	}
	
	static void stripUnusedTags(std::vector<TAG_LSB *> *tagList, std::vector<LsbObject *> objects) {
		typedef std::map<int, bool> TagMap;
		TagMap tagMap;
		for (int i=0; i<objects.size(); ++i) {
			std::stack<LsbObject *> dirStack;
			dirStack.push(objects[i]);
			while (!dirStack.empty()) {
				LsbObject *currentDirectory = dirStack.top();
				dirStack.pop();
				tagMap[currentDirectory->getIndex()] = true;
				std::vector<LsbObject *> items = currentDirectory->getItemsOnly();
				std::vector<LsbObject *> dirs = currentDirectory->getDirectoriesOnly();
				for (int j=0; j<items.size(); ++j) {
					LsbObject *item = items[j];
					tagMap[item->getIndex()] = true;
				}
				for (int j=dirs.size() - 1; j>=0; --j) {
					dirStack.push(dirs[j]);
				}
			}
		}
		for (int i=0; i<tagList->size(); ++i) {
			TAG_LSB *tag = tagList->at(i);
			if (tagMap.find(tag->index) == tagMap.end()) {
				tagList->erase(tagList->begin() + i);
				delete tag;
				--i;
			}
		}
	}
	
	static long getNextFreeTagIndex(std::vector<TAG_LSB *> *tagList) {
		long nextFree = 0;
		for (int i=0; i<tagList->size(); ++i) {
			TAG_LSB *tag = tagList->at(i);
			if (tag->index >= nextFree) {
				nextFree = tag->index + 1;
			}
		}
		return nextFree;
	}
	
	std::vector<TAG_LSB *> *getTagList() {
		return this->tagList;
	}
	
	void setTagList(std::vector<TAG_LSB *> *tagList) {
		this->tagList = tagList;
	}
	
	bool replaceChild(LsbObject *oldChild, LsbObject *newChild) {
		for (int i=0; i<children.size(); ++i) {
			if (children[i] == oldChild) {
				children[i] = newChild;
				delete oldChild;
				return true;
			}
		}
		return false;
	}
	
	bool removeChild(LsbObject *targetChild) {
		for (int i=0; i<this->getChildren().size(); ++i) {
			LsbObject *child = this->getChildren()[i];
			if (child == targetChild) {
				this->getChildren().erase(this->getChildren().begin() + i);
				delete child;
				return true;
			}
		}
		return false;
	}
	
	long getChildrenCount() {
		return this->childrenCount;
	}
	
	void setChildrenCount(long childrenCount) {
		this->childrenCount = childrenCount;
	}
	
	long getChildId() {
		return this->childId;
	}
	
	void setChildId(long childId) {
		this->childId = childId;
	}
	
	std::vector<LsbObject *> getItemsOnly() {
		std::vector<LsbObject *> items;
		for (int i=0; i<children.size(); ++i) {
			if (!children[i]->isDirectory()) {
				items.push_back(children[i]);
			}
		}
		return items;
	}
	
	std::vector<LsbObject *> getDirectoriesOnly() {
		std::vector<LsbObject *> dirs;
		for (int i=0; i<children.size(); ++i) {
			if (children[i]->isDirectory()) {
				dirs.push_back(children[i]);
			}
		}
		return dirs;
	}
	
	LsbObject *getParent() {
		return parent;
	}
	
	void setParent(LsbObject *parent) {
		this->parent = parent;
	}
	
	long getTotalNodesRemaining() {
		return directoriesLeft + itemsLeft;
	}
	
	bool isDirectory() {
		return bIsDirectory;
	}
	
	void setIsDirectory(bool isDirectory) {
		this->bIsDirectory = isDirectory;
	}
	
	long getEntitySize() {
		return entitySize;
	}
	
	void setEntitySize(long entitySize) {
		this->entitySize = entitySize;
	}
	
	bool isEntity() {
		return entityId != 0;
	}
	
	void setEntityId(long entityId) {
		this->entityId = entityId;
	}
	
	long getEntityId() {
		return entityId;
	}
	
	long decrementItemsLeft() {
		if (itemsLeft > 0) {
			--itemsLeft;
		}
		return itemsLeft;
	}
	
	long decrementDirectoriesLeft() {
		if (directoriesLeft > 0) {
			--directoriesLeft;
		}
		return directoriesLeft;
	}
	
	void setItemsLeft(long itemsLeft) {
		this->itemsLeft = itemsLeft;
	}
	
	long getItemsLeft() {
		return itemsLeft;
	}
	
	void setDirectoriesLeft(long directoriesLeft) {
		this->directoriesLeft = directoriesLeft;
	}
	
	long getDirectoriesLeft() {
		return directoriesLeft;
	}
	
	void setIndex(long index) {
		this->index = index;
	}
	
	long getIndex() {
		return index;
	}
	
	void setType(long type) {
		this->type = type;
	}
	
	long getType() {
		return type;
	}
	
	LsbObject *addChild(LsbObject *obj) {
		children.push_back(obj);
		obj->setParent(this);
		return children.back();
	}
	
	LsbObject *insertChild(LsbObject *obj, unsigned index) {
		if (index < children.size()) {
			children.insert(children.begin() + index, obj);
			obj->setParent(this);
			return obj;
		} else {
			return addChild(obj);
		}
	}
	
	bool insertLast(LsbObject *object, const char *objectType) {
		bool found = false;
		for (int i=0; i<this->getChildren().size(); ++i) {
			LsbObject *child = this->getChildren()[i];
			if (found && child->getName() != objectType) {
				//assumed to be continuous
				insertChild(object, i);
				return true;
			}
			if (child->getName() == objectType) {
				found = true;
			}
		}
		if (!found) {
			this->addChild(object);
		}
		return false;
	}
	
	std::vector<LsbObject*>& getChildren() {
		return children;
	}
	
	void setName(std::string name) {
		this->name = name;
	}
	
	std::string getName() {
		return name;
	}
	
	void setData(const char *data, int size) {
		if (this->data != 0) {
			delete[] this->data;
		}
		this->data = new char[size];
		memcpy(this->data, data, size);
		dataSize = size;
	}
	
	char *getData() {
		return data;
	}
	
	long getDataSize() {
		return dataSize;
	}
	
	long getIntData() {
		return *((long *)data);
	}
	
	char getByteData() {
		return *((char *)data);
	}
	
	std::string toString () {
		std::ostringstream ss;
		char *dataPtr = this->getData();
		switch (type) {
		case 0x0:{
			ss<<"null";
			break;}
		case 0x1:{
			unsigned char value = *((unsigned char *)dataPtr);
			unsigned long temp = value;
			ss<<temp;
			break;}
		case 0x2:{
			signed short value = *((signed short *)dataPtr);
			ss<<value;
			break;}
		case 0x3:{
			unsigned short value = *((unsigned short *)dataPtr);
			ss<<value;
			break;}
		case 0x4:{
			signed long value = *((signed long *)dataPtr);
			ss<<value;
			break;}
		case 0x5:{
			unsigned long value = *((unsigned long *)dataPtr);
			ss<<value;
			break;}
		case 0x6:{
			float value = *((float *)dataPtr);
			ss<<value;
			break;}
		case 0x7:{
			double value = *((double *)dataPtr);
			ss<<value;
			break;}
		case 0x8:{
			signed long value = *((signed long *)dataPtr);
			signed long value2 = *((signed long *)(dataPtr + 4));
			ss<<value<<", "<<value2;
			break;}
		case 0x9:{
			signed long value = *((signed long *)dataPtr);
			signed long value2 = *((signed long *)dataPtr + 1);
			signed long value3 = *((signed long *)dataPtr + 2);
			ss<<value<<", "<<value2<<", "<<value3;
			break;}
		case 0x0A:{
			signed long value = *((signed long *)dataPtr);
			signed long value2 = *((signed long *)dataPtr + 1);
			signed long value3 = *((signed long *)dataPtr + 2);
			signed long value4 = *((signed long *)dataPtr + 3);
			ss<<value<<", "<<value2<<", "<<value3<<", "<<value4;
			break;}
		case 0x0B:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			ss<<value<<", "<<value2;
			break;}
		case 0x0C:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			ss<<value<<", "<<value2<<", "<<value3;
			break;}
		case 0x0D:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			ss<<value<<", "<<value2<<", "<<value3<<", "<<value4;
			break;}
		case 0x0E:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			ss<<"{"<<value<<", "<<value2<<"},"
			 <<" {"<<value3<<", "<<value4<<"}";
			break;}
		case 0x0F:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			float value5 = *((float *)dataPtr + 4);
			float value6 = *((float *)dataPtr + 5);
			float value7 = *((float *)dataPtr + 6);
			float value8 = *((float *)dataPtr + 7);
			float value9 = *((float *)dataPtr + 8);
			ss<<"{"<<value<<", "<<value2<<", "<<value3<<"},"
			 <<" {"<<value4<<", "<<value5<<","<<value6<<"},"
			<<" {"<<value7<<", "<<value8<<", "<<value9<<"}";
			break;}
		case 0x10:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			float value5 = *((float *)dataPtr + 4);
			float value6 = *((float *)dataPtr + 5);
			float value7 = *((float *)dataPtr + 6);
			float value8 = *((float *)dataPtr + 7);
			float value9 = *((float *)dataPtr + 8);
			float value10 = *((float *)dataPtr + 9);
			float value11 = *((float *)dataPtr + 10);
			float value12 = *((float *)dataPtr + 11);
			ss<<"{"<<value<<", "<<value2<<", "<<value3<<", "<<value4<<"},"
			 <<" {"<<value5<<", "<<value6<<", "<<value7<<", "<<value8<<"},"
			<<" {"<<value9<<", "<<value10<<", "<<value11<<", "<<value12<<"}";
			break;}
		case 0x11:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			float value5 = *((float *)dataPtr + 4);
			float value6 = *((float *)dataPtr + 5);
			float value7 = *((float *)dataPtr + 6);
			float value8 = *((float *)dataPtr + 7);
			float value9 = *((float *)dataPtr + 8);
			float value10 = *((float *)dataPtr + 9);
			float value11 = *((float *)dataPtr + 10);
			float value12 = *((float *)dataPtr + 11);
			ss<<"{"<<value<<", "<<value2<<", "<<value3<<"},"
			 <<" {"<<value4<<", "<<value5<<", "<<value6<<"},"
			<<" {"<<value7<<", "<<value8<<", "<<value9<<"},"
			<<" {"<<value10<<", "<<value11<<", "<<value12<<"}";
			break;}
		case 0x12:{
			float value = *((float *)dataPtr);
			float value2 = *((float *)dataPtr + 1);
			float value3 = *((float *)dataPtr + 2);
			float value4 = *((float *)dataPtr + 3);
			float value5 = *((float *)dataPtr + 4);
			float value6 = *((float *)dataPtr + 5);
			float value7 = *((float *)dataPtr + 6);
			float value8 = *((float *)dataPtr + 7);
			float value9 = *((float *)dataPtr + 8);
			float value10 = *((float *)dataPtr + 9);
			float value11 = *((float *)dataPtr + 10);
			float value12 = *((float *)dataPtr + 11);
			float value13 = *((float *)dataPtr + 12);
			float value14 = *((float *)dataPtr + 13);
			float value15 = *((float *)dataPtr + 14);
			float value16 = *((float *)dataPtr + 15);
			ss<<"{"<<value<<", "<<value2<<", "<<value3<<", "<<value4<<"},"
			 <<" {"<<value5<<", "<<value6<<", "<<value7<<", "<<value8<<"},"
			<<" {"<<value9<<", "<<value10<<", "<<value11<<", "<<value12<<"},"
			<<" {"<<value13<<", "<<value14<<", "<<value15<<", "<<value16<<"}";
			break;}
		case 0x13:{
			unsigned char value = *((unsigned char *)dataPtr);
			ss<<(value == 1 ? "true" : "false");
			break;}
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:{
			ss.write(dataPtr, this->getDataSize());
			ss<<'\0';
			break;}
		case 0x1A:{
			unsigned long long value = *((unsigned long long *)dataPtr);
			ss<<value;
			break;}
		case 0x1B:{
			signed char value = *((signed char *)dataPtr);
			signed long temp = value;
			ss<<temp;
			break;}
		case 0x1C:{
			ss.write(dataPtr, strlen(dataPtr) + 1);
			break;}
		case 0x1D:
		case 0x1E:{
			char *alloc = new char[this->getDataSize() / 2];
			wcstombs(alloc, (const wchar_t*)dataPtr, this->getDataSize());
			ss<<alloc;
			delete []alloc;
			break;}
		}
		return ss.str();
	}
	std::string &getLocalized1();
	void setLocalized1(const std::string &value);
	std::string &getLocalized2();
	void setLocalized2(const std::string &value);
};

#endif // LSBOBJECT_H
