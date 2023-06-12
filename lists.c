#include "lists.h"

static inline void specialCharCounter()
{
    countList = 0;
    for(size_t i = 0; i < argc; ++i) {
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0) || (strcmp(argv[i], "|") == 0))
            ++countList;
    }

}

size_t* specialCharIndexArray()
{
    if(countList == 0)
        return NULL;

    size_t*  SpecialCharIndexArray = calloc(countList, sizeof(size_t));
    
    size_t index_specialCharArray = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0 || strcmp(argv[i], "|") == 0) {
            SpecialCharIndexArray[index_specialCharArray] = i;
            ++index_specialCharArray;
        }
    }

    return SpecialCharIndexArray;
}

size_t* numberOfStringsInEachBlock(size_t* SpecialCharIndexArray)
{
    if(countList == 0 || SpecialCharIndexArray == NULL)
        return NULL;

    size_t* numOfStringArray = calloc(countList+1, sizeof(size_t));

    for(size_t i = 0; i < countList+1; ++i) {
        if(i == 0) {
            numOfStringArray[i] = SpecialCharIndexArray[i];
            continue;
        }
        if(i == countList) {
            numOfStringArray[countList] = (argc-1) - SpecialCharIndexArray[countList-1];
            continue;
        }

        
        numOfStringArray[i] = SpecialCharIndexArray[i] - SpecialCharIndexArray[i-1] - 1;
    }
    return numOfStringArray;
}

pid_t andList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, pid_t waitReturn)
{
    if(countList == 0 || waitReturn < 0)
        return -1;
    
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);

}

pid_t orList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, pid_t waitReturn)
{
    if(countList == 0 || waitReturn >= 0)
        return -1;
    
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);
}

static inline pid_t semicolonList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock)
{
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);
}