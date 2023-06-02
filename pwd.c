#include "pwd.h"

static inline void pwdCommand(size_t argc)
{
    if(argc > 1)
        return;

    char* PWD = getenv("PWD");
    printf("%s\n", PWD);
    fflush(stdout);   
}