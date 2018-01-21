#ifndef PAKINFO_H
#define PAKINFO_H

#define COMPRESSION_METHOD_LZ4	0x22

#pragma pack(push, 1)

struct HEADER_PAK {
	long version;
	long unknown0;
	long pakCount;
	char unknown[5];
	long fileCount;
};

//struct HEADER_PAK_LSPK {
//	long magic;
//	long version;
//	long dataOffset;
//	long fileInfoSectionSize;
//	short pakCount;
//	short unknown2;
//	long fileCount;
//};
//New header v13
struct HEADER_PAK_LSPK {
    long version;
    long dataOffset;
    long fileInfoSectionSize;
    short pakCount;
    short unknown2;
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
	long compressionMethod;
	long unknown2;
};

#pragma pack(pop)

#endif // PAKINFO_H
