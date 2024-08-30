#ifndef Hide_h
#define Hide_h

#include <math.h>
#include "Tools.h"

typedef struct {

    unsigned char R, G, B, luminosity, originalIndex;

} PaletteEntry;

void HideProcedure(char *messageFileName, char *coverFileName, char *outputFileName, int bits);
void LoadPaletteHeader();
unsigned char CalculateLuminosity(unsigned char R, unsigned char G, unsigned char B);
void CoverLimitCheck(int bits);

#endif