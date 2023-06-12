#include "executeCommand.h"

pid_t executeBlockCommand(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, bool toFork)
{
    if(strcmp(argv[BegIndexOfCommandBlock], "cd") == 0) {
            cdCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "exit") == 0) {
            exitCommand(numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "exec") == 0) {
            execCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "pwd") == 0) {
            pwdCommand(numOfStringsInCommandBlock);
            return 0;
        }

    if(toFork == true)
        return nonBuiltInCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);

    else
        exec_nonBuiltInCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
}