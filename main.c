#include "basic.c"
#include <assert.h>

static size_t counting = 0;

struct CommandBlock
{
    size_t begIndex;
    size_t endIndex;
    size_t numOfStrings;
    size_t sizeOfLocalSpecialCharIndexArray;
    size_t* localSpecialCharIndexArray;
    size_t* numOfStringsInEachPipe;
    char* commandType;
};

void signalHandler(int signal)
{
    if(signal == SIGINT){
        freeAllBlocks();
        printf("\n");
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

    if(signal == SIGTSTP) {
    if(currentHistoryBlock == NULL)
        return;
    if(currentHistoryBlock->command == NULL) {
        printf("\nnull!\n");
        fflush(stdout);
    }
    else {
        printf("\n%s\n", currentHistoryBlock->command);
        fflush(stdout);
    }
    if(currentHistoryBlock->next == NULL) {
        printShellSign();
        return;
    }
    currentHistoryBlock = currentHistoryBlock->next;
    printShellSign();

    }

}

//--------------------------------------------------Test Here-------------------------------------

//----------------------------------------------GLOBAL_SPECIAL_CHAR_INDEX_ARRAY-----------------------------------------
static inline void GlobalSpecialCharCounter(void) 
{
    counting = 0;
    for(size_t i = 0; i < argc; ++i) {
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0))
            ++counting;
    }

}

static inline size_t* GlobalSpecialCharIndexArray(void) 
{

    if(counting == 0)
        return NULL;

    size_t*  SpecialCharIndexArray = calloc(counting, sizeof(size_t));
    
    size_t index_specialCharArray = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0) {
            SpecialCharIndexArray[index_specialCharArray] = i;
            ++index_specialCharArray;
        }
    }

    return SpecialCharIndexArray;
}
//----------------------------------------------GLOBAL_SPECIAL_CHAR_INDEX_ARRAY-----------------------------------------





//--------------------------------------------commandBlock.c--------------------------------------------

/*Takes care of the empty command block case.
 The array is assumed to be non-null as the null case is taken care of before this function is called.*/
static inline size_t emptyCommandBlockErrorHandling(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{
    if(sizeOfGlobalSpecialCharIndexArray < commandBlockNumber)
        return 0;

//The case where &&, ||, or ; appear at the start of the command and the block number is 0
    if(commandBlockNumber == 0 && GlobalSpecialCharIndexArray[0] == 0)
        return 0;

//The case where &&, ||, or ; appear at the end of the command and the requested block command is the last one
    if(commandBlockNumber == sizeOfGlobalSpecialCharIndexArray && GlobalSpecialCharIndexArray[sizeOfGlobalSpecialCharIndexArray-1] == argc-1)
        return 0;

//The case where block number is somewhere in the middle and the block is empty e.g && &&, || ||, or ; ;
    if(0 < commandBlockNumber && commandBlockNumber < sizeOfGlobalSpecialCharIndexArray && GlobalSpecialCharIndexArray[commandBlockNumber] == GlobalSpecialCharIndexArray[commandBlockNumber-1] + 1)
        return 0;

    return 1;
}


static inline size_t findBegIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{

    if(argc == 0 || GlobalSpecialCharIndexArray == NULL || commandBlockNumber == 0)
        return 0;

    if(emptyCommandBlockErrorHandling(GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;


    return GlobalSpecialCharIndexArray[commandBlockNumber - 1] + 1;
}

static inline size_t findEndIndexCommandBlock(size_t GlobalSpecialCharIndexArray[], size_t commandBlockNumber, size_t sizeOfGlobalSpecialCharIndexArray)
{
    if(argc == 0)
        return 0;

    if(GlobalSpecialCharIndexArray == NULL)
        return argc - 1;

    if(emptyCommandBlockErrorHandling(GlobalSpecialCharIndexArray, commandBlockNumber, sizeOfGlobalSpecialCharIndexArray) == 0)
        return 0;

    if(commandBlockNumber == sizeOfGlobalSpecialCharIndexArray)
        return argc - 1;
    else
        return GlobalSpecialCharIndexArray[commandBlockNumber] - 1;
}

static inline size_t numOfStringsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t commandBlockNumber)
{
    //The case where begIndex = endIndex and begIndex == 0 where it could be a valid command like ls or an invalid one like NULL
    if(begIndexCommandBlock == endIndexCommandBlock && begIndexCommandBlock == 0) {
        if(argc == 0)
            return 0;

        if(strcmp(argv[0], "&&") == 0 || strcmp(argv[0], "||") == 0 || strcmp(argv[0], ";") == 0)
            return 0;
        else {
            if(commandBlockNumber == 0)
                return 1;
            else
                return 0;
            }
    }
    else
        return endIndexCommandBlock - begIndexCommandBlock + 1;
}

static inline size_t numOfSpecialCharsInCommandBlock(size_t begIndexCommandBlock, size_t endIndexCommandBlock)
{
    if(begIndexCommandBlock == endIndexCommandBlock)
        return 0;

    size_t count = 0;
    for(size_t i = begIndexCommandBlock; i <= endIndexCommandBlock; ++i) {
        if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0)
            ++count;
    }
    return count;
}

size_t* numberOfStringsInEachPipe(struct CommandBlock* commandBlock)
{
    if(commandBlock->localSpecialCharIndexArray == NULL)
        return NULL;

    size_t size = commandBlock->sizeOfLocalSpecialCharIndexArray + 1;
    size_t* tempIndex = commandBlock->localSpecialCharIndexArray;
    size_t* numOfStringArray = calloc(size, sizeof(size_t));

    for(size_t i = 0; i < size; ++i) {
        if(i == 0) {
            numOfStringArray[i] = tempIndex[i] - commandBlock->begIndex;
            continue;
        }
        if(i == (size - 1)) {
            numOfStringArray[size-1] = commandBlock->endIndex - tempIndex[size-2];
            continue;
        }

        
        numOfStringArray[i] = tempIndex[i] - tempIndex[i-1] - 1;
    }
    return numOfStringArray;
}

static inline size_t* initializeLocalSpecialCharIndexArray(size_t begIndexCommandBlock, size_t endIndexCommandBlock, size_t SizeOfLocalSpecialCharIndexArray)
{
    if(SizeOfLocalSpecialCharIndexArray == 0)
        return NULL;

    size_t* LocalSpecialCharIndexArray = calloc(SizeOfLocalSpecialCharIndexArray,sizeof(size_t));

    size_t index_LocalSpecialCharIndexArray = 0;
    for(size_t i = begIndexCommandBlock; i <= endIndexCommandBlock; ++i) {
        if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0) {
            LocalSpecialCharIndexArray[index_LocalSpecialCharIndexArray] = i;
            ++index_LocalSpecialCharIndexArray;
        }
    }

    return LocalSpecialCharIndexArray;
}
static inline struct CommandBlock* constructCommandBlock(size_t* _GlobalSpecialCharIndexArray, size_t commandBlockNumber, char* CommandType)
{
    struct CommandBlock* commandBlock = calloc(1, sizeof(struct CommandBlock));

    assert(commandBlock != NULL && "calloc failed!");

    commandBlock->begIndex = findBegIndexCommandBlock(_GlobalSpecialCharIndexArray, commandBlockNumber, counting);
    commandBlock->endIndex = findEndIndexCommandBlock(_GlobalSpecialCharIndexArray, commandBlockNumber, counting);
    commandBlock->numOfStrings = numOfStringsInCommandBlock(commandBlock->begIndex, commandBlock->endIndex, commandBlockNumber);
    commandBlock->sizeOfLocalSpecialCharIndexArray = numOfSpecialCharsInCommandBlock(commandBlock->begIndex, commandBlock->endIndex);
    commandBlock->localSpecialCharIndexArray = initializeLocalSpecialCharIndexArray(commandBlock->begIndex, commandBlock->endIndex, commandBlock->sizeOfLocalSpecialCharIndexArray);
    commandBlock->numOfStringsInEachPipe = numberOfStringsInEachPipe(commandBlock);
    commandBlock->commandType = CommandType;
    return commandBlock;
}
static inline int destroyCommandBlock(struct CommandBlock* commandBlock)
{
    if(commandBlock == NULL)
        return 0;
    
    if(commandBlock->localSpecialCharIndexArray != NULL) {
        if(commandBlock->numOfStringsInEachPipe != NULL) {
            free(commandBlock->numOfStringsInEachPipe);

            free(commandBlock->localSpecialCharIndexArray);
            free(commandBlock);
        }
        else {
            free(commandBlock->localSpecialCharIndexArray);
            free(commandBlock);
        }
    }
    else
        free(commandBlock);

    commandBlock = NULL;
}

static inline void destroyArrayOfCommandBlocks(struct CommandBlock** commandBlocks, size_t sizeOfCommandBlocks)
{
    if(commandBlocks == NULL || sizeOfCommandBlocks == 0)
        return;

    for(size_t i = 0; i < sizeOfCommandBlocks; ++i) {
        destroyCommandBlock(commandBlocks[i]);
    }

    free(commandBlocks);
}

bool brokenPipeRedirection(struct CommandBlock* commandBlock)
{
    if(commandBlock == NULL)
        return false;
    if(commandBlock->numOfStringsInEachPipe == NULL)
        return false;

    for(size_t i = 0; i < commandBlock->sizeOfLocalSpecialCharIndexArray + 1; ++i) {
        if(commandBlock->numOfStringsInEachPipe[i] == 0)
            return true;
    }
    return false;
}

void parseCommandBlock(struct CommandBlock* commandBlock)
{
    size_t lastIndex = commandBlock->sizeOfLocalSpecialCharIndexArray - 1;

    if(commandBlock->begIndex == commandBlock->endIndex) {
        if(commandBlock->numOfStrings == 1) {
            size_t i = commandBlock->begIndex;

            if(strcmp(argv[i], "|") == 0 || strcmp(argv[i], ">") == 0) {
                commandBlock->commandType = "Invalid";
                return;
            }
            else {
                commandBlock->commandType = "N";
                return;
            }
        }
        else {
            commandBlock->commandType = "Invalid";
            return;
        }
    }

    switch(commandBlock->sizeOfLocalSpecialCharIndexArray) {
        case 0:
            commandBlock->commandType = "N";    //Normal command
            break;
        case 1:
            if(strcmp(argv[commandBlock->localSpecialCharIndexArray[0]], "|") == 0)
                commandBlock->commandType = "P";    //Pipeline
            else
                commandBlock->commandType = "R";    //Redirection
            break;
        default:
            if(strcmp(argv[commandBlock->localSpecialCharIndexArray[lastIndex]], ">") == 0)
                commandBlock->commandType = "P+R";  //Pipeline + Redirection at the end of command
            else
                commandBlock->commandType = "P";    //Pipeline
            break;
    }
}
static inline void printCommandBlock(struct CommandBlock* commandBlock)
{
    size_t x = commandBlock->begIndex;
    size_t y = commandBlock->endIndex;
    size_t z = commandBlock->numOfStrings;
    size_t w = commandBlock->sizeOfLocalSpecialCharIndexArray;
    char* s = commandBlock->commandType;
    size_t* array1 = commandBlock->numOfStringsInEachPipe;
    size_t* array2 = commandBlock->localSpecialCharIndexArray;

    printf("\n");
    printf("----------------------------------------------------------\n");
    printf("BegIndex: %zu\n", x);
    printf("EndIndex: %zu\n", y);
    printf("NumOfStrings: %zu\n", z);
    printf("sizeOfLocalSCharIndexArray: %zu\n", w);
    printf("CommandType: %s\n", s);
    
    printf("specCHarIndexArray: ");
    if(array1 != NULL) {
        for(size_t i = 0; i < w+1; ++i)
            printf(" %zu ", array1[i]);
        printf("\n");
    }
    else
        printf("specCHarIndexArray Is NULL!\n");


    if(array2 != NULL) {
        printf("SpecialCharIndexArray: ");
        for(size_t i = 0; i < w; ++i)
            printf(" %zu ", array2[i]);
        printf("\n");
    }
    else
        printf("SpecialCharIndexArray Is NULL!\n");
    if(brokenPipeRedirection(commandBlock) == true)
        printf("pipe or redirection is broken!\n");
    
    printf("----------------------------------------------------------\n");
    fflush(stdout);

}
//--------------------------------------------------commandBlock.c--------------------------------------




//---------------------------------------BUILT_INS---------------------------------------------------------

static inline void pwdCommand(size_t argc)
{
    if(argc > 1)
        return;
    char* PWD = getenv("PWD");
    if(PWD == NULL) {
        printf("pwd failed!\n");
        fflush(stdout);
        return;
    }
    printf("%s\n", PWD);
    fflush(stdout);   
}

void execCommand(size_t indexArgv, size_t argc)
{
    if(argc < 3) {
        if(argc == 2) {
            if(execvp(argv[indexArgv+1], NULL) == -1) {
                freeAllBlocks();
                errExit("execvp");
            }

        }
    }
        
    else {
        size_t sizeOfNewArgv = argc;
        char** newArgv = malloc(sizeOfNewArgv*(sizeof(char*)));
        initializePointerBuffer((void*)newArgv, sizeOfNewArgv);

        for(size_t i = 0; i < sizeOfNewArgv-1 && i < BUFFSIZE; ++i)
            newArgv[i] = argv[indexArgv+ i + 1];
        if(execvp(argv[indexArgv+1], newArgv) == -1) {
            freeAllBlocks();
            free(newArgv);
            newArgv = NULL;
            errExit("execvp");
        }
        free(newArgv);
        newArgv = NULL;
    }

}

char* returnUpPath(const char* currentPath) 
{
    size_t numOfLastSlash = 0, lengthOfCurrentPath = countLengthOfString(currentPath) + 1, indexOfLastSlash = 0;

    if(strcmp("", currentPath) == 0)
        return NULL;
    
    if(currentPath[0] != '/') {
        printf("The input is either a relative path or an invalid input!\n");
        fflush(stdout);
        return NULL;
    }
    if(currentPath[1] == '\0') {
        char* pathToReturn = malloc(2);
        pathToReturn[0] = '/';
        pathToReturn[1] = '\0';
        return pathToReturn;
    }

    for(size_t i = 0; currentPath[i] != '\0'; ++i) {
        if(currentPath[i] == '/')
            ++numOfLastSlash;
    }

    while(numOfLastSlash != 0) {
        if(indexOfLastSlash >= lengthOfCurrentPath) {
            printf("ERROR: indexOfLastSlash went over bound!\n");
            return NULL;
        }
        if(currentPath[indexOfLastSlash] == '/') {
            --numOfLastSlash;
            ++indexOfLastSlash;
            continue;
        }
        ++indexOfLastSlash;
    }
    if(indexOfLastSlash == 1) {
        char* newPathToReturn = malloc(indexOfLastSlash+1);
        initializeCharBuffer(newPathToReturn, indexOfLastSlash+1);
        strncpy(newPathToReturn, currentPath, indexOfLastSlash);
        return newPathToReturn;
    }
    else {
        char* newPathToReturn = malloc(indexOfLastSlash);
        initializeCharBuffer(newPathToReturn, indexOfLastSlash);
        strncpy(newPathToReturn, currentPath, indexOfLastSlash-1);
        return newPathToReturn;
    }

}

static inline void update_PWD_OLDPWD(const char* newPWD, const char* newOLDPWD)
{
    if(setenv("PWD", newPWD, 1) == -1)
        errExit("setenv() for PWD in update_PWD_OLDPWD()");
    if(setenv("OLDPWD", newOLDPWD, 1) == -1)
        errExit("setenv() for OLDPWD in update_PWD_OLDPWD()");
}

int cdCommand(size_t indexArgv, size_t argc)
{
    if(argc > 2)
        return -1;
    if(argc == 1) {
        char* newPWD = getenv("HOME");
        if(newPWD == NULL)
            return -2;

        if(chdir(newPWD) == -1)
            errExit("chdir() in cd() in NULL case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -3;
        
        update_PWD_OLDPWD(newPWD, newOLDPWD);

        return 0;
    }

    if(strcmp(argv[indexArgv+1], "..") == 0) {
        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -4;
        char* newPWD = returnUpPath(newOLDPWD);
        if(newPWD == NULL) {
            printf("PWD variable was changed during the program and so cannot be used anymore\n");
            fflush(stdout);
            free(newPWD);
            return -5;
        }
        if(chdir(newPWD) == -1) {
            free(newPWD);
            errExit("chdir() in cd() in .. case");
        }

        update_PWD_OLDPWD(newPWD, newOLDPWD);
        
        free(newPWD);
        return 0;
    }

    if(strcmp(argv[indexArgv+1], "-") == 0) {

        char* newPWD = getenv("OLDPWD");
        if(newPWD == NULL)
            return -6;

        if(chdir(newPWD) == -1) 
            errExit("chdir() in cd() - case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -7;

        printf("%s\n", newPWD);
        fflush(stdout);
        
        update_PWD_OLDPWD(newPWD, newOLDPWD);
        
        return 0;
    }
    

    //The relative path and absolute path cases are combined in one step
    char* newOLDPWD = getenv("PWD");
    if(newOLDPWD == NULL)
        return -8;
            
    if(chdir(argv[indexArgv+1]) == -1) 
        errExit("chdir() in cd() in relative/absolute path case");
                
    char* newPWD = getcwd(NULL, BUFFSIZE);
    if(newPWD == NULL)
        errExit("getcwd() in cd()");
        
    if(setenv("PWD", newPWD, 1) == -1) {
        free(newPWD);
        errExit("setenv() for PWD in cd() in relative/absolute path case");
    }
    if(setenv("OLDPWD", newOLDPWD, 1) == -1) {
        free(newPWD);
        errExit("setenv() for OLDPWD in cd() in relative/absolute path case");
    }
    printf("%s\n", newPWD);
    fflush(stdout);
    free(newPWD);
    return 0;
}


static inline void exitCommand(size_t argc)
{
    if(argc > 1)
        return;
    freeAllBlocks();
    exit(EXIT_SUCCESS);
}

//---------------------------------------BUILT_INS---------------------------------------------------------





//----------------------------------------------EXECUTE_COMMAND---------------------------------------------------------------
void exec_nonBuiltInCommand(size_t indexArgv, size_t argc)
{
    if(argc == 1) {
            if(execvp(argv[indexArgv], NULL) == -1) {
                freeAllBlocks();
                errExit("execvp");
            }

        }

        
    else {
        size_t sizeOfNewArgv = argc+1;
        char** newArgv = malloc(sizeOfNewArgv*(sizeof(char*)));
        initializePointerBuffer((void*)newArgv, sizeOfNewArgv);

        for(size_t i = 0; i < sizeOfNewArgv - 1 && i < BUFFSIZE; ++i)
            newArgv[i] = argv[indexArgv+i];
        if(execvp(argv[indexArgv], newArgv) == -1) {
            freeAllBlocks();
            free(newArgv);
            newArgv = NULL;
            errExit("execvp");
        }
    }
}
static inline pid_t nonBuiltInCommand(size_t indexArgv, size_t argc)
{
    pid_t childPID = 0;
    switch(fork()) {
        case 0:
            exec_nonBuiltInCommand(indexArgv, argc);
            break;
        default:
            childPID = wait(NULL);
            break;
    }

    return childPID;
}

pid_t executeCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork)
{
    if(strcmp(argv[begIndexOfCommand], "cd") == 0) {
        if(cdCommand(begIndexOfCommand, numOfStringsInCommand) == -1)
            return -1;
        return 0;
    }

    if(strcmp(argv[begIndexOfCommand], "exit") == 0) {
        exitCommand(numOfStringsInCommand);
        return -1;
    }

    if(strcmp(argv[begIndexOfCommand], "exec") == 0) {
        execCommand(begIndexOfCommand, numOfStringsInCommand);
        return 0;
    }
    
    if(strcmp(argv[begIndexOfCommand], "pwd") == 0) {
        pwdCommand(numOfStringsInCommand);
        return 0;
    }
        

    if(toFork == true)
        return nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);

    else
        exec_nonBuiltInCommand(begIndexOfCommand, numOfStringsInCommand);
}

//----------------------------------------------EXECUTION_COMMAND---------------------------------------------------------------



//--------------------------------------------pipe.c----------------------------------------------

pid_t pipeline(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion)
{
    if(NumberOfStringsInEachPipe == NULL)
        return -1;

    int filedes[2];

    if(pipe(filedes) == -1)
        errExit("pipe");
    
    if(dup2(filedes[0], 0) == -1)
        errExit("dup2");

    pid_t childPID = 0;
    
    switch(fork()){

        case 0:
            if(dup2(filedes[1], 1) == -1)
                errExit("dup2 again");
            close(filedes[0]);

            --baseCaseRecursion;
            if(baseCaseRecursion > 0) 
               childPID =  pipeline(BegIndexOfCommandBlock, NumberOfStringsInEachPipe, baseCaseRecursion);
            else 
               childPID = executeCommand(BegIndexOfCommandBlock, NumberOfStringsInEachPipe[0], false);
            break;

        case -1:
            errExit("fork");
            break;

        default:
            close(filedes[1]);

            childPID = wait(NULL);

            size_t i = 0;
            for(size_t j = 0; j < baseCaseRecursion ; ++j)
                i += NumberOfStringsInEachPipe[j]+1;

            childPID = executeCommand(BegIndexOfCommandBlock + i, NumberOfStringsInEachPipe[baseCaseRecursion], false);
            break;
    }
    
    return childPID;
}
pid_t start_pipeline(struct CommandBlock* commandBlock)
{
    pid_t childPID = 0;
    switch(fork()) {
        case 0:
            childPID = pipeline(commandBlock->begIndex, commandBlock->numOfStringsInEachPipe, commandBlock->sizeOfLocalSpecialCharIndexArray);
        break;

        case -1:
            errExit("fork() in start_pipeline()");
        break;

        default:
            childPID = wait(NULL);
        break;
    }
    return childPID;
}


void redirection(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommand, FILE* file)
{
    int filedes[2];

    if(pipe(filedes) == -1)
        errExit("pipe");
    
    if(dup2(filedes[0], 0) == -1)
        errExit("dup2");

    switch(fork()) {
        case 0:
            if(close(filedes[0]) == -1)
                errExit("close() in child redirection");
            if(dup2(filedes[1], 1) == -1)
                errExit("dup2()");
            executeCommand(BegIndexOfCommandBlock, numOfStringsInCommand, false);
            break;
        case -1:
            errExit("fork() in redirection()");
            break;
        default:
            if(close(filedes[1]) == -1)
                errExit("close() in redirection()");
            if(wait(NULL) == -1)
                errExit("Execution of the command block of redirection failed!");

            FILE* readEndOfPipe = fdopen(filedes[0], "r");
            if(readEndOfPipe == NULL)
                errExit("fdopen()");
            int c;
            while((c = fgetc(readEndOfPipe)) != EOF) {
                if(fputc(c, file) == EOF)
                    errExit("fputc");
            }
            if(fclose(readEndOfPipe) == -1)
                errExit("fclose() readEndOfPipe!");
            if(fclose(file) == -1)
                errExit("fclose() file!");
            exit(EXIT_SUCCESS);
            break;
    }
}
pid_t start_redirection(struct CommandBlock* commandBlock, const char* filePathName)
{

    if(filePathName == NULL || commandBlock->numOfStringsInEachPipe == NULL)
        return -1;

    FILE* file = fopen(filePathName, "a");


    if(file == NULL)
        errExit("fopen() in start_redirection()");

    pid_t childPID = 0;

    switch(fork()) {
        case 0:
            redirection(commandBlock->begIndex , commandBlock->numOfStringsInEachPipe[0], file);
            break;

        case -1:
            errExit("fork() in start_redirection()");
            break;

        default:
            childPID = wait(NULL);
            break;
    }
    return childPID;
}


//Pipeline with a file redirection at the end of it
pid_t pipelineRedirection(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion,size_t numOfPipesInCommandBlock ,FILE* file)
{

    if(NumberOfStringsInEachPipe == NULL)
        return -1;

    int filedes[2];

    if(pipe(filedes) == -1)
        errExit("pipe");
    
    if(dup2(filedes[0], 0) == -1)
        errExit("dup2");

    pid_t childPID = 0;
    
    switch(fork()){

        case 0:
            if(dup2(filedes[1], 1) == -1)
                errExit("dup2 again");
            close(filedes[0]);

            --baseCaseRecursion;
            if(baseCaseRecursion > 0) 
               childPID = pipelineRedirection(BegIndexOfCommandBlock, NumberOfStringsInEachPipe, baseCaseRecursion, numOfPipesInCommandBlock, file);
            else 
               childPID = executeCommand(BegIndexOfCommandBlock, NumberOfStringsInEachPipe[0], false);
            break;

        case -1:
            errExit("fork");
            break;

        default:
            if(close(filedes[1]) == -1)
                errExit("close() in pipeRedirection()");

            childPID = wait(NULL);

            size_t i = 0;
            for(size_t j = 0; j < baseCaseRecursion ; ++j)
                i += NumberOfStringsInEachPipe[j]+1;
            
            //File redirection at the end of pipeline happens here
            if(baseCaseRecursion == numOfPipesInCommandBlock) {

                FILE* readEndOfPipe = fdopen(filedes[0], "r");
                if(readEndOfPipe == NULL)
                    errExit("fdopen()");
                int c;
                while((c = fgetc(readEndOfPipe)) != EOF) {
                    if(fputc(c, file) == EOF)
                        errExit("fputc");
                }
                if(fclose(readEndOfPipe) == -1)
                    errExit("fclose() readEndOfPipe!");
                if(fclose(file) == -1)
                    errExit("fclose() file!");
                exit(EXIT_SUCCESS);
            }
            else
                childPID = executeCommand(BegIndexOfCommandBlock + i, NumberOfStringsInEachPipe[baseCaseRecursion], false);
            break;
    }
    
    return childPID;
}
pid_t start_pipelineRedirection(struct CommandBlock* commandBlock, const char* filePathName)
{
    if(filePathName == NULL || commandBlock->numOfStringsInEachPipe == NULL)
        return -1;

    FILE* file = fopen(filePathName, "a");


    if(file == NULL)
        errExit("fopen() in start_pipelineRedirection()");

    pid_t childPID = 0;

    switch(fork()) {
        case 0:
            pipelineRedirection(commandBlock->begIndex , commandBlock->numOfStringsInEachPipe, commandBlock->sizeOfLocalSpecialCharIndexArray, commandBlock->sizeOfLocalSpecialCharIndexArray, file);
            break;

        case -1:
            errExit("fork() in start_pipelineRedirection()");
            break;

        default:
            childPID = wait(NULL);

            if(fclose(file) == -1)
                errExit("fclose() in start_pipelineRedirection()");
            break;
    }
    return childPID;
}

//--------------------------------------------pipe.c----------------------------------------------




//------------------------------------------EXECUTE_COMMAND_BLOCK--------------------------------------------
pid_t executeCommandBlock(struct CommandBlock* commandBlock)
{
    pid_t childPID = -1;

    if(commandBlock->commandType == NULL || strcmp(commandBlock->commandType, "Invalid") == 0)
        return -1;

    if(strcmp(commandBlock->commandType, "N") == 0) {
        childPID =  executeCommand(commandBlock->begIndex, commandBlock->numOfStrings, true);   
        return childPID;
    }    

    if(brokenPipeRedirection(commandBlock) == true)
        return childPID;
    else {
        if(strcmp(commandBlock->commandType, "P") == 0) {         
            childPID =  start_pipeline(commandBlock);
            return childPID;
        }
    
        if(strcmp(commandBlock->commandType, "R") == 0) {
            childPID =  start_redirection(commandBlock, argv[commandBlock->localSpecialCharIndexArray[0] + 1]);
            return childPID;
        }

        if(strcmp(commandBlock->commandType, "P+R") == 0) {
            childPID = start_pipelineRedirection(commandBlock, argv[commandBlock->localSpecialCharIndexArray[commandBlock->sizeOfLocalSpecialCharIndexArray - 1] + 1]);
            return childPID;
        }
    }
    
}

//------------------------------------------EXECUTE_COMMAND_BLOCK---------------------------------------------



//------------------------------------------------------LIST---------------------------------------------

pid_t andList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(counting == 0 || waitReturn < 0)
        return -1;
    
    return executeCommandBlock(commandBlock);

}

pid_t orList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(counting == 0 || waitReturn >= 0)
        return -1;
    
    return executeCommandBlock(commandBlock);
}

static inline pid_t semicolonList(struct CommandBlock* commandBlock, pid_t waitReturn)
{
    if(waitReturn == -1)
        return -1;
    return executeCommandBlock(commandBlock);
}

//-------------------------------------------------------LIST-----------------------------------------------------



void start_shell(void)
{
    char* command = read_storeCommands();
    if(command == NULL)
        return;

    pid_t childPID = 0;
    GlobalSpecialCharCounter();

    size_t* globalSpecialCharIndexArray = GlobalSpecialCharIndexArray();

    struct CommandBlock** commandBlockArray = calloc(counting+1, sizeof(struct CommandBlock*));

    for(size_t i = 0; i < counting+1; ++i) {
        commandBlockArray[i] = constructCommandBlock(globalSpecialCharIndexArray, i, NULL);
        parseCommandBlock(commandBlockArray[i]);
    
    }

    if(globalSpecialCharIndexArray != NULL) {
        
        childPID = executeCommandBlock(commandBlockArray[0]);

        for(size_t i = 0; i < counting; ++i) {

            if(strcmp(argv[globalSpecialCharIndexArray[i]], "&&") == 0) {
                   if(childPID == -1)
                        break;
                   childPID =  andList(commandBlockArray[i+1], childPID);
                   continue;
                }

            if(strcmp(argv[globalSpecialCharIndexArray[i]], "||") == 0) {
                    if(childPID == -1)
                        break;
                    childPID = orList(commandBlockArray[i+1], childPID);
                    continue;
            }
            if(strcmp(argv[globalSpecialCharIndexArray[i]], ";") == 0) {
                    if(childPID == -1)
                        break;
                    childPID  = semicolonList(commandBlockArray[i+1], childPID);
            }
        }
        
    }

    else 
        executeCommandBlock(commandBlockArray[0]);
    
        
    free(globalSpecialCharIndexArray);
    destroyArrayOfCommandBlocks(commandBlockArray, counting + 1);
}




int main(void)
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &signalHandler;
    sig.sa_flags = SA_RESTART;

    sigaction(SIGTSTP, &sig, NULL);
    sigaction(SIGINT, &sig, NULL);

    while(1) {
        start_shell();
    }

    freeAllBlocks();
    exit(EXIT_SUCCESS);
}