#ifndef PAKREADER_H
#define PAKREADER_H
#include <cstdlib>
#include <istream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "PakCompressor.h"

#pragma pack(push, 1)

struct HEADER_PAK {
	long version;
	long unknown0;
	long pakCount;
	char unknown[5];
	long fileCount;
};

struct HEADER_PAK_LSPK {
	long magic;
	long version;
	long dataOffset;
	long unknown1;
	short pakCount;
	short unknown2;
	long fileCount;
};

struct HEADER_PAK_FILEINFO {
	char fileName[0x100];
	long dataSectionOffset;
	long fileSize;
	long unknown1;
	long pakNumber;
};

struct HEADER_PAK_FILEINFO_LSPK {
	char fileName[0x100];
	long dataSectionOffset;
	long fileSize;
	long decompressedSize;
	long pakNumber;
	long unknown1;
	long unknown2;
};

#pragma pack(pop)

class PakReader
{
	PakCompressor compressor;
	bool isLspk;
	long dataStart;
	std::vector<boost::shared_ptr<HEADER_PAK_FILEINFO_LSPK> > fileInfoLspk;
	long getNextBlock(long offset);
	long dataOffsetToAbsolute(long dataOffset, long pakNumber);
	long getStreamLength(std::istream& input);
	std::wstring getFileNameByPakNumber(std::wstring &fileName, long pakNumber);
	HEADER_PAK_FILEINFO_LSPK *getHeaderForFileLspk(std::string& filePath);
	std::wstring lastExtractPath;
public:
	static const long BLOCK_SIZE = 0x8000;
	static const long expectedVersion = 0x09;
	PakReader();
	bool loadFile(std::wstring fileName);
	std::vector<std::string> getFileList();
	bool extractFile(std::wstring fileName, std::string& filePath, std::wstring &destination, bool preservePath);
	char *extractFileIntoMemory(std::wstring fileName, std::string& filePath, std::wstring &destination, bool preservePath, unsigned long *fileSize);
	std::wstring getLastExtractPath() const;
};

#endif // PAKREADER_H
