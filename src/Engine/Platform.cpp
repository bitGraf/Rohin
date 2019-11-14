#include "Platform.hpp"

#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)

int getDirectory(char* _curDirectory) {
    int length = GetModuleFileName(NULL, _curDirectory, MAX_PATH);

    int i = 0;
    for (i = length - 1; _curDirectory[i] != '\\'; i--) {
    }
    _curDirectory[i] = '\\';
    _curDirectory[i + 1] = '\0';

    //printf("Executable location is '%s'\n", _curDirectory);

    return i+1;
}

void setDirectory(char* _newDirectory) {
    //printf("Setting working directory to '%s'.\n", _newDirectory);
    //Console::logMessage(_newDirectory);
    if (!SetCurrentDirectory(_newDirectory)) {
        //Console::logMessage("Failed to set directory: ");
        //printf("SetCurrentDirectory failed (%d)\n", GetLastError());
    }
}

/*
struct process {
PROCESS_INFORMATION p_info;
STARTUPINFO s_info;
};

int create_process(process* p, const char* exe_path, char* cmd_line_args) {
p->s_info.cb = sizeof(STARTUPINFO);

return CreateProcessA(
exe_path,
cmd_line_args,
NULL,
NULL,
FALSE,
CREATE_NEW_CONSOLE, // <-- here
NULL,
NULL,
&p->s_info,
&p->p_info
);
}
*/

#elif __APPLE__
// All Apple platforms
void getCurrentDirectory(char* _curDirectory) {
    printf("Apple: get current directory: ");
    printf("NOT IMPLEMENTED\n");
}

void setCurrentDirectory(char* _curDirectory) {
    printf("Apple: set current directory: ");
    printf("NOT IMPLEMENTED\n");
}

#elif __linux__
// linux
void getCurrentDirectory(char* _curDirectory) {
    printf("Linux: get current directory: ");
    printf("NOT IMPLEMENTED\n");
}

void setCurrentDirectory(char* _curDirectory) {
    printf("Linux: set current directory: ");
    printf("NOT IMPLEMENTED\n");
}
#endif