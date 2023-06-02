#ifndef LISTS_H
#define LISTS_H

#include "basic.h"

static size_t count = 0;

static inline size_t specialCharCounter();
size_t* specialCharIndexArray();
size_t* numberOfStringsInEachBlock();
void andList(char** specialCharArray, size_t* index_specialCharArray,size_t indexStartCommand, size_t indexEndCommand,pid_t waitReturn);
void orList(char** specialCharArray, size_t* index_specialCharArray,size_t indexStartCommand, size_t indexEndCommand ,pid_t waitReturn);
void semicolonList(char** specialCharArray, size_t* index_specialCharArray, size_t indexStartCommand, size_t indexEndCommand);

#endif