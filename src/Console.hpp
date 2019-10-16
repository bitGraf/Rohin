#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <iostream>
#include <string>
#include <vector>

#include "MessageBus.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <chrono>
#include <thread>
#include <mutex>

const int TARGET_RATE = 10;
const int MILLS_PER_UPDATE = 1000 / TARGET_RATE;

#define CONSOLE_TEXT_FIELD_SIZE 64

enum class CONSOLE_KEY {
    enter,
    letter
};

class Console
{
public:
    Console(MessageBus* _msgBus);
    ~Console();

    void update();

    void listen();

    void logMessage(const char* text);

    void killConsole();

    void keyPress(CONSOLE_KEY k, char c);

private:
    MessageBus* msgBus;
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    COORD cursorPos;

    bool clear();
    void  setCursorPos(COORD newPos);
    COORD getCursorPos();

    std::vector<std::string> textBuffer;
    char textField[CONSOLE_TEXT_FIELD_SIZE];
    int numChars;
    int bufferPos;

    bool done;

    bool hasChanged;

    std::mutex lock_text_vector, lock_done_flag, lock_key_press, lock_has_changed;
};

#endif