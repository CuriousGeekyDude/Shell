#ifndef NONBUILTINC_H
#define NONBUILTINC_H

#include "basic.h"

void exec_nonBuiltInCommand(size_t indexArgv, size_t argc);
static inline pid_t nonBuiltInCommand(size_t indexArgv, size_t argc);

#endif