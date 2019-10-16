#include "Console.hpp"

Console::Console(MessageBus* _msgBus) {
    msgBus = _msgBus;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    textField[0] = 0;
    numChars = 0;

    bufferPos = 0;
    hasChanged = true;
    update();
}

Console::~Console() {
    logMessage("destroying console");
}

void Console::update() {
    if (hasChanged) {
        hasChanged = false;

        if (!clear()) {
            printf("FAILED TO CLEAR SCREEN\N");
            system("pause");
        }

        int numMessages = textBuffer.size();

        cursorPos = { 0, 0 };
        setCursorPos(cursorPos);
        for (int n = bufferPos; n < numMessages; n++) {
            printf(textBuffer[n].c_str());
            printf("\n");
        }
        COORD savePos = getCursorPos();
        cursorPos = { 0,29 };
        setCursorPos(cursorPos);
        printf("[%d] ", textBuffer.size());
        for (int n = 0; n < numChars; n++) {
            printf("%c", textField[n]);
        }
        cursorPos = getCursorPos();
        setCursorPos(savePos);
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

    lock_text_vector.lock();
        textBuffer.push_back(m);
        hasChanged = true;
    lock_text_vector.unlock();
}

void Console::listen() {
    update();

    auto nowTime = std::chrono::system_clock::now();

    int num = 0;
    done = false;
    while (!done) {
        nowTime = std::chrono::system_clock::now();
        update();

        num++;
        if (num % 10 == 0) {
            //logMessage("10 cycles passed");
        }

        nowTime += std::chrono::milliseconds(MILLS_PER_UPDATE);
        std::this_thread::sleep_until(nowTime);
    }
    update();
}

void Console::killConsole() {
    lock_done_flag.lock();
        done = true;
    lock_done_flag.unlock();
}

void Console::keyPress(CONSOLE_KEY k, char c) {
    switch (k) {
    case CONSOLE_KEY::enter: {
        // pressed enter
        logMessage("command entered: ");
        logMessage(textField);
        numChars = 0;
    } break;
    case CONSOLE_KEY::letter: {
        // enter a character
        lock_key_press.lock();
            textField[numChars++] = c;
            hasChanged = true;
            textField[numChars] = 0;
        lock_key_press.unlock();
    } break;
    default: {
        // ignore
    } break;
    }
}