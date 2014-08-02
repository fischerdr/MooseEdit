#include "PakReader.h"
#include <fstream>
#include <sstream>

long PakReader::getNextBlock(long offset) {
	return offset - (offset % BLOCK_SIZE) + BLOCK_SIZE;
}

long PakReader::dataOffsetToAbsolute(long dataOffset, long pakNumber) {
	if (pakNumber == 0) {
		return dataStart + dataOffset;
	}
	return dataOffset;
}

long PakReader::getStreamLength(std::istream& input) {
	long current = input.tellg();
	input.seekg(0, std::ios_base::beg);
	long fileStart = input.tellg();
	input.seekg(0, std::ios_base::end);
	long streamSize = input.tellg() - fileStart;
	input.seekg(current);
	return streamSize;
}

std::string PakReader::getFileNameByPakNumber(std::string& fileName, long pakNumber) {
	if (pakNumber == 0) {
		return fileName;
	}
	long lastDot = fileName.find_last_of(".");
	std::ostringstream result;
	result<<fileName.substr(0, lastDot)<<"_"<<pakNumber<<".pak";
	return result.str();
}

HEADER_PAK_FILEINFO *PakReader::getHeaderForFile(std::string& filePath) {
	for (int i=0; i<fileInfo.size(); ++i) {
		if (fileInfo[i]->fileName == filePath) {
			return fileInfo[i].get();
		}
	}
	return 0;
}

bool PakReader::loadFile(std::string fileName) {
	fileInfo.clear();
	std::ifstream input(fileName.c_str(), std::ios_base::binary);
	HEADER_PAK pakHeader;
	if (input) {
		input.read((char *)&pakHeader, sizeof(HEADER_PAK));
//		if (pakHeader.version > this->expectedVersion) {
//			input.close();
//			return false;
//		}
		long dataHeaderSize = sizeof(HEADER_PAK_FILEINFO) * pakHeader.fileCount;
		long headerEnd = sizeof(HEADER_PAK) + dataHeaderSize;
		dataStart = getNextBlock(headerEnd);
		for (int i=0; i<pakHeader.fileCount; ++i) {
			fileInfo.push_back(boost::shared_ptr<HEADER_PAK_FILEINFO>(new HEADER_PAK_FILEINFO));
			HEADER_PAK_FILEINFO *currentInfo = fileInfo[fileInfo.size() - 1].get();
			if (input) {
				input.read((char *)currentInfo, sizeof(HEADER_PAK_FILEINFO));
			}
			else {
				input.close();
				return false;
			}
		}
	} else {
		return false;
	}
	input.close();
	return true;
}

std::vector<std::string> PakReader::getFileList() {
	std::vector<std::string> fileList;
	for (int i=0; i<fileInfo.size(); ++i) {
		fileList.push_back(fileInfo[i]->fileName);
	}
	return fileList;
}

//TODO: implement preservePath
bool PakReader::extractFile(std::string fileName, std::string& filePath, std::string& destination, bool preservePath) {
	HEADER_PAK_FILEINFO *info = getHeaderForFile(filePath);
	if (info != 0) {
		std::ifstream input(getFileNameByPakNumber(fileName, info->pakNumber).c_str(), std::ios_base::binary);
		long fileStart = dataOffsetToAbsolute(info->dataSectionOffset, info->pakNumber);
		input.seekg(fileStart);
		char *alloc = new char[info->fileSize];
		input.read(alloc, info->fileSize);
		if (!input) {
			delete []alloc;
			input.close();
			return false;
		}
		std::ostringstream ss;
		if (!preservePath) {
			std::string extractFileName = info->fileName;
			long lastPathDelimiter = extractFileName.find_last_of('/');
			if (lastPathDelimiter == std::string::npos) {
				ss<<destination<<(destination[destination.size() - 1] == '/' ? "" : "/")<<extractFileName;
			}
			else {
				ss<<destination<<(destination[destination.size() - 1] == '/' ? "" : "/")<<extractFileName.substr(lastPathDelimiter + 1);
			}
		}
		std::ofstream outFile(ss.str().c_str(), std::ios_base::binary);
		outFile.write(alloc, info->fileSize);
		delete []alloc;
		if (!outFile) {
			outFile.close();
			input.close();
			return false;
		}
		outFile.close();
		input.close();
		return true;
	}
	return false;
}

PakReader::PakReader()
{
}
