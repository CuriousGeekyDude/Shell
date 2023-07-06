#include <string.h>
#include <basic.h>
#include <builtInC.h>
#include <nonBuiltInC.h>
#include <weather.h>
#include <executeCommand.h>




pid_t executeCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, const char* commandType, bool toFork)
{
    if(strcmp(argv[begIndexOfCommand], "cd") == 0) {
        if(cdCommand(begIndexOfCommand) == -1)
            return -1;
        return 0;
    }

    if(strcmp(argv[begIndexOfCommand], "exit") == 0) {
        exitCommand();
        return -1;
    }

    if(strcmp(argv[begIndexOfCommand], "exec") == 0) {
        execCommand(begIndexOfCommand);
        return 0;
    }
    
    if(strcmp(argv[begIndexOfCommand], "pwd") == 0) {
        pwdCommand();
        return 0;
    }

    if(strcmp(argv[begIndexOfCommand], "weather") == 0) {
        return weatherCommand(begIndexOfCommand, numOfStringsInCommand, commandType);
    }
        

    if(toFork == true)
        return nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);

    else
        exec_nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);
}