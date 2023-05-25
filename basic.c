#include "basic.h"

static inline void errorHandling(const char* stringToPrint)
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

void readInput()

{
    char singleCharRead[1] = {'\0'};
    size_t index_inputWords = 0;
    int numRead;

    argc = 0;
    initializeCharBuffer(inputWords, BUFFSIZE);
    initializePointerBuffer((void*)argv, BUFFSIZE);

    const char* pwd = getenv("PWD");
    printf("sh$%s:", pwd);
    fflush(stdout);
    
    //Get all the input from the user into inputWords[], eliminating any consecutive 
    //spaces,except the first space, until a non-space character is found.
    int spaceCounter = 0;
    while((numRead = read(STDIN_FILENO, singleCharRead, 1)) > 0) {
        if(singleCharRead[0] == '\n') {
            index_inputWords = 0;   //Restart the index for the next while loop 
            break;
        }
        else {
            if(index_inputWords < BUFFSIZE) {
                if(singleCharRead[0] == ' ') {
                    ++spaceCounter;
                    if(spaceCounter < 2) {
                        inputWords[index_inputWords] = singleCharRead[0];
                        ++index_inputWords;
                    }
                    else
                        continue;
                }
                else {
                    inputWords[index_inputWords] = singleCharRead[0];
                    ++index_inputWords;
                    spaceCounter = 0;
                }
            }
            else
                errorHandling("ERROR: index_inputWords went out of bounds in readInput()!\n");
        }
    }

    if(numRead == -1)
        errExit("read() in readInput()");

    //Counts words seperated by space in inputWords[]
    while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] != '\0') {
        if(inputWords[index_inputWords] == ' ')
            ++index_inputWords;

        else {
            while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] != ' ' && inputWords[index_inputWords] != '\0') 
                ++index_inputWords;
            ++argc;
        }
    }

    if(index_inputWords >= BUFFSIZE)
        errorHandling("ERROR: index_inputWords went out of bounds in readInput() while trying to count the words of inputWords[]!\n");
    


    //Make pointers in argv[] point to the beginning characters
    //of their respective words in inputWords[].
    int argcDummy = argc; 
    size_t index_argv = 0;
    index_inputWords = 0;
    while(argcDummy != 0) {

        while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] == ' ')
            ++index_inputWords;

        if(index_inputWords >= BUFFSIZE)
            errorHandling("ERROR1: index_inputWords went out of bounds in readInput() while trying to point the pointers of argv[] to their words!\n");
        
        argv[index_argv] = &inputWords[index_inputWords];

        while(index_inputWords < BUFFSIZE && inputWords[index_inputWords] != ' ' && inputWords[index_inputWords] != '\0')
            ++index_inputWords;
        
        if(index_inputWords >= BUFFSIZE)
            errorHandling("ERROR2: index_inputWords went out of bounds in readInput() while trying to point the pointers of argv[] to their words!\n");

        inputWords[index_inputWords] = '\0';
        ++index_inputWords;
        ++index_argv;
        --argcDummy;
    }

    
}

