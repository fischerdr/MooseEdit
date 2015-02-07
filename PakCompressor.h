#ifndef PAKCOMPRESSOR_H
#define PAKCOMPRESSOR_H

class PakCompressor
{
public:
	char *decompress(char *dest, char *src, int compressedSize, int decompressedSize);
	PakCompressor();
};

#endif // PAKCOMPRESSOR_H
