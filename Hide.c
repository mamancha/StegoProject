#include "Hide.h"

FILE *pCoverFile, *pMessageFile, *pOutputFile;
PaletteEntry *palette;
unsigned int coverSize = 0, messageSize = 0, coverWidth = 0, coverLength = 0;
short paletteSize = 0;

void HideProcedure(char *messageFileName, char *coverFileName, char *outputFileName, int bitsToHide) {

    //Opens the cover file for reading and returns the file pointer. Also saves the size of the file.
    pCoverFile = FileRead(coverFileName, &coverSize);

    //Verifies if the file is a valid cover file
    unsigned char bitMapVerification[2];
    fread(bitMapVerification, 1, 2, pCoverFile);

    if(!VerifyBMP(bitMapVerification)) {
        printf("Error. The file you entered is not a valid cover file. Use a BMP image.\n");
        exit(1);
    }

    //Opens the message file for reading and returns the file pointer. Also saves the size of the file.
    pMessageFile = FileRead(messageFileName, &messageSize);

    //Opens the output file for writing
    pOutputFile = FileWrite(outputFileName);

    //Loads the palette and header into the output file. Also saves the palette for reference.
    LoadPaletteHeader();

    //Sorts the palette by luminance
    BubbleSortLuminance(palette);

    //Embeds the length of the message file into the cover
    EmbedLengthOfFile(bitsToHide);

    int bitCounter = 0,  remainderBits = 0;
    unsigned char coverByte, messageByte, lumIndex, tempByte, bitMask;
    
    int cursor = ftell(pCoverFile);
    int maxB = ceil( (messageSize * 8) / (unsigned int) bitsToHide);

    bool padding = PaddingCheck(coverWidth);

    //Loops through the entire cover file
    while(ftell(pCoverFile) < coverSize) {

        //If padding is required it does not embed and writes straight to file
        if(padding && (ftell(pCoverFile) - cursor % (coverWidth + 1) == 0)) {

            int paddingNum = 4 - (coverWidth % 4);
            fread(&coverByte, 1, 1, pCoverFile);
            fwrite(&coverByte, 1, paddingNum, pOutputFile);
            fflush(pOutputFile);
            
        }

        //read in one cover byte and find the lumIndex
        fread(&coverByte, 1, 1, pCoverFile);
        lumIndex = GetLuminanceIndex(palette, coverByte);

        if((ftell(pCoverFile) - cursor) <= maxB) {

            if((bitCounter + bitsToHide) > 8) {
                
                tempByte = 0;
                
                bitCounter += bitsToHide;

                remainderBits = bitCounter % 8;
                
                //bitmask for lsb wipe
                bitMask = ~GetBitMask(bitsToHide);
                
                //wipe lsbs of lumIndex
                lumIndex &= bitMask;
                
                //shift to correct pos
                messageByte <<= remainderBits;
                
                //set bits to msg bits
                lumIndex |= messageByte;
                
                //read new msg byte
                fread(&messageByte, 1, 1, pMessageFile);
                
                //new bitmask to retrieve remaining bits for lumIndex
                bitMask = GetBitMask(remainderBits);
                
                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;

                //set remaining bits in lumIndex
                lumIndex |= tempByte;
                
                //write originalIndex of lumIndex to outfile
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
      
                //shift message byte to write pos
                messageByte >>= remainderBits;
          
                //set bitCounter to correct number
                bitCounter = remainderBits;
                
            } else if((bitCounter + bitsToHide) <= 8) {
                
                if(bitCounter == 0)
                    fread(&messageByte, 1, 1, pMessageFile);
                
                bitCounter += bitsToHide;
                
                tempByte = 0;

                //bitMask for lsbwipe
                bitMask = ~GetBitMask(bitsToHide);

                //wipe lsbs of lumIndex
                lumIndex &= bitMask;


                //bitMask for desired msgBytes
                bitMask = GetBitMask(bitsToHide);

                //tempByte will contain bitmask we need
                tempByte = messageByte & bitMask;

                //set bits of lumIndex
                lumIndex |= tempByte;


                //write to outfile
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
                messageByte >>= bitsToHide;

                if(bitCounter == 8) 
                    bitCounter = 0;

            }

        }  else {
            
            remainderBits = bitCounter % 8;
            
            if(remainderBits > 0) {
                
                fread(&coverByte, 1, 1, pCoverFile);
                lumIndex = GetLuminanceIndex(palette, coverByte);
        
                bitCounter += bitsToHide;

                remainderBits = bitCounter % 8;
                
                bitMask = ~GetBitMask(bitsToHide);
                
                lumIndex &= bitMask;
                
                bitMask = GetBitMask(bitsToHide);
                
                messageByte <<= remainderBits;

                tempByte = messageByte & bitMask;

                lumIndex |= tempByte;
                
                fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
                fflush(pOutputFile);
                
                bitCounter = 8;

            }

            fwrite(&coverByte, 1, 1, pOutputFile);
            fflush(pOutputFile);

        }
       
    }// END WHILE

    FreeHide();

}

void LoadPaletteHeader(int bits) {

    unsigned char header[54];
    unsigned char bitsPerPixel;

    fseek(pCoverFile, 0, SEEK_SET);
    fread(header, 1, sizeof(header), pCoverFile);
    fwrite(header, 1, sizeof(header), pOutputFile);
    fflush(pOutputFile);

    fseek(pCoverFile, 18, SEEK_SET);
    fread(&coverWidth, 1, sizeof(coverWidth), pCoverFile);
    fread(&coverLength, 1, sizeof(coverLength), pCoverFile);

    fseek(pCoverFile, 28, SEEK_SET);
    fread(&bitsPerPixel, sizeof(bitsPerPixel), 1, pCoverFile);

    CoverLimitCheck(bits);

    if(bitsPerPixel != 8) {
        printf("Error. The file you entered is not a valid cover file. Use an 8 bit BMP image.\n");
        exit(1);
    }

    paletteSize = pow(2, bitsPerPixel);

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

    while((bitCounter + bitsToHide) < (sizeof(messageSizeCopy) * 8)) {

        bitCounter += bitsToHide;

        //read in new byte
        fread(&coverByte, 1, 1, pCoverFile);

        //get lumIndex of coverByte
        lumIndex = GetLuminanceIndex(palette, coverByte);

        //bitmask
        bitMask = ~GetBitMask(bitsToHide);

        lumIndex &= bitMask;

        bitMask = GetBitMask(bitsToHide);

        tempByte = messageSizeCopy & bitMask;

        lumIndex |= tempByte;

        fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
        fflush(pOutputFile);
        
        messageSizeCopy >>= bitsToHide;
        
    }

    //if remainder exists shift over remainder and get new bitmask
    bitCounter += bitsToHide;

    if(bitCounter % (sizeof(messageSizeCopy) * 8) > 0) {

        //read in new byte
        fread(&coverByte, 1, 1, pCoverFile);

        //get lumIndex of coverByte
        lumIndex = GetLuminanceIndex(palette, coverByte);

        //bitmask
        bitMask = ~GetBitMask(bitsToHide);

        //wipe lsbs of lumIndex
        lumIndex &= bitMask;

        //shift bits
        messageSizeCopy <<= (bitCounter % (sizeof(messageSizeCopy)* 8));

        lumIndex |= messageSizeCopy;

        fwrite(&palette[lumIndex].originalIndex, 1, 1, pOutputFile);
        fflush(pOutputFile);

    }

}

void CoverLimitCheck(int bits) {

    unsigned int uBits = (unsigned int) bits;
    double minimum = ((messageSize*8) / uBits);
    unsigned int bytesUsed = ceil((messageSize * 8)/ uBits);
    unsigned int paddingNum, hidingCapacity;
  
    if(PaddingCheck(coverWidth)) {

        paddingNum = 4 - (coverWidth % 4);
        bytesUsed += (paddingNum * coverLength);
        hidingCapacity = coverSize - 1078 - (paddingNum * coverLength) - (unsigned int) (ceil((sizeof(messageSize) * 8) / uBits));
  
    }
    
    hidingCapacity = coverSize - 1078 - (unsigned int) (ceil((sizeof(messageSize) * 8) / uBits));

    if(bytesUsed >= hidingCapacity) {

        printf("Warning! The cover file is not big enough to contain the entire message file. Increase the number of bits to hide. If you have reached the maximum limit try either a smaller message file or a larger cover file. Use the -help command if you need assistance using the program.\n");
        exit(0);
        
    } else {

        printf("Percentage of cover file used for hiding: %%%.2f\n", (double)(bytesUsed)/(double)(coverSize - 1078));

    }

}
