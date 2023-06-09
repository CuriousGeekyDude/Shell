#include <stdlib.h>
#include <string.h>
#include <basic.h>
#include <executeCommandBlock.h>
#include <lists.h>




void GlobalSpecialCharCounter(void) 
{
    countList = 0;
    for(size_t i = 0; i < argc; ++i) {
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0))
            ++countList;
    }
}

size_t* GlobalSpecialCharIndexArray(void) 
{
    if(countList == 0)
        return NULL;

    size_t*  SpecialCharIndexArray = calloc(countList, sizeof(size_t));
    
    size_t index_specialCharArray = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0) {
            SpecialCharIndexArray[index_specialCharArray] = i;
            ++index_specialCharArray;
        }
    }

    return SpecialCharIndexArray;
}


pid_t andList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(countList == 0 || waitReturn < 0)
        return -1;
    
    return executeCommandBlock(commandBlock);

}

pid_t orList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(countList == 0 || waitReturn >= 0)
        return -1;
    
    return executeCommandBlock(commandBlock);
}

pid_t semicolonList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(waitReturn == -1)
        return -1;
    return executeCommandBlock(commandBlock);
}