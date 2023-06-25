#include <unistd.h>
#include <basic.h>
#include <stdio.h>
#include <string.h>
#include <error_functions.h>
#include <sys/wait.h>
#include <weather.h>


pid_t weatherCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, const char* commandType)
{
    if(numOfStringsInCommand > 3 || numOfStringsInCommand < 3)
        return -1;

    pid_t childPID = -1;
    if(strcmp("N", commandType) == 0) {

        switch(fork()) {
            case 0:
                if(execlp("python3", "python3","weather.py", "--city", argv[1], "--country", argv[2], NULL) == -1) {
                    errExit("execlp");
                }
                break;

            default:
                childPID = wait(NULL);
                return childPID;
                break;
        }
    }

    else {

        if(execlp("python3", "python3" ,"weather.py", "--city", argv[1], "--country", argv[2], NULL) == -1) {
                    errExit("execlp");
        }
    }
}