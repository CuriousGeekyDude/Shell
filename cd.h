#ifndef CD_H
#define CD_H
#define _POSIX_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

char* returnUpPath(const char* currentPath); //Free the memory after done using it
int cd(char* argv[]);

#endif