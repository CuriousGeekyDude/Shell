#ifndef BUILT_IN_C_H
#define BUILT_IN_C_H

#include <stddef.h>


//-----------------------------CD_COMMAND---------------------------------
size_t numOfAllTheSlashesInPath(const char* currentPath);
int findIndexOfLastSlash(const char* currentPath);
char* returnUpPath(const char* currentPath);
void update_PWD_OLDPWD(const char* newPWD, const char* newOLDPWD);
int cdCommand(size_t indexArgv, size_t numOfStringsInCommand);
//-----------------------------CD_COMMAND---------------------------------



//-----------------------------EXEC_COMMAND---------------------------------
void run_execvp(size_t indexArgv, char** newArgv);
void execCommand(size_t indexArgv);
//-----------------------------EXEC_COMMAND---------------------------------


//-----------------------------EXECUTABLES----------------------------------
int executable();
//-----------------------------EXECUTABLES----------------------------------



//-----------------------------PWD_COMMAND---------------------------------

//argc must not exceed 1
void pwdCommand();
//-----------------------------PWD_COMMAND---------------------------------



//-----------------------------EXIT_COMMAND---------------------------------

//argc must not exceed 1
void exitCommand();
//-----------------------------EXIT_COMMAND---------------------------------

#endif