#ifndef NONBUILTINC_H
#define NONBUILTINC_H

#include <sys/types.h>

void exec_nonBuiltInCommand(size_t indexArgv, int argc);
pid_t nonBuiltInCommand(size_t indexArgv, int argc);

#endif