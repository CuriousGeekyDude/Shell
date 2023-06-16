#include "lists.h"

static inline void specialCharCounter()
{
    countList = 0;
    for(size_t i = 0; i < argc; ++i) {
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0) || (strcmp(argv[i], "|") == 0))
            ++countList;
    }

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