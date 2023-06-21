#include "history.h"

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


char* storeCommands(const char* commandToStore)
{
    add(commandToStore);
    currentHistoryBlock = firstHistoryBlock;
    return commandToStore;
}


char* showPreviousCommand(void)
{
    if(currentHistoryBlock == NULL)
        return NULL;
    char* command = currentHistoryBlock->command;

    if(currentHistoryBlock->next == NULL) {
        return command;
    }

    currentHistoryBlock = currentHistoryBlock->next;
    return command;

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
