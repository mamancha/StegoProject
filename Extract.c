#include "extract.h"

FILE *pStegoFile, *pMessageFile;
PaletteEntry *palette;
unsigned int stegoFileSize, messageSize, stegoWidth, stegoLength;
short paletteSize;

void ExtractProcedure(char *stegoFileName, int bitsToExtract, char *outputFileName) {

    pStegoFile = FileRead(stegoFileName, &stegoFileSize);

    unsigned char bitMapVerification[2];
    fread(bitMapVerification, 1, 2, pStegoFile);

    if(!VerifyBMP(bitMapVerification)) {
        printf("Error. The file you entered is not a valid cover file. Use a BMP image.\n");
        exit(1);
    }

    HeaderInfo();

    BubbleSortLuminance(palette);

    ExtractMessageLength(bitsToExtract);

    unsigned int capacity = stegoFileSize - 1078 - ceil((messageSize * 8)/ (unsigned int) bitsToExtract) - (unsigned int) (ceil((sizeof(messageSize) * 8) / (unsigned int) bitsToExtract));
    if(PaddingCheck(stegoWidth)) {
        int paddingNum =  4 - (stegoWidth % 4);
        capacity -= (paddingNum * stegoLength);
    }

    if(capacity < 0 || capacity > stegoFileSize) {
        printf("Error writing message file. You may have extracted with the incorrect bit number. Try a different one. Use the -help command if you need assistance using the program.\n");
        exit(1);
    }

    pMessageFile = FileWrite(outputFileName);

    int extractStart = ftell(pStegoFile);

    bool padding = PaddingCheck(stegoWidth);

    int bitCounter = 0, remainderBits = 0;
    unsigned char stegoByte = 0, messageByte = 0, tempByte = 0, lumIndex = 0, bitMask = 0;
    unsigned int bytes = 0;
    
    while(bytes < messageSize) {

        if(padding && (ftell(pStegoFile)-extractStart % (stegoWidth + 1) == 0)) {

            int paddingNum = 4 - (stegoWidth % 4);
            fread(&stegoByte, 1, paddingNum, pStegoFile);
            bytes += paddingNum;

        }
        
        //case if last byte and leftover
        if((bitCounter + bitsToExtract) > 8) {

            tempByte = 0;
            
            bitCounter += bitsToExtract;
            
            remainderBits = bitCounter % 8;

            fread(&stegoByte, 1, 1, pStegoFile);
            lumIndex = GetLuminanceIndex(palette, stegoByte);

            bitMask = GetBitMask(bitsToExtract);

            lumIndex &= bitMask;

            tempByte = lumIndex >> remainderBits;

            tempByte <<= (bitCounter - bitsToExtract);

            messageByte |= tempByte;
                    
            fwrite(&messageByte, 1, 1, pMessageFile);
            fflush(pMessageFile);
            bytes++;

            messageByte = 0;
            bitMask = GetBitMask(remainderBits);

            lumIndex &= bitMask;

            messageByte |= lumIndex;

            bitCounter = remainderBits;

        } else if((bitCounter + bitsToExtract) <= 8) {

            fread(&stegoByte, 1, 1, pStegoFile);
            lumIndex = GetLuminanceIndex(palette, stegoByte);

            bitCounter += bitsToExtract;

            tempByte = 0;

            bitMask = GetBitMask(bitsToExtract);

            lumIndex &= bitMask;

            tempByte |= lumIndex;

            tempByte <<= (bitCounter - bitsToExtract);

            messageByte |= tempByte;

            if(bitCounter == 8) {

                fwrite(&messageByte, 1, 1, pMessageFile);
                fflush(pMessageFile);
                bitCounter = 0;
                bytes++;
                messageByte = 0;

            }

        }

    }    


    FreeExtract();

}

void HeaderInfo() {

    unsigned char bitsPerPixel;

    fseek(pStegoFile, 18, SEEK_SET);
    fread(&stegoWidth, 1, sizeof(stegoWidth), pStegoFile);
    fread(&stegoLength, 1, sizeof(stegoLength), pStegoFile);

    fseek(pStegoFile, 28, SEEK_SET);
    fread(&bitsPerPixel, sizeof(bitsPerPixel), 1, pStegoFile);

    if(bitsPerPixel != 8) {
        printf("Error. The file you entered is not a valid cover file. Use an 8 bit BMP image.\n");
        exit(1);
    }
    
    paletteSize = pow(2, bitsPerPixel);
    
    fseek(pStegoFile, 54, SEEK_SET);

    palette = CachePalette(paletteSize, pStegoFile);

}

void ExtractMessageLength(int bitsToExtract) {

    int bitCounter = 0, remainderBits = 0;
    unsigned char stegoByte, messageByte, lumIndex, bitMask;
    unsigned int tempInt;
    messageSize = 0;
    
    while((bitCounter + bitsToExtract) < (sizeof(messageSize) * 8)) {

        tempInt = 0;
        bitCounter += bitsToExtract;

        //read in new byte
        fread(&stegoByte, 1, 1, pStegoFile);

        //get lumIndex of stegoByte
        lumIndex = GetLuminanceIndex(palette, stegoByte);

        bitMask = GetBitMask(bitsToExtract);

        lumIndex &= bitMask;

        tempInt |= lumIndex;

        tempInt <<= (bitCounter - bitsToExtract);

        messageSize |= tempInt;

    }

    bitCounter += bitsToExtract;
    remainderBits = bitCounter % (sizeof(messageSize) * 8);

    if(remainderBits > 0) {

        tempInt = 0;

        //read in new byte
        fread(&stegoByte, 1, 1, pStegoFile);

        //get lumIndex of stegoByte
        lumIndex = GetLuminanceIndex(palette, stegoByte);

        //bitMask
        bitMask = GetBitMask(bitsToExtract);

        lumIndex >>= remainderBits;

        tempInt |= lumIndex;

        tempInt <<= (bitCounter - bitsToExtract);

        messageSize |= tempInt;

    } 

}

void FreeExtract() {

    free(pStegoFile);
    free(pMessageFile);
    free(palette);

}
