#ifndef PAKCOMPRESSOR_H
#define PAKCOMPRESSOR_H

#include "lz4.h"
#include "lz4hc.h"

class PakCompressor
{
	bool shouldCompress = true;
	bool highCompressionMode = true;
public:
	int compress(const char* source, char* dest, int sourceSize);
	int decompress(char *dest, char *src, int compressedSize, int decompressedSize);
	PakCompressor();
	bool getShouldCompress() const;
	void setShouldCompress(bool value);
	bool getHighCompressionMode() const;
	void setHighCompressionMode(bool value);
};

#endif // PAKCOMPRESSOR_H
