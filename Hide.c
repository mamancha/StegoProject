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

    pMessageFile = FileRead(messageFileName, &messageSize);
    pOutputFile = FileWrite(outputFileName);

    LoadPaletteHeader();

    printf("Cover Size = %u\n", coverSize);
    printf("Message Size = %u\n", messageSize);

    CoverLimitCheck(bitsToHide);

    EmbedLengthOfFile(bitsToHide);

    int messageBitCounter = 0,  remainderBits = 0;
    unsigned char coverByte, messageByte, tempByte, bitMask;

    bool padding = PaddingCheck(coverWidth);
    //TODO: remove
    if(padding) {
        printf("Padding Required!\n");
    } else {
        printf("No Padding!\n");
    }

    while(ftell(pCoverFile) < coverSize) {

        fread(&coverByte, 1, 1, pCoverFile);
        if(ftell(pMessageFile) < 9)
            printf("New CoverByte = %u\n", coverByte);

        if(padding && (ftell(pCoverFile) % (coverWidth + 1) == 0)) {

            fwrite(&coverByte, 1, 1, pOutputFile);

        }

        if(ftell(pMessageFile) < messageSize) {

            if((messageBitCounter + bitsToHide) > 8) {
                
                messageBitCounter += bitsToHide;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("MBC = %d\n", messageBitCounter);
                remainderBits = messageBitCounter % 8;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("remainderBits = %d\n", remainderBits);
                //bitmask for lsb wipe
                bitMask = ~GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Bitmask4C = %u\n", bitMask);
                //wipe lsbs of cover
                coverByte &= bitMask;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("CoverByte & bitMask = %u\n", coverByte);
                //shift to correct pos
                messageByte <<= remainderBits;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("MesssageByte << remainderBits = %u\n", messageByte);
                //set bits to msg bits
                coverByte |= messageByte;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("CoverByte | messageByte = %u\n", coverByte);
                //read new msg byte
                fread(&messageByte, 1, 1, pMessageFile);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("New MessageByte = %u\n", messageByte);
                //new bitmask to retrieve remaining bits for coverByte
                bitMask = GetBitMask(remainderBits);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Bitmask4M = %u\n", bitMask);
                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("tempByte = %u\n", tempByte);
                //set remaining bits in coverByte
                coverByte |= tempByte;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("CoverByte | tempByte = %u\n", coverByte);
                //write coverByte to outfile
                fwrite(&coverByte, 1, 1, pOutputFile);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Writing %u to outputFile\n", coverByte);
                //shift message byte to write pos
                messageByte >>= remainderBits;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("MesssageByte >> remainderBits = %u\n", messageByte);
                //set messageBitCounter to correct number
                messageBitCounter = remainderBits;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("MBC = %d\n", messageBitCounter);

            } else if((messageBitCounter + bitsToHide) <= 8) {
                
                if(messageBitCounter == 0) {
                    fread(&messageByte, 1, 1, pMessageFile);

                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("New MessageByte = %u\n", messageByte);

                }

                messageBitCounter += bitsToHide;
                    
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("MBC = %d\n", messageBitCounter);
                //bitMask for lsbwipe
                bitMask = ~GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Bitmask4C = %u\n", bitMask);
                //wipe lsbs of coverByte
                coverByte &= bitMask;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("CoverByte & bitMask = %u\n", coverByte);
                //bitMask for desired msgBytes
                bitMask = GetBitMask(bitsToHide);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Bitmask4M = %u\n", bitMask);
                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("tempByte = %u\n", tempByte);
                //set bits of coverByte
                coverByte |= tempByte;
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("CoverByte | tempByte = %u\n", coverByte);
                //write to outfile
                fwrite(&coverByte, 1, 1, pOutputFile);
                //TODO remove
                if(ftell(pMessageFile) < 9)
                    printf("Writing %u to outputFile\n", coverByte);
                messageByte >>= bitsToHide;
                if(ftell(pMessageFile) < 9)
                    printf("MessageByte >> bitsToHide = %u\n", messageByte);

                if(messageBitCounter == 8) {
                       
                    messageBitCounter = 0;
                        
                }

            }

        }  else {
            fwrite(&coverByte, 1, 1, pOutputFile);
        }
       
    }// END WHILE

}

void LoadPaletteHeader() {

    unsigned char header[54];
    unsigned char bitsPerPixel;

    fseek(pCoverFile, 0, SEEK_SET);
    fread(header, 1, sizeof(header), pCoverFile);
    fwrite(header, 1, sizeof(header), pOutputFile);

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
    unsigned char paletteData[4];

    //TODO remove
    printf("Palette Size = %d\n", paletteSize * 4);

    fseek(pCoverFile, 54, SEEK_SET);

    palette = (PaletteEntry*) malloc(sizeof(PaletteEntry) * paletteSize);

    int index = 0;

    //TODO: check for ALL -1 if needed or not in ALL fread/fwrite
    while(((ftell(pCoverFile)) < (54 + (paletteSize * 4))) && fread(paletteData, 1, sizeof(paletteData), pCoverFile)) {

        palette[index].B = paletteData[0];
        palette[index].G = paletteData[1];
        palette[index].R = paletteData[2];
        palette[index].luminosity = CalculateLuminosity(palette[index].R, palette[index].G, palette[index].B);
        //TODO: remove if palette order is not changed
        palette[index].originalIndex = index;

        //TODO: remove
        printf("Palette Index %d R = %u\n", index, palette[index].R);
        printf("Palette Index %d G = %u\n", index, palette[index].G);
        printf("Palette Index %d B = %u\n", index, palette[index].B);
        printf("Palette Index %d luminosity = %u\n", index, palette[index].luminosity);


        fwrite(paletteData, 1, sizeof(paletteData), pOutputFile);
        index++;
    }

}

void FreeAll() {

    free(pCoverFile);
    free(pMessageFile);
    free(pOutputFile);
    free(palette);

}

void EmbedLengthOfFile(int bitsToHide) {

    int bitCounter = 0;
    unsigned char coverByte, messageByte, tempByte, bitMask;

    printf("-----------STARTING LENGTH EMBED-----------");
    printf("Message Size = %u\n", messageSize);

    while(bitCounter <= (sizeof(messageSize) * 8)) {

        //read in new byte
        fread(&coverByte, 1, 1, pCoverFile);
        //TODO remove
        printf("New CoverByte = %u\n", coverByte);
        //bitmask
        bitMask = ~GetBitMask(bitsToHide);
        //TODO remove
        printf("Bitmask4C = %u\n", bitMask);
        coverByte &= bitMask;
        //TODO remove
        printf("CoverByte & bitMask = %u\n", coverByte);
        tempByte = messageSize & bitMask;
        //TODO remove
        printf("tempByte = %u\n", tempByte);
        coverByte |= tempByte;
        //TODO remove
        printf("CoverByte | tempByte = %u\n", coverByte);
        messageSize >>= bitsToHide;
        //TODO remove
        printf("Message Size >> bitsToHide = %u\n", messageSize);
        bitCounter += bitsToHide;
        //TODO remove
        printf("bitCounter = %d\n", bitCounter);

    }

    //if remainder exists shift over remainder and get new bitmask

    if(bitCounter % 32 > 0) {

        //TODO remove
        printf("REMAINDER LEFT IN LENGTH!\n");
        messageSize <<= (bitCounter % 32);
        //TODO remove
        printf("Message Size << remainder = %u\n", messageSize);
        coverByte |= messageSize;
        //TODO remove
        printf("CoverByte | messageSize = %u\n", coverByte);
        fwrite(&coverByte, 1, 1, pOutputFile);
        //TODO remove
        printf("Writing %u to outfile!\n", coverByte);

    }

}

unsigned char CalculateLuminosity(unsigned char R, unsigned char G, unsigned char B) {

      return (unsigned char) round((R * 0.299) + (G * 0.587) + (B * 0.114));

}

void CoverLimitCheck(int bits) {

  char userInput;

  unsigned int uBits = (unsigned int) bits;
  double minimum = ((messageSize*8) / uBits);

  //TODO remove
  printf("Minimum = %lf\n", round(minimum));
  printf("Storage Cap = %u\n", coverSize - 1078);

  if(round(minimum) > (coverSize - 1078)) {

    printf("Warning! The cover file is not big enough to contain the entire message file. If you want to proceed type any key. If not enter N.\n");
    scanf("%c", &userInput);

    if(userInput == 'N')
        exit(0);
  }

}