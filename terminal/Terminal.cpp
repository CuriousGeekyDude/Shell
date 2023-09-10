#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <exception>
#include <system_error>
#include <array>
#include <list>
extern "C" {
#include <color.h>
}
#include <string.h>
#include <Terminal.hpp>



Terminal::Terminal(const int Fd) : fd(Fd)
{
    errno = 0;
    if(tcgetattr(fd, &original_termios) == -1)
        throw std::system_error(errno, std::generic_category());
    updateScreenSize();
    clearScreen();
}

void Terminal::enterRawMode()
{
    struct termios raw;
    clearScreen();

    errno = 0;
    if(tcgetattr(fd, &raw) == -1)
        throw std::system_error(errno, std::generic_category());
            
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
            
    errno = 0;
    if(tcsetattr(fd, TCSAFLUSH, &raw) == -1)
        throw std::system_error(errno, std::generic_category());
            
}

std::list<unsigned char> Terminal::getInput()
{
    return input;
}

        
void Terminal::processKeyPress()
{
    try {   //Exception handling is not the goal here. Breaking out of the while loop when enter is pressed is the goal of try/catch
        while(1) {
            checkLimitOfHistory();
            updateScreenSize();
            char c = readKeyPress();
            updateScreenSize();

            if(c == Ctrl_Key('q')) {
                clearScreen();
                std::string error = "Ctrl-q was pressed.";
                throw error;
            }

            if(c == Ctrl_Key('j')) {
                clearScreen();
                return;
            }
                
            if(checkForArrowKeys(c) == -1)
                printPrintableKeys(c);
        }
    }
    catch(int enterASCII_Code) {
        disableRawMode();   
    }

}


void Terminal::disableRawMode()
{
    errno = 0;
    if(tcsetattr(fd, TCSAFLUSH, &original_termios) == -1)
        perror("tcsetattr()");
    std::cout << std::endl;
}

void Terminal::emptyInput()
{
    input.clear();
}

Terminal::~Terminal()
{
    disableRawMode();
}




void Terminal::copyHistoryToInput()
{
    input.clear();
    std::string hisCommand = *iteratorHistory;
    auto sizeOfHisCommand = hisCommand.size();

    for(size_t i = 0; i < sizeOfHisCommand; ++i) {
        input.push_back(hisCommand[i]);
    }
    iterator = input.begin();
}

void Terminal::updateScreenSize()    //Main use in constructor and also when screen size is updated
{
    struct winsize window = {0};
            
    errno = 0;
    if(ioctl(fd, TIOCGWINSZ, &window) == -1 || window.ws_col == 0 || window.ws_row == 0) {
        throw std::system_error(errno, std::generic_category());
    }

    screenRow = window.ws_row;
    screenColumn = window.ws_col;
}

char Terminal::readKeyPress()
{
    if(fd != STDIN_FILENO)
        throw "readKeyPress() was called while fd != STDIN_FILENO.";

    int numRead;
    char c;
    errno = 0;
    if((numRead = read(fd, &c, 1)) != 1) {
        if(numRead == -1 && errno != EAGAIN)
            throw std::system_error(errno, std::generic_category());
    }
    return c;
}
        

void Terminal::WRITE(const char* string, const int count) //Helper for clearScreen()
{   

    errno = 0;
    if(write(STDOUT_FILENO, string, count) == -1 || fflush(stdout) == EOF)
        throw std::system_error(errno, std::generic_category());
    updateCursorPos();
    printCursor();
}
        
std::string Terminal::getenviron(const std::string& env)
{
    std::string envirValue = secure_getenv(env.c_str());
    if(envirValue.empty())
        throw "Environment variable was not found!";
    return envirValue;
}

void Terminal::printCurrentDir()
{
    char* pwd = getenv("PWD");
    if(pwd == NULL)
        throw "PWD was not found!";

    green();
    printf("FAN$");
    randomColorGenerator();
    printf("%s:", pwd);
    resetColor();
    fflush(stdout);
}

void Terminal::updateBegin_cursorX()
{
    cursorX_begin = cursorX;
}

void Terminal::updateBegin_cursorY(const std::string& pwd)
{
    cursorY = 6 + pwd.size();   //FAN$ and : along with necessary spaces amount to 7 chars
    updateCursorPos();
    cursorY_begin = cursorY;
}
        


void Terminal::clearScreen()
{
    WRITE("\x1b[2J", 5);
    WRITE("\x1b[H", 4);
    std::string pwd = getenviron("PWD");
    cursorX = 1;
    cursorY = 1;
    printCursor();
    printCurrentDir();
    updateBegin_cursorY(pwd);
    updateBegin_cursorX();
    printCursor();
    
}



void Terminal::updateCursorPos()
{
    if(cursorY == screenColumn + 1) {
        cursorY = 1;
        cursorX++;                
    }
            
    if(cursorY == 0) {
        cursorY = screenColumn;
        if(cursorX == 0) {
            cursorX = 1;
        }
        else {
            cursorX--;
        }
    }
}
        
void Terminal::printCursor()
{
    printf("\x1b[%d;%dH", cursorX, cursorY);
    fflush(stdout);
}
       
char Terminal::Ctrl_Key(char key)
{
    return key &= 0x1f;
}

        

//--------------------------------------------------------------------------------------
void Terminal::printHisCommand()
{
    cursorX = cursorX_begin;
    cursorY = cursorY_begin;
    printCursor();
    printf("\x1b[0J");
    
    for(; iterator != input.end(); ++iterator) {
        printf("%c", *iterator);
        cursorY++;
        updateCursorPos();
        printCursor();
    }
}

void Terminal::checkLimitOfHistory()
{
    if(historyCommands.empty()) {
        return;
    }

    if(historyCommands.size() == 200) { //Hard limit for number of history commands it can store
        historyCommands.erase(historyCommands.begin());
        iteratorHistory = historyCommands.end();
    }
}

void Terminal::pageUpAction()
{
    for(; iterator != input.begin(); --iterator) {
        cursorY--;
        updateCursorPos();
        printCursor();
    }
}
void Terminal::pageDownAction()
{
    for(; iterator != input.end(); ++iterator) {
        cursorY++;
        updateCursorPos();
        printCursor();
    }
}

void Terminal::deleteAction()
{
    if(input.empty()) {
        return;
    }
    
    cursorX= cursorX_begin;
    cursorY = cursorY_begin;
    printCursor();
    printf("\x1b[0J");
    fflush(stdout);
    input.clear();
    iterator = input.begin();
}

void Terminal::upArrowAction()
{
    if(historyCommands.size() == 0 || iteratorHistory == historyCommands.begin()) {
        return;
    }

    iteratorHistory--;
    copyHistoryToInput();

    printHisCommand();

}

void Terminal::downArrowAction()
{
    auto iter = iteratorHistory;
    if(historyCommands.size() == 0 || iteratorHistory == historyCommands.end()) {
        return;
    }

    if(iteratorHistory != historyCommands.end() && (++iter) != historyCommands.end()) {
        iteratorHistory++;
    }
    else {
        ++iteratorHistory;
        input.clear();
        cursorX = cursorX_begin;
        cursorY = cursorY_begin;
        updateCursorPos();
        printCursor();
        printf("\x1b[0J");
        fflush(stdout);
        return;
    }
    copyHistoryToInput();

    printHisCommand();
}
void Terminal::rightArrowAction()
{
    if(iterator == input.end() || input.empty())
        return;
    cursorY++;
    updateCursorPos();
    iterator++;
    printCursor();
}
void Terminal::leftArrowAction()
{
    if(iterator == input.begin() || input.empty())
        return;
    cursorY--;
    updateCursorPos();
    iterator--;
    printCursor();
}

void Terminal::arrowKeysAction(const ArrowKeys key)
{            
    switch(key) {
        case ARROW_UP:     
            upArrowAction();       
            break;

        case ARROW_DOWN:
            downArrowAction();            
            break;

        case ARROW_RIGHT:
            rightArrowAction();
            break;

        case ARROW_LEFT:
            leftArrowAction();
            break;

        case PAGE_UP:
            pageUpAction();
            break;
        case PAGE_DOWN:
            pageDownAction();
            break;
        case DELETE_KEY:
            deleteAction();
            break;
                    
    }
                
}

int Terminal::checkForArrowKeys(const char key)
{
    std::array<char, 3> seq;
    seq.fill(0);

    if(key == 27) {
        errno = 0;
        if(read(STDIN_FILENO, seq.data(), 1) != 1 || read(STDIN_FILENO, seq.data() + 1, 1) != 1)
            throw std::system_error(errno, std::generic_category());
        if(seq[0] == '[') {
                   
            switch(seq[1]) {
                case 'A':   arrowKeysAction(ARROW_UP);      return 0;
                case 'B':   arrowKeysAction(ARROW_DOWN);    return 0;
                case 'C':   arrowKeysAction(ARROW_RIGHT);   return 0;
                case 'D':   arrowKeysAction(ARROW_LEFT);    return 0;
                case 51://51='3'
                if(read(STDIN_FILENO, seq.data() + 2, 1) != 1)
                        throw std::system_error(errno, std::generic_category());
                    if(seq[2] == '~')   
                        arrowKeysAction(DELETE_KEY);
                    return 0;
                case 53 ://53 = '5'
                    if(read(STDIN_FILENO, seq.data() + 2, 1) != 1)
                        throw std::system_error(errno, std::generic_category());
                    if(seq[2] == '~')   
                        arrowKeysAction(PAGE_UP);
                    return 0;
                case 54 ://54 = '6'
                    if(read(STDIN_FILENO, seq.data() + 2, 1) != 1)
                        throw std::system_error(errno, std::generic_category());
                            if(seq[2] == '~')
                        arrowKeysAction(PAGE_DOWN);
                    return 0;
                default :   return 1;
            }
        }
        else
            return 1;
    }

    return -1;
}
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------

void Terminal::enterKeyAction()
{

    throw 13;
}

void Terminal::printInputAfterInsertion(std::list<unsigned char>::iterator iter)
{
    int cursorY_Temp = cursorY;
    cursorY -= 1;

    for(; iter != input.end(); ++iter) {
        printf("%c", *iter);
        fflush(stdout);
        cursorY++;
        updateCursorPos();
        printCursor();
    }
    cursorY = cursorY_Temp;
    iterator++;
    updateCursorPos();
    printCursor();
}

void Terminal::printInputAfterDeletion()
{
    if(iterator == input.begin() || input.empty())
        return;
    iterator--;
    iterator = input.erase(iterator);
    auto iterator_Temp = iterator;
    iterator = input.begin();

    int cursorY_Temp = cursorY-1;
    int cursorX_Temp = cursorX;

    printHisCommand();
    cursorY = cursorY_Temp;
    iterator = iterator_Temp;
    cursorX = cursorX_Temp;

    if(cursorY == 0) {
        cursorY = screenColumn;
        cursorX--;
    }
    

    printCursor();
}


void Terminal::printPrintableKeys(const char key)
{
    if(key == 13) {
        enterKeyAction();
        return;
    }

    if(32 <= key && key <= 126) {
        cursorY++; 
        if(iterator == input.end()){
            input.push_back(static_cast<unsigned char>(key)); 
            iterator = input.end(); 
            const char* string_key = &key;
            WRITE(string_key, 1);
        } 
        else {
            iterator = input.insert(iterator, static_cast<unsigned char>(key));
            printInputAfterInsertion(iterator);
        }
        return;
    }

    if(key == 127) {
        printInputAfterDeletion();
        return;
    }        
            
}

//--------------------------------------------------------------------------------------
