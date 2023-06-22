#ifndef NONBUILTINC_H
#define NONBUILTINC_H

#include <sys/types.h>

void exec_nonBuiltInCommand(size_t indexArgv, size_t argc, char** argv);
pid_t nonBuiltInCommand(size_t indexArgv, size_t argc, char** argv);

#endif