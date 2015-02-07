#include "PakCompressor.h"
#include <stdint.h>
#include <cstring>

char *PakCompressor::decompress(char *dest, char *src, int compressedSize, int decompressedSize) {
	char *originalSrc = src;
	char *originalDest = dest;
	char *endOfCompressedData = src + compressedSize;
	char *endOfDestBuffer = dest + decompressedSize;
	long valueTable[8] = {4, 1, 2, 1, 4, 4, 4, 4};
	if (decompressedSize == 0) {
		if (compressedSize != 1 || *src == '\0')
			return (char *)-1;
		return 0;
	}
	uint32_t upperBits;
	char *ptr1;
	for (;;) {
		uint32_t currentByte = *((unsigned char *)src);
		upperBits = currentByte >> 4;
		++src;
		if (upperBits == 0x0F) {
			char *compressedDataPtr = endOfCompressedData - 0x0F;
			for (;;) {
				uint8_t current = *((unsigned char *)src);
				++src;
				upperBits += current;
				if ((unsigned)src >= (unsigned)compressedDataPtr)
					break;
				if (current != 0xFF)
					break;
			}
			char *end1 = dest + upperBits;
			if (end1 < dest) {
				return originalSrc;
			}
			char *end2 = src + upperBits;
			if (end2 < src) {
				return originalSrc;
			}
		}
		ptr1 = dest + upperBits;
		char *cmp1 = endOfDestBuffer - 0x0C;
		if (ptr1 > cmp1) {
			break;
		}
		char *ptr2 = src + upperBits;
		char *cmp2 = endOfCompressedData - 0x08;
		if (ptr2 > cmp2) {
			break;
		}
		do {
			long dword1 = *(long *)(src);
			long dword2 = *(long *)(src+4);
			*((long *)dest) = dword1;
			src += 4;
			dest += 4;
			*((long *)dest) = dword2;
			src += 4;
			dest += 4;
		} while (dest < ptr1);
		long temp = ptr1 - dest;
		uint16_t word1 = *(uint16_t*)((unsigned char *)(src + temp));
		src += temp;
		char *ptr = ptr1 - word1;
		src += 2;
		if (ptr < originalDest) {
			return originalSrc;
		}
		currentByte &= 0x0F;
		if (currentByte == 0x0F) {
			dest = endOfCompressedData - 0x05;
			uint8_t result;
			do {
				if (src > dest) {
					return originalSrc;
				}
				result = *((unsigned char *)src);
				++src;
				currentByte += result;
			} while (result == 0xFF);
			char *test = ptr1 + currentByte;
			if (test < ptr1) {
				return originalSrc;
			}
		}
		temp = ptr1 - ptr;
		if (temp < 4) {
			uint8_t current = *((unsigned char *)ptr);
			*ptr1 = current;
			current = *(((unsigned char *)ptr) + 1);
			*(ptr1 + 1) = current;
			current = *(((unsigned char *)ptr) + 2);
			*(ptr1 + 2) = current;
			current = *(((unsigned char *)ptr) + 3);
			ptr += valueTable[temp];
			*(ptr1 + 3) = current;
			long currentDword = *((long *)ptr);
			ptr1 += 4;
			*((long *)ptr1) = currentDword;
		} else {
			long dword = *((long *)ptr);
			*((long *)ptr1) = dword;
			ptr1 += 4;
			ptr += 4;
		}
		char *skipBuf = endOfDestBuffer - 0x08;
		dest = ptr1 + currentByte;
		if ((unsigned)dest <= (unsigned)skipBuf) {
			do {
				long dword = *((long *)ptr);
				*((long *)ptr1) = dword;
				dword = *((long *)(ptr+4));
				ptr += 4;
				ptr1 += 4;
				*((long *)ptr1) = dword;
				ptr1 += 4;
				ptr += 4;
			} while ((unsigned)ptr1 < (unsigned)dest);
			continue;
		}
		char *skipBuf2 = endOfDestBuffer - 0x05;
		if (dest > skipBuf2) {
			return originalSrc;
		}
		if ((unsigned)ptr1 < (unsigned)skipBuf) {
			do {
				long dword = *((long *)ptr);
				*((long *)ptr1) = dword;
				dword = *((long *)(ptr+4));
				ptr += 4;
				ptr1 += 4;
				*((long *)ptr1) = dword;
				ptr1 += 4;
				ptr += 4;
			} while (ptr1 < skipBuf);
		}
		if ((unsigned)ptr1 < (unsigned)dest) {
			do {
				char current = *ptr;
				*ptr1 = current;
				++ptr1;
				++ptr;
			} while((unsigned)ptr1 < (unsigned)dest);
		}
	}
	char *end1 = upperBits + src;
	if (end1 != endOfCompressedData)
		return (char *)-1;
	if (ptr1 > endOfDestBuffer)
		return (char *)-1;
	memcpy(dest, src, upperBits);
	return originalDest;
}

PakCompressor::PakCompressor()
{
}
