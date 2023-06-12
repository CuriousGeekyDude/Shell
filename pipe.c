#include "pipe.h"

int indexOfFirstPipeSignInPipeline(size_t specialCharIndexArray[],size_t currentSpecialCharIndex)
{

    size_t index = -1;

    for(size_t i = currentSpecialCharIndex; i < countList; ++i) {
        if(strcmp("|", argv[specialCharIndexArray[i]]) == 0) {
            index = specialCharIndexArray[i];
            return index;
        }
    }

    return index;
}

size_t numOfPipesInPipeline(size_t SpecialCharIndexArray[], size_t IndexToBeginFrom)
{
    //Assumes that the index before IndexToBeginFrom is already a pipe sign 
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

pid_t pipeline(size_t BegIndexOfPipeline, size_t numOfStringsInEachCommandBlock[], size_t IndexOfFirstPipeSignInPipeline, int numOfPipeSignsInPipeline)
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
            --numOfPipeSignsInPipeline;
            if(numOfPipeSignsInPipeline > 0) {
               childPID =  pipeline(BegIndexOfPipeline, numOfStringsInEachCommandBlock, IndexOfFirstPipeSignInPipeline, numOfPipeSignsInPipeline);
            }
            else {
               childPID = executeBlockCommand(BegIndexOfPipeline, numOfStringsInEachCommandBlock[IndexOfFirstPipeSignInPipeline], false);
            }
            break;

        case -1:
            errExit("fork");
            break;

        default:
            close(filedes[1]);
            childPID = wait(NULL);
            size_t i = 0;
            for(size_t j = 0; j < numOfPipeSignsInPipeline ; ++j) {
                    i += numOfStringsInEachCommandBlock[j]+1;
              }
            childPID = executeBlockCommand(BegIndexOfPipeline + i, numOfStringsInEachCommandBlock[IndexOfFirstPipeSignInPipeline + numOfPipeSignsInPipeline], false);
            break;
    }
    
    return childPID;
}

pid_t start_pipeline(size_t BegIndexOfPipeline, size_t numOfStringsInEachCommandBlock[], size_t IndexOfFirstPipeSignInPipeline, int numOfPipeSignsInPipeline)
{
    pid_t childPID = 0;
    switch(fork()) {
        case 0:
            childPID = pipeline(BegIndexOfPipeline, numOfStringsInEachCommandBlock, IndexOfFirstPipeSignInPipeline, numOfPipeSignsInPipeline);
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
