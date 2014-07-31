#include "LsbWriter.h"
#include <stack>
#include <sstream>
#include <cstring>
#include <ctime>

long LsbWriter::writeSingleObject(LsbObject *object, std::ostream& output) {
	std::ostringstream ss;
	long idx = object->getIndex();
	long type = object->getType();
	bool isStr = false;
	bool isUnicodeStr = false;
	bool isLocalizedStr = false;
	if (!object->isDirectory()) {
		if (type == 0x16 || type == 0x17 || type == 0x19 || type == 0x1E)
			isStr = true;
		if (type == 0x1C) {
			isLocalizedStr = true;
		}
		if (type == 0x1E) {
			isUnicodeStr = true;
		}
	}
	else {
		std::vector<LsbObject *> items = object->getItemsOnly();
		std::vector<LsbObject *> dirs = object->getDirectoriesOnly();
		type = items.size();
		long dirCount = dirs.size();
		object->setData((char *)&dirCount, sizeof(long));
	}
	ss.write((char *)&idx, sizeof(long));\
	ss.write((char *)&type, sizeof(long));
	if (isStr) {
		long dataSize = object->getDataSize();
		if (isUnicodeStr) {
			dataSize /= 2;
		}
		ss.write((char *)&dataSize, sizeof(long));
	}
	
	if (isLocalizedStr) {
		char *firstString = object->getData();
		long firstStringLength = strlen(firstString);
		long firstStringBytes = firstStringLength + 2;
		ss.write((char *)&(firstStringBytes), sizeof(long));
		ss.write(firstString, firstStringBytes);
		char *secondString = object->getData() + firstStringLength + 2;
		long secondStringBytes = object->getDataSize() - firstStringBytes;
		ss.write((char *)&(secondStringBytes), sizeof(long));
		ss.write(secondString, secondStringBytes);
	}
	else {
		ss.write(object->getData(), object->getDataSize());
	}
	std::string result = ss.str();
	long size = result.size();
	output.write(result.data(), result.size());
	return size;
}

bool LsbWriter::writeFile(std::vector<LsbObject *>& objects, std::vector<TAG_LSB *>& tagList, std::ostream& output) {
	long bytesWritten = 0;
	HEADER_LSB header;
	memset(&header, 0, sizeof(HEADER_LSB));
	header.magic = 0x40000000;
	header.fileLength = 0x0;
	header.fileTimestamp = time(0);
	header.verMajor = 2;
	header.verMinor = 0;
	header.verBuild = 9;
	header.verRevision = 0;
	header.tagCount = tagList.size();
	output.write((char *)&header, sizeof(HEADER_LSB));
	bytesWritten += sizeof(HEADER_LSB);
	for (int i=0; i<tagList.size(); ++i) {
		output.write((char *)&tagList[i]->tagLength, sizeof(tagList[i]->tagLength));
		bytesWritten += sizeof(tagList[i]->tagLength);
		output.write(tagList[i]->tag, tagList[i]->tagLength);
		bytesWritten += tagList[i]->tagLength;
		output.write((char *)&tagList[i]->index, sizeof(tagList[i]->index));
		bytesWritten += sizeof(tagList[i]->index);
	}
	long dirCount = objects.size();
	output.write((char *)&dirCount, sizeof(dirCount));
	bytesWritten += sizeof(dirCount);
	long dirHeaderOffset = output.tellp();
	for (int i=0; i<objects.size(); ++i) {
		ENTITY_HEADER_LSB dirHeader;
		dirHeader.id = objects[i]->getIndex();
		dirHeader.dataOffset = 0x0;
		output.write((char *)&dirHeader, sizeof(ENTITY_HEADER_LSB));
		bytesWritten += sizeof(ENTITY_HEADER_LSB);
	}
	std::vector<long> headerOffsets;
	for (int i=0; i<objects.size(); ++i) {
		headerOffsets.push_back(output.tellp());
		std::stack<LsbObject *> dirStack;
		dirStack.push(objects[i]);
		while (!dirStack.empty()) {
			LsbObject *currentDirectory = dirStack.top();
			dirStack.pop();
			if (!currentDirectory->isEntity())
				bytesWritten += writeSingleObject(currentDirectory, output);
			std::vector<LsbObject *> items = currentDirectory->getItemsOnly();
			std::vector<LsbObject *> dirs = currentDirectory->getDirectoriesOnly();
			for (int j=0; j<items.size(); ++j) {
				bytesWritten += writeSingleObject(items[j], output);
			}
			for (int j=dirs.size() - 1; j>=0; --j) {
				dirStack.push(dirs[j]);
			}
		}
	}
	output.seekp((long)&header.fileLength - (long)&header, std::ios_base::beg);
	header.fileLength = bytesWritten;
	output.write((char *)&header.fileLength, sizeof(header.fileLength));
	
	output.seekp(dirHeaderOffset, std::ios_base::beg);
	for (int i=0; i<objects.size(); ++i) {
		ENTITY_HEADER_LSB dirHeader;
		dirHeader.id = objects[i]->getIndex();
		dirHeader.dataOffset = headerOffsets[i];
		output.write((char *)&dirHeader, sizeof(ENTITY_HEADER_LSB));
	}
	return true;
}

LsbWriter::LsbWriter()
{
}
