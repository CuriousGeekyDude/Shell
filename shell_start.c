#include "shell_start.h"

void start_shell()
{
    if(read_storeCommands() == NULL)
        return;
    
    int IndexOfFirstPipeSignInPipeline = 0;
    pid_t childPID = 0;
    specialCharCounter();

    size_t* SpecialCharIndexArray = specialCharIndexArray();
    size_t* numOfStringsInEachBlock = numberOfStringsInEachBlock(SpecialCharIndexArray);
        
    
    IndexOfFirstPipeSignInPipeline = indexOfFirstPipeSignInPipeline(SpecialCharIndexArray,0);
    size_t NumOfPipesInPipeline = numOfPipesInPipeline(SpecialCharIndexArray, IndexOfFirstPipeSignInPipeline);

    if(numOfStringsInEachBlock != NULL) {
        
        size_t i;   //used in the next for loop

    //Checks to see if there is a pipeline at the beginning and adjust the value of i accordingly
        if(IndexOfFirstPipeSignInPipeline == SpecialCharIndexArray[0]) {
            childPID = start_pipeline(0, numOfStringsInEachBlock, 0, NumOfPipesInPipeline);
            if(NumOfPipesInPipeline == countList)
                return;
            else {
                i = NumOfPipesInPipeline;
                IndexOfFirstPipeSignInPipeline = findNextPipeSignIndex(SpecialCharIndexArray, i);
            }
        }
        else {
                childPID = executeBlockCommand(0, numOfStringsInEachBlock[0], true);
                i = 0;
            }

        
        for(; i < countList; ++i) {
            
            if(IndexOfFirstPipeSignInPipeline == SpecialCharIndexArray[i]) {
                NumOfPipesInPipeline = numOfPipesInPipeline(SpecialCharIndexArray, IndexOfFirstPipeSignInPipeline);
                childPID = start_pipeline(SpecialCharIndexArray[i-1] + 1, numOfStringsInEachBlock, i, NumOfPipesInPipeline);
                i += (NumOfPipesInPipeline-1);
                
        //updates IndexOfFirstPipeSignInPipeline to the first pipe sign index that appears after the pipeline
                IndexOfFirstPipeSignInPipeline = indexOfFirstPipeSignInPipeline(SpecialCharIndexArray, i+1);
                continue;
            }

            if(strcmp(argv[SpecialCharIndexArray[i]], "&&") == 0) {

                if(numOfStringsInEachBlock[i+1] != 0 && SpecialCharIndexArray[i+1] != IndexOfFirstPipeSignInPipeline)
                   childPID =  andList(SpecialCharIndexArray[i] + 1, numOfStringsInEachBlock[i+1], childPID);
                else
                    continue;
                }
            if(strcmp(argv[SpecialCharIndexArray[i]], "||") == 0) {

                if(numOfStringsInEachBlock[i+1] != 0 && SpecialCharIndexArray[i+1] != IndexOfFirstPipeSignInPipeline)
                    childPID = orList(SpecialCharIndexArray[i] + 1, numOfStringsInEachBlock[i+1], childPID);
                else
                    continue;
            }
            if(strcmp(argv[SpecialCharIndexArray[i]], ";") == 0) {
                if(numOfStringsInEachBlock[i+1] != 0 && SpecialCharIndexArray[i+1] != IndexOfFirstPipeSignInPipeline)
                    childPID  = semicolonList(SpecialCharIndexArray[i] + 1, numOfStringsInEachBlock[i+1]);
                
            }

        }

        free(SpecialCharIndexArray);
        free(numOfStringsInEachBlock);
    }

    else
        executeBlockCommand(0, argc, true);
        
    
}