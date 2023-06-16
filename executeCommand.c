#include "executeCommand.h"

pid_t executeCommandBlock(struct CommandBlock* commandBlock, bool toFork)
{
    if(strcmp(argv[commandBlock->begIndex], "cd") == 0) {
            cdCommand(commandBlock->begIndex, commandBlock->numOfStrings);
            return 0;
        }
        if(strcmp(argv[commandBlock->begIndex], "exit") == 0) {
            exitCommand(commandBlock->numOfStrings);
            return 0;
        }
        if(strcmp(argv[commandBlock->begIndex], "exec") == 0) {
            execCommand(commandBlock->begIndex, commandBlock->numOfStrings);
            return 0;
        }
        if(strcmp(argv[commandBlock->begIndex], "pwd") == 0) {
            pwdCommand(commandBlock->numOfStrings);
            return 0;
        }

    if(toFork == true)
        return nonBuiltInCommand(commandBlock->begIndex, commandBlock->numOfStrings);

    else
        exec_nonBuiltInCommand(commandBlock->begIndex, commandBlock->numOfStrings);
}