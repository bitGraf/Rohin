#include "Console.hpp"

HANDLE Console::hConsole;
CONSOLE_SCREEN_BUFFER_INFO Console::csbi;
COORD Console::cursorPos;

std::vector<std::string> Console::textBuffer;
int Console::bufferPos;

Console::eConsoleStatus Console::status;
bool Console::forceKill;

std::mutex Console::status_lock;
std::thread Console::myThread;
bool Console::threaded;

Console::Console() {
}


void Console::update(double dt) {
    switch (status) {
    case eConsoleStatus::sleep: {
        // Do nothing this update cycle
        printf(".");
    } break;
    case eConsoleStatus::update: {
        // redraw the console

        status = eConsoleStatus::sleep;

        clear();

        int numMessages = textBuffer.size();

        cursorPos = { 0, 0 };
        setCursorPos(cursorPos);

        if (numMessages > CONSOLE_MAX_MESSAGES) {
            bufferPos = numMessages - CONSOLE_MAX_MESSAGES;
        }

        for (int n = bufferPos; n < numMessages; n++) {
            printf(textBuffer[n].c_str());
            printf("\n");
            cursorPos.Y++;
        }
    } break;
    case eConsoleStatus::prompt: {
        // ask for input

        std::string input;

        std::cout << "> ";
        std::cin >> input;

        input.insert((size_t)0, "Command entered: ", (size_t)17);
        logMessage(input);

        status = eConsoleStatus::update;
    } break;
    case eConsoleStatus::kill: {
        logMessage("Killing Console");
    } break;
    }

    if (forceKill) {
        status = eConsoleStatus::kill;
    }
}

void Console::handleMessage(Message msg) {
    std::ostringstream stringStream;

    stringStream <<
        Message::getNameFromMessageType(msg.type);
    for (u8 n = 0; n < msg.numArgs; n++) {
        stringStream << ", " << std::to_string(msg.data[n]);
    }
    /*if (msg.type > 0) {
        if (msg.type == Configuration::getMessageType("WindowMove")) {
            int xpos, ypos;
            Configuration::decodeData(msg, xpos, ypos);
            stringStream << ": xpos=" << xpos << ", ypos=" << ypos;
        }
    }*/
    std::string copyOfStr = stringStream.str();

    if (threaded) {
        textBuffer.push_back(copyOfStr);

        status_lock.lock();
        status = eConsoleStatus::update;
        status_lock.unlock();
    }
    else {
        logMessage(copyOfStr);
    }
}

void Console::destroy() {
    status_lock.lock();
    status = eConsoleStatus::kill;
    forceKill = true;
    status_lock.unlock();
}

void Console::create() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    bufferPos = 0;
    status = eConsoleStatus::update;
    logMessage("Console created");
    forceKill = false;
    update(0);
    threaded = false;
}



void Console::prompt() {
    // Tell the console to ask for input
    status_lock.lock();
    status = eConsoleStatus::prompt;
    status_lock.unlock();
}

void Console::startListening(bool separateThread) {
    threaded = separateThread;

    if (threaded) {
        myThread = std::thread(&Console::startThread);
    }
}

void Console::startThread() {
    if (threaded) {
        update(0);

        auto nowTime = std::chrono::system_clock::now();

        int num = 0;
        while (status != eConsoleStatus::kill) {
            nowTime = std::chrono::system_clock::now();
            update(0);

            //limit this loop to the update rate set
            nowTime += std::chrono::milliseconds(MILLS_PER_UPDATE);
            std::this_thread::sleep_until(nowTime);
        }
        update(0);
    }
}

void Console::rejoin() {
    if (threaded) {
        myThread.join();
        threaded = false;
    }
}

void Console::logMessage(std::string text) {
    std::cout << "< " << text << std::endl;
}




bool Console::clear() {
    cursorPos = { 0, 0 };
    DWORD cCharsWritten;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return false;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(
        hConsole,
        (TCHAR) ' ',
        dwConSize,
        cursorPos,
        &cCharsWritten)) {
        return false;
    }

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return false;
    }

    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer 
        csbi.wAttributes, // Character attributes to use
        dwConSize,        // Number of cells to set attribute 
        cursorPos,      // Coordinates of first cell 
        &cCharsWritten)) // Receive number of characters written
    {
        return false;
    }

    SetConsoleCursorPosition(hConsole, cursorPos);

    return true;
}

void Console::setCursorPos(COORD newPos) {
    if (!SetConsoleCursorPosition(hConsole, newPos)) {
        printf("Error setting cursor position!\n");
    }
}

COORD Console::getCursorPos() {
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        printf("Error getting cursor position!\n");
    }

    return csbi.dwCursorPosition;
}