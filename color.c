#include "color.h"

void black(void)
{
    printf("\033[0;30m");
}
void red(void)
{
    printf("\033[1;31m");
}
void green(void)
{
    printf("\033[1;32m");
}
void yellow(void)
{
    printf("\033[1;33m");
}
void blue(void)
{
    printf("\033[1;34m");
}
void purple(void)
{
    printf("\033[1;35m");
}
void white(void)
{
    printf("\033[1;37m");
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
}
