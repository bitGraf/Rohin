#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <iostream>
#include <string>
#include <sstream>
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

/// Console Status
enum class eConsoleStatus {
    sleep,
    update,
    prompt,
    kill
};

class MessageBus;

/**
  * \class Console
  *
  * \ingroup Systems
  *
  * \brief Command Console Wrapper
  * 
  * This is intended to be a wrapper for a 
  * command console that can receive and send
  * messages on the MessageBus
  * 
  * \author $Author: bv $
  * 
  * \version $Revision: 1.5 $
  * 
  * \date $Date: 2005/04/14 14:16:20 $
  */

class Console
{
public:
    /// Empty constructor
    Console();
    ~Console();

    /// Initialize console with MessageBus
    void create(MessageBus* _msgBus);

    /** Run this method on a separate thread
      * to begin listening for messages
      * and output log messages
      */
    void startListening();

    /// Post a message to be printed
    void logMessage(const char* text);
    /// Post a message to be printed
    void logMessage(const char* text, int count, ...); // ONLY ALLOWS INTS
    /// Post a message to be printed
    void logMessage(std::string text);

    /// Handle standard event message
    void handleMessage(Message msg);

    /// End this console instance
    void killConsole();
    /// Ask console for input
    void prompt();

    /// Update Console
    void update();
private:
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