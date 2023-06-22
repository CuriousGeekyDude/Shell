#ifndef COMMANDBLOCK_H
#define COMMANDBLOCK_H



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

size_t emptyCommandBlockErrorHandling(size_t argc, size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);

size_t findBegIndexCommandBlock(size_t argc, size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
size_t findEndIndexCommandBlock(size_t argc, size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray);
size_t numOfStringsInCommandBlock(char** argv, size_t argc, size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber);
size_t numOfSpecialCharsInCommandBlock(char** argv, size_t begIndexCommandBlock, size_t endIndexCommandBlock);
size_t* numberOfStringsInEachPipe(struct CommandBlock* commandBlock);
size_t* initializeLocalSpecialCharIndexArray(char** argv, size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray);

struct CommandBlock* constructCommandBlock(char** argv ,size_t argc, size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType, size_t countList);
void destroyCommandBlock(struct CommandBlock* commandBlock);

bool brokenPipeRedirection(struct CommandBlock* commandBlock);
void parseCommandBlock(struct CommandBlock* commandBlock, char** argv);



void printCommandBlock(struct CommandBlock* commandBlock);    //Used to print CommandBlock structures for debugging only


#endif


