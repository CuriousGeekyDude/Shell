#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <color.h>
#include <basic.h>

int argc;
size_t countList;
char inputWords[BUFFSIZE];
char* argv[BUFFSIZE];

void errorHandling(const char* stringToPrint)
{
    printf("%s", stringToPrint);
    fflush(stdout);
    exit(EXIT_FAILURE);
}



size_t countLengthOfString(const char* string)
{
    size_t lengthOfString = 0;
    if(string == NULL)
        return lengthOfString;
    else {
        for(size_t i = 0; string[i] != '\0'; ++i)
            ++lengthOfString;
        return lengthOfString;
    }
}

void initializeCharBuffer(char buffer[], size_t sizeOfBuffer)
{
    for(size_t i = 0; i < sizeOfBuffer; ++i)
        buffer[i] = '\0';
}

void initializePointerBuffer(void* buffer[], size_t sizeOfBuffer)
{
    for(size_t i = 0; i < sizeOfBuffer; ++i)
        buffer[i] = NULL;
}



size_t countLengthOfCommand(void)
{
    size_t lengthOfCommand = 0;
    for(size_t j = 0; j < argc; ++j) 
        lengthOfCommand += (countLengthOfString(argv[j])+1);
    return lengthOfCommand;
}



void storeInputWords(void)
{
    size_t spaceCount = 0, loopCount = 1, index_inputWords = 0;
    char c;
    while((c = fgetc(stdin)) != '\n') {

        if(index_inputWords < BUFFSIZE - 1) {

    //loopCounter is only used to eliminate all the spaces at the beginning of input when c
    //has not yet had the chance to be any other character except space. After it is
    //incremented once, it outlives its usefulness COMPLETELY.
            if(c == ' ') {
                if(loopCount == 1)
                    continue;
                ++spaceCount;
                if(spaceCount < 2) {
                    inputWords[index_inputWords] = c;
                    ++index_inputWords;
                }
                else
                    continue;
            }
            else {
                inputWords[index_inputWords] = c;
                ++index_inputWords;
                spaceCount = 0;
                ++loopCount;
            }
        }
        else
            return;
    }
}

//Counts words seperated by space in inputWords[]
void countArgc(void)
{
    size_t index_inputWords = 0;
    argc = 0;
    while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] != '\0') {
        if(inputWords[index_inputWords] == ' ')
            ++index_inputWords;
        else {
            while(inputWords[index_inputWords] != ' ' && inputWords[index_inputWords] != '\0') 
                ++index_inputWords;
            ++argc;
        }
    }
}

int nullifyAllSpacesOfInputWords(void)
{
    //Making sure that inputWords[] does contain at least one non-null char before traversing it
    if(inputWords[0] != '\0') {
        for(size_t i = 0; i < BUFFSIZE; ++i) {
            if(inputWords[i] == ' ')
                inputWords[i] = '\0';
        }
        return 0;
    }
    else
        return -1;
}

//Make pointers in argv[] point to the beginning characters
//of their respective words in inputWords[].
void initializeArgv(int Argc)
{
    size_t index_argv = 0, index_inputWords = 0;

    if(nullifyAllSpacesOfInputWords() == -1)
        return;

    argv[0] = &inputWords[0];
    while(Argc != 0) {

        //Traverse non-empty word to reach the first null character that comes immediately after the word
        while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] != '\0')
            ++index_inputWords;
        
        if(index_inputWords >= BUFFSIZE)
            return;

        //Traverse null chars to reach the first non-null in order to point argv[index_argv] to it
        while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] == '\0')
            ++index_inputWords;

        if(index_inputWords >= BUFFSIZE)
            return;

        ++index_argv;
        argv[index_argv] = &inputWords[index_inputWords];
        --Argc;
    }

}

void readInput(void)
{
    initializePointerBuffer((void*)argv, BUFFSIZE);

 // storeInputWords();
    countArgc();
    initializeArgv(argc);
}


/*char* read_storeCommands(void)
{
    readInput();
    char* command = commandToStoreInHistBlock();
    add(command);
    currentHistoryBlock = firstHistoryBlock;
    return command;
}*/

