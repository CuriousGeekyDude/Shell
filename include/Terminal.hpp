#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <string>
#include <list>
#include <vector>
#include <termios.h>

extern "C" {
    #include <history.h>
}

class Terminal 
{
    public:
        std::vector<historyBlock> historyCommands;
        std::vector<historyBlock>::iterator iteratorHistory = historyCommands.end();

        Terminal(const int Fd);

        void enterRawMode();

        std::list<unsigned char> getInput();

        void processKeyPress();

        void disableRawMode();

        void emptyInput();

        ~Terminal();

    private:

        enum ArrowKeys
        {
            ARROW_UP = 130,
            ARROW_DOWN,
            ARROW_LEFT,
            ARROW_RIGHT,
            PAGE_UP,
            PAGE_DOWN,
            DELETE_KEY
        };
        void copyHistoryToInput();

        void updateScreenSize();

        char readKeyPress();

        void WRITE(const char* string, const int count);

        std::string getenviron(const std::string& env);
        void printCurrentDir();
        void updateBegin_cursorX();
        void updateBegin_cursorY(const std::string& pwd);
        void clearScreen();


        void updateCursorPos();
        void printCursor();

        char Ctrl_Key(char key);

        void printHisCommand();
        void pageUpAction();
        void pageDownAction();
        void upArrowAction();
        void downArrowAction();
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
        std::list<unsigned char> input;
        std::list<unsigned char>::iterator iterator = input.begin();

};

#endif