#include "FileSystem.hpp"

FileSystem::FileSystem() : exeLoc{ 0 }
{
}

FileSystem::~FileSystem() {
}

FileSystem* FileSystem::_singleton = 0;

FileSystem* FileSystem::GetInstance() {
    if (!_singleton) {
        _singleton = new FileSystem;
    }
    return _singleton;
}

bool FileSystem::Destroy() {
    fileList.clear();

    if (_singleton) {
        delete _singleton;
        _singleton = 0;
    }
    return true;
}

bool FileSystem::Init(char* directory) {
    /* Strip the executeable name off the end */
    auto L = strlen(directory) - 1;

    char c = directory[L];
    while (c != '\\' && c != '/') {
        if (L == 0) {
            L = strlen(directory) - 1;
            break;
        }
        c = directory[--L];
    }

    memcpy(exeLoc, directory, L + 1);

    char currDir[128];
    Console::logMessage("CWD: " + std::string(cwd(currDir, sizeof currDir)));

    if (0 == cd(exeLoc)) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logError("Error changing directory.");
        return false;
    }

#ifdef _WIN32
    if (0 == cd("../run_tree/")) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logError("Error changing directory.");
        return false;
    }
#else
    if (0 == cd("../run_tree/")) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logError("Error changing directory.");
        return false;
    }
#endif

    /* Register for events */
    Register(eveCheckFileMod, this, (Callback)&FileSystem::checkForFileUpdates);

    return true;
}

void FileSystem::checkForFileUpdates(void*, u32) {
    for (int n = 0; n < fileList.size(); n++) {
        fileList[n];
        
        struct _stat buf;
        int result = _stat(fileList[n].filename, &buf);

        if (result != 0) {
        Console::logError("stat failed: Error code (%d)", result);
        }
        else {
            if (buf.st_mtime > fileList[n].time
                ||
                buf.st_size != fileList[n].size) {

                /* File has been modified */
                fileList[n].time = buf.st_mtime;
                fileList[n].size = buf.st_size;

                Post(Events::eveFileUpdate, fileList[n].filename, strlen(fileList[n].filename));
            }
        }
    }
}

std::vector<std::string> FileSystem::getAllFilesOnPath(std::string searchPath) {
    // watch resource locations
    char currDir[128];
    std::string path = std::string(cwd(currDir, sizeof currDir)) + "\\" + searchPath;
    std::vector<std::string> files;

    DIR* dir;
    dirent* ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') { // ignore all hidden entries
                switch (ent->d_type) {
                case 16384: {
                    // is a folder?
                } break;
                case 32768: {
                    // is a file
                    files.push_back(ent->d_name);
                } break;
                }
            }
        }
        closedir(dir);
    }
    else {
        printf("Could not open path: [%s]\n", path.c_str());
    }

    return files;
}

// This allocates memory. Returns nullptr and 0 bytes read if failure to load
char* FileSystem::readAllBytes(std::string filepath, size_t& bytesRead, bool shouldWatchFile) {
    //open file
    std::ifstream infile(filepath, std::ifstream::binary);

    //get length of file
    if (!infile.seekg(0, std::ios::end))
        Console::logError("Failed to seek to eof");
    size_t length = infile.tellg();
    if (!infile.seekg(0, std::ios::beg))
        Console::logError("Failed to seek to bof");

    if (length > 0 && length != std::string::npos) {
        //create buffer
        char* buffer = (char*)malloc(length);

        //read file
        if (!infile.read(buffer, length))
            Console::logError("Failed to read the file");

        if (infile) {
            // successful read
            bytesRead = length;

            if (shouldWatchFile) {
                watchFile(filepath);
            }
            return buffer;
        }
        else {
            Console::logError("Error: %zu/%zu bytes read\n", infile.gcount(), length);
            bytesRead = 0;
            return nullptr;
        }
    }
    else {
        bytesRead = 0;
        return nullptr;
    }
}

void FileSystem::watchFile(std::string filepath, bool empty) {
    // Create file registry to track updates
    FileEntry fe;
    strcpy(fe.filename, filepath.c_str());

    if (empty) {
        // placeholder entry
        fe.time = 0;
        fe.size = 0;
        fileList.push_back(fe);
    }
    else {
        struct _stat buf;
        int result = _stat(fe.filename, &buf);

        if (result != 0) {
            Console::logError("stat failed: Error code (%d)", result);
        }
        else {
            fe.time = buf.st_mtime;
            fe.size = buf.st_size;

            fileList.push_back(fe);
        }
    }
}