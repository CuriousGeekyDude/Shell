#ifndef COMMANDBLOCK_H
#define COMMANDBLOCK_H
#include "basic.h"
#include <assert.h>


struct CommandBlock
{
    size_t begIndex;
    size_t endIndex;
    size_t numOfStrings;
    size_t sizeOfLocalSpecialCharIndexArray;
    size_t* localSpecialCharIndexArray;
    char* commandType;
};

static inline size_t emptyCommandBlockErrorHandling(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);

static inline size_t findBegIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
static inline size_t findEndIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
static inline size_t numOfStringsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber);
static inline size_t numOfSpecialCharsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock);
static inline size_t* initializeLocalSpecialCharIndexArray(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray);

static inline struct CommandBlock* constructCommandBlock(size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType);

void parseCommandBlock(struct CommandBlock* commandBlock);

static inline void printCommandBlock(struct CommandBlock* commandBlock);    //Used to print CommandBlock structures for debugging only

static inline int destroyCommandBlock(struct CommandBlock* commandBlock);


#endif


