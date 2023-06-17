#ifndef BASIC_H
#define BASIC_H
/*#define _POSIX_SOURCE*/       //check if there is any need for this or not!

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <limits.h>
#include "color.c"  //chandge .c to .h!
#include "history.c"    //change .c to .h!
#include "error_functions.c"


#define BUFFSIZE 4096

int argc;
char inputWords[BUFFSIZE];
char* argv[BUFFSIZE];

void errorHandling(const char* stringToPrint);

void printShellSign();

size_t countLengthOfString(const char* string);
void initializeCharBuffer(char buffer[], size_t sizeOfBuffer);
void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer);

char* findPath_ColonSepDirectories(char* colonSepDirectories, const char* path); 


char* commandToStoreInHistBlock(); 


void storeInputWords();
size_t countArgc();
void initializeArgv(int Argc);
void readInput();
char* read_storeCommands();



#endif