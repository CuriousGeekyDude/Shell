#include "exec.h"

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

        for(size_t i = 0; i < sizeOfNewArgv-1 && i < BUFFSIZE; ++i)
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