#include <stdio.h>
#include <stdlib.h>
#include "color.h"

void black(void)
{
    printf("\033[0;30m");
    fflush(stdout);
}
void red(void)
{
    printf("\033[1;31m");
    fflush(stdout);

}
void green(void)
{
    printf("\033[1;32m");
    fflush(stdout);

}
void yellow(void)
{
    printf("\033[1;33m");
    fflush(stdout);

}
void blue(void)
{
    printf("\033[1;34m");
    fflush(stdout);

}
void purple(void)
{
    printf("\033[1;35m");
    fflush(stdout);

}
void white(void)
{
    printf("\033[1;37m");
    fflush(stdout);

}


void randomColorGenerator(void)
{
    switch(rand() % 6) {
        case 0:
            red();
            break;
        case 1:
            green();
            break;
        case 2:
            yellow();
            break;
        case 3:
            blue();
            break;
        case 4:
            purple();
            break;
        case 5:
            white();
            break;
    }
}

void resetColor(void)
{
    printf("\033[0m");
    fflush(stdout);

}
