#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <basic.h>
#include <history.h>
#include <error_functions.h>
#include <builtInC.h>
#include <executeCommand.h>


size_t numOfAllTheSlashesInPath(const char* currentPath)
{
    size_t numOfAllTheSlashes = 0;
    for(size_t i = 0; currentPath[i] != '\0'; ++i) {
        if(currentPath[i] == '/')
            ++numOfAllTheSlashes;
    }
    return numOfAllTheSlashes;
}

int findIndexOfLastSlash(const char* currentPath)
{
    size_t indexOfLastSlash = 0;
    size_t numOfAllTheSlashes = numOfAllTheSlashesInPath(currentPath);
    size_t lengthOfCurrentPath = countLengthOfString(currentPath) + 1;

    while(numOfAllTheSlashes != 0) {
        if(indexOfLastSlash >= lengthOfCurrentPath) {
            printf("ERROR: indexOfLastSlash went over bound!\n");
            return -1;
        }
        if(currentPath[indexOfLastSlash] == '/') {
            --numOfAllTheSlashes;
            ++indexOfLastSlash;
            continue;
        }
        ++indexOfLastSlash;
    }

    return indexOfLastSlash;

}

char* returnUpPath(const char* currentPath) 
{
    size_t  indexOfLastSlash = 0;

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
  
    indexOfLastSlash = findIndexOfLastSlash(currentPath);

    if(indexOfLastSlash == -1)
        return NULL;
    
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

void update_PWD_OLDPWD(const char* newPWD, const char* newOLDPWD)
{
    if(setenv("PWD", newPWD, 1) == -1)
        errExit("setenv() for PWD in update_PWD_OLDPWD()");
    if(setenv("OLDPWD", newOLDPWD, 1) == -1)
        errExit("setenv() for OLDPWD in update_PWD_OLDPWD()");
}

int cdCommand(size_t indexArgv)
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



void run_execvp(size_t indexArgv, char** newArgv)
{
    if(newArgv == NULL) {
        if(execlp(argv[indexArgv+1], argv[indexArgv+1], NULL) == -1) {
            freeAllBlocks();
            errExit("execlp");
        }
    }
    else {
        if(execvp(argv[indexArgv+1], newArgv) == -1) {
            freeAllBlocks();
            free(newArgv);
            newArgv = NULL;
            errExit("execvp");
        }
    }
}
void execCommand(size_t indexArgv)
{
    if(argc == 0 || argc == 1)
        return;

    if(argc == 2) {
        run_execvp(indexArgv, NULL);
    }
        
    else {
        size_t sizeOfNewArgv = argc;
        char** newArgv = malloc(sizeOfNewArgv*(sizeof(char*)));
        initializePointerBuffer((void*)newArgv, sizeOfNewArgv);

        for(size_t i = 0; i < sizeOfNewArgv-1 && i < BUFFSIZE; ++i)
            newArgv[i] = argv[indexArgv+ i + 1];

        run_execvp(indexArgv, newArgv);
    }

}



int executable()
{
    if(argc <= 2) {
        return -1;
    }
    else {
        executeCommand(2, 1, "N", true);
    }
}



void pwdCommand(int argc)
{
    if(argc > 1)
        return;

    char* PWD = getenv("PWD");

    if(PWD == NULL) {
        printf("PWD does not exit!\n");
        fflush(stdout);
        return;
    }

    printf("%s\n", PWD);
    fflush(stdout);   
}





void exitCommand(int argc)
{
    if(argc > 1)
        return;

    freeAllBlocks();
    exit(EXIT_SUCCESS);
}