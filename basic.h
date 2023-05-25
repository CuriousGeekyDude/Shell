#ifndef BASIC_H
#define BASIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "error_functions.c"
#define BUFFSIZE 4096

int argc;
char inputWords[BUFFSIZE];
char cdPATH[BUFFSIZE] = "/home/feri/Desktop/repos:/home:/home/feri/Desktop/repos/shell:shell/Shell";
char* argv[BUFFSIZE];

static inline void errorHandling(const char* stringToPrint);

size_t countLengthOfString(const char* string);
void initializeCharBuffer(char buffer[], size_t sizeOfBuffer);
void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer);

void readInput();


#endif