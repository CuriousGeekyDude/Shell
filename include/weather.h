#ifndef WEATHER_H
#define WEATHER_H

#include <sys/types.h>

pid_t weatherCommand(size_t begIndexOfCommand, size_t numOfStringsInCommand, const char* commandType);

#endif