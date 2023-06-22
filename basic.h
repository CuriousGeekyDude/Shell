#ifndef BASIC_H
#define BASIC_H

#define BUFFSIZE 4096

int argc;
size_t countList;
char inputWords[BUFFSIZE];
char* argv[BUFFSIZE];

void errorHandling(const char* stringToPrint);

void printShellSign(void);

size_t countLengthOfString(const char* string);
void initializeCharBuffer(char buffer[], size_t sizeOfBuffer);
void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer);

char* findPath_ColonSepDirectories(char* colonSepDirectories, const char* path); 


size_t countLengthOfCommand(void);

void storeInputWords(void);
size_t countArgc(void);
void initializeArgv(int Argc);
void readInput(void);

char** returnArgv(void);
size_t returnCountList(void);
int returnArgc(void);


#endif