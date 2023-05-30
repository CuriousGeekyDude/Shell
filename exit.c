#include "exit.h"
#include "basic.h"

void exitCommand()
{
    freeAllBlocks();
    exit(EXIT_SUCCESS);
}