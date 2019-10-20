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

#include "optick.h"

const int TARGET_RATE = 10; // Hz
const int MILLS_PER_UPDATE = 1000 / TARGET_RATE;

#define CONSOLE_MAX_MESSAGES 30

enum class eConsoleStatus {
    sleep,
    update,
    prompt,
    kill
};

class Console
{
public:
    Console();
    ~Console();

    void create(MessageBus* _msgBus);
    void startListening();

    void logMessage(const char* text);
    void logMessage(const char* text, int count, ...); // ONLY ALLOWS INTS
    void logMessage(std::string text);

    void killConsole();
    void prompt();

private:
    void update();
    bool clear();
    void  setCursorPos(COORD newPos);
    COORD getCursorPos();

    MessageBus* msgBus;
    HANDLE hConsole;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    COORD cursorPos;

    std::vector<std::string> textBuffer;
    int bufferPos;

    eConsoleStatus status;
    bool forceKill;

    std::mutex status_lock;
};

#endif