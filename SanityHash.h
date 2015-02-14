#ifndef SANITYHASH_H
#define SANITYHASH_H

#include <vector>
#include "LsbObject.h"
#include "md5.h"

#define COMPUTE_SIZE	16

class SanityHash
{
	md5_context ctx;
	std::vector<TAG_LSB> sanityTags;
	std::vector<LsbObject> sanityObjects;
	bool freed = false;
	unsigned char output[COMPUTE_SIZE];
	template <typename T> void copyVec(std::vector<T>& vec, std::vector<T *>& ptrVec);
public:
	void addFile(const unsigned char *file, unsigned long fileSize);
	std::string generateHexString();
	void generateLsb(std::vector<TAG_LSB *> &sanityTags, std::vector<LsbObject *> &sanityObjects);
	unsigned char *compute();
	~SanityHash();
	SanityHash();
};

#endif // SANITYHASH_H
