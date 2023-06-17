#ifndef BUILT_IN_C_H
#define BUILT_IN_C_H

#include "basic.h"


//-----------------------------CD_COMMAND---------------------------------
char* returnUpPath(const char* currentPath);
void update_PWD_OLDPWD(const char* newPWD, const char* newOLDPWD);
int cdCommand(size_t indexArgv, size_t argc);
//-----------------------------CD_COMMAND---------------------------------



//-----------------------------EXEC_COMMAND---------------------------------
void run_execvp(size_t indexArgv, char* const * newArgv);
void execCommand(size_t indexArgv, size_t argc);
//-----------------------------EXEC_COMMAND---------------------------------


//-----------------------------PWD_COMMAND---------------------------------

//argc must not exceed 1
void pwdCommand(size_t argc);
//-----------------------------PWD_COMMAND---------------------------------



//-----------------------------EXIT_COMMAND---------------------------------

//argc must not exceed 1
void exitCommand(size_t argc);
//-----------------------------EXIT_COMMAND---------------------------------

#endif