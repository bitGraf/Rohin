#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdio.h>

#include "DataTypes.hpp"
#include "Platform.hpp"

#include <cstring>

#include <dirent.h>
#include <fstream>
#include <vector>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "Message\EMS.hpp"


struct FileEntry {
    char filename[128];
    __time64_t time;
    _off_t size;

    FileEntry() : filename{ 0 }, time(0), size(0) {}
};

class FileSystem : MessageReceiver {
public:
    static FileSystem* GetInstance();

    FileSystem();
    ~FileSystem();

    bool Init(char* directory);
    bool Destroy();


    /* Allocates Memory */
    char* readAllBytes(std::string filepath, size_t& bytesRead, bool shouldWatchFile = false);

    void watchFile(std::string filepath, bool empty = false);
    void checkForFileUpdates(void* data = 0, u32 size = 0);

    std::vector<std::string> getAllFilesOnPath(std::string path);

private:
    char exeLoc[1024];
    std::vector<FileEntry> fileList;

    static FileSystem* _singleton;
};

#endif
