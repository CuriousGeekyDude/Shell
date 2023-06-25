#include <string.h>
#include <basic.h>
#include <pipe_redirection.h>
#include <executeCommand.h>
#include <executeCommandBlock.h>

pid_t executeCommandBlock(struct CommandBlock* commandBlock)
{
    pid_t childPID = -1;

    if(commandBlock->commandType == NULL || strcmp(commandBlock->commandType, "Invalid") == 0)
        return -1;

    if(strcmp(commandBlock->commandType, "N") == 0) {
        childPID =  executeCommand(commandBlock->begIndex, commandBlock->numOfStrings, commandBlock->commandType,true);   
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