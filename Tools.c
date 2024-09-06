#include "Tools.h"

//Parameters: Two by
//Verifies if the file is in BMP format. Returns a boolean.
bool VerifyBMP(unsigned char *fileData) {

    if(fileData[0] == 'B' && fileData[1] == 'M')
        return true;
    else
        return false;

}

//Parameters: String, unsigned int pointer
//Takes the given file name and opens it for binary reading. Also saves the size of the file to the unsigned int. Returns the file pointer.
FILE* FileRead(char *fileName, unsigned int *fileSize) {

    FILE *pFile = fopen(fileName, "rb");

    if(pFile == NULL) {
        printf("Error. Could not open file <%s>. Please enter a valid file.\n", fileName);
        exit(1);
    }

    fseek(pFile, 0, SEEK_END);
    *fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    return pFile;

}

//Parameters: String
//Takes the given file name and opens it for binary writing. Returns the file pointer.
FILE* FileWrite(char *fileName) {

    FILE *pFile = fopen(fileName, "wb");

    if(pFile == NULL) {
        printf("Error. Could not open file <%s>.\n", fileName);
        exit(1);
    }
    
    return pFile;

}

//Parameters: unsigned int
//Checks to see if a file requires padding. Returns a boolean.
bool PaddingCheck(unsigned int width) {

    if(width % 4 == 0)
        return false;

    return true;
}

//Parameters: int
//Given a number, returns a matching byte mask
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

//Paramets: 3 unsigned char
//Given 3 color values represented in an unsigned char, return the luminosity.
unsigned char CalculateLuminosity(unsigned char R, unsigned char G, unsigned char B) {

      return (unsigned char) round((R * 0.299) + (G * 0.587) + (B * 0.114));

}

//Parameters: PaletteEntry, unsigned char
//Searches through the given PaletteEntry array and returns the index that has the same originalIndex provided.
unsigned char GetLuminanceIndex(PaletteEntry *palette, unsigned char originalIndex) {

    unsigned char i;
    int length = pow(2, 8);

    for(i = 0; i < length; i++) {

        if(palette[i].originalIndex == originalIndex) 
            break;

    }

    return i;

}

//Parameter: PaletteEntry
//Given a PaletteEntry array, sort it based on increasing luminance
void BubbleSortLuminance(PaletteEntry *originalPalette) {
    
    int i, j;
    int n = pow(2, 8);
    PaletteEntry temp;

    for (i = 0; i < n-1; i++) {
        // Flag to check if any swapping happened
        int swapped = 0;

        // Last i elements are already in place
        for (j = 0; j < n-i-1; j++) {
            if (originalPalette[j].luminosity > originalPalette[j+1].luminosity) {
                // Swap if the element found is greater than the next element
                temp = originalPalette[j];
                originalPalette[j] = originalPalette[j+1];
                originalPalette[j+1] = temp;
                
                swapped = 1;
            }
        }

        // If no two elements were swapped in the inner loop, then break
        if (swapped == 0)
            break;
    }
}

//Parameters: short, 2 FILE*, 
//Locates the palette in the first file and copies it to the second file
PaletteEntry* CopyPalette(short paletteSize, FILE *pCoverFile, FILE *pOutputFile) {

    PaletteEntry *palette;
    unsigned char paletteData[4];
    int index = 0;

    palette = (PaletteEntry*) malloc(sizeof(PaletteEntry) * paletteSize);

    while(((ftell(pCoverFile)) < (54 + (paletteSize * 4))) && fread(paletteData, 1, sizeof(paletteData), pCoverFile)) {

        palette[index].B = paletteData[0];
        palette[index].G = paletteData[1];
        palette[index].R = paletteData[2];
        palette[index].luminosity = CalculateLuminosity(palette[index].R, palette[index].G, palette[index].B);
        palette[index].originalIndex = index;

        fwrite(paletteData, 1, sizeof(paletteData), pOutputFile);
        index++;
    }

    return palette;

}

//Parameters: short, FILE*
//Finds the palette in the file and saves it. Returns the FILE pointer.
PaletteEntry* CachePalette(short paletteSize, FILE *pCoverFile) {

    PaletteEntry *palette;
    unsigned char paletteData[4];
    int index = 0;

    palette = (PaletteEntry*) malloc(sizeof(PaletteEntry) * paletteSize);

    //TODO: check for ALL -1 if needed or not in ALL fread/fwrite
    while(((ftell(pCoverFile)) < (54 + (paletteSize * 4))) && fread(paletteData, 1, sizeof(paletteData), pCoverFile)) {

        palette[index].B = paletteData[0];
        palette[index].G = paletteData[1];
        palette[index].R = paletteData[2];
        palette[index].luminosity = CalculateLuminosity(palette[index].R, palette[index].G, palette[index].B);
        palette[index].originalIndex = index;

        //TODO: remove
        /* printf("Palette Index %d R = %u\n", index, palette[index].R);
        printf("Palette Index %d G = %u\n", index, palette[index].G);
        printf("Palette Index %d B = %u\n", index, palette[index].B);
        printf("Palette Index %d luminosity = %u\n", index, palette[index].luminosity); */

        index++;
    }

    return palette;

}
