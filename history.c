#include "history.h"

historyBlock* deleteLastBlock()
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

historyBlock* add(char* command)
{
    while(count >= COUNTER_LIMIT)
        deleteLastBlock();

    historyBlock* historyBlockToAdd = malloc(sizeof(historyBlock));
    historyBlockToAdd->before = NULL;
    historyBlockToAdd->next = firstHistoryBlock;
    historyBlockToAdd->command = command;
    historyBlockToAdd->counter = &count;
    ++count;

    if(firstHistoryBlock == NULL) {
        firstHistoryBlock = historyBlockToAdd;
        lastHistoryBlock = firstHistoryBlock;
        currentHistoryBlock = firstHistoryBlock;
        return firstHistoryBlock;
    }
    firstHistoryBlock = historyBlockToAdd;
    currentHistoryBlock = firstHistoryBlock;
    firstHistoryBlock->next->before = historyBlockToAdd;

    return firstHistoryBlock;
}

char* showPreviousCommand()
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

void freeAllBlocks()
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
