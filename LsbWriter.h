#ifndef LSBWRITER_H
#define LSBWRITER_H

#include "LsbObject.h"
#include <vector>
#include <ostream>

class WriterProgressCallback {
public:
	virtual void onSaveBegin(int dirCount) = 0;
	virtual void onSaveUpdate(int dirsLeft) = 0;
	virtual void onSaveEnd() = 0;
};

class LsbWriter
{
	WriterProgressCallback *writerProgressCallback = 0;
public:
	void registerProgressCallback(WriterProgressCallback *writerProgressCallback) {
		this->writerProgressCallback = writerProgressCallback;
	}

	LsbWriter();
	bool writeFile(std::vector<LsbObject *>& objects, std::vector<TAG_LSB *>& tagList, std::ostream& output);
	long writeSingleObject(LsbObject *object, std::ostream& output);
};

#endif // LSBWRITER_H
