/* 
The program has three command line arguments: an input file, search string, and 
the name of an output file.

The program prints to stdout the size of the file and the number of occurrences
of the search string in the input file. The program can handle searching for
character strings or strings of bytes. 

This information is also printed in the output file.

Author: Sydney Clough

*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ALLOC 100
#define MAX_SEARCH 20

/* Prints the size of the file (in bytes) to std out */
void printSizeOfFile(FILE *inputFile, FILE *outputFile) {
    /* Move to end of file. */
    fseek(inputFile, 0L, SEEK_END);
    /* Get file pointer position. */
    long size = ftell(inputFile);
    /* Move pointer back to beginning of file. */
    fseek(inputFile, 0, SEEK_SET);

    printf("Size of file is %ld \n", size);
    fprintf(outputFile, "Size of file is %ld \n", size);
}

/* Returns the number of bytes in the searchString. */
int searchStringLength(char *searchString) {
    int length = 0;
    while (searchString[length] != '\0') {
        length++;
    }
    return length;
    
}

/* 
Counts the number of times the specified search string occurs in 
the input file.
*/
    
int countStringOccurrence(char *searchString, FILE *inputFile) {
    /* Buffer for reading in the file. */
    char *buffer = malloc(MAX_ALLOC * sizeof(char));
    int searchLength = searchStringLength(searchString);

    /* 
    possiblePosition keeps track of the current characters that have been matched 
    with the search string. For instance a value of 3 would indicate that 
    the first 3 characters of the the search string have been found in the buffer.
    This value is reset to 0 if the next character is not matched. All elements in
    the array will be used only if each byte of the search string is the same.

    Array is used for the case when matched strings overlap.
    */
    int *possiblePosition = calloc(searchLength, sizeof(int));


    int count = 0;

    /* Exits with error message if machine is out of memory. */
    if (!buffer || !possiblePosition) {
        printf("Error allocating memory. \n");
        exit(1);
    }

    int bytesRead = fread(buffer, sizeof(char), MAX_ALLOC, inputFile);

    /* Continues execution until no new bytes are read in from the input file. */
    while(bytesRead > 0) {
        for (int i = 0; i < bytesRead; i++) {

            /*
            Boolean
            Keeps track if a 0 was already incrememted to a 1. This is so when the first character is matched,
            the array is not made to be all 1s. 
            */
            char firstFound = 0;
            for (int j = 0; j < searchLength; j++) {
                /* Adds char with 0xff to work with bytes. */
                if (buffer[i] == (searchString[possiblePosition[j]] & 0xff) && (possiblePosition[j] != 0 || !firstFound)) {
                    /* If the first character is matched, set firstFound to true so whole array is not incremented. */
                    if (possiblePosition[j] == 0) {
                        firstFound = 1;
                    }
                    /* Increment the number of characters that have been matched with the search string. */
                    possiblePosition[j] = possiblePosition[j] + 1;

                    /* Increment count when the whole string has been matched. Reset the matched characters to 0. */
                    if (searchLength == possiblePosition[j]) {
                        possiblePosition[j] = 0;
                        count++;
                    }
                }

                /* If the next character is not matched, reset the matched characters to 0. */ 
                else {
                    possiblePosition[j] = 0;
                }
            }
        }

        /* All bytes have been read. Read in next chunk of data. */
        bytesRead = fread(buffer, sizeof(char), MAX_ALLOC, inputFile);

    }

    /* Frees allocated memory. */
    free(buffer);
    free(possiblePosition);   
    
    return count;

}

/* Prints the number of matches found within the input file. */
void printStringCount(char *searchString, FILE *inputFile, FILE *outputFile) {
    int count = countStringOccurrence(searchString, inputFile);
    printf("Number of matches = %d \n", count);
    fprintf(outputFile, "Number of matches = %d \n", count);
}


int main (int argc, char **argv) {
    /* Checks that the correct command was entered. */
    if (argc != 4) {
        printf("Incorrect number of arguments. The correct command is: \n");
        printf("%s <input-filename> <search-string> <output-filename> \n", argv[0]);
        exit(1);
    }

    /* Checks if the file to read was opened properly. */
    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        printf("Error opening input file %s. The correct command is: \n", argv[1]);
        printf("%s <input-filename> <search-string> <output-filename> \n", argv[0]);
        exit(1);
    }

    /* 
        Only attempts to open output file if it is different from the input file.
    */
    FILE *outputFile;
    if (strcmp(argv[1], argv[3]) == 0) {
        printf("Error. Output file is same as input file. The correct command is: \n");
        printf("%s <input-filename> <search-string> <output-filename> \n", argv[0]);
        exit(1);
    }
    else {
        outputFile = fopen(argv[3], "wb");
        /* Checks that the output file opened correctly. */
        if (outputFile == NULL) {
            printf("Error opening output file %s. The correct command is: \n", argv[3]);
            printf("%s <input-filename> <search-string> <output-filename> \n", argv[0]);
            exit(1);    
        }
    }

    printSizeOfFile(inputFile, outputFile);
    printStringCount(argv[2], inputFile, outputFile);

    /* Close files. */
    fclose(inputFile);
    fclose(outputFile);

    return 0;

}