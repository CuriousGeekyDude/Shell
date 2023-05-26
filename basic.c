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

char* findPath_ColonSepDirectories(char* colonSepDirectories, const char* path)
{
    size_t numOfColons = 0, length_ColonsepDirectories = countLengthOfString(colonSepDirectories) + 1;


    if(colonSepDirectories == NULL || path == NULL)
        return NULL;

    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        if(colonSepDirectories[i] == ':')
            ++numOfColons;
    }

    if(numOfColons == 0) {
        if(strcmp(colonSepDirectories, path) == 0) {
            char* pathCopyToReturn = malloc(length_ColonsepDirectories);
            initializeCharBuffer(pathCopyToReturn, length_ColonsepDirectories);
            strncpy(pathCopyToReturn, colonSepDirectories, length_ColonsepDirectories);            
            return pathCopyToReturn;
        }
        else
            return NULL;
    }

    ++numOfColons; //The number of directories seperated by colon

    char** pointerToDirectories = malloc((numOfColons + 1) * (sizeof(char*)));
    for(size_t i = 0; i < (numOfColons+1); ++i)
        pointerToDirectories[i] = NULL;


    size_t* lengthOfEachDirectory = malloc(numOfColons*sizeof(size_t));
    for(size_t i = 0; i < numOfColons; ++i)
        lengthOfEachDirectory[i] = 0;


    //Finding the length of each directory seperated by colon
    size_t index_lengthOfEachDirectory = 0;
    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        size_t lengthOfDirectory = 0;

        while(colonSepDirectories[i] != ':' && colonSepDirectories[i] != '\0') {
            ++lengthOfDirectory;
            ++i;
        }
        if(index_lengthOfEachDirectory < numOfColons) {
            lengthOfEachDirectory[index_lengthOfEachDirectory] = lengthOfDirectory;
            ++index_lengthOfEachDirectory;
        }
    }

    char* directories = malloc(length_ColonsepDirectories*sizeof(char));
    initializeCharBuffer(directories, length_ColonsepDirectories);

    index_lengthOfEachDirectory = 0;
    for(size_t i = 0; i < length_ColonsepDirectories; ++i) {
        if(index_lengthOfEachDirectory < numOfColons)
            pointerToDirectories[index_lengthOfEachDirectory] = &directories[i];

        for(size_t j = 0; j < lengthOfEachDirectory[index_lengthOfEachDirectory]; ++j) {
            directories[i] = colonSepDirectories[i];
            ++i;
        }
        ++index_lengthOfEachDirectory;
        directories[i] = '\0';
    }
    free(lengthOfEachDirectory);
    
    for(size_t i = 0; pointerToDirectories[i] != NULL; ++i) {
        if(strcmp(pointerToDirectories[i], path) == 0) {
            size_t lengthOfPathCopyToReturn = countLengthOfString(pointerToDirectories[i]);
            char* pathCopyToReturn = malloc(lengthOfPathCopyToReturn + 1);
            initializeCharBuffer(pathCopyToReturn, lengthOfPathCopyToReturn+1);
            strncpy(pathCopyToReturn, pointerToDirectories[i], lengthOfPathCopyToReturn);
            free(directories);
            free(pointerToDirectories);
            return pathCopyToReturn;
        }
    }

    free(directories);
    free(pointerToDirectories);
    return NULL;
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

