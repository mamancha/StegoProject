#ifndef Extract_h
#define Extract_h

#include "Tools.h"

void ExtractProcedure(char *stegoFileName, int bitsToExtract, char *outputFileName);
void FreeExtract();
void HeaderInfo();
void ExtractMessageLength(int bitsToExtract);

#endif