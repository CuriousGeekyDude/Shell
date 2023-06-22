#ifndef LISTS_H
#define LISTS_H

#include "commandBlock.h"


size_t GlobalSpecialCharCounter(int argc, char** argv);
size_t* GlobalSpecialCharIndexArray(int argc, char** argv);

pid_t andList(struct CommandBlock* commandBlock, char** argv, size_t countList, pid_t waitReturn);
pid_t orList(struct CommandBlock* commandBlock, char** argv, size_t countList, pid_t waitReturn);
pid_t semicolonList(struct CommandBlock* commandBlock, char** argv,pid_t waitReturn);

#endif