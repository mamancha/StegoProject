#ifndef Tools_h
#define Tools_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct {

    unsigned char R, G, B, luminosity, originalIndex;

} PaletteEntry;

bool VerifyBMP(unsigned char *fileData);
FILE* FileRead(char *fileName, unsigned int *fileSize);
FILE* FileWrite(char *fileName);
bool PaddingCheck(unsigned int width);
unsigned char GetBitMask(int bits);
unsigned char CalculateLuminosity(unsigned char R, unsigned char G, unsigned char B);
unsigned char GetLuminanceIndex(PaletteEntry *palette, unsigned char index);
void BubbleSortLuminance(PaletteEntry *originalPalette);
PaletteEntry* CopyPalette(short paletteSize, FILE *pCoverFile, FILE *pOutputFile);
PaletteEntry* CachePalette(short paletteSize, FILE *pCoverFile);

#endif