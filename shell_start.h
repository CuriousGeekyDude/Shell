#ifndef SHELL_START_H
#define SHELL_START_H

#include "commandBlock.h"

struct CommandBlock** initializeArrayOfCommandBlocks(char** argv, size_t argc,size_t countList ,size_t* globalSpecialCharIndexArray);
void destroyArrayOfCommandBlocks(struct CommandBlock** commandBlocks, size_t sizeOfCommandBlocks);


void start_shell(char** argv, size_t argc, size_t countList);

#endif