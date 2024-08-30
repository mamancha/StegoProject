#ifndef Tools_h
#define Tools_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

bool VerifyBMP(unsigned char *fileData);
FILE* FileRead(char *fileName, unsigned int *fileSize);
FILE* FileWrite(char *fileName);
bool PaddingCheck(unsigned int width);
unsigned char GetBitMask(int bits);

#endif