#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdio.h>

#include "Console.hpp"
#include "MessageBus.hpp"

#include "DataTypes.hpp"
#include "Platform.hpp"


class FileSystem {
public:
    /* 
    Plans and Features:
    -create a FileSystem object that "sits" somewhere on the disk. 
     Will probably change this to be mostly a static class that 
     just tracks the filesystem of the whole engine
    -Resource manager will always go through the Filesystem, all paths
     are relative to where the FileSystem is
    -Can track all the files in the system and notify if any 
     are updated during runtime (for hot-swapping)
    -Eventually will be made into its own thread to do asynchronus 
     fileIO
    */
    FileSystem();
    ~FileSystem();

    void create();
    void destroy();
    void setMessageBus(MessageBus* _messageBus);
    void setConsole(Console* console);

    /* File IO */
    bool syncReadFile(const char* filepath, u8* buffer, size_t bufferSize, size_t& rBytesRead);

    /* File System Navigation */
    void setRootDirectory(char directory[128]);

private:
    MessageBus* m_msgBus;
    Console*    m_console;

    char rootDirectory[MAX_PATH];
};

#endif