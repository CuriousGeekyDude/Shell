#ifndef PIPE_H
#define PIPE_H

#include <stdio.h>
#include <sys/types.h>
#include "commandBlock.h"

pid_t pipeline(char** argv, size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion);
pid_t start_pipeline(struct CommandBlock* commandBlock, char** argv);

void readToFileFromPipe(int fd_readEndOfPipe, FILE* file);

void redirection(char** argv, size_t BegIndexOfCommandBlock, size_t numOfStringsInCommand, FILE* file);
pid_t start_redirection(struct CommandBlock* commandBlock, const char* filePathName, char** argv);

pid_t pipelineRedirection(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion,size_t numOfPipesInCommandBlock ,FILE* file);
pid_t start_pipelineRedirection(struct CommandBlock* commandBlock, const char* filePathName);


#endif