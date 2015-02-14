#include "SanityHash.h"
#include <cmath>

void SanityHash::addFile(const unsigned char *file, unsigned long fileSize) {
	const unsigned long BLOCK_SIZE = 64;
	unsigned char block[64] = {0};
	unsigned long bytesLeft = fileSize;
	unsigned long bytesToRead;
	
	const unsigned char *current = file;
	while (bytesLeft > 0) {
		bytesToRead = std::min(BLOCK_SIZE, bytesLeft);
		memcpy(block, current, bytesToRead);
		current += bytesToRead;
		bytesLeft -= bytesToRead;
		md5_update(&ctx, block, BLOCK_SIZE);
	}
}

unsigned char *SanityHash::compute() {
	if (!freed) {
		freed = true;
		md5_finish(&ctx, output);
		md5_free( &ctx );
	}
	return output;
}

template <typename T>
void SanityHash::copyVec(std::vector<T>& vec, std::vector<T *>& ptrVec) {
	for (int i=0; i<vec.size(); ++i) {
		ptrVec.push_back(&vec[i]);
	}
}

const static std::string hexmap = "0123456789abcdef";
std::string SanityHash::generateHexString() {
	std::string result = "";
	unsigned char *computed = this->compute();
	for (int i=0; i<COMPUTE_SIZE; ++i) {
		unsigned char byte = computed[i];
		result += hexmap[byte/0x10];
		result += hexmap[byte%0x10];
	}
	return result;
}

void SanityHash::generateLsb(std::vector<TAG_LSB *>& sanityTags, std::vector<LsbObject *>& sanityObjects) {
	this->sanityTags.resize(3);
	
	TAG_LSB& sanityTag = this->sanityTags[0];
	sanityTag.index = 0;
	std::string sanityTagText = "Sanity";
	sanityTag.tag = new char[sanityTagText.length() + 1];
	strcpy(sanityTag.tag, sanityTagText.c_str());
	sanityTag.tagLength = sanityTagText.length();
	
	TAG_LSB& rootTag = this->sanityTags[1];
	rootTag.index = 1;
	std::string rootTagText = "root";
	rootTag.tag = new char[rootTagText.length() + 1];
	strcpy(rootTag.tag, rootTagText.c_str());
	rootTag.tagLength = rootTagText.length();
	
	TAG_LSB& hashTag = this->sanityTags[2];
	hashTag.index = 2;
	std::string hashTagText = "Hash";
	hashTag.tag = new char[hashTagText.length() + 1];
	strcpy(hashTag.tag, hashTagText.c_str());
	hashTag.tagLength = hashTagText.length();
	
	this->sanityObjects.resize(4);
	this->sanityObjects[0] = LsbObject(true, sanityTag.index, sanityTag.tag, 0, 0);
	LsbObject& sanityEntity = this->sanityObjects[0];
	this->sanityObjects[1] = LsbObject(true, rootTag.index, rootTag.tag, 0, &sanityEntity);
	LsbObject& rootNode = this->sanityObjects[1];
	this->sanityObjects[2] = LsbObject(true, sanityTag.index, sanityTag.tag, 0, &rootNode);
	LsbObject& sanityNode = this->sanityObjects[2];
	this->sanityObjects[3] = LsbObject(false, hashTag.index, hashTag.tag, 0x17, &sanityNode);
	LsbObject& hashNode = this->sanityObjects[3];
	
	sanityEntity.setEntityId(1);
	
	std::string md5Text = this->generateHexString();
	hashNode.setData(md5Text.c_str(), md5Text.length() + 1);
	
	sanityEntity.addChild(&rootNode);
	rootNode.addChild(&sanityNode);
	sanityNode.addChild(&hashNode);
	
	copyVec(this->sanityTags, sanityTags);
	sanityObjects.push_back(&sanityEntity);
}

SanityHash::~SanityHash() {
	if (!freed) {
		md5_finish( &ctx, output );
		md5_free( &ctx );
	}
	for (int i=0; i<sanityTags.size(); ++i) {
		delete[] sanityTags[i].tag;
	}
	for (int i=0; i<sanityObjects.size(); ++i) {
		LsbObject &object = sanityObjects[i];
		object.getChildren().clear();
	}
}

SanityHash::SanityHash() {
	md5_init( &ctx );
	md5_starts( &ctx );
}
