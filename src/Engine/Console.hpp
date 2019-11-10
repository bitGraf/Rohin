#ifndef __CONSOLE_H__
#define __CONSOLE_H__

class Console;

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

#include "Platform.hpp"
#include "Message\Message.hpp"
#include "Configuration.hpp"

const int TARGET_RATE = 10; // Hz
const int MILLS_PER_UPDATE = 1000 / TARGET_RATE;

#define CONSOLE_MAX_MESSAGES 30

class Console
{
public:
    static void create();
    static void update(double dt);
    static void handleMessage(Message msg);
    static void destroy();

    static void logMessage(std::string text);

    static void prompt();
    static void startListening(bool separateThread);
    static void rejoin();

private:
    Console();

    enum class eConsoleStatus {
        sleep,
        update,
        prompt,
        kill
    };

    static bool clear();
    static void  setCursorPos(COORD newPos);
    static COORD getCursorPos();

    static HANDLE hConsole;
    static CONSOLE_SCREEN_BUFFER_INFO csbi;
    static COORD cursorPos;

    static std::vector<std::string> textBuffer;
    static int bufferPos;

    static eConsoleStatus status;
    static bool forceKill;

    static std::mutex status_lock;
    static std::thread myThread;
    static bool threaded;
    static void startThread();
};

#endif