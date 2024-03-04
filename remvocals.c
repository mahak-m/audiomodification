/* #include <stdio.h>
#include <stdlib.h> // to fix error

    // move function declaration to the top
    void remVocals(const char *sourceFile, const char *destFile);

    int main(int argc, char *argv[]) {

    // check if 2 arguments are passed through standard input
    if (argc != 3) {
        fprintf(stderr, "error: wrong number of arguments!\n");
        exit(1);
    }

    // check to see if files don't open
    FILE *sourceFile = fopen(argv[1], "rb");
    FILE *outputFile = fopen(argv[2], "wb");

    if (sourceFile == NULL || outputFile == NULL) {
        fprintf(stderr, "error: file couldn't be openedddddd! \n");
        exit(1);
    }

    // call remVocals method 
    remVocals(sourceFile, outputFile); 

    // close the files
    fclose(sourceFile);
    fclose(outputFile);

    return 0;
   }

    void remVocals(const char *sourceFile, const char *destFile) {

        // define variables
        // FILE *source;
        // FILE *dest;
        FILE *source = fopen(sourceFile, "rb");
        FILE *dest = fopen(destFile, "wb");
    
        // do this again
        if (source == NULL || dest == NULL) {
            fprintf(stderr, "error: file couldn't be opened! \n");
            exit(1);
        }

        // define left and right
        short left;
        short right;
        #define HEADER_SIZE 44

        // copy the first 44 bytes from input -> output file
        unsigned char header[HEADER_SIZE];
        fread(header, sizeof(unsigned char), HEADER_SIZE, source);
        fwrite(header, sizeof(unsigned char), HEADER_SIZE, dest);

        while (fread(&left, sizeof(short), 1, source) == 1 && fread(&right, sizeof(short), 1, source) == 1) 
        {
            short combined = (left - right) / 2;
            // write two compies combined to the output file
            fwrite(combined, sizeof(short), 1, dest);
            fwrite(combined, sizeof(short), 1, dest);
        }

        // close the files
        fclose(source);
        fclose(dest);
   }
   */
#include <stdio.h>
#include <stdlib.h>

// Function declaration
void remVocals(const char *sourceFile, const char *destFile);

int main(int argc, char *argv[]) {
    // Check if 2 arguments are passed through standard input
    if (argc != 3) {
        fprintf(stderr, "Error: Wrong number of arguments!\n");
        exit(1);
    }

    // Call remVocals method 
    remVocals(argv[1], argv[2]); 

    return 0;
}

void remVocals(const char *sourceFile, const char *destFile) {
    // Open source and destination files
    FILE *source = fopen(sourceFile, "rb");
    FILE *dest = fopen(destFile, "wb");

    // Check if files couldn't be opened
    if (source == NULL || dest == NULL) {
        fprintf(stderr, "Error: Files couldn't be opened!\n");
        exit(1);
    }

    // Define left and right variables
    short *left;
    short *right;
    #define HEADER_SIZE 44

    // Copy the first 44 bytes from input to output file
    unsigned char header[HEADER_SIZE];
    fread(header, sizeof(unsigned char), HEADER_SIZE, source);
    fwrite(header, sizeof(unsigned char), HEADER_SIZE, dest);

    // Process audio data
    while (fread(&left, sizeof(short), 1, source) == 1 && fread(&right, sizeof(short), 1, source) == 1) {
        short combined = (left - right) / 2;
        // Write two copies of combined to the output file
        fwrite(&combined, sizeof(short), 1, dest);
        fwrite(&combined, sizeof(short), 1, dest);
    }

    // Close the files
    fclose(source);
    fclose(dest);
}