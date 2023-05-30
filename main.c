#include "basic.c"
void signalHandler(int signal)
{
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

//--------------------------------------------------Test Here-------------------------------------

void pwdCommand()
{
    char* PWD = getenv("PWD");
    printf("%s\n", PWD);
    fflush(stdout);   
}

void execCommand()
{
    if(argc < 3) {
        if(argc == 2) {
            if(execvp(argv[1], NULL) == -1) {
                freeAllBlocks();
                errExit("execvp");
            }

        }
    }
        
    else {
        size_t sizeOfNewArgv = argc;
        char** newArgv = malloc(sizeOfNewArgv*(sizeof(char*)));
        initializePointerBuffer((void*)newArgv, sizeOfNewArgv);

        for(size_t i = 0; i < sizeOfNewArgv - 1 && i < BUFFSIZE; ++i)
            newArgv[i] = argv[i+1];
        if(execvp(argv[1], newArgv) == -1) {
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

int cdCommand(char* argv[])
{
    if(argv[1] == NULL) {
        char* newPWD = getenv("HOME");
        if(newPWD == NULL)
            return -1;

        if(chdir(newPWD) == -1)
            errExit("chdir() in cd() in NULL case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -2;
        
        update_PWD_OLDPWD(newPWD, newOLDPWD);

        return 0;
    }

    if(strcmp(argv[1], "..") == 0) {
        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -3;
        char* newPWD = returnUpPath(newOLDPWD);
        if(newPWD == NULL) {
            printf("PWD variable was changed during the program and so cannot be used anymore\n");
            fflush(stdout);
            free(newPWD);
            return -4;
        }
        if(chdir(newPWD) == -1) {
            free(newPWD);
            errExit("chdir() in cd() in .. case");
        }

        update_PWD_OLDPWD(newPWD, newOLDPWD);
        
        free(newPWD);
        return 0;
    }

    if(strcmp(argv[1], "-") == 0) {

        char* newPWD = getenv("OLDPWD");
        if(newPWD == NULL)
            return -5;

        if(chdir(newPWD) == -1) 
            errExit("chdir() in cd() - case");

        char* newOLDPWD = getenv("PWD");
        if(newOLDPWD == NULL)
            return -6;

        printf("%s\n", newPWD);
        fflush(stdout);
        
        update_PWD_OLDPWD(newPWD, newOLDPWD);
        
        return 0;
    }
    

    //The relative path and absolute path cases are combined in one step
    char* newOLDPWD = getenv("PWD");
    if(newOLDPWD == NULL)
        return -7;
            
    if(chdir(argv[1]) == -1) 
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

void exitCommand()
{
    freeAllBlocks();
    exit(EXIT_SUCCESS);
}



//--------------------------------------------------Test End--------------------------------------




int main(void)
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &signalHandler;
    sig.sa_flags = SA_RESTART;

    sigaction(SIGTSTP, &sig, NULL);


    while(1) {
        char* command = read_storeCommands();
        if(strcmp(argv[0], "cd") == 0)
            cdCommand(argv);
        if(strcmp(argv[0], "exit") == 0)
            exitCommand();
        if(strcmp(argv[0], "exec") == 0)
            execCommand();
        if(strcmp(argv[0], "pwd") == 0)
            pwdCommand();
        
    }

    freeAllBlocks();
    exit(EXIT_SUCCESS);
}