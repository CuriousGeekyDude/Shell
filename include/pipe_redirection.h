#ifndef PIPE_H
#define PIPE_H

#include <stdio.h>
#include <sys/types.h> 
#include "commandBlock.h"

pid_t pipeline(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion, const char* commandType);
pid_t start_pipeline(struct CommandBlock* commandBlock);

void readToFileFromPipe(int fd_readEndOfPipe, FILE* file);

void redirection(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommand, const char* commandType,FILE* file);
pid_t start_redirection(struct CommandBlock* commandBlock, const char* filePathName);

pid_t pipelineRedirection(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion,size_t numOfPipesInCommandBlock , const char* commandType, FILE* file);
pid_t start_pipelineRedirection(struct CommandBlock* commandBlock, const char* filePathName);


#endif