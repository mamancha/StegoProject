/*

Author: Mark Mancha
Description: This program uses the steganographic technique Nearest Luminance to embed a secret
message file into a cover file. It does this by sorting the palette by increasing luminance.
Then the message file is broken down into smaller parts so it can be embedded into the 
Least Significant Bits of the index into the sorted palette.
Data: 09/06/24

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Hide.h"
#include "Extract.h"

char stegoFileName[260], coverFileName[260], messageFileName[260], outputFileName[260], bits;

void main(int argc, char *argv[]) {


    /* for(int i = 0; i < argc; i++) {
        printf("%s\t%d\n", argv[i], i);
    } */

    //printf("argc = %d\n", argc);

    //StegoProgram.exe -hide -m (img, txt, random, etc.)"filepath" -c "filepath" -b number (optional) -o "filename" 
    if(strcmp(argv[1], "-hide") == 0) {

        if(argc < 8) {
            printf("Requires more arguments. Use the -help command if you need assistance using the program.\n");
            exit(1);
        } else if(argc > 10) {
            printf("Requires less arguments. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        if(strcmp(argv[2], "-m") != 0) {
            printf("Use -m and type the Path of the message file. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        strcpy(messageFileName, argv[3]);

        //TODO remove
        //printf("MessageFile = %s\n", messageFileName);

        if(strcmp(argv[4], "-c") != 0) {
            printf("Use -c and type the name of a valid cover file. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        strcpy(coverFileName, argv[5]);
        
        //TODO remove
        //printf("CoverFile = %s\n", coverFileName);

        if(strcmp(argv[6], "-b") != 0) {
            printf("Use -b and type the desired number of bits to embed into the Least Significant Bits of the cover file. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        bits = argv[7][0];

        if(atoi(&bits) < 1 || atoi(&bits) > 7) {
            printf("Invalid number! Please use -b <number> using a number greater than 0 and less than 8. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        //TODO remove
        //printf("Bits to hide = %c\n", bits);

        if(argc == 9 || argc == 10) {

            if(strcmp(argv[8], "-o") != 0 || argc == 9) {
                printf("Use -o and type the desired name of the output file. Optional. Use the -help command if you need assistance using the program.\n");
                exit(1);
            }

            strcpy(outputFileName, argv[9]);

        } else
            strcpy(outputFileName, "StegoImg.bmp");

        //TODO remove
        //printf("OutputFile = %s\n", outputFileName);

        //EMBED / OUTPUT PROCEDURE
        HideProcedure(messageFileName, coverFileName, outputFileName, atoi(&bits));

        exit(0);

    } //END if 
    
    //StegoProgram.exe -extract -s "filepath" (optional) -b number -o "filename"
    else if(strcmp(argv[1], "-extract") == 0) {

        if(argc < 6) {
            printf("Requires more arguments. Use the -help command if you need assistance using the program.\n");
            exit(1);
        } else if(argc > 8) {
            printf("Requires less arguments. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        if(strcmp(argv[2], "-s") != 0) {
            printf("Use -s and type the name of a valid stego file. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        strcpy(stegoFileName, argv[3]);

        //TODO remove
        //printf("stegoFile = %s\n", stegoFileName);

        if(strcmp(argv[4], "-b") != 0) {
            printf("Use -b and type the desired number of bits to embed into the Least Significant Bits of the cover file. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }

        bits = argv[5][0];
        if(atoi(&bits) < 1 || atoi(&bits) > 7) {
            printf("Invalid number! Please use -b <number> using a number greater than 0 and less than 8. Use the -help command if you need assistance using the program.\n");
            exit(1);
        }
        //TODO remove
        //printf("Bits to hide = %c\n", bits);
        
        //edit all number values
        if(argc == 7 || argc == 8) {

            if(strcmp(argv[6], "-o") != 0 || argc == 7) {
                printf("Use -o and type the name of the output file with the desired file extension. Optional. Use the -help command if you need assistance using the program.\n");
                exit(1);
            }

            strcpy(outputFileName, argv[7]);

        } else {
            strcpy(outputFileName, "MessageFile.out");
        }

        //TODO remove
        //printf("outputFile = %s\n", outputFileName);

        //EXTRACT PROCEDURE
        ExtractProcedure(stegoFileName, atoi(&bits), outputFileName);

        exit(0);

    } //END else if

    //StegoProgram.exe -help 
    else if(strcmp(argv[1], "-help") == 0) {

        printf("Directions for hide: StegoProgram.exe -hide -m \"filename\" -c \"filename\" -b bitsToHide (optional) -o \"desired output filename\"\n");
        printf("Directions for extract: StegoProgram.exe -extract -s \"stegofile\" -b bitsToExtract\n");
        printf("If you are extracting and do not provide an output file name, the file will be saved as \"MessageFile.out\". Change the extension to the desired format to open the file.");
        exit(1);

    } //END else if

    else {

        printf("Invalid input. Type -help if you need assistance using the program.\n");
        exit(1);

    } //END else

}