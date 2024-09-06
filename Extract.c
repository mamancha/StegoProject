#include "extract.h"

FILE *pStegoFile, *pMessageFile;
PaletteEntry *palette;
unsigned int stegoFileSize, messageSize, stegoWidth;
short paletteSize;

void ExtractProcedure(char *stegoFileName, int bitsToExtract, char *outputFileName) {

    pStegoFile = FileRead(stegoFileName, &stegoFileSize);

    unsigned char bitMapVerification[2];
    fread(bitMapVerification, 1, 2, pStegoFile);

    if(!VerifyBMP(bitMapVerification)) {
        printf("Error. The file you entered is not a valid cover file. Use a BMP image.\n");
        exit(1);
    } else {
        //TODO remove
        //printf("The cover file is a valid bitmap image.\n");
    }

    pMessageFile = FileWrite(outputFileName);

    HeaderInfo();

    BubbleSortLuminance(palette);

    ExtractMessageLength(bitsToExtract);

    int cursor = ftell(pStegoFile);

    bool padding = PaddingCheck(stegoWidth);

    //TODO: remove
    /* if(padding) {
        printf("Padding Required!\n");
    } else {
        printf("No Padding!\n");
    } */

    int bitCounter = 0, remainderBits = 0;
    unsigned char stegoByte = 0, messageByte = 0, tempByte = 0, lumIndex = 0, bitMask = 0;
    unsigned int bytes = 0;

    while(bytes < (messageSize)) {

        //printf("ftell mpessagefile = %d\n", ftell(pMessageFile));

        if(padding && (ftell(pStegoFile)-cursor % (stegoWidth + 1) == 0)) {
            int paddingNum = 4 - (stegoWidth % 4);
            fread(&stegoByte, 1, paddingNum, pStegoFile);
            bytes += paddingNum;
        }
        //case if last byte and leftover
        if((bitCounter + bitsToExtract) > 8) {

            tempByte = 0;
            bitCounter += bitsToExtract;
            
            //TODO remove
            /* if(bytes < messageSize) {
                printf("bytes = %d\n", bytes);
                printf("bitCounter = %d\n", bitCounter);
            } */ 

            remainderBits = bitCounter % 8;

            //TODO remove
            /* if(bytes < messageSize)
                printf("remainderBits = %d\n", remainderBits); */

            fread(&stegoByte, 1, 1, pStegoFile);
            lumIndex = GetLuminanceIndex(palette, stegoByte);

            //TODO remove
            /* if(bytes < messageSize) {
                printf("New stegoByte = %u\n", stegoByte);
                printf("New lumIndex = %u\n", lumIndex);
            } */

            bitMask = GetBitMask(bitsToExtract);

            //TODO remove
            /* if(bytes < messageSize)
                printf("Bitmask4LI = %u\n", bitMask); */

            lumIndex &= bitMask;
            
            //TODO remove
            /* if(bytes < messageSize)
                printf("lumIndex >> remainderBits = %u\n", lumIndex); */

            tempByte = lumIndex >> remainderBits;

            //TODO remove
            /* if(bytes < messageSize)
                printf("tempByte = %u\n", tempByte); */

            tempByte <<= (bitCounter - bitsToExtract);

            //TODO remove
            /* if(bytes < messageSize)
                printf("tempByte << %d = %u\n", (bitCounter - bitsToExtract), tempByte); */

            messageByte |= tempByte;

            //TODO remove
            /* if(bytes < messageSize)
                printf("messageByte | tempByte = %u\n", messageByte); */
                
            fwrite(&messageByte, 1, 1, pMessageFile);
            fflush(pMessageFile);
            bytes++;

            /* if(bytes < messageSize)
                printf("Writing %u to outputFile\n", messageByte); */

            messageByte = 0;
            bitMask = GetBitMask(remainderBits);

            //TODO remove
            /* if(bytes < messageSize)
                printf("Bitmask4LI = %u\n", bitMask); */

            lumIndex &= bitMask;

             //TODO remove
            /* if(bytes < messageSize)
                printf("Bitmask4LI = %u\n", bitMask); */

            messageByte |= lumIndex;

            //TODO remove
            /* if(bytes < messageSize)
                printf("messageByte | lumIndex = %u\n", messageByte); */

            bitCounter = remainderBits;

            //TODO remove
            /* if(bytes < messageSize)
                printf("bitCounter = %d\n", bitCounter); */

        } else if((bitCounter + bitsToExtract) <= 8) {

            fread(&stegoByte, 1, 1, pStegoFile);
            lumIndex = GetLuminanceIndex(palette, stegoByte);

            //TODO remove
            /* if(bytes < messageSize) {
                printf("New stegoByte = %u\n", stegoByte);
                printf("New lumIndex = %u\n", lumIndex);
            } */

            bitCounter += bitsToExtract;

            tempByte = 0;
            
            //TODO remove
            /* if(bytes < messageSize)
                printf("bitCounter = %d\n", bitCounter); */

            bitMask = GetBitMask(bitsToExtract);

            //TODO remove
            /* if(bytes < messageSize)
                printf("Bitmask4LI = %u\n", bitMask); */

            lumIndex &= bitMask;

            //TODO remove
            /* if(bytes < messageSize)
                printf("lumIndex & bitMask = %u\n", lumIndex); */

            tempByte |= lumIndex;

            //TODO remove
            /* if(bytes < messageSize)
                printf("tempByte = %u\n", tempByte); */

            tempByte <<= (bitCounter - bitsToExtract);

            //TODO remove
            /* if(bytes < messageSize)
                printf("tempByte << %d = %u\n", (bitCounter - bitsToExtract), tempByte); */

            messageByte |= tempByte;

            //TODO remove
            /* if(bytes < messageSize)
                printf("messageByte | tempByte = %u\n", messageByte); */

            if(bitCounter == 8) {

                fwrite(&messageByte, 1, 1, pMessageFile);
                fflush(pMessageFile);
                bitCounter = 0;
                bytes++;

                /* if(bytes < messageSize)
                    printf("Writing %u to outputFile\n", messageByte); */

                //TODO remove
                /* if(bytes < messageSize)
                    printf("bitCounter = %d\n", bitCounter); */

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

    //TODO remove
    //printf("Width = %u\n", stegoWidth);

    fseek(pStegoFile, 28, SEEK_SET);
    fread(&bitsPerPixel, sizeof(bitsPerPixel), 1, pStegoFile);

    //TODO remove
    //printf("Bits per pixel = %u\n", bitsPerPixel);

    if(bitsPerPixel != 8) {
        printf("Error. The file you entered is not a valid cover file. Use an 8 bit BMP image.\n");
        exit(1);
    } else {
        //TODO: remove
        //printf("The cover file is a %u bitmap image.\n", bitsPerPixel);
    }

    paletteSize = pow(2, bitsPerPixel);
    
    //TODO remove
    //printf("Palette Size = %d\n", paletteSize * 4);

    fseek(pStegoFile, 54, SEEK_SET);

    palette = CachePalette(paletteSize, pStegoFile);

}

void ExtractMessageLength(int bitsToExtract) {

    int bitCounter = 0, remainderBits = 0;
    unsigned char stegoByte, messageByte, lumIndex, bitMask;
    unsigned int tempInt;
    messageSize = 0;
    
    //printf("-----------STARTING LENGTH EXTRACT-----------\n");
    
    while((bitCounter + bitsToExtract) < (sizeof(messageSize) * 8)) {

        tempInt = 0;
        bitCounter += bitsToExtract;

        //TODO remove
        //printf("bitCounter = %d\n", bitCounter);

        //read in new byte
        fread(&stegoByte, 1, 1, pStegoFile);

        //TODO remove
        //printf("New stegoByte IN EML = %u\n", stegoByte);

        //get lumIndex of stegoByte
        lumIndex = GetLuminanceIndex(palette, stegoByte);

        //TODO remove
        //printf("New lumIndex IN EML = %u\n", lumIndex);

        bitMask = GetBitMask(bitsToExtract);

        //TODO remove
        //printf("Bitmask4LI = %u\n", bitMask);

        lumIndex &= bitMask;

        //TODO remove
        //printf("lumIndex & bitMask = %u\n", lumIndex);

        tempInt |= lumIndex;

        //TODO remove
        //printf("tempInt | lumIndex = %u\n", tempInt);
        tempInt <<= (bitCounter - bitsToExtract);

        //TODO remove
        //printf("tempInt << %d = %u\n", (bitCounter - bitsToExtract), tempInt);

        messageSize |= tempInt;

        //TODO remove
        //printf("messageSize | tempInt = %u\n", messageSize);
        
    }

    bitCounter += bitsToExtract;
    remainderBits = bitCounter % (sizeof(messageSize) * 8);

    if(remainderBits > 0) {

        tempInt = 0;

        //TODO remove
        //printf("REMAINDER LEFT IN LENGTH!\n"); 

        //read in new byte
        fread(&stegoByte, 1, 1, pStegoFile);

        //TODO remove
        //printf("New stegoByte IN EML = %u\n", stegoByte);

        //get lumIndex of stegoByte
        lumIndex = GetLuminanceIndex(palette, stegoByte);

        //TODO remove
        //printf("New lumIndex IN EML = %u\n", lumIndex);

        //bitMask
        bitMask = GetBitMask(bitsToExtract);

        //TODO remove
        //printf("Bitmask4LI = %u\n", bitMask);

        lumIndex >>= remainderBits;

        //TODO remove
        //printf("lumIndex >> remainderBits = %u\n", lumIndex);

        tempInt |= lumIndex;

        //TODO remove
        //printf("tempInt | lumIndex = %u\n", tempInt);
        //tempInt << bitCounter - bitsToExtract

        tempInt <<= (bitCounter - bitsToExtract);

        //TODO remove
        //printf("tempInt << %d = %u\n", (bitCounter - bitsToExtract), tempInt);

        messageSize |= tempInt;
        //TODO remove
        //printf("messageSize | tempInt = %u\n", messageSize);

    } 

    //TODO remove
    //printf("Message Size = %u\n", messageSize);

    //TODO remove
    //printf("-----------ENDING LENGTH EXTRACT-----------\n");

}

void FreeExtract() {

    free(pStegoFile);
    free(pMessageFile);
    free(palette);

}