#include "PakReader.h"
#include <fstream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <boost/filesystem/fstream.hpp>

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

std::wstring PakReader::getFileNameByPakNumber(std::wstring& fileName, long pakNumber) {
	if (pakNumber == 0) {
		return fileName;
	}
	long lastDot = fileName.find_last_of(L".");
	std::wstringstream result;
	result<<fileName.substr(0, lastDot)<<L"_"<<pakNumber<<L".pak";
	return result.str();
}

HEADER_PAK_FILEINFO_LSPK *PakReader::getHeaderForFileLspk(std::string& filePath) {
	for (int i=0; i<fileInfoLspk.size(); ++i) {
		if (fileInfoLspk[i]->fileName == filePath) {
			return fileInfoLspk[i].get();
		}
	}
	return 0;
}

std::wstring PakReader::getLastExtractPath() const
{
	return lastExtractPath;
}


bool PakReader::loadFile(std::wstring fileName) {
	fileInfoLspk.clear();
	boost::filesystem::ifstream input(fileName, std::ios_base::binary);
	if (input) {
		int magicSampler = 0;
		input.read((char *)&magicSampler, sizeof(int));
		input.seekg(0, std::ios_base::beg);
		if (magicSampler == 0x4B50534C) {
			isLspk = true;
		} else {
			isLspk = false;
		}
		long fileCount = 0;
		if (isLspk) {
			HEADER_PAK_LSPK pakHeader;
			input.read((char *)&pakHeader, sizeof(HEADER_PAK_LSPK));
			dataStart = pakHeader.dataOffset;
			fileCount = pakHeader.fileCount;
		} else {
			HEADER_PAK pakHeader;
			input.read((char *)&pakHeader, sizeof(HEADER_PAK));
			long dataHeaderSize = sizeof(HEADER_PAK_FILEINFO) * pakHeader.fileCount;
			long headerEnd = sizeof(HEADER_PAK) + dataHeaderSize;
			dataStart = getNextBlock(headerEnd);
			fileCount = pakHeader.fileCount;
		}
		for (int i=0; i<fileCount; ++i) {
			fileInfoLspk.push_back(boost::shared_ptr<HEADER_PAK_FILEINFO_LSPK>(new HEADER_PAK_FILEINFO_LSPK));
			HEADER_PAK_FILEINFO_LSPK *currentInfo = fileInfoLspk[fileInfoLspk.size() - 1].get();
			if (input) {
				if (isLspk) {
					input.read((char *)currentInfo, sizeof(HEADER_PAK_FILEINFO_LSPK));
				} else {
					HEADER_PAK_FILEINFO tempInfo;
					input.read((char *)&tempInfo, sizeof(HEADER_PAK_FILEINFO));
					strcpy(currentInfo->fileName, tempInfo.fileName);
					currentInfo->dataSectionOffset = tempInfo.dataSectionOffset;
					currentInfo->fileSize = tempInfo.fileSize;
					currentInfo->pakNumber = tempInfo.pakNumber;
					currentInfo->compressionMethod = 0;
					currentInfo->unknown2 = 0;
					currentInfo->decompressedSize = 0;
				}
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
	for (int i=0; i<fileInfoLspk.size(); ++i) {
		fileList.push_back(fileInfoLspk[i]->fileName);
	}
	return fileList;
}

bool PakReader::extractFile(std::wstring fileName, std::string& filePath, std::wstring& destination, bool preservePath) {
	unsigned long fileSize;
	char *fileBytes = extractFileIntoMemory(fileName, filePath, destination, preservePath, &fileSize);
	if (fileBytes == 0) {
		return false;
	}
	
	boost::filesystem::ofstream outFile(lastExtractPath, std::ios_base::binary);
	outFile.write(fileBytes, fileSize);
	delete[] fileBytes;
	if (!outFile) {
		outFile.close();
		return false;
	}
	outFile.close();
	
	return true;
}
	
char *PakReader::extractFileIntoMemory(std::wstring fileName, std::string& filePath, std::wstring& destination, bool preservePath, unsigned long *fileSize) {
	if (fileSize == 0) {
		return 0;
	}
	HEADER_PAK_FILEINFO_LSPK *info = getHeaderForFileLspk(filePath);
	if (info != 0) {
		boost::filesystem::ifstream input(getFileNameByPakNumber(fileName, info->pakNumber).c_str(), std::ios_base::binary);
		long fileStart = dataOffsetToAbsolute(info->dataSectionOffset, info->pakNumber);
		input.seekg(fileStart);
		char *alloc = new char[info->fileSize];
		input.read(alloc, info->fileSize);
		if (!input) {
			delete []alloc;
			input.close();
			return 0;
		}
		long cwdSize = 1024;
		wchar_t cwd[cwdSize];
		_wgetcwd(cwd, cwdSize);
		std::wstringstream ss;
		if (!preservePath) {
			long fileNameSize = strlen(info->fileName) + 1;
			std::wstring extractFileName;
			{
				wchar_t alloc[fileNameSize];
				mbstowcs(alloc, info->fileName, fileNameSize);
				extractFileName = alloc;
			}
			long lastPathDelimiter = extractFileName.find_last_of(L'/');
			if (lastPathDelimiter == std::string::npos) {
				ss<<destination<<(destination[destination.size() - 1] == L'/' ? L"" : L"/")<<extractFileName;
			}
			else {
				ss<<destination<<(destination[destination.size() - 1] == L'/' ? L"" : L"/")<<extractFileName.substr(lastPathDelimiter + 1);
			}
		} else {
			boost::char_separator<wchar_t> sep(L"\\/");
			typedef boost::tokenizer<boost::char_separator<wchar_t> , std::wstring::const_iterator, std::wstring> PathTokenizer;
			long fileNameSize = strlen(info->fileName) + 1;
			std::wstring name;
			{
				wchar_t alloc[fileNameSize];
				mbstowcs(alloc, info->fileName, fileNameSize);
				name = alloc;
			}
			PathTokenizer tok(name, sep);
			std::wstring lastToken = L"";
			_wchdir(destination.c_str());
			ss<<destination<<(destination[destination.size() - 1] == '/' ? "" : "/");
			for (PathTokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
				std::wstring token = *it;
				if (lastToken.length() != 0) {
					_wmkdir(lastToken.c_str());
					_wchdir(lastToken.c_str());
					ss<<lastToken<<L"/";
				}
				lastToken = token;
			}
			ss<<lastToken;
		}
		lastExtractPath = ss.str();
		if (info->decompressedSize > 0) {
			char *decompressionBuffer = new char[info->decompressedSize];
			int result = compressor.decompress((char *)decompressionBuffer, (char *)alloc, info->fileSize, info->decompressedSize);
			if (result > 0) {
				*fileSize = info->decompressedSize;
				delete[] alloc;
				alloc = decompressionBuffer;
			}
		} else {
			*fileSize = info->fileSize;
		}

		input.close();
		_wchdir(cwd);
		return alloc;
	}
	return 0;
}

PakReader::PakReader()
{
}
