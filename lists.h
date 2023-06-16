#ifndef LISTS_H
#define LISTS_H

#include "executeCommandBlock.h"

static size_t countList = 0;

void GlobalSpecialCharCounter(void);
size_t* GlobalSpecialCharIndexArray(void);

pid_t andList(struct CommandBlock* commandBlock, pid_t waitReturn);
pid_t orList(struct CommandBlock* commandBlock, pid_t waitReturn);
pid_t semicolonList(struct CommandBlock* commandBlock, pid_t waitReturn);

#endif