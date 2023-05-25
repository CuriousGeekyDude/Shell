#ifndef CD_H
#define CD_H
#define _POSIX_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

char* returnUpPath(const char* currentPath); //Free the memory after done using the returned non-null string
char* findPath_ColonSepDirectories(const char* colonSepDirectories, const char* path); //Free the memory after done using the returned non-null string
int updatePWD_OLDPWD(const char* newOLDPWD, const char* newPWD);
int cd(int argc, char* argv[]);

#endif