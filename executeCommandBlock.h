#ifndef EXECUTECOMMAND_H
#define EXECUTECOMMAND_H

#include "basic.h"
#include "commandBlock.h"
#include "builtInC.h"
#include "nonBuiltInC.h"


pid_t executeCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork);
pid_t executeCommandBlock(struct CommandBlock* commandBlock);

#endif