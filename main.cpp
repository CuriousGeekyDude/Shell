#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <exception>
#include <system_error>
#include <string>
#include <iostream> 
#include <Shell_Terminal.hpp>

extern "C" {
    #include <basic.h>
    #include <history.h>
    #include <shell_start.h>
}


void signalHandler(int signal)
{
/*
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
    */
}







int main(void)
{
    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_handler = &signalHandler;
    sig.sa_flags = SA_RESTART;

    sigaction(SIGTSTP, &sig, NULL);

    try {
        Shell_Terminal binder;
        while(1) {
            binder.terminal.enterRawMode();
            binder.terminal.processKeyPress();
            binder.transferInputTo_InputWords();
            binder.terminal.disableRawMode();
            binder.terminal.emptyInput();
            start_shell();
            binder.transferHistoryTo_Terminal();
        }
    }

    catch(std::system_error& error) {
        std::cerr << "Error1: " << error.code().message() << ".\nAborting..." << std::endl;
        return -1;            
    }
    catch(std::string& error) {
        std::cerr << error << "\nAborting..." << std::endl;
    }

    freeAllBlocks();
    return 0;
}