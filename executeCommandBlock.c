#include "executeCommand.h"


pid_t executeCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork)
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
        exec_nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);
}

pid_t executeCommandBlock(struct CommandBlock* commandBlock)
{
    pid_t childPID = -1;

    if(commandBlock->commandType == NULL || strcmp(commandBlock->commandType, "Invalid") == 0)
        return -1;

    if(strcmp(commandBlock->commandType, "N") == 0) {
        childPID =  executeCommand(commandBlock->begIndex, commandBlock->numOfStrings, true);   
        return childPID;
    }    

    if(brokenPipeRedirection(commandBlock) == true)
        return childPID;
    else {
        if(strcmp(commandBlock->commandType, "P") == 0) {         
            childPID =  start_pipeline(commandBlock);
            return childPID;
        }
    
        if(strcmp(commandBlock->commandType, "R") == 0) {
            childPID =  start_redirection(commandBlock, argv[commandBlock->localSpecialCharIndexArray[0] + 1]);
            return childPID;
        }

        if(strcmp(commandBlock->commandType, "P+R") == 0) {
            childPID = start_pipelineRedirection(commandBlock, argv[commandBlock->localSpecialCharIndexArray[commandBlock->sizeOfLocalSpecialCharIndexArray - 1] + 1]);
            return childPID;
        }
    }
    
}