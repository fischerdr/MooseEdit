#include "PakWriter.h"
#include <cstring>
#include <cstdlib>

bool PakWriter::getShouldCompress() const
{
	return compressor.getShouldCompress();
}

void PakWriter::setShouldCompress(bool value)
{
	compressor.setShouldCompress(value);
}

bool PakWriter::getHighCompressionMode() const
{
	return compressor.getHighCompressionMode();
}

void PakWriter::setHighCompressionMode(bool value)
{
	compressor.setHighCompressionMode(value);
}

void PakWriter::addFile(std::string &fileName, char *buffer, unsigned long bufferSize) {
	lspkFileData.push_back(LSPK_FILE_DATA());
	LSPK_FILE_DATA &back = lspkFileData.back();
	char *alloc = new char[bufferSize];
	bool shouldCompress = compressor.getShouldCompress();
	int compressedSize = compressor.compress(buffer, alloc, bufferSize);
	void *temp = realloc(alloc, compressedSize);
	if (temp != alloc) {
		delete[] alloc;
		alloc = (char *)temp;
	}
	back.buffer = alloc;
	back.isCompressed = shouldCompress;
	strncpy(back.fileInfo.fileName, fileName.c_str(), sizeof(back.fileInfo.fileName));
	
	if (shouldCompress) {
		back.fileInfo.decompressedSize = bufferSize;
		back.fileInfo.fileSize = compressedSize;
		back.fileInfo.compressionMethod = COMPRESSION_METHOD_LZ4;
	} else {
		back.fileInfo.decompressedSize = 0;
		back.fileInfo.fileSize = bufferSize;
		back.fileInfo.compressionMethod = 0;
	}
	back.fileInfo.pakNumber = 0;
	back.fileInfo.unknown2 = 0;
	
    ++lspkHeader.pakCount;
}

void PakWriter::writeFile(std::ostream &file) {
    lspkHeader.pakCount = lspkFileData.size();
	lspkHeader.fileInfoSectionSize = sizeof(HEADER_PAK_FILEINFO_LSPK)*lspkFileData.size() + 4;
	unsigned long *offsets = new unsigned long[lspkFileData.size()];
	file.write((char *)&lspkHeader, sizeof(lspkHeader));
	for (int i=0; i<lspkFileData.size(); ++i) {
		LSPK_FILE_DATA &data = lspkFileData[i];
		file.write((char *)&data.fileInfo, sizeof(data.fileInfo));
	}
	lspkHeader.dataOffset = file.tellp();
	for (int i=0; i<lspkFileData.size(); ++i) {
		offsets[i] = file.tellp() - lspkHeader.dataOffset;
		LSPK_FILE_DATA &data = lspkFileData[i];
		file.write(data.buffer, data.fileInfo.fileSize);
	}
	
	
	for (int i=0; i<lspkFileData.size(); ++i) {
		LSPK_FILE_DATA &data = lspkFileData[i];
		file.seekp(sizeof(lspkHeader) + sizeof(data.fileInfo) * i + ((char *)&data.fileInfo.dataSectionOffset - (char *)&data.fileInfo), std::ios_base::beg);
		file.write((char *)&offsets[i], sizeof(offsets[i]));
	}
	file.seekp((char *)&lspkHeader.dataOffset - (char *)&lspkHeader, std::ios_base::beg);
	file.write((char *)&lspkHeader.dataOffset, sizeof(lspkHeader.dataOffset));
	delete[] offsets;
}

PakWriter::~PakWriter() {
	for (int i=0; i<lspkFileData.size(); ++i) {
		LSPK_FILE_DATA &data = lspkFileData[i];
		if (data.isCompressed) {
			delete[] data.buffer;
		}
	}
}

PakWriter::PakWriter()
{
	lspkHeader.version = 0x0A;
    lspkHeader.pakCount = 0;
	lspkHeader.fileInfoSectionSize = 0;
	lspkHeader.unknown2 = 1;
}
