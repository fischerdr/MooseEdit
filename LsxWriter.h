#ifndef LSBWRITER_H
#define LSBWRITER_H

#include "LsbObject.h"
#include <vector>
#include <ostream>

class LsxWriterProgressCallback {
public:
	virtual void onSaveBegin(int dirCount) = 0;
	virtual void onSaveUpdate(int dirsLeft) = 0;
	virtual void onSaveEnd() = 0;
};

class LsxWriter
{
	LsxWriterProgressCallback *writerProgressCallback = 0;
public:
	void registerProgressCallback(LsxWriterProgressCallback *writerProgressCallback) {
		this->writerProgressCallback = writerProgressCallback;
	}

	LsxWriter();
	bool writeFile(std::vector<LsbObject *>& objects, std::vector<TAG_LSB *>& tagList, std::ostream& output);
	long writeSingleObject(LsbObject *object, std::ostream& output);
};

#endif // LSBWRITER_H
