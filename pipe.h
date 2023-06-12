#ifndef PIPE_H
#define PIPE_H

#include "lists.h"
#include "nonBuiltInC.h"

int indexOfFirstPipeSignInPipeline(size_t specialCharIndexArray[],size_t currentSpecialCharIndex);
size_t numOfPipesInPipeline(size_t SpecialCharIndexArray[], size_t IndexToBeginFrom);
pid_t pipeline(size_t BegIndexOfPipeline, size_t numOfStringsInEachCommandBlock[], size_t IndexOfFirstPipeSignInPipeline, int numOfPipeSignsInPipeline);
pid_t start_pipeline(size_t BegIndexOfPipeline, size_t numOfStringsInEachCommandBlock[], size_t IndexOfFirstPipeSignInPipeline, int numOfPipeSignsInPipeline);


#endif