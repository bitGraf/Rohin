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
    Console::logMessage("CWD: " + std::string(cwd(currDir, sizeof currDir)));

    if (0 == cd(exeLoc)) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logMessage("Error changing directory.");
    }

#ifdef _WIN32
    if (0 == cd("../../run_tree/")) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logMessage("Error changing directory.");
    }
#else
    if (0 == cd("../run_tree/")) {
        Console::logMessage("CWD changes to: " + std::string(cwd(currDir, sizeof currDir)));
    }
    else {
        Console::logMessage("Error changing directory.");
    }
#endif



    return this;
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

// This allocates memory
char* FileSystem::readAllBytes(std::string filepath, size_t& bytesRead) {
    //open file
    std::ifstream infile(filepath, std::ifstream::binary);

    //get length of file
    if (!infile.seekg(0, std::ios::end))
        printf("Error\n");
    size_t length = infile.tellg();
    if (!infile.seekg(0, std::ios::beg))
        printf("Error\n");

    if (length > 0 && length != std::string::npos) {
        //create buffer
        char* buffer = (char*)malloc(length);

        //read file
        if (!infile.read(buffer, length))
            printf("Error\n");

        if (infile) {
            // successful read
            bytesRead = length;
            return buffer;
        }
        else {
            printf("Error: %zu/%zu bytes read\n", infile.gcount(), length);
            bytesRead = 0;
            return nullptr;
        }
    }
    else {
        bytesRead = 0;
        return nullptr;
    }
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
