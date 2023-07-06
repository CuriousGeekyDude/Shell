#include <stdlib.h>
#include <basic.h>
#include <history.h>

int count = 0;

historyBlock* lastHistoryBlock = NULL;
historyBlock* firstHistoryBlock = NULL;
historyBlock* currentHistoryBlock = NULL;


historyBlock* deleteLastBlock(void)
{
    if(lastHistoryBlock == NULL) {
        count = 0;
        firstHistoryBlock = NULL;
        return NULL;
    }
    
    if(lastHistoryBlock->before == NULL) {
        --count;
        lastHistoryBlock->next = NULL;
        if(lastHistoryBlock->command == NULL) 
            free(lastHistoryBlock);
        if(lastHistoryBlock->command != NULL) {
            free(lastHistoryBlock->command);
            free(lastHistoryBlock);
        }
        firstHistoryBlock = NULL;
        lastHistoryBlock = NULL;
        return NULL;
    }

    
    lastHistoryBlock->next = NULL;
    lastHistoryBlock = lastHistoryBlock->before;
    lastHistoryBlock->next->before = NULL;
    
    if(lastHistoryBlock->next->command == NULL) {
        --count;
        free(lastHistoryBlock->next);
        lastHistoryBlock->next = NULL;
        return lastHistoryBlock;
    }
    else {
        free(lastHistoryBlock->next->command);
        --count;
        free(lastHistoryBlock->next);
        lastHistoryBlock->next = NULL;
        return lastHistoryBlock;
    }

}



void makeSpaceForNewHisBlock(void)
{
    while(count >= COUNTER_LIMIT)
        deleteLastBlock();
}

historyBlock* initializeNewHisBlock(char* command)
{
    makeSpaceForNewHisBlock();

    historyBlock* historyBlockToAdd = malloc(sizeof(historyBlock));
    historyBlockToAdd->before = NULL;
    historyBlockToAdd->next = firstHistoryBlock;
    historyBlockToAdd->command = command;
    historyBlockToAdd->counter = &count;

    return historyBlockToAdd;
}

char* commandToStoreInHistBlock(void)
{
    if(argv[0] == NULL)
        return NULL;

    size_t lengthOfCommand = countLengthOfCommand();
    char* command = malloc(lengthOfCommand);
    initializeCharBuffer(command, lengthOfCommand);

    for(size_t j = 0; j < lengthOfCommand && j < BUFFSIZE ; ++j) {
        if(inputWords[j] == '\0' && j != lengthOfCommand-1)
            command[j] = ' ';
        else
            command[j] = inputWords[j];
    }
    
    return command;
}


historyBlock* add(char* command)
{
    historyBlock* historyBlockToAdd = initializeNewHisBlock(command);   

    if(firstHistoryBlock == NULL) {
        firstHistoryBlock = historyBlockToAdd;
        lastHistoryBlock = firstHistoryBlock;
        currentHistoryBlock = firstHistoryBlock;
        ++count;
        return firstHistoryBlock;
    }
    else {
        firstHistoryBlock = historyBlockToAdd;
        currentHistoryBlock = firstHistoryBlock;
        firstHistoryBlock->next->before = historyBlockToAdd;
        ++count;
        return firstHistoryBlock;
    }
}


char* storeCommands(char* commandToStore)
{
    if(commandToStore == NULL)
        return NULL;
    add(commandToStore);
    currentHistoryBlock = firstHistoryBlock;
    return commandToStore;
}



void freeAllBlocks(void)
{
    if(firstHistoryBlock == NULL)
        return;

    while(count != 0) {
        
        if(firstHistoryBlock->command == NULL) {
            --count;
            if(firstHistoryBlock->next != NULL) {
                firstHistoryBlock = firstHistoryBlock->next;
                firstHistoryBlock->before->next = NULL;
                free(firstHistoryBlock->before);
                firstHistoryBlock->before = NULL;
                continue;
            }
            firstHistoryBlock->before = NULL;
            free(firstHistoryBlock);
        }
        else {
            free(firstHistoryBlock->command);
            --count;
            if(firstHistoryBlock->next == NULL)
                free(firstHistoryBlock);
            else {
            firstHistoryBlock = firstHistoryBlock->next;
            firstHistoryBlock->before->next = NULL;
            free(firstHistoryBlock->before);
            firstHistoryBlock->before = NULL;
            }
        }
    }
    firstHistoryBlock = NULL;
    lastHistoryBlock = NULL;
    currentHistoryBlock = NULL;
}
