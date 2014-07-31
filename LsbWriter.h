#ifndef LSBWRITER_H
#define LSBWRITER_H

#include "LsbObject.h"
#include <vector>
#include <ostream>

class LsbWriter
{
public:
	LsbWriter();
	bool writeFile(std::vector<LsbObject *>& objects, std::vector<TAG_LSB *>& tagList, std::ostream& output);
	long writeSingleObject(LsbObject *object, std::ostream& output);
};

#endif // LSBWRITER_H
