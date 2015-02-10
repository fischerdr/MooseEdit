#ifndef PAKCOMPRESSOR_H
#define PAKCOMPRESSOR_H

#include "lz4.h"

class PakCompressor
{
public:
	int compress(const char* source, char* dest, int sourceSize);
	int decompress(char *dest, char *src, int compressedSize, int decompressedSize);
	PakCompressor();
};

#endif // PAKCOMPRESSOR_H
