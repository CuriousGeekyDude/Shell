#include "pwd.h"

void pwdCommand()
{
    char* PWD = getenv("PWD");
    printf("%s\n", PWD);
    fflush(stdout);   
}