#ifndef SHELL_TERMINAL_HPP
#define SHELL_TERMINAL_HPP

#include <cstddef>
extern "C" {
#include <basic.h>
}
#include <Terminal.hpp>
#include <unistd.h>



class Shell_Terminal
{
    public:
        Shell_Terminal()=default;

        void transferInputTo_InputWords();

        void transferHistoryTo_Terminal();
        
        Terminal terminal = Terminal(STDIN_FILENO);
        
};

#endif