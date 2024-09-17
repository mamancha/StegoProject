#ifndef Hide_h
#define Hide_h

#include "Tools.h"

void HideProcedure(char *messageFileName, char *coverFileName, char *outputFileName, int bits);
void LoadPaletteHeader(int bits);
void CoverLimitCheck(int bits);
void EmbedLengthOfFile(int bitsToHide);
void FreeHide();

#endif
