#include "basic.h"

static inline size_t emptyCommandBlockErrorHandling(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
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



static inline size_t findBegIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{

    if(argc == 0 || GlobalSpecialCharIndexArray == NULL || commandBlockNumber == 0)
        return 0;

    if(emptyCommandBlockErrorHandling(GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;


    return GlobalSpecialCharIndexArray[commandBlockNumber - 1] + 1;
}
static inline size_t findEndIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{
    if(argc == 0)
        return 0;

    if(GlobalSpecialCharIndexArray == NULL)
        return argc - 1;

    if(emptyCommandBlockErrorHandling(GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;

    if(commandBlockNumber == sizeOfGlobalSpecialCharIndexArray)
        return argc - 1;
    else
        return GlobalSpecialCharIndexArray[commandBlockNumber] - 1;
}
static inline size_t numOfStringsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber)
{
    //The case where begIndex = endIndex and begIndex == 0 where it could be a valid command like ls or an invalid one like NULL
    if(begIndexCommandBlock == endIndexCommandBlock && begIndexCommandBlock == 0) {
        if(argc == 0)
            return 0;

        if(argc == 1) {
            if(strcmp(argv[0], "&&") != 0 && strcmp(argv[0], "||") != 0 && strcmp(argv[0], ";") != 0 && strcmp(argv[0], "|") != 0 && strcmp(argv[0], ">") != 0)
                return 1;
            else
                return 0;
        }

        else {
            if(strcmp(argv[0], "&&") == 0 || strcmp(argv[0], "||") == 0 || strcmp(argv[0], ";") == 0 || strcmp(argv[0], "|") == 0 || strcmp(argv[0], ">") == 0)
                return 0;
            else {
                if(commandBlockNumber == 0)
                    return 1;
                else
                    return 0;
            }
        }

    }
    else
        return endIndexCommandBlock - begIndexCommandBlock + 1;
}
static inline size_t numOfSpecialCharsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock)
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
static inline size_t* initializeLocalSpecialCharIndexArray(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray)
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


static inline struct CommandBlock* constructCommandBlock(size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType)
{
    struct CommandBlock* commandBlock = calloc(1, sizeof(struct CommandBlock));

    assert(commandBlock != NULL && "calloc failed!");

    commandBlock->begIndex = findBegIndexCommandBlock(_GlobalSpecialCharIndexArray, commandBlockNumber, counting);
    commandBlock->endIndex = findEndIndexCommandBlock(_GlobalSpecialCharIndexArray, commandBlockNumber, counting);
    commandBlock->numOfStrings = numOfStringsInCommandBlock(commandBlock->begIndex, commandBlock->endIndex, commandBlockNumber);
    commandBlock->sizeOfLocalSpecialCharIndexArray = numOfSpecialCharsInCommandBlock(commandBlock->begIndex, commandBlock->endIndex);
    commandBlock->localSpecialCharIndexArray = initializeLocalSpecialCharIndexArray(commandBlock->begIndex, commandBlock->endIndex, commandBlock->sizeOfLocalSpecialCharIndexArray);
    commandBlock->commandType = CommandType;
    return commandBlock;
}


void parseCommandBlock(struct CommandBlock* commandBlock)
{
    size_t lastIndex = commandBlock->sizeOfLocalSpecialCharIndexArray - 1;

    if(commandBlock->begIndex == commandBlock->endIndex && commandBlock->begIndex == 0) {
        if(commandBlock->numOfStrings == 1)
            commandBlock->commandType = "N";
        else
            commandBlock->commandType = "Invalid";
        return;
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


static inline void printCommandBlock(struct CommandBlock* commandBlock)
{
    size_t x = commandBlock->begIndex;
    size_t y = commandBlock->endIndex;
    size_t z = commandBlock->numOfStrings;
    size_t w = commandBlock->sizeOfLocalSpecialCharIndexArray;
    char* s = commandBlock->commandType;
    size_t* array = commandBlock->localSpecialCharIndexArray;

    printf("\n");
    printf("----------------------------------------------------------\n");
    printf("BegIndex: %zu\n", x);
    printf("EndIndex: %zu\n", y);
    printf("NumOfStrings: %zu\n", z);
    printf("sizeOfLocalSCharIndexArray: %zu\n", w);
    printf("CommandType: %s\n", s);
    
    printf("array: ");
    for(size_t i = 0; i < w; ++i)
        printf(" %zu ", array[i]);

    printf("\n");
    printf("----------------------------------------------------------\n");
    fflush(stdout);

}


static inline int destroyCommandBlock(struct CommandBlock* commandBlock)
{
    if(commandBlock == NULL)
        return 0;
    
    if(commandBlock->localSpecialCharIndexArray == NULL) {
        free(commandBlock);
        return 0;
    }
    else {
        free(commandBlock->localSpecialCharIndexArray);
        free(commandBlock);
        return 0;
    }
}