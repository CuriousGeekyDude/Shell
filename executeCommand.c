#include <string.h>
#include "builtInC.h"
#include "nonBuiltInC.h"
#include "executeCommand.h"



pid_t executeCommand(char** argv, size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork)
{
    if(strcmp(argv[begIndexOfCommand], "cd") == 0) {
        if(cdCommand(begIndexOfCommand, numOfStringsInCommand) == -1)
            return -1;
        return 0;
    }

    if(strcmp(argv[begIndexOfCommand], "exit") == 0) {
        exitCommand(numOfStringsInCommand);
        return -1;
    }

    if(strcmp(argv[begIndexOfCommand], "exec") == 0) {
        execCommand(begIndexOfCommand, numOfStringsInCommand);
        return 0;
    }
    
    if(strcmp(argv[begIndexOfCommand], "pwd") == 0) {
        pwdCommand(numOfStringsInCommand);
        return 0;
    }
        

    if(toFork == true)
        return nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);

    else
        exec_nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand, argv);
}