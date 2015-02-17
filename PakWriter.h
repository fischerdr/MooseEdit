#ifndef PAKWRITER_H
#define PAKWRITER_H

#include "PakInfo.h"
#include "PakCompressor.h"
#include <vector>
#include <ostream>

struct LSPK_FILE_DATA {
	HEADER_PAK_FILEINFO_LSPK fileInfo;
	bool isCompressed;
	char *buffer;
};

class PakWriter
{
	PakCompressor compressor;
	HEADER_PAK_LSPK lspkHeader;
	std::vector<LSPK_FILE_DATA> lspkFileData;
public:
	bool getShouldCompress() const;
	void setShouldCompress(bool value);
	bool getHighCompressionMode() const;
	void setHighCompressionMode(bool value);
	PakWriter();
	~PakWriter();
	void writeFile(std::ostream &file);
	void addFile(std::string &fileName, char *buffer, unsigned long bufferSize);
};

#endif // PAKWRITER_H
