#ifndef HISTORY_H
#define HISTORY_H

#define COUNTER_LIMIT 100   //Max number of commands to store in history blocks

extern int count;

typedef struct historyBlock{
    char* command;
    int* counter;   //Points to count
    struct historyBlock* next;
    struct historyBlock* before;
} historyBlock;

extern historyBlock* lastHistoryBlock;
extern historyBlock* firstHistoryBlock;
extern historyBlock* currentHistoryBlock;

historyBlock* deleteLastBlock(void);

void makeSpaceForNewHisBlock(void);
historyBlock* initializeNewHisBlock(char* command);
char* commandToStoreInHistBlock(void);
historyBlock* add(char* command);

char* storeCommands(char* commandToStore);

void freeAllBlocks(void);


#endif