#include "Hide.h"

FILE *pCoverFile, *pMessageFile, *pOutputFile;
PaletteEntry *palette;
unsigned int coverSize, messageSize, coverWidth;
short paletteSize;

void HideProcedure(char *messageFileName, char *coverFileName, char *outputFileName, int bitsToHide) {

    pCoverFile = FileRead(coverFileName, &coverSize);

    unsigned char bitMapVerification[2];
    fread(bitMapVerification, 1, 2, pCoverFile);

    if(!VerifyBMP(bitMapVerification)) {
        printf("Error. The file you entered is not a valid cover file. Use a BMP image.\n");
        exit(1);
    } else {
        //TODO remove
        printf("The cover file is a valid bitmap image.\n");
    }

    //TODO implement random option which uses an unsigned int and contains random number
    //messageSize = sizeof(unsigned int)
    //message = random()
    pMessageFile = FileRead(messageFileName, &messageSize);

    //TODO remove
    printf("Cover Size = %u\n", coverSize);
    printf("Message Size = %u\n", messageSize);

    CoverLimitCheck(bitsToHide);

    pOutputFile = FileWrite(outputFileName);

    LoadPaletteHeader();

    BubbleSortLuminance(palette);

    EmbedLengthOfFile(bitsToHide);

    int bitCounter = 0,  remainderBits = 0;
    unsigned char coverByte, messageByte, lumIndex, tempByte, bitMask;
    int cursor = ftell(pCoverFile);
    int test = ceil((messageSize * 8)/(unsigned int)bitsToHide);
    printf("TEST = %d\n", test);

    bool padding = PaddingCheck(coverWidth);
    //TODO: remove
    if(padding) {
        printf("Padding Required!\n");
    } else {
        printf("No Padding!\n");
    }

    while(ftell(pCoverFile) < coverSize) {

        if(padding && (ftell(pCoverFile)-cursor % (coverWidth+1) == 0)) {
            //TODO FIX PADDING
            int paddingNum = 4 - (coverWidth % 4);
            fread(&coverByte, 1, 1, pCoverFile);
            fwrite(&coverByte, 1, paddingNum, pOutputFile);
            fflush(pOutputFile);
        }

        fread(&coverByte, 1, 1, pCoverFile);
        lumIndex = GetLuminanceIndex(palette, coverByte);
        if((ftell(pMessageFile) > (messageSize - 3)) && (ftell(pMessageFile) != (messageSize))) {
            printf("New CoverByte IN EMBED LOOP= %u\n", coverByte);
            printf("New lumIndex IN EMBED LOOP= %u\n", lumIndex);
        }

        if((ftell(pCoverFile) - cursor) <= test) {

            if((bitCounter + bitsToHide) > 8) {
                
                tempByte = 0;
                bitCounter += bitsToHide;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MBC = %d\n", bitCounter);
                remainderBits = bitCounter % 8;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("remainderBits = %d\n", remainderBits);
                //bitmask for lsb wipe
                bitMask = ~GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Bitmask4LI = %u\n", bitMask);
                //wipe lsbs of lumIndex
                lumIndex &= bitMask;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("lumIndex & bitMask = %u\n", lumIndex);
                //shift to correct pos
                messageByte <<= remainderBits;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MesssageByte << remainderBits = %u\n", messageByte);
                //set bits to msg bits
                lumIndex |= messageByte;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("lumIndex | messageByte = %u\n", lumIndex);
                //read new msg byte
                fread(&messageByte, 1, 1, pMessageFile);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("New MessageByte = %u\n", messageByte);
                //new bitmask to retrieve remaining bits for lumIndex
                bitMask = GetBitMask(remainderBits);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Bitmask4M = %u\n", bitMask);
                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("tempByte = %u\n", tempByte);
                //set remaining bits in lumIndex
                lumIndex |= tempByte;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("lumIndex | tempByte = %u\n", lumIndex);
                //write originalIndex of lumIndex to outfile
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Writing %u with original index of %u to outputFile\n", lumIndex, palette[lumIndex].originalIndex);
                //shift message byte to write pos
                messageByte >>= remainderBits;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MesssageByte >> remainderBits = %u\n", messageByte);
                //set bitCounter to correct number
                bitCounter = remainderBits;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MBC = %d\n", bitCounter);

            } else if((bitCounter + bitsToHide) <= 8) {
                
                if(bitCounter == 0) {
                    fread(&messageByte, 1, 1, pMessageFile);

                    //TODO remove
                    if(ftell(pMessageFile) > (messageSize - 3))
                        printf("New MessageByte = %u\n", messageByte);

                }

                bitCounter += bitsToHide;
                
                tempByte = 0;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MBC = %d\n", bitCounter);
                //bitMask for lsbwipe
                bitMask = ~GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Bitmask4LI = %u\n", bitMask);
                //wipe lsbs of lumIndex
                lumIndex &= bitMask;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("lumIndex & bitMask = %u\n", lumIndex);
                //bitMask for desired msgBytes
                bitMask = GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Bitmask4M = %u\n", bitMask);
                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("tempByte = %u\n", tempByte);
                //set bits of lumIndex
                lumIndex |= tempByte;
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("lumIndex | tempByte = %u\n", lumIndex);
                //write to outfile
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
                //TODO remove
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("Writing %u with original index of %u to outputFile\n", lumIndex, palette[lumIndex].originalIndex);
                messageByte >>= bitsToHide;
                if(ftell(pMessageFile) > (messageSize - 3))
                    printf("MessageByte >> bitsToHide = %u\n", messageByte);

                if(bitCounter == 8) {
                    
                    bitCounter = 0;
                        
                }

            }

        }  else {
            
            remainderBits = bitCounter % 8;
            
            if(remainderBits > 0) {
                
                printf("-------------------HERE!!-------------\n");
                fread(&coverByte, 1, 1, pCoverFile);
                lumIndex = GetLuminanceIndex(palette, coverByte);
        
                printf("New CoverByte IN EMBED LOOP= %u\n", coverByte);
                printf("New lumIndex IN EMBED LOOP= %u\n", lumIndex);
        
                bitCounter += bitsToHide;
                //TODO remove
                printf("MBC = %d\n", bitCounter);
                remainderBits = bitCounter % 8;
                //TODO remove
                printf("remainderBits = %d\n", remainderBits);
                bitMask = ~GetBitMask(bitsToHide);
                //TODO remove
                printf("Bitmask4LI = %u\n", bitMask);
                lumIndex &= bitMask;
                //TODO remove
                printf("lumIndex & bitMask = %u\n", lumIndex);
                bitMask = GetBitMask(bitsToHide);
                //TODO remove
                printf("Bitmask4M = %u\n", bitMask);
                messageByte <<= remainderBits;
                printf("MesssageByte << remainderBits = %u\n", messageByte);
                tempByte = messageByte & bitMask;
                printf("tempByte = %u\n", tempByte);
                lumIndex |= tempByte;
                printf("lumIndex | tempByte = %u\n", lumIndex);
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
                printf("Writing %u with original index of %u to outputFile\n", lumIndex, palette[lumIndex].originalIndex);
                
                bitCounter = 8;

            }

            if(ftell(pCoverFile) == messageSize)
            {
                printf("FTELL %ld\tMessageSize %d\tBitCounter %d\n", ftell(pMessageFile), messageSize, bitCounter);
                printf("MessageByte %u\tCoverByte %u\n", messageByte, coverByte);
            }

            fwrite(&coverByte, 1, 1, pOutputFile);
            fflush(pOutputFile);
        }
       
    }// END WHILE

    FreeHide();

}

void LoadPaletteHeader() {

    unsigned char header[54];
    unsigned char bitsPerPixel;

    fseek(pCoverFile, 0, SEEK_SET);
    fread(header, 1, sizeof(header), pCoverFile);
    fwrite(header, 1, sizeof(header), pOutputFile);
    fflush(pOutputFile);

    fseek(pCoverFile, 18, SEEK_SET);
    fread(&coverWidth, 1, sizeof(coverWidth), pCoverFile);

    //TODO remove
    printf("Width = %u\n", coverWidth);

    fseek(pCoverFile, 28, SEEK_SET);
    fread(&bitsPerPixel, sizeof(bitsPerPixel), 1, pCoverFile);

    //TODO remove
    printf("Bits per pixel = %u\n", bitsPerPixel);

    if(bitsPerPixel != 8) {
        printf("Error. The file you entered is not a valid cover file. Use an 8 bit BMP image.\n");
        exit(1);
    } else {
        //TODO: remove
        printf("The cover file is a %u bitmap image.\n", bitsPerPixel);
    }

    paletteSize = pow(2, bitsPerPixel);
    

    //TODO remove
    printf("Palette Size = %d\n", paletteSize * 4);

    fseek(pCoverFile, 54, SEEK_SET);

    palette = CopyPalette(paletteSize, pCoverFile, pOutputFile);

}

void FreeHide() {

    free(pCoverFile);
    free(pMessageFile);
    free(pOutputFile);
    free(palette);

}

void EmbedLengthOfFile(int bitsToHide) {

    int bitCounter = 0;
    unsigned char coverByte, messageByte, lumIndex, tempByte, bitMask;
    unsigned int messageSizeCopy = messageSize;

    printf("-----------STARTING LENGTH EMBED-----------\n");
    printf("Message Size = %u\n", messageSizeCopy);

    while((bitCounter + bitsToHide) < (sizeof(messageSizeCopy) * 8)) {

        bitCounter += bitsToHide;
        //TODO remove
        printf("bitCounter = %d\n", bitCounter);
        //read in new byte
        fread(&coverByte, 1, 1, pCoverFile);
        //TODO remove
        printf("New CoverByte IN ELOF = %u\n", coverByte);
        //get lumIndex of coverByte
        lumIndex = GetLuminanceIndex(palette, coverByte);
        //TODO remove
        printf("New lumIndex IN ELOF = %u\n", lumIndex);
        //bitmask
        bitMask = ~GetBitMask(bitsToHide);
        //TODO remove
        printf("Bitmask4LI = %u\n", bitMask);
        lumIndex &= bitMask;
        //TODO remove
        printf("lumIndex & bitMask = %u\n", lumIndex);
        bitMask = GetBitMask(bitsToHide);
        //TODO remove
        printf("Bitmask4M = %u\n", bitMask);
        tempByte = messageSizeCopy & bitMask;
        //TODO remove
        printf("tempByte = %u\n", tempByte);
        lumIndex |= tempByte;
        //TODO remove
        printf("lumIndex | tempByte = %u\n", lumIndex);
        fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
        fflush(pOutputFile);
        //TODO remove
        printf("Writing %u with original index of %u to outputFile\n", lumIndex, palette[lumIndex].originalIndex);
        messageSizeCopy >>= bitsToHide;
        //TODO remove
        printf("Message Size >> bitsToHide = %u\n", messageSizeCopy);
        

    }

    //if remainder exists shift over remainder and get new bitmask
    bitCounter += bitsToHide;

    if(bitCounter % (sizeof(messageSizeCopy) * 8) > 0) {

        //TODO remove
        printf("REMAINDER LEFT IN LENGTH!\n");
        //read in new byte
        fread(&coverByte, 1, 1, pCoverFile);
        //TODO remove
        printf("New CoverByte IN ELOFR = %u\n", coverByte);
        //get lumIndex of coverByte
        lumIndex = GetLuminanceIndex(palette, coverByte);
        //TODO remove
        printf("New lumIndex IN ELOFR = %u\n", lumIndex);
        //bitmask
        bitMask = ~GetBitMask(bitsToHide);
        //TODO remove
        printf("Bitmask4LI = %u\n", bitMask);
        //wipe lsbs of lumIndex
        lumIndex &= bitMask;
        //shift bits
        messageSizeCopy <<= (bitCounter % (sizeof(messageSizeCopy)* 8));
        //TODO remove
        printf("Message Size << remainder = %u\n", messageSizeCopy);
        lumIndex |= messageSizeCopy;
        //TODO remove
        printf("lumIndex | messageSizeCopy = %u\n", lumIndex);
        fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
        fflush(pOutputFile);
        //TODO remove
        printf("Writing %u with original index of %u to outputFile\n", lumIndex, palette[lumIndex].originalIndex);

    }

    printf("-----------ENDING LENGTH EMBED-----------\n");

}

void CoverLimitCheck(int bits) {

  char userInput;

  unsigned int uBits = (unsigned int) bits;
  double minimum = ((messageSize*8) / uBits);

  //TODO remove
  printf("Minimum = %lf\n", round(minimum));
  printf("Storage Cap = %u\n", (coverSize - 1078 - (unsigned int) ceil((sizeof(messageSize)*8)/bits)));

  if(round(minimum) > (coverSize - 1078 - (unsigned int) ceil((sizeof(messageSize)*8)/bits))) {

    printf("Warning! The cover file is not big enough to contain the entire message file. If you want to exit enter N.\n");
    scanf("%c", &userInput);

    if(userInput == 'n' || userInput == 'N')
        exit(0);
  }

}
