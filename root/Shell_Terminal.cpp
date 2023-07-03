#include <Shell_Terminal.hpp>
#include <list>

void Shell_Terminal::transferInputTo_InputWords()
{
    std::list<unsigned char> input = terminal.getInput();

    auto iter = input.cbegin();
    size_t spaceCount = 0, loopCount = 1, index_inputWords = 0;
    unsigned char c;
    while(iter != input.cend()) {
        if(index_inputWords < BUFFSIZE - 1) {
            c = *iter;
    //loopCounter is only used to eliminate all the spaces at the beginning of input when c
    //has not yet had the chance to be any other character except space. After it is
    //incremented once, it outlives its usefulness COMPLETELY.
            if(c == ' ') {
                if(loopCount == 1) {
                    ++iter;
                    continue;
                }
                ++spaceCount;
                if(spaceCount < 2) {
                    inputWords[index_inputWords] = c;
                    ++index_inputWords;
                }
                else
                    continue;
            }
            else {
                inputWords[index_inputWords] = c;
                ++index_inputWords;
                spaceCount = 0;
                ++loopCount;
            }
        }
        else
            return;
        ++iter;
    }
}

