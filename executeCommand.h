#ifndef EXECUTECOMMAND_H
#define EXECUTECOMMAND_H

#include "basic.h"
#include "commandBlock.h"
#include "cd.h"
#include "exec.h"
#include "exit.h"
#include "nonBuiltInC.h"
#include "pwd.h"

pid_t executeCommandBlock(struct CommandBlock* commandBlock, bool toFork);

#endif