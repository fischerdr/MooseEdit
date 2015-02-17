#include "PakCompressor.h"
#include <stdint.h>
#include <cstring>


bool PakCompressor::getShouldCompress() const
{
	return shouldCompress;
}

void PakCompressor::setShouldCompress(bool value)
{
	shouldCompress = value;
}

bool PakCompressor::getHighCompressionMode() const
{
	return highCompressionMode;
}

void PakCompressor::setHighCompressionMode(bool value)
{
	highCompressionMode = value;
}

int PakCompressor::compress(const char* source, char* dest, int sourceSize) {
	if (shouldCompress) {
		if (highCompressionMode) {
			return LZ4_compressHC(source, dest, sourceSize);
		} else {
			return LZ4_compress(source, dest, sourceSize);
		}
	} else {
		memcpy(dest, source, sourceSize);
	}
	return sourceSize;
}

int PakCompressor::decompress(char *dest, char *src, int compressedSize, int decompressedSize) {
	return LZ4_decompress_safe(src, dest, compressedSize, decompressedSize);
}

PakCompressor::PakCompressor()
{
}
