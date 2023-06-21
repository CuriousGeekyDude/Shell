#ifndef SHELL_START_H
#define SHELL_START_H

#include "commandBlock.h"

struct CommandBlock** initializeArrayOfCommandBlocks(size_t* globalSpecialCharIndexArray);
void destroyArrayOfCommandBlocks(struct CommandBlock** commandBlocks, size_t sizeOfCommandBlocks);


void start_shell(void);

#endif