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

unsigned char CalculateLuminosity(unsigned char R, unsigned char G, unsigned char B) {

      return (unsigned char) round((R * 0.299) + (G * 0.587) + (B * 0.114));

}

unsigned char GetLuminanceIndex(PaletteEntry *palette, unsigned char originalIndex) {

    unsigned char i;
    int length = pow(2, 8);

    for(i = 0; i < length; i++) {

        if(palette[i].originalIndex == originalIndex) 
            break;

    }

    return i;

}

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

    for(i = 0; i < n; i++) {
        //TODO: remove
        printf("-------------------------------------\n");
        printf("lumIndex %d\tOriginal Index %d\n", i, originalPalette[i].originalIndex);
        printf("R = %u\n", originalPalette[i].R);
        printf("G = %u\n", originalPalette[i].G);
        printf("B = %u\n", originalPalette[i].B);
        printf("luminosity = %u\n",originalPalette[i].luminosity);
        printf("-------------------------------------\n");
    }

}

PaletteEntry* CopyPalette(short paletteSize, FILE *pCoverFile, FILE *pOutputFile) {

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
        printf("Palette Index %d R = %u\n", index, palette[index].R);
        printf("Palette Index %d G = %u\n", index, palette[index].G);
        printf("Palette Index %d B = %u\n", index, palette[index].B);
        printf("Palette Index %d luminosity = %u\n", index, palette[index].luminosity);


        fwrite(paletteData, 1, sizeof(paletteData), pOutputFile);
        index++;
    }

    return palette;

}

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
        printf("Palette Index %d R = %u\n", index, palette[index].R);
        printf("Palette Index %d G = %u\n", index, palette[index].G);
        printf("Palette Index %d B = %u\n", index, palette[index].B);
        printf("Palette Index %d luminosity = %u\n", index, palette[index].luminosity);

        index++;
    }

    return palette;

}