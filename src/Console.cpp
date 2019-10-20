#include "Console.hpp"

Console::Console() {
}

void Console::create(MessageBus* _msgBus) {
    msgBus = _msgBus;

    // TODO: decouple this from the main console:
    //       each console should have its own window
    //       and input/output buffer
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    bufferPos = 0;
    status = eConsoleStatus::update;
    logMessage("Console created");
    forceKill = false;
    update();
}

Console::~Console() {
}

void Console::update() {
    OPTICK_EVENT();

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
        logMessage("Killing console");
    } break;
    }

    if (forceKill) {
        status = eConsoleStatus::kill;
    }
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

void Console::logMessage(const char* text) {
    std::string m(text);
    logMessage(m);
}

void Console::logMessage(const char* text, int count, ...) {
    va_list args;
    va_start(args, count);

    std::string m(text);

    for (int n = 0; n < count-1; n++) {
        int num = va_arg(args, int);
        m.append(std::to_string(num));
        m.append(", ");
    }
    int num = va_arg(args, int);
    m.append(std::to_string(num));

    logMessage(m);
}

void Console::logMessage(std::string text) {
    status_lock.lock();
    textBuffer.push_back(text);
    status = eConsoleStatus::update;
    status_lock.unlock();
}

void Console::startListening() {
    OPTICK_THREAD("Console");

    update();

    auto nowTime = std::chrono::system_clock::now();

    int num = 0;
    while (status != eConsoleStatus::kill) {
        nowTime = std::chrono::system_clock::now();
        update();

        //limit this loop to the update rate set
        nowTime += std::chrono::milliseconds(MILLS_PER_UPDATE);
        std::this_thread::sleep_until(nowTime);
    }
    update();
}

void Console::killConsole() {
    status_lock.lock();
    status = eConsoleStatus::kill;
    forceKill = true;
    status_lock.unlock();
}

void Console::prompt() {
    // Tell the console to ask for input
    status_lock.lock();
    status = eConsoleStatus::prompt;
    status_lock.unlock();
}