#ifndef COMMANDBLOCK_H
#define COMMANDBLOCK_H

#include <stdbool.h>

struct CommandBlock
{
    size_t begIndex;
    size_t endIndex;
    size_t numOfStrings;
    size_t sizeOfLocalSpecialCharIndexArray;
    size_t* localSpecialCharIndexArray;
    size_t* numOfStringsInEachPipe;
    char* commandType;
};

size_t emptyCommandBlockErrorHandling(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);

size_t findBegIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
size_t findEndIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
size_t numOfStringsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber);
size_t numOfSpecialCharsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock);
size_t* numberOfStringsInEachPipe(struct CommandBlock* commandBlock);
size_t* initializeLocalSpecialCharIndexArray(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray);

struct CommandBlock* constructCommandBlock (size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType);
void destroyCommandBlock(struct CommandBlock* commandBlock);

bool brokenPipeRedirection(struct CommandBlock* commandBlock);
void parseCommandBlock(struct CommandBlock* commandBlock);



void printCommandBlock(struct CommandBlock* commandBlock);    //Used to print CommandBlock structures for debugging only


#endif


