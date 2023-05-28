#ifndef HISTORY_H
#define HISTORY_H

#include <malloc.h>
#define COUNTER_LIMIT 100

static int count = 0;

typedef struct historyBlock{
    char* command;
    int* counter;   //Points to count
    struct historyBlock* next;
    struct historyBlock* before;
} historyBlock;

historyBlock* lastHistoryBlock = NULL;
historyBlock* firstHistoryBlock = NULL;
historyBlock* currentHistoryBlock = NULL;

historyBlock* deleteLastBlock();
historyBlock* add(char* command);
char* showPreviousCommand();
void freeAllBlocks();


#endif