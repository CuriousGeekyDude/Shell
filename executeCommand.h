#ifndef EXECUTECOMMAND_H
#define EXECUTECOMMAND_H

#include "basic.h"
#include "cd.h"
#include "exec.h"
#include "exit.h"
#include "nonBuiltInC.h"
#include "pwd.h"

void executeBlockCommand(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock,bool toFork);

#endif