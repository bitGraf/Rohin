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

void FileSystem::sys_create(ConfigurationManager* configMgr) {
    // TODO: Make this not poopoo

    getDirectory(rootDirectory);
    strcpy(rootDirectory, "D:\\GameDev\\Big-Disc-Mk-II\\run_tree");
    setDirectory(rootDirectory);
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
void FileSystem::setRootDirectory(char directory[128]) {
    setDirectory(directory);
}