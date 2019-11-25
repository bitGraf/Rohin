#include "FileSystem.hpp"

FileSystem::FileSystem() {
}

FileSystem::~FileSystem() {
}

void FileSystem::update(double dt) {
}

void FileSystem::handleMessage(Message msg) {
}

void FileSystem::destroy() {
}

CoreSystem* FileSystem::create() {
    char currDir[128];
    std::cout << "CWD: " << cwd(currDir, sizeof currDir) << std::endl;

    if (0 == cd(exeLoc)) {
        std::cout << "CWD changed to: " << cwd(currDir, sizeof currDir) << std::endl;
    }
    else {
        std::cout << "Error changing directory." << std::endl;
    }

    if (0 == cd("../../run_tree/")) {
        std::cout << "CWD changed to: " << cwd(currDir, sizeof currDir) << std::endl;
    }
    else {
        std::cout << "Error changing directory." << std::endl;
    }

    return this;
}



/* File IO */
bool FileSystem::syncReadFile(
    const char* filepath, 
    u8* buffer, 
    size_t bufferSize, 
    size_t& out_bytesRead) {
    
    // read all data, blocking the current thread
    FILE* handle = fopen(filepath, "rb");

    if (handle) {
        size_t bytesRead = fread(buffer, 1, bufferSize, handle);
        fclose(handle);

        int err = ferror(handle);

        if (err == 0) {
            out_bytesRead = bytesRead;
            buffer[bytesRead] = 0;
            return true;
        }
    }

    out_bytesRead = 0;
    return false;
}

/* File System Navigation */
void FileSystem::setRootDirectory(char* directory) {
    auto L = strlen(directory) - 1;
    
    char c = directory[L];
    while (c != '\\' && c != '/') {
        c = directory[--L];
    }

    memcpy(exeLoc, directory, L+1);
}
