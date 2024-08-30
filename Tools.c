#include "Tools.h"

bool VerifyBMP(unsigned char *fileData) {

    if(fileData[0] == 'B' && fileData[1] == 'M')
        return true;
    else
        return false;

}

FILE* FileRead(char *fileName, unsigned int *fileSize) {

    FILE *pFile = fopen(fileName, "rb");

    if(pFile == NULL) {
        printf("Error. Could not open file <%s>. Please enter a valid file.\n", fileName);
        exit(1);
    } else {
        //TODO remove
        printf("File  %s is valid and has been opened.\n", fileName);
    }

    fseek(pFile, 0, SEEK_END);
    *fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    return pFile;

}

FILE* FileWrite(char *fileName) {

    FILE *pFile = fopen(fileName, "wb");

    if(pFile == NULL) {
        printf("Error. Could not open file <%s>.\n", fileName);
        exit(1);
    } else {
        //TODO REMOVE
        printf("Successfully opened file %s for writing\n", fileName);
    }

    return pFile;

}

bool PaddingCheck(unsigned int width) {

    if(width % 4 == 0)
        return false;

    return true;


}

unsigned char GetBitMask(int bits) {

    unsigned char byte;
    switch (bits) {
        case 1:
            byte = 0b00000001;
            break;
        case 2:
            byte = 0b00000011;
            break;
        case 3:
            byte = 0b00000111;
            break;
        case 4:
            byte = 0b00001111;
            break;
        case 5:
            byte = 0b00011111;
            break;
        case 6:
            byte = 0b00111111;
            break;
        case 7:
            byte = 0b01111111;
            break;
    }

    return byte;

}