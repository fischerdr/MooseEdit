#ifndef PAKREADER_H
#define PAKREADER_H
#include <cstdlib>
#include <istream>
#include <vector>
#include <boost/shared_ptr.hpp>

#pragma pack(push, 1)

struct HEADER_PAK {
	long version;
	long unknown0;
	long pakCount;
	char unknown[5];
	long fileCount;
};

struct HEADER_PAK_FILEINFO {
	char fileName[0x100];
	long dataSectionOffset;
	long fileSize;
	long unknown1;
	long pakNumber;
};

#pragma pack(pop)

class PakReader
{
	long dataStart;
	std::vector<boost::shared_ptr<HEADER_PAK_FILEINFO> > fileInfo;
	long getNextBlock(long offset);
	long dataOffsetToAbsolute(long dataOffset, long pakNumber);
	long getStreamLength(std::istream& input);
	std::string getFileNameByPakNumber(std::string& fileName, long pakNumber);
	HEADER_PAK_FILEINFO *getHeaderForFile(std::string& filePath);
public:
	static const long BLOCK_SIZE = 0x8000;
	static const long expectedVersion = 0x09;
	PakReader();
	bool loadFile(std::string fileName);
	std::vector<std::string> getFileList();
	bool extractFile(std::string fileName, std::string& filePath, std::string& destination, bool preservePath);
};

#endif // PAKREADER_H
