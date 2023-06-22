#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "commandBlock.h"



/*Takes care of the empty command block case.
 The array is assumed to be non-null as the null case is taken care of before this function is called.*/
size_t emptyCommandBlockErrorHandling(size_t argc, size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{
    if(sizeOfGlobalSpecialCharIndexArray < commandBlockNumber)
        return 0;

//The case where &&, ||, or ; appear at the start of the command and the block number is 0
    if(commandBlockNumber == 0 && GlobalSpecialCharIndexArray[0] == 0)
        return 0;

//The case where &&, ||, or ; appear at the end of the command and the requested block command is the last one
    if(commandBlockNumber == sizeOfGlobalSpecialCharIndexArray && GlobalSpecialCharIndexArray[sizeOfGlobalSpecialCharIndexArray-1] == argc-1)
        return 0;

//The case where block number is somewhere in the middle and the block is empty e.g && &&, || ||, or ; ;
    if(0 < commandBlockNumber && commandBlockNumber < sizeOfGlobalSpecialCharIndexArray && GlobalSpecialCharIndexArray[commandBlockNumber] == GlobalSpecialCharIndexArray[commandBlockNumber-1] + 1)
        return 0;

    return 1;
}


size_t findBegIndexCommandBlock(size_t argc,size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{

    if(argc == 0 || GlobalSpecialCharIndexArray == NULL || commandBlockNumber == 0)
        return 0;

    if(emptyCommandBlockErrorHandling(argc ,GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;


    return GlobalSpecialCharIndexArray[commandBlockNumber - 1] + 1;
}

size_t findEndIndexCommandBlock(size_t argc, size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{
    if(argc == 0)
        return 0;

    if(GlobalSpecialCharIndexArray == NULL)
        return argc - 1;

    if(emptyCommandBlockErrorHandling(argc ,GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;

    if(commandBlockNumber == sizeOfGlobalSpecialCharIndexArray)
        return argc - 1;
    else
        return GlobalSpecialCharIndexArray[commandBlockNumber] - 1;
}

size_t numOfStringsInCommandBlock(char** argv, size_t argc, size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber)
{
    //The case where begIndex = endIndex and begIndex == 0 where it could be a valid command like ls or an invalid one like NULL
    if(begIndexCommandBlock == endIndexCommandBlock && begIndexCommandBlock == 0) {
        if(argc == 0)
            return 0;

        if(strcmp(argv[0], "&&") == 0 || strcmp(argv[0], "||") == 0 || strcmp(argv[0], ";") == 0)
            return 0;
        else {
            if(commandBlockNumber == 0)
                return 1;
            else
                return 0;
            }
    }
    else
        return endIndexCommandBlock - begIndexCommandBlock + 1;
}

size_t numOfSpecialCharsInCommandBlock(char** argv, size_t begIndexCommandBlock, size_t endIndexCommandBlock)
{
    if(begIndexCommandBlock == endIndexCommandBlock)
        return 0;

    size_t count = 0;
    for(size_t i = begIndexCommandBlock; i <= endIndexCommandBlock; ++i) {
        if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0)
            ++count;
    }
    return count;
}

size_t* numberOfStringsInEachPipe(struct CommandBlock* commandBlock)
{
    if(commandBlock->localSpecialCharIndexArray == NULL)
        return NULL;

    size_t size = commandBlock->sizeOfLocalSpecialCharIndexArray + 1;
    size_t* tempIndex = commandBlock->localSpecialCharIndexArray;
    size_t* numOfStringArray = calloc(size, sizeof(size_t));

    for(size_t i = 0; i < size; ++i) {
        if(i == 0) {
            numOfStringArray[i] = tempIndex[i] - commandBlock->begIndex;
            continue;
        }
        if(i == (size - 1)) {
            numOfStringArray[size-1] = commandBlock->endIndex - tempIndex[size-2];
            continue;
        }

        
        numOfStringArray[i] = tempIndex[i] - tempIndex[i-1] - 1;
    }
    return numOfStringArray;
}

size_t* initializeLocalSpecialCharIndexArray(char** argv, size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray)
{
    if(SizeOfLocalSpecialCharIndexArray == 0)
        return NULL;

    size_t* LocalSpecialCharIndexArray = calloc(SizeOfLocalSpecialCharIndexArray,sizeof(size_t));

    size_t index_LocalSpecialCharIndexArray = 0;
    for(size_t i = begIndexCommandBlock; i <= endIndexCommandBlock; ++i) {
        if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0) {
            LocalSpecialCharIndexArray[index_LocalSpecialCharIndexArray] = i;
            ++index_LocalSpecialCharIndexArray;
        }
    }

    return LocalSpecialCharIndexArray;
}
struct CommandBlock* constructCommandBlock(char** argv, size_t argc,size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType, size_t countList)
{
    struct CommandBlock* commandBlock = calloc(1, sizeof(struct CommandBlock));

    assert(commandBlock != NULL && "calloc failed!");

    commandBlock->begIndex = findBegIndexCommandBlock(argc ,_GlobalSpecialCharIndexArray, commandBlockNumber, countList);
    commandBlock->endIndex = findEndIndexCommandBlock(argc, _GlobalSpecialCharIndexArray, commandBlockNumber, countList);
    commandBlock->numOfStrings = numOfStringsInCommandBlock(argv,argc,commandBlock->begIndex, commandBlock->endIndex, commandBlockNumber);
    commandBlock->sizeOfLocalSpecialCharIndexArray = numOfSpecialCharsInCommandBlock(argv,commandBlock->begIndex, commandBlock->endIndex);
    commandBlock->localSpecialCharIndexArray = initializeLocalSpecialCharIndexArray(argv,commandBlock->begIndex, commandBlock->endIndex, commandBlock->sizeOfLocalSpecialCharIndexArray);
    commandBlock->numOfStringsInEachPipe = numberOfStringsInEachPipe(commandBlock);
    commandBlock->commandType = CommandType;
    return commandBlock;
} 
void destroyCommandBlock(struct CommandBlock* commandBlock)
{
    if(commandBlock == NULL)
        return;
    
    if(commandBlock->localSpecialCharIndexArray != NULL) {
        if(commandBlock->numOfStringsInEachPipe != NULL) {
            free(commandBlock->numOfStringsInEachPipe);

            free(commandBlock->localSpecialCharIndexArray);
            free(commandBlock);
        }
        else {
            free(commandBlock->localSpecialCharIndexArray);
            free(commandBlock);
        }
    }
    else
        free(commandBlock);

    commandBlock = NULL;
}


bool brokenPipeRedirection(struct CommandBlock* commandBlock)
{
    if(commandBlock == NULL)
        return false;
    if(commandBlock->numOfStringsInEachPipe == NULL)
        return false;

    for(size_t i = 0; i < commandBlock->sizeOfLocalSpecialCharIndexArray + 1; ++i) {
        if(commandBlock->numOfStringsInEachPipe[i] == 0)
            return true;
    }
    return false;
}


void parseCommandBlock(struct CommandBlock* commandBlock, char** argv)
{
    size_t lastIndex = commandBlock->sizeOfLocalSpecialCharIndexArray - 1;

    if(commandBlock->begIndex == commandBlock->endIndex) {
        if(commandBlock->numOfStrings == 1) {
            size_t i = commandBlock->begIndex;

            if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0) {
                commandBlock->commandType = "Invalid";
                return;
            }
            else {
                commandBlock->commandType = "N";
                return;
            }
        }
        else {
            commandBlock->commandType = "Invalid";
            return;
        }
    }

    switch(commandBlock->sizeOfLocalSpecialCharIndexArray) {
        case 0:
            commandBlock->commandType = "N";    //Normal command
            break;
        case 1:
            if(strcmp(argv[commandBlock->localSpecialCharIndexArray[0]], "|") == 0)
                commandBlock->commandType = "P";    //Pipeline
            else
                commandBlock->commandType = "R";    //Redirection
            break;
        default:
            if(strcmp(argv[commandBlock->localSpecialCharIndexArray[lastIndex]], ">") == 0)
                commandBlock->commandType = "P+R";  //Pipeline + Redirection at the end of command
            else
                commandBlock->commandType = "P";    //Pipeline
            break;
    }
}
void printCommandBlock(struct CommandBlock* commandBlock)
{
    size_t x = commandBlock->begIndex;
    size_t y = commandBlock->endIndex;
    size_t z = commandBlock->numOfStrings;
    size_t w = commandBlock->sizeOfLocalSpecialCharIndexArray;
    char* s = commandBlock->commandType;
    size_t* array1 = commandBlock->numOfStringsInEachPipe;
    size_t* array2 = commandBlock->localSpecialCharIndexArray;

    printf("\n");
    printf("----------------------------------------------------------\n");
    printf("BegIndex: %zu\n", x);
    printf("EndIndex: %zu\n", y);
    printf("NumOfStrings: %zu\n", z);
    printf("sizeOfLocalSCharIndexArray: %zu\n", w);
    printf("CommandType: %s\n", s);
    
    printf("specCHarIndexArray: ");
    if(array1 != NULL) {
        for(size_t i = 0; i < w+1; ++i)
            printf(" %zu ", array1[i]);
        printf("\n");
    }
    else
        printf("specCHarIndexArray Is NULL!\n");


    if(array2 != NULL) {
        printf("SpecialCharIndexArray: ");
        for(size_t i = 0; i < w; ++i)
            printf(" %zu ", array2[i]);
        printf("\n");
    }
    else
        printf("SpecialCharIndexArray Is NULL!\n");
    if(brokenPipeRedirection(commandBlock) == true)
        printf("pipe or redirection is broken!\n");
    
    printf("----------------------------------------------------------\n");
    fflush(stdout);

}
