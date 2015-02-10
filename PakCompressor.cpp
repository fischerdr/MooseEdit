#include "PakCompressor.h"
#include <stdint.h>
#include <cstring>

int PakCompressor::decompress(char *dest, char *src, int compressedSize, int decompressedSize) {
	return LZ4_decompress_safe(src, dest, compressedSize, decompressedSize);
}

PakCompressor::PakCompressor()
{
}
