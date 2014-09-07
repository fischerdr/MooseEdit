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

class ReaderProgressCallback {
public:
	virtual void onLoadBegin(int dirCount) = 0;
	virtual void onLoadUpdate(int dirsLeft) = 0;
	virtual void onLoadEnd() = 0;
};

class LsbReader
{
	ReaderProgressCallback *readerProgressCallback = 0;
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
	int readCount;
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
	void registerProgressCallback(ReaderProgressCallback *readerProgressCallback) {
		this->readerProgressCallback = readerProgressCallback;
	}
	std::vector<LsbObject *> loadFile(std::istream& input);
	HEADER_LSB *getHeader();
	int getReadCount() const;
};

#endif // LSBREADER_H

