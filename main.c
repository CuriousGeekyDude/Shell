#include "basic.c"
#include <assert.h>

static size_t counting = 0;


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

static inline void pwdCommand(size_t argc)
{
    if(argc > 1)
        return;
    char* PWD = getenv("PWD");
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

static inline void specialCharCounter()
{
    counting = 0;
    for(size_t i = 0; i < argc; ++i) {
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0) || (strcmp(argv[i], "|") == 0))
            ++counting;
    }

}

size_t* specialCharIndexArray()
{

    if(counting == 0)
        return NULL;

    size_t*  SpecialCharIndexArray = calloc(counting, sizeof(size_t));
    
    size_t index_specialCharArray = 0;
    for(size_t i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "||") == 0 || strcmp(argv[i], "&&") == 0 || strcmp(argv[i], ";") == 0 || strcmp(argv[i], "|") == 0) {
            SpecialCharIndexArray[index_specialCharArray] = i;
            ++index_specialCharArray;
        }
    }

    return SpecialCharIndexArray;
}

size_t* numberOfStringsInEachBlock(size_t* SpecialCharIndexArray)
{
    if(counting == 0 || SpecialCharIndexArray == NULL)
        return NULL;

    size_t* numOfStringArray = calloc(counting+1, sizeof(size_t));

    for(size_t i = 0; i < counting+1; ++i) {
        if(i == 0) {
            numOfStringArray[i] = SpecialCharIndexArray[i];
            continue;
        }
        if(i == counting) {
            numOfStringArray[counting] = (argc-1) - SpecialCharIndexArray[counting-1];
            continue;
        }

        
        numOfStringArray[i] = SpecialCharIndexArray[i] - SpecialCharIndexArray[i-1] - 1;
    }
    return numOfStringArray;
}

pid_t executeBlockCommand(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, bool toFork)
{
    if(strcmp(argv[BegIndexOfCommandBlock], "cd") == 0) {
            cdCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "exit") == 0) {
            exitCommand(numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "exec") == 0) {
            execCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
            return 0;
        }
        if(strcmp(argv[BegIndexOfCommandBlock], "pwd") == 0) {
            pwdCommand(numOfStringsInCommandBlock);
            return 0;
        }

    if(toFork == true)
        return nonBuiltInCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);

    else
        exec_nonBuiltInCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock);
}
pid_t andList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, pid_t waitReturn)
{
    if(counting == 0 || waitReturn < 0)
        return -1;
    
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);

}

pid_t orList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, pid_t waitReturn)
{
    if(counting == 0 || waitReturn >= 0)
        return -1;
    
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);
}

static inline pid_t semicolonList(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock)
{
    return executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, true);
}


int findNextPipeSignIndex(size_t specialCharIndexArray[],size_t currentPipeSignIndex)
{

    size_t index = -1;

    for(size_t i = currentPipeSignIndex; i < counting; ++i) {
        if(strcmp("|", argv[specialCharIndexArray[i]]) == 0) {
            index = specialCharIndexArray[i];
            return index;
        }
    }

    return index;
}

size_t numOfPipesInEachBlockOfPipeline(size_t SpecialCharIndexArray[], size_t IndexToBeginFrom)
{
    size_t count = 1;
    for(size_t i = IndexToBeginFrom+1; i < argc; ++i) {
        if(strcmp("|", argv[i]) == 0) {
            ++count;
            continue;
        }
        if((strcmp(argv[i], "||") == 0 ) || (strcmp(argv[i], "&&") == 0) || (strcmp(argv[i], ";") == 0))
            return count;
    }
    return count;
}

pid_t pipeline(size_t BegIndexOfCommandBlock, size_t numOfStringsInEachCommandBlock[], size_t index, int counterPipes)
{
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
            --counterPipes;
            if(counterPipes > 0) {
               childPID =  pipeline(BegIndexOfCommandBlock, numOfStringsInEachCommandBlock, index, counterPipes);
            }
            else {
               childPID = executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInEachCommandBlock[index], false);
            }
            break;

        case -1:
            errExit("fork");
            break;

        default:
            close(filedes[1]);
            childPID = wait(NULL);
            size_t i = 0;
            for(size_t j = 0; j < counterPipes ; ++j) {
                    i += numOfStringsInEachCommandBlock[j]+1;
              }
            childPID = executeBlockCommand(BegIndexOfCommandBlock + i, numOfStringsInEachCommandBlock[index+counterPipes], false);
            break;
    }
    
    return childPID;
}

pid_t start_pipeline(size_t BegIndexOfCommandBlock, size_t numOfStringsInEachCommandBlock[], size_t index, int counterPipes)
{
    pid_t childPID = 0;
    switch(fork()) {
        case 0:
            childPID = pipeline(BegIndexOfCommandBlock, numOfStringsInEachCommandBlock, index, counterPipes);
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

void redirection(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, FILE* file)
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
            executeBlockCommand(BegIndexOfCommandBlock, numOfStringsInCommandBlock, false);
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

pid_t start_redirection(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommandBlock, const char* filePathName)
{

    if(filePathName == NULL)
        exit(EXIT_FAILURE);

    FILE* file = fopen(filePathName, "a");


    if(file == NULL)
        errExit("fopen() in start_redirection()");

    pid_t childPID = 0;

    switch(fork()) {
        case 0:
            redirection(BegIndexOfCommandBlock, numOfStringsInCommandBlock, file);
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

void start_shell()
{
    char* command = read_storeCommands();
    if(command == NULL)
        return;
    int firstPipeSignIndex = 0;
    pid_t childPID = 0;
    specialCharCounter();

    size_t* indexArray = specialCharIndexArray();
    size_t* numOfStringArray = numberOfStringsInEachBlock(indexArray);
        
    
    firstPipeSignIndex = findNextPipeSignIndex(indexArray,0);
    size_t numOfPipesInBlockofPipeline = numOfPipesInEachBlockOfPipeline(indexArray, firstPipeSignIndex);

    if(numOfStringArray != NULL) {
        
        size_t i;   //used in the next for loop

        if(firstPipeSignIndex == indexArray[0]) {
            childPID = start_pipeline(0, numOfStringArray, 0, numOfPipesInBlockofPipeline);
            if(numOfPipesInBlockofPipeline == counting)
                return;
            else {
                i = numOfPipesInBlockofPipeline;
                firstPipeSignIndex = findNextPipeSignIndex(indexArray, i);
            }
        }
        else {
                childPID = executeBlockCommand(0, numOfStringArray[0], true);
                i = 0;
            }

        
        for(; i < counting; ++i) {
            
            if(firstPipeSignIndex == indexArray[i]) {
                numOfPipesInBlockofPipeline = numOfPipesInEachBlockOfPipeline(indexArray, firstPipeSignIndex);
                childPID = start_pipeline(indexArray[i-1] + 1, numOfStringArray, i, numOfPipesInBlockofPipeline);
                i += (numOfPipesInBlockofPipeline-1);
                firstPipeSignIndex = findNextPipeSignIndex(indexArray, i+1);
                continue;
            }

            if(strcmp(argv[indexArray[i]], "&&") == 0) {

                if(numOfStringArray[i+1] != 0 && indexArray[i+1] != firstPipeSignIndex)
                   childPID =  andList(indexArray[i] + 1, numOfStringArray[i+1], childPID);
                else
                    continue;
                }
            if(strcmp(argv[indexArray[i]], "||") == 0) {

                if(numOfStringArray[i+1] != 0 && indexArray[i+1] != firstPipeSignIndex)
                    childPID = orList(indexArray[i] + 1, numOfStringArray[i+1], childPID);
                else
                    continue;
            }
            if(strcmp(argv[indexArray[i]], ";") == 0) {
                if(numOfStringArray[i+1] != 0 && indexArray[i+1] != firstPipeSignIndex)
                    childPID  = semicolonList(indexArray[i] + 1, numOfStringArray[i+1]);
                
            }

        }

        free(indexArray);
        free(numOfStringArray);
    }

    else
        executeBlockCommand(0, argc, true);
        
    
}

//--------------------------------------------------Test End--------------------------------------


int main(void)
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &signalHandler;
    sig.sa_flags = SA_RESTART;

    sigaction(SIGTSTP, &sig, NULL);
    sigaction(SIGINT, &sig, NULL);

    while(1) {
        read_storeCommands();
        start_redirection(0, 2, argv[3]);
        
        
    }

    freeAllBlocks();
    exit(EXIT_SUCCESS);
}