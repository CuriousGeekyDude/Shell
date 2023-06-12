#ifndef LISTS_H
#define LISTS_H

#include "executeCommand.h"

static size_t countList = 0;

static inline size_t specialCharCounter();
size_t* specialCharIndexArray();
size_t* numberOfStringsInEachBlock(size_t* SpecialCharIndexArray);
pid_t andList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, pid_t waitReturn);
pid_t orList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock ,pid_t waitReturn);
static inline pid_t semicolonList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock);

#endif