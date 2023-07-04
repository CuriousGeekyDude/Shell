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
    std::string hisCommand = iteratorHistory->command;
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
    if(historyCommands.size() == 0) {
        return;
    }

    if(iteratorHistory == historyCommands.end())
        return;
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
        case PAGE_DOWN:
                    
            return;
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
                case  5 :
                    if(read(STDIN_FILENO, seq.data() + 2, 1) != 1)
                        throw std::system_error(errno, std::generic_category());
                    if(seq[2] == '~')   
                        arrowKeysAction(PAGE_UP);
                    return 0;
                case  6 :
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

    int cursorY_Temp = cursorY-1;
    int cursorX_Temp = cursorX;
    cursorY = cursorY_begin;
    cursorX = cursorX_begin;
    printCursor();
            
    WRITE("\x1b[0J", 5);
            
    iterator = input.begin();

            
    for(; iterator != input.end(); ++iterator) {
        printf("%c", *iterator);
        cursorY++;
        updateCursorPos();
        printCursor();
    }
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
            switch(key)
            {
                case 13:      enterKeyAction(); break;
                case 32:      cursorY++; if(iterator == input.end()){input.push_back(32); iterator = input.end(); WRITE(" ", 1);} else {iterator = input.insert(iterator, 32);printInputAfterInsertion(iterator);}break;
                case 33:      cursorY++; if(iterator == input.end()){input.push_back(33); iterator = input.end(); WRITE("!", 1);} else {iterator = input.insert(iterator, 33);printInputAfterInsertion(iterator);}break;
                case 34:      cursorY++; if(iterator == input.end()){input.push_back(34); iterator = input.end(); WRITE("\"",1);} else {iterator = input.insert(iterator, 34);printInputAfterInsertion(iterator);}break;
                case 35:      cursorY++; if(iterator == input.end()){input.push_back(35); iterator = input.end(); WRITE("#", 1);} else {iterator = input.insert(iterator, 35);printInputAfterInsertion(iterator);}break;
                case 36:      cursorY++; if(iterator == input.end()){input.push_back(36); iterator = input.end(); WRITE("$", 1);} else {iterator = input.insert(iterator, 36);printInputAfterInsertion(iterator);}break;
                case 37:      cursorY++; if(iterator == input.end()){input.push_back(37); iterator = input.end(); WRITE("%%",1);} else {iterator = input.insert(iterator, 37);printInputAfterInsertion(iterator);}break;
                case 38:      cursorY++; if(iterator == input.end()){input.push_back(38); iterator = input.end(); WRITE("&", 1);} else {iterator = input.insert(iterator, 38);printInputAfterInsertion(iterator);}break;
                case 39:      cursorY++; if(iterator == input.end()){input.push_back(39); iterator = input.end(); WRITE("'", 1);} else {iterator = input.insert(iterator, 39);printInputAfterInsertion(iterator);}break;
                case 40:      cursorY++; if(iterator == input.end()){input.push_back(40); iterator = input.end(); WRITE("(", 1);} else {iterator = input.insert(iterator, 40);printInputAfterInsertion(iterator);}break;
                case 41:      cursorY++; if(iterator == input.end()){input.push_back(41); iterator = input.end(); WRITE(")", 1);} else {iterator = input.insert(iterator, 41);printInputAfterInsertion(iterator);}break;
                case 42:      cursorY++; if(iterator == input.end()){input.push_back(42); iterator = input.end(); WRITE("*", 1);} else {iterator = input.insert(iterator, 42);printInputAfterInsertion(iterator);}break;
                case 43:      cursorY++; if(iterator == input.end()){input.push_back(43); iterator = input.end(); WRITE("+", 1);} else {iterator = input.insert(iterator, 43);printInputAfterInsertion(iterator);}break;
                case 44:      cursorY++; if(iterator == input.end()){input.push_back(44); iterator = input.end(); WRITE(",", 1);} else {iterator = input.insert(iterator, 44);printInputAfterInsertion(iterator);}break;
                case 45:      cursorY++; if(iterator == input.end()){input.push_back(45); iterator = input.end(); WRITE("-", 1);} else {iterator = input.insert(iterator, 45);printInputAfterInsertion(iterator);}break;
                case 46:      cursorY++; if(iterator == input.end()){input.push_back(46); iterator = input.end(); WRITE(".", 1);} else {iterator = input.insert(iterator, 46);printInputAfterInsertion(iterator);}break;
                case 47:      cursorY++; if(iterator == input.end()){input.push_back(47); iterator = input.end(); WRITE("/", 1);} else {iterator = input.insert(iterator, 47);printInputAfterInsertion(iterator);}break;
                case 48:      cursorY++; if(iterator == input.end()){input.push_back(48); iterator = input.end(); WRITE("0", 1);} else {iterator = input.insert(iterator, 48);printInputAfterInsertion(iterator);}break;
                case 49:      cursorY++; if(iterator == input.end()){input.push_back(49); iterator = input.end(); WRITE("1", 1);} else {iterator = input.insert(iterator, 49);printInputAfterInsertion(iterator);}break;
                case 50:      cursorY++; if(iterator == input.end()){input.push_back(50); iterator = input.end(); WRITE("2", 1);} else {iterator = input.insert(iterator, 50);printInputAfterInsertion(iterator);}break;
                case 51:      cursorY++; if(iterator == input.end()){input.push_back(51); iterator = input.end(); WRITE("3", 1);} else {iterator = input.insert(iterator, 51);printInputAfterInsertion(iterator);}break;
                case 52:      cursorY++; if(iterator == input.end()){input.push_back(52); iterator = input.end(); WRITE("4", 1);} else {iterator = input.insert(iterator, 52);printInputAfterInsertion(iterator);}break;
                case 53:      cursorY++; if(iterator == input.end()){input.push_back(53); iterator = input.end(); WRITE("5", 1);} else {iterator = input.insert(iterator, 53);printInputAfterInsertion(iterator);}break;
                case 54:      cursorY++; if(iterator == input.end()){input.push_back(54); iterator = input.end(); WRITE("6", 1);} else {iterator = input.insert(iterator, 54);printInputAfterInsertion(iterator);}break;
                case 55:      cursorY++; if(iterator == input.end()){input.push_back(55); iterator = input.end(); WRITE("7", 1);} else {iterator = input.insert(iterator, 55);printInputAfterInsertion(iterator);}break;
                case 56:      cursorY++; if(iterator == input.end()){input.push_back(56); iterator = input.end(); WRITE("8", 1);} else {iterator = input.insert(iterator, 56);printInputAfterInsertion(iterator);}break;
                case 57:      cursorY++; if(iterator == input.end()){input.push_back(57); iterator = input.end(); WRITE("9", 1);} else {iterator = input.insert(iterator, 57);printInputAfterInsertion(iterator);}break;
                case 58:      cursorY++; if(iterator == input.end()){input.push_back(58); iterator = input.end(); WRITE(":", 1);} else {iterator = input.insert(iterator, 58);printInputAfterInsertion(iterator);}break;
                case 59:      cursorY++; if(iterator == input.end()){input.push_back(59); iterator = input.end(); WRITE(";", 1);} else {iterator = input.insert(iterator, 59);printInputAfterInsertion(iterator);}break;
                case 60:      cursorY++; if(iterator == input.end()){input.push_back(60); iterator = input.end(); WRITE("<", 1);} else {iterator = input.insert(iterator, 60);printInputAfterInsertion(iterator);}break;
                case 61:      cursorY++; if(iterator == input.end()){input.push_back(61); iterator = input.end(); WRITE("=", 1);} else {iterator = input.insert(iterator, 61);printInputAfterInsertion(iterator);}break;
                case 62:      cursorY++; if(iterator == input.end()){input.push_back(62); iterator = input.end(); WRITE(">", 1);} else {iterator = input.insert(iterator, 62);printInputAfterInsertion(iterator);}break;
                case 63:      cursorY++; if(iterator == input.end()){input.push_back(63); iterator = input.end(); WRITE("?", 1);} else {iterator = input.insert(iterator, 63);printInputAfterInsertion(iterator);}break;
                case 64:      cursorY++; if(iterator == input.end()){input.push_back(64); iterator = input.end(); WRITE("@", 1);} else {iterator = input.insert(iterator, 64);printInputAfterInsertion(iterator);}break;
                case 65:      cursorY++; if(iterator == input.end()){input.push_back(65); iterator = input.end(); WRITE("A", 1);} else {iterator = input.insert(iterator, 65);printInputAfterInsertion(iterator);}break;
                case 66:      cursorY++; if(iterator == input.end()){input.push_back(66); iterator = input.end(); WRITE("B", 1);} else {iterator = input.insert(iterator, 66);printInputAfterInsertion(iterator);}break;
                case 67:      cursorY++; if(iterator == input.end()){input.push_back(67); iterator = input.end(); WRITE("C", 1);} else {iterator = input.insert(iterator, 67);printInputAfterInsertion(iterator);}break;
                case 68:      cursorY++; if(iterator == input.end()){input.push_back(68); iterator = input.end(); WRITE("D", 1);} else {iterator = input.insert(iterator, 68);printInputAfterInsertion(iterator);}break;
                case 69:      cursorY++; if(iterator == input.end()){input.push_back(69); iterator = input.end(); WRITE("E", 1);} else {iterator = input.insert(iterator, 69);printInputAfterInsertion(iterator);}break;
                case 70:      cursorY++; if(iterator == input.end()){input.push_back(70); iterator = input.end(); WRITE("F", 1);} else {iterator = input.insert(iterator, 70);printInputAfterInsertion(iterator);}break;
                case 71:      cursorY++; if(iterator == input.end()){input.push_back(71); iterator = input.end(); WRITE("G", 1);} else {iterator = input.insert(iterator, 71);printInputAfterInsertion(iterator);}break;
                case 72:      cursorY++; if(iterator == input.end()){input.push_back(72); iterator = input.end(); WRITE("H", 1);} else {iterator = input.insert(iterator, 72);printInputAfterInsertion(iterator);}break;
                case 73:      cursorY++; if(iterator == input.end()){input.push_back(73); iterator = input.end(); WRITE("I", 1);} else {iterator = input.insert(iterator, 73);printInputAfterInsertion(iterator);}break;
                case 74:      cursorY++; if(iterator == input.end()){input.push_back(74); iterator = input.end(); WRITE("J", 1);} else {iterator = input.insert(iterator, 74);printInputAfterInsertion(iterator);}break;
                case 75:      cursorY++; if(iterator == input.end()){input.push_back(75); iterator = input.end(); WRITE("K", 1);} else {iterator = input.insert(iterator, 75);printInputAfterInsertion(iterator);}break;
                case 76:      cursorY++; if(iterator == input.end()){input.push_back(76); iterator = input.end(); WRITE("L", 1);} else {iterator = input.insert(iterator, 76);printInputAfterInsertion(iterator);}break;
                case 77:      cursorY++; if(iterator == input.end()){input.push_back(77); iterator = input.end(); WRITE("M", 1);} else {iterator = input.insert(iterator, 77);printInputAfterInsertion(iterator);}break;
                case 78:      cursorY++; if(iterator == input.end()){input.push_back(78); iterator = input.end(); WRITE("N", 1);} else {iterator = input.insert(iterator, 78);printInputAfterInsertion(iterator);}break;
                case 79:      cursorY++; if(iterator == input.end()){input.push_back(79); iterator = input.end(); WRITE("O", 1);} else {iterator = input.insert(iterator, 79);printInputAfterInsertion(iterator);}break;
                case 80:      cursorY++; if(iterator == input.end()){input.push_back(80); iterator = input.end(); WRITE("P", 1);} else {iterator = input.insert(iterator, 80);printInputAfterInsertion(iterator);}break;
                case 81:      cursorY++; if(iterator == input.end()){input.push_back(81); iterator = input.end(); WRITE("Q", 1);} else {iterator = input.insert(iterator, 81);printInputAfterInsertion(iterator);}break;
                case 82:      cursorY++; if(iterator == input.end()){input.push_back(82); iterator = input.end(); WRITE("R", 1);} else {iterator = input.insert(iterator, 82);printInputAfterInsertion(iterator);}break;
                case 83:      cursorY++; if(iterator == input.end()){input.push_back(83); iterator = input.end(); WRITE("S", 1);} else {iterator = input.insert(iterator, 83);printInputAfterInsertion(iterator);}break;
                case 84:      cursorY++; if(iterator == input.end()){input.push_back(84); iterator = input.end(); WRITE("T", 1);} else {iterator = input.insert(iterator, 84);printInputAfterInsertion(iterator);}break;
                case 85:      cursorY++; if(iterator == input.end()){input.push_back(85); iterator = input.end(); WRITE("U", 1);} else {iterator = input.insert(iterator, 85);printInputAfterInsertion(iterator);}break;
                case 86:      cursorY++; if(iterator == input.end()){input.push_back(86); iterator = input.end(); WRITE("V", 1);} else {iterator = input.insert(iterator, 86);printInputAfterInsertion(iterator);}break;
                case 87:      cursorY++; if(iterator == input.end()){input.push_back(87); iterator = input.end(); WRITE("W", 1);} else {iterator = input.insert(iterator, 87);printInputAfterInsertion(iterator);}break;
                case 88:      cursorY++; if(iterator == input.end()){input.push_back(88); iterator = input.end(); WRITE("X", 1);} else {iterator = input.insert(iterator, 88);printInputAfterInsertion(iterator);}break;
                case 89:      cursorY++; if(iterator == input.end()){input.push_back(89); iterator = input.end(); WRITE("Y", 1);} else {iterator = input.insert(iterator, 89);printInputAfterInsertion(iterator);}break;
                case 90:      cursorY++; if(iterator == input.end()){input.push_back(90); iterator = input.end(); WRITE("Z", 1);} else {iterator = input.insert(iterator, 90);printInputAfterInsertion(iterator);}break;
                case 91:      cursorY++; if(iterator == input.end()){input.push_back(91); iterator = input.end(); WRITE("[", 1);} else {iterator = input.insert(iterator, 91);printInputAfterInsertion(iterator);}break;
                case 92:      cursorY++; if(iterator == input.end()){input.push_back(92); iterator = input.end(); WRITE("\\", 1);}else {iterator = input.insert(iterator, 92);printInputAfterInsertion(iterator);}break;
                case 93:      cursorY++; if(iterator == input.end()){input.push_back(93); iterator = input.end(); WRITE("]", 1);} else {iterator = input.insert(iterator, 93);printInputAfterInsertion(iterator);}break;
                case 94:      cursorY++; if(iterator == input.end()){input.push_back(94); iterator = input.end(); WRITE("^", 1);} else {iterator = input.insert(iterator, 94);printInputAfterInsertion(iterator);}break;
                case 95:      cursorY++; if(iterator == input.end()){input.push_back(95); iterator = input.end(); WRITE("_", 1);} else {iterator = input.insert(iterator, 95);printInputAfterInsertion(iterator);}break;
                case 96:      cursorY++; if(iterator == input.end()){input.push_back(96); iterator = input.end(); WRITE("`", 1);} else {iterator = input.insert(iterator, 96);printInputAfterInsertion(iterator);}break;
                case 97:      cursorY++; if(iterator == input.end()){input.push_back(97); iterator = input.end(); WRITE("a", 1);} else {iterator = input.insert(iterator, 97);printInputAfterInsertion(iterator);}break;
                case 98:      cursorY++; if(iterator == input.end()){input.push_back(98); iterator = input.end(); WRITE("b", 1);} else {iterator = input.insert(iterator, 98);printInputAfterInsertion(iterator);}break;
                case 99:      cursorY++; if(iterator == input.end()){input.push_back(99); iterator = input.end(); WRITE("c", 1);} else {iterator = input.insert(iterator, 99);printInputAfterInsertion(iterator);}break;
                case 100:     cursorY++; if(iterator == input.end()){input.push_back(100); iterator = input.end(); WRITE("d",1);} else {iterator = input.insert(iterator,100);printInputAfterInsertion(iterator);}break;
                case 101:     cursorY++; if(iterator == input.end()){input.push_back(101); iterator = input.end(); WRITE("e",1);} else {iterator = input.insert(iterator,101);printInputAfterInsertion(iterator);}break;
                case 102:     cursorY++; if(iterator == input.end()){input.push_back(102); iterator = input.end(); WRITE("f",1);} else {iterator = input.insert(iterator,102);printInputAfterInsertion(iterator);}break;
                case 103:     cursorY++; if(iterator == input.end()){input.push_back(103); iterator = input.end(); WRITE("g",1);} else {iterator = input.insert(iterator,103);printInputAfterInsertion(iterator);}break;
                case 104:     cursorY++; if(iterator == input.end()){input.push_back(104); iterator = input.end(); WRITE("h",1);} else {iterator = input.insert(iterator,104);printInputAfterInsertion(iterator);}break;
                case 105:     cursorY++; if(iterator == input.end()){input.push_back(105); iterator = input.end(); WRITE("i",1);} else {iterator = input.insert(iterator,105);printInputAfterInsertion(iterator);}break;
                case 106:     cursorY++; if(iterator == input.end()){input.push_back(106); iterator = input.end(); WRITE("j",1);} else {iterator = input.insert(iterator,106);printInputAfterInsertion(iterator);}break;
                case 107:     cursorY++; if(iterator == input.end()){input.push_back(107); iterator = input.end(); WRITE("k",1);} else {iterator = input.insert(iterator,107);printInputAfterInsertion(iterator);}break;
                case 108:     cursorY++; if(iterator == input.end()){input.push_back(108); iterator = input.end(); WRITE("l",1);} else {iterator = input.insert(iterator,108);printInputAfterInsertion(iterator);}break;
                case 109:     cursorY++; if(iterator == input.end()){input.push_back(109); iterator = input.end(); WRITE("m",1);} else {iterator = input.insert(iterator,109);printInputAfterInsertion(iterator);}break;
                case 110:     cursorY++; if(iterator == input.end()){input.push_back(110); iterator = input.end(); WRITE("n",1);} else {iterator = input.insert(iterator,110);printInputAfterInsertion(iterator);}break;
                case 111:     cursorY++; if(iterator == input.end()){input.push_back(111); iterator = input.end(); WRITE("o",1);} else {iterator = input.insert(iterator,111);printInputAfterInsertion(iterator);}break;
                case 112:     cursorY++; if(iterator == input.end()){input.push_back(112); iterator = input.end(); WRITE("p",1);} else {iterator = input.insert(iterator,112);printInputAfterInsertion(iterator);}break;
                case 113:     cursorY++; if(iterator == input.end()){input.push_back(113); iterator = input.end(); WRITE("q",1);} else {iterator = input.insert(iterator,113);printInputAfterInsertion(iterator);}break;
                case 114:     cursorY++; if(iterator == input.end()){input.push_back(114); iterator = input.end(); WRITE("r",1);} else {iterator = input.insert(iterator,114);printInputAfterInsertion(iterator);}break;
                case 115:     cursorY++; if(iterator == input.end()){input.push_back(115); iterator = input.end(); WRITE("s",1);} else {iterator = input.insert(iterator,115);printInputAfterInsertion(iterator);}break;
                case 116:     cursorY++; if(iterator == input.end()){input.push_back(116); iterator = input.end(); WRITE("t",1);} else {iterator = input.insert(iterator,116);printInputAfterInsertion(iterator);}break;
                case 117:     cursorY++; if(iterator == input.end()){input.push_back(117); iterator = input.end(); WRITE("u",1);} else {iterator = input.insert(iterator,117);printInputAfterInsertion(iterator);}break;
                case 118:     cursorY++; if(iterator == input.end()){input.push_back(118); iterator = input.end(); WRITE("v",1);} else {iterator = input.insert(iterator,118);printInputAfterInsertion(iterator);}break;
                case 119:     cursorY++; if(iterator == input.end()){input.push_back(119); iterator = input.end(); WRITE("w",1);} else {iterator = input.insert(iterator,119);printInputAfterInsertion(iterator);}break;
                case 120:     cursorY++; if(iterator == input.end()){input.push_back(120); iterator = input.end(); WRITE("x",1);} else {iterator = input.insert(iterator,120);printInputAfterInsertion(iterator);}break;
                case 121:     cursorY++; if(iterator == input.end()){input.push_back(121); iterator = input.end(); WRITE("y",1);} else {iterator = input.insert(iterator,121);printInputAfterInsertion(iterator);}break;
                case 122:     cursorY++; if(iterator == input.end()){input.push_back(122); iterator = input.end(); WRITE("z",1);} else {iterator = input.insert(iterator,122);printInputAfterInsertion(iterator);}break;
                case 123:     cursorY++; if(iterator == input.end()){input.push_back(123); iterator = input.end(); WRITE("{",1);} else {iterator = input.insert(iterator,123);printInputAfterInsertion(iterator);}break;
                case 124:     cursorY++; if(iterator == input.end()){input.push_back(124); iterator = input.end(); WRITE("|",1);} else {iterator = input.insert(iterator,124);printInputAfterInsertion(iterator);}break;
                case 125:     cursorY++; if(iterator == input.end()){input.push_back(125); iterator = input.end(); WRITE("}",1);} else {iterator = input.insert(iterator,125);printInputAfterInsertion(iterator);}break;
                case 126:     cursorY++; if(iterator == input.end()){input.push_back(126); iterator = input.end(); WRITE("~",1);} else {iterator = input.insert(iterator,126);printInputAfterInsertion(iterator);}break;
                case BACKSPACE: printInputAfterDeletion(); break;
             }
             
            
}

//--------------------------------------------------------------------------------------

        
        
        





/*int main()
{
    
    try {
        Terminal terminal(STDIN_FILENO);
        terminal.enterRawMode();
        terminal.processKeyPress();
        //terminal.debugWindowSize();
        
    }

    catch(std::system_error& error) {
        std::cerr << "Error1: " << error.code().message() << ".\nAborting..." << std::endl;
        return -1;            
    }
    catch(std::string& error) {
        std::cerr << "Error2: " << error << "\nAborting..." << std::endl;
    }
    

    return 0;
}*/