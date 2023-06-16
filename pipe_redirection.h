#ifndef PIPE_H
#define PIPE_H

#include "executeCommandBlock.h"

pid_t pipeline(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion);
pid_t start_pipeline(struct CommandBlock* commandBlock);

void readToFileFromPipe(int fd_readEndOfPipe, FILE* file);

void redirection(size_t BegIndexOfCommandBlock, size_t numOfStringsInCommand, FILE* file);
pid_t start_redirection(struct CommandBlock* commandBlock, const char* filePathName);

pid_t pipelineRedirection(size_t BegIndexOfCommandBlock, size_t NumberOfStringsInEachPipe[], size_t baseCaseRecursion,size_t numOfPipesInCommandBlock ,FILE* file);
pid_t start_pipelineRedirection(struct CommandBlock* commandBlock, const char* filePathName);


#endif