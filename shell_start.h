#ifndef SHELL_START_H
#define SHELL_START_H

#include "pipe_redirection.h"
#include "lists.h"

struct CommandBlock** initializeArrayOfCommandBlocks(void);
void destroyArrayOfCommandBlocks(struct CommandBlock** commandBlocks, size_t sizeOfCommandBlocks);


void start_shell(void);

#endif