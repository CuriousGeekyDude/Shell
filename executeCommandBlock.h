#ifndef EXECUTECOMMAND_H
#define EXECUTECOMMAND_H



#include "commandBlock.h"
#include <sys/types.h>



pid_t executeCommandBlock(struct CommandBlock* commandBlock, char** argv);

#endif