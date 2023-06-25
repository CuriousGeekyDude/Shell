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
    

    char* cityArgument = NULL;
    char* countryArgument = NULL;

    if(sprintf(cityArgument, "%s", argv[1]) < 0) {
        errExit("sprintf()1 in weatherCommand");
    }
    if(sprintf(countryArgument, "%s", argv[2]) < 0) {
        errExit("sprintf()2 in weatherCommand");
    }

    pid_t childPID = -1;
    if(strcmp("N", commandType) == 0) {

        switch(fork()) {
            case 0:
                if(execlp("python3", "weather.py", "--city", cityArgument, "--country", countryArgument, NULL) == -1) {
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

        if(execlp("python3", "weather.py", "--city", cityArgument, "--country", countryArgument, NULL) == -1) {
                    errExit("execlp");
        }
    }
}