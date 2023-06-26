#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "basic.h"
#include "history.h"
#include "shell_start.h"

void signalHandler(int signal)
{

    if(signal == SIGTSTP) {
        if(currentHistoryBlock == NULL)
            return;
        if(currentHistoryBlock->command == NULL) {
            printf("\nnull!\n");
            fflush(stdout);
        }
        else {
            printf("\n%s\n", currentHistoryBlock->command);
            fflush(stdout);
        }
        if(currentHistoryBlock->next == NULL) {
            printShellSign();
            return;
        }
        currentHistoryBlock = currentHistoryBlock->next;
        printShellSign();

    }

}






int main(void)
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &signalHandler;
    sig.sa_flags = SA_RESTART;

    sigaction(SIGTSTP, &sig, NULL);
    sigaction(SIGINT, &sig, NULL);

    while(1) {
        start_shell();
    }

    freeAllBlocks();
    exit(EXIT_SUCCESS);
}