#include <stdlib.h>
#include <string.h>
#include "basic.h"
#include "history.h"
#include "executeCommandBlock.h"
#include "lists.h"
#include "shell_start.h"


struct CommandBlock** initializeArrayOfCommandBlocks(char* argv, size_t argc,size_t countList, size_t* globalSpecialCharIndexArray)
{
    struct CommandBlock** commandBlockArray = calloc(countList+1, sizeof(struct CommandBlock*));

    if(commandBlockArray == NULL)
        return NULL;

    for(size_t i = 0; i < countList+1; ++i) {
        commandBlockArray[i] = constructCommandBlock(argv, argc, globalSpecialCharIndexArray, i, NULL, countList);
        parseCommandBlock(commandBlockArray[i], argv);
    }

    return commandBlockArray;
}

void destroyArrayOfCommandBlocks(struct CommandBlock** commandBlocks, size_t sizeOfCommandBlocks)
{
    if(commandBlocks == NULL || sizeOfCommandBlocks == 0)
        return;

    for(size_t i = 0; i < sizeOfCommandBlocks; ++i) {
        destroyCommandBlock(commandBlocks[i]);
    }

    free(commandBlocks);
    commandBlocks = NULL;
}

void start_shell(char* argv, size_t argc,size_t countList)
{
    char* command = readInput();
    if(command == NULL)
        return;
    command = storeCommands(command);
    
    pid_t childPID = 0;

    size_t* globalSpecialCharIndexArray = GlobalSpecialCharIndexArray(argc, argv);

    struct CommandBlock** commandBlockArray = initializeArrayOfCommandBlocks(argv, argc, countList,globalSpecialCharIndexArray);

    if(commandBlockArray == NULL)
        return;

    if(globalSpecialCharIndexArray != NULL) {
        
        childPID = executeCommandBlock(commandBlockArray[0], argv);

        for(size_t i = 0; i < countList; ++i) {

            if(strcmp(argv[globalSpecialCharIndexArray[i]], "&&") == 0) {
                   if(childPID == -1)
                        break;
                   childPID =  andList(commandBlockArray[i+1],argv, countList, childPID);
                   continue;
                }

            if(strcmp(argv[globalSpecialCharIndexArray[i]], "||") == 0) {
                    if(childPID == -1)
                        break;
                    childPID = orList(commandBlockArray[i+1],argv, countList childPID);
                    continue;
            }
            if(strcmp(argv[globalSpecialCharIndexArray[i]], ";") == 0) {
                    if(childPID == -1)
                        break;
                    childPID  = semicolonList(commandBlockArray[i+1], argv,childPID);
            }
        }
        
    }

    else 
        executeCommandBlock(commandBlockArray[0], argv);
    
        
    free(globalSpecialCharIndexArray);
    destroyArrayOfCommandBlocks(commandBlockArray, countList + 1);
}
