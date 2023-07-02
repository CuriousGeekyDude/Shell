#ifndef BASIC_H
#define BASIC_H

#define BUFFSIZE 4096


extern int argc;
extern size_t countList;
extern char inputWords[BUFFSIZE];
extern char* argv[BUFFSIZE];


void errorHandling(const char* stringToPrint);


size_t countLengthOfString(const char* string);
void initializeCharBuffer(char buffer[], size_t sizeOfBuffer);
void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer);



size_t countLengthOfCommand(void);

void storeInputWords(void);
void countArgc(void);
void initializeArgv(int Argc);
void readInput(void);



#endif