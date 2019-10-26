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

#include "CoreSystem.hpp"

const int TARGET_RATE = 10; // Hz
const int MILLS_PER_UPDATE = 1000 / TARGET_RATE;

#define CONSOLE_MAX_MESSAGES 30

class Console : public CoreSystem
{
public:
    Console();
    ~Console();

    void update(double dt);
    void handleMessage(Message msg);
    void destroy();
    void sys_create(ConfigurationManager* configMgr);

    void prompt();
    void startListening();

private:
    enum class eConsoleStatus {
        sleep,
        update,
        prompt,
        kill
    };

    bool clear();
    void  setCursorPos(COORD newPos);
    COORD getCursorPos();

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