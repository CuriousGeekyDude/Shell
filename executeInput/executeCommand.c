#include <string.h>
#include "basic.h"
#include "builtInC.h"
#include "nonBuiltInC.h"
#include "executeCommand.h"



pid_t executeCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork)
{
    if(strcmp(argv[begIndexOfCommand], "cd") == 0) {
        if(cdCommand(argv ,begIndexOfCommand, numOfStringsInCommand) == -1)
            return -1;
        return 0;
    }

    if(strcmp(argv[begIndexOfCommand], "exit") == 0) {
        exitCommand(numOfStringsInCommand);
        return -1;
    }

    if(strcmp(argv[begIndexOfCommand], "exec") == 0) {
        execCommand(argv, begIndexOfCommand, numOfStringsInCommand);
        return 0;
    }
    
    if(strcmp(argv[begIndexOfCommand], "pwd") == 0) {
        pwdCommand(numOfStringsInCommand);
        return 0;
    }
        

    if(toFork == true)
        return nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);

    else
        exec_nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);
}