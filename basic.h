#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include "history.c"    //change .c to .h!
#include "error_functions.c"


#define BUFFSIZE 4096

int argc;
char inputWords[BUFFSIZE];
char* argv[BUFFSIZE];

static inline void errorHandling(const char* stringToPrint);

static inline void printShellSign();

size_t countLengthOfString(const char* string);
void initializeCharBuffer(char buffer[], size_t sizeOfBuffer);
void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer);

char* findPath_ColonSepDirectories(char* colonSepDirectories, const char* path); //Free the memory after you are done with it
char* commandToStoreInHistBlock();  //Free the memory after done using it

void readInput();

static inline char* read_storeCommands();

#endif