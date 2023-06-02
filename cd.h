#ifndef CD_H
#define CD_H
#define _POSIX_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

char* returnUpPath(const char* currentPath); //Free the memory after done using it

static inline void update_PWD_OLDPWD(const char* newPWD, const char* newOLDPWD);

int cdCommand(size_t indexArgv, size_t argc);

#endif