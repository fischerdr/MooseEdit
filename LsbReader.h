#ifndef LSBREADER_H
#define LSBREADER_H
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stack>
#include "LsbObject.h"

class LsbReader
{
	bool loanedTagList = false;
	std::vector<TAG_LSB *> tagList;
	HEADER_LSB cachedHeader;
	bool headerCached = false;
    long entityCount;
    std::vector<ENTITY_HEADER_LSB *> entityHeaderList;
	bool dataHeaderCached = false;
	void addItemListToDirectory(long& entitiesBuilt, long& builtEntityTagCount, long& readTagCount, std::vector<LsbObject *>& arrayEntity, LsbObject *array, TAG_LSB *tagPtr, std::stack<LsbObject *>& arrayStack, bool& doneBuilding, bool& readFirst);
	LsbObject *addChildToCurrentDirectory(LsbObject *child, LsbObject *array, TAG_LSB *tagPtr, std::stack<LsbObject *>& directoryStack);
	bool readTags(std::istream& input, long tagCount, int& bytesLeft, std::vector<TAG_LSB *>& tagList, char *alloc);
    bool readTagData(std::istream& input, long readSize, std::stack<LsbObject *>& arrayStack, long tagCount, int& bytesLeft, std::vector<TAG_LSB *>& tagList, char *alloc);
	TAG_LSB * getTagByIndex(long &index, std::vector<TAG_LSB *>& tagList);
	void readDataHeader(std::istream& input, int& bytesLeft, char *alloc);
	void cleanup();
public:
	~LsbReader() {
		cleanup();
	}
	LsbReader();
	std::vector<TAG_LSB *> getTagList() {
		loanedTagList = true;
		return tagList;
	}
	void clearTagList(std::vector<TAG_LSB *>& tagList) {
		if (!loanedTagList) {
			for (int i=0; i<tagList.size(); ++i) {
				delete tagList[i];
			}
		}
		tagList.clear();
	}
	std::vector<LsbObject *> loadFile(std::istream& input);
	HEADER_LSB *getHeader();
	static LsbObject *lookupByUniquePathEntity(LsbObject *entity, const char *path);
	static LsbObject *lookupByUniquePath(std::vector<LsbObject *>& entities, const char *path);
	static std::vector<LsbObject *> lookupAllEntitiesWithName(LsbObject *object, const char *name);
	static std::vector<LsbObject *> extractPropertyForEachListItem(std::vector<LsbObject *>& list, const char *propertyName);
	static std::vector<LsbObject *> findItemsByAttribute(std::vector<LsbObject *>& list, const char *attributeName, const char *attributeValue, int valueLength);
	static LsbObject *getObjectCreator(LsbObject *object);
	static LsbObject *getObjectFromCreator(LsbObject *creator, const char *objectGroupName);
	static TAG_LSB * getTagByName(const char *name, std::vector<TAG_LSB *> *tagList);
	static TAG_LSB *createTagIfNeeded(const char *name, std::vector<TAG_LSB *> *tagList);
};

#endif // LSBREADER_H

