#include "lists.h"
#include "basic.h"

static inline size_t specialCharCounter()
{
    count = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0)
            ++count;
    }

    return count;
}

size_t* specialCharIndexArray()
{
    count = specialCharCounter();
    if(count == 0)
        return NULL;

    size_t*  specialCharIndexArray = calloc(count, sizeof(size_t));
    
    size_t index_specialCharArray = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0) {
            specialCharIndexArray[index_specialCharArray] = i
            ++index_specialCharArray;
        }
    }

    return specialCharIndexArray;
}

size_t* numberOfStringsInEachBlock(size_t* SpecialCharIndexArray)
{
    if(count == 0)
        return NULL;

    size_t* numOfStringArray = calloc(count+1, sizeof(size_t));

    for(size_t i = 0; i < count+1; ++i) {
        if(i == 0) {
            numOfStringArray[i] = SpecialCharIndexArray[i];
            continue;
        }
        if(i == count) {
            numOfStringArray[count] = (argc-1) - SpecialCharIndexArray[count-1];
            continue;
        }
        
        numOfStringArray[i] = SpecialCharIndexArray[i+1] - SpecialCharIndexArray[i] - 1;
    }
    return numOfStringArray;
}

void andList(char** specialCharArray, size_t* index_specialCharArray,size_t indexStartCommand, size_t indexEndCommand,pid_t waitReturn)
{
    if(specialCharArray == NULL || waitReturn == -1)
        return;
    

    

}

void orList(char specialCharArray[], size_t index_specialCharArray,pid_t waitReturn)
{

}

void semicolonList(char specialCharArray[], size_t index_specialCharArray)
{

}