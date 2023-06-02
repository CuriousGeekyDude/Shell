#include "exit.h"
#include "basic.h"

static inline void exitCommand(size_t argc)
{
    if(argc > 1)
        return;
    freeAllBlocks();
    exit(EXIT_SUCCESS);
}