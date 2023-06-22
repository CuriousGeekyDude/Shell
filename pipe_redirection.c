
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include "executeCommand.h"
#include "error_functions.h"
#include "pipe_redirection.h"



pid_t pipeline(char** argv,size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion)
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
               childPID = executeCommand(argv,BegIndexOfCommandBlock, NumberOfStringsInEachPipe[0], false);
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

            childPID = executeCommand(argv, BegIndexOfCommandBlock + i, NumberOfStringsInEachPipe[baseCaseRecursion], false);
            break;
    }
    
    return childPID;
}
pid_t start_pipeline(struct CommandBlock* commandBlock, char** argv)
{
    pid_t childPID = 0;
    switch(fork()) {
        case 0:
            childPID = pipeline(argv,commandBlock->begIndex, commandBlock->numOfStringsInEachPipe, commandBlock->sizeOfLocalSpecialCharIndexArray);
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


void readToFileFromPipe(int fd_readEndOfPipe, FILE* file)
{
    if(file == NULL)
        errExit("file is NULL in readToFileFromPipe()");
    FILE* readEndOfPipe = fdopen(fd_readEndOfPipe , "r");
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

            readToFileFromPipe(filedes[0], file);
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
                readToFileFromPipe(filedes[0], file);
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