#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <string>
#include <list>
#include <termios.h>

class Terminal 
{
    public:
        Terminal(const int Fd);

        void enterRawMode();

        std::list<unsigned char> getInput();

        void processKeyPress();

        void disableRawMode();

    private:

        enum ArrowKeys
        {
            BACKSPACE = 127,
            ARROW_UP = 130,
            ARROW_DOWN,
            ARROW_LEFT,
            ARROW_RIGHT,
            PAGE_UP,
            PAGE_DOWN
        };
        
        void updateScreenSize();

        char readKeyPress();

        void WRITE(const char* string, const int count);

        std::string getenviron(const std::string& env);
        std::string printCurrentDir();
        void updateBegin_cursorY(const std::string& pwd);
        void clearScreen();

        void scrollingDown();

        void updateCursorPos();
        void printCursor();

        char Ctrl_Key(char key);

        void rightArrowAction();
        void leftArrowAction();
        void arrowKeysAction(const ArrowKeys key);
        int checkForArrowKeys(const char key);


        void enterKeyAction();
        void printInputAfterInsertion(std::list<unsigned char>::iterator iter);
        void printInputAfterDeletion();
        void printPrintableKeys(const char key);

        struct termios original_termios;
        const int fd;
        int screenRow, screenColumn;
        int cursorX = 1,  cursorY;
        int cursorX_begin, cursorY_begin;
        bool ENTER_KEY_PRESSED = false;
        std::list<unsigned char> input;
        std::list<unsigned char>::iterator iterator = input.begin();


};

#endif