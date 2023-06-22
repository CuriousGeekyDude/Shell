#ifndef EXECUTE_COMMAND_H
#define EXECUTE_COMMAND_H

#include <stdbool.h>
#include <sys/types.h>


pid_t executeCommand(char** argv, size_t begIndexOfCommand, size_t numOfStringsInCommand, bool toFork);


#endif