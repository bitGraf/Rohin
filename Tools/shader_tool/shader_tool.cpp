#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstdio>
#include <vector>

static int CurrentFileIndex;
static char FailedFiles[256][64];

static size_t StringLength(char* String) {
    size_t length = 0;
    for (char* Scan = String; *Scan; Scan++) {
        length = (Scan - String);
    }
    return length;
}

static char* FindLastOf(char* String, char Token) {
    size_t len = StringLength(String);
    for (size_t n = len-1; n > 0; n--) {
        if (String[n] == Token) {
            return &String[n];
        }
    }
    
    return nullptr;
}

static char* FindFirstOf(char* String, char Token) {
    size_t len = StringLength(String);
    for (size_t n = 0; n < len; n++) {
        if (String[n] == Token) {
            return &String[n];
        }
    }
    
    return nullptr;
}

static char* FindFirstWhitespace(char* String) {
    size_t len = StringLength(String);
    for (size_t n = 0; n < len; n++) {
        if ((String[n] == ' ') || (String[n] == '\t')) {
            return &String[n];
        }
    }
    
    return nullptr;
}

static bool StringMatch(char* String, const char Token[]) {
    int n = 0;
    for (char* Scan = String; *Scan; Scan++) {
        if (Token[n] == 0){
            return true;
        }
        if (*Scan != Token[n]) {
            return false;
        }
        n++;
    }
    return true;
}

static char* GetLine(char* Buffer, int* BytesLeft) {
    char* Scan = 0;
    for (Scan = Buffer; *Scan; Scan++) {
        (*BytesLeft)--;
        if (*Scan == '\n') {
            *Scan = 0;
            if (*(Scan-1) == '\r') {
                *(Scan-1) = 0;
            }
            break;
        }
    }
    return Scan+1;
}

static bool StringContainsString(char* String, const char Token[]) {
    size_t len = StringLength(String);
    for (size_t n = 0; n < len; n++) {
        if (StringMatch(String+n, Token)) {
            return true;
        }
    }
    return false;
}

static bool StringContainsStringWithinLength(char* String, const char Token[], size_t Length) {
    size_t len = StringLength(String);
    if (len > Length) {
        len = Length;
    }
    for (size_t n = 0; n < len; n++) {
        if (StringMatch(String+n, Token)) {
            return true;
        }
    }
    return false;
}

static char* EatWhiteSpace(char* String) {
    char* Scan = String;
    while ((*Scan == ' ') || (*Scan == '\t')) {
        Scan++;
    }
    return Scan;
}

static int ReadIntFromString(char* String, size_t Length) {
    return atoi(String);
}

static bool IsCustomType(char* Type, size_t Length) {
         if (StringContainsStringWithinLength(Type, "int",   Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "float", Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "vec2",  Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "vec3",  Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "vec4",  Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "mat4",  Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "sampler2D",  Length)) { return false; }
    else if (StringContainsStringWithinLength(Type, "samplerCube",  Length)) { return false; }

    return true;
}

static bool IsMoreThanOneUniform(char* Line) {
    size_t Length = StringLength(Line);
    if (StringContainsString(Line, "[") && StringContainsString(Line, "]")) {
        return true;
    }

    return false;
}

static int CountUniforms(char* Line) {
    char* Start = 1+FindFirstOf(Line, '[');
    char* End = FindFirstOf(Line, ']');
    size_t Length = End - Start;
    int Count = atoi(Start);
    if (Count == 0) {
        if (StringContainsStringWithinLength(Start, "MAX_LIGHTS", Length)) {
            Count = 32;
        } else if (StringContainsStringWithinLength(Start, "MAX_BONES", Length)) {
            Count = 128;
        } else {
            printf("Detected a non-number string in the uniform definition!\n  Could be a constant\n");
            printf("  What is the value of [%.*s]?\n", (int)Length, Start);
            scanf("%d", &Count);
            printf("  Using %d as the vaue!\n", Count);
        }
    }
    return Count;
}

static int GetCustomSize(char* Type, size_t Length, bool* Success) {
    if (StringContainsStringWithinLength(Type, "Light",   Length)) { 
        printf("  Detected struct uniform: %s [assuming size=%d]\n", Type, 6);
        //printf("    You should verify this to be sure\n");
        return 6; 
    }

    printf("!!!!!Unrecognized uniform type: %s!!!!!!\n", Type);
    *Success = false;
    return 0;
}

struct uniform_definition {
    char Name[64];
    char Type[64];
    int Size;
    int Count;
    int Location;
    bool SimpleType;
};

struct shader_definition {
    char Name[256];

    std::vector<uniform_definition> Uniforms;
};

static size_t FindNameLength(char* Name) {
    size_t Length = 0;
    for (char* Scan = Name; *Scan; Scan++) {
        if ((*Scan == ';') || (*Scan == '[')) {
            break;
        }
        Length++;
    }
    return Length;
}

static std::vector<shader_definition> ShaderList;
static bool ProcessShaderFile(char* Buffer, int BytesRead, char* ShaderName) {
    shader_definition Shader = {};
    strcpy(Shader.Name, ShaderName);

    bool Success = true;
    int LineNumber = 1;
    int CurrLocation = 1;
    char* Line = Buffer;
    char* NextLine = 0;
    do {
        NextLine = GetLine(Line, &BytesRead);

        // operate on Line
        //printf("Line: [%s]\n", Line);
        if (StringContainsString(Line, "uniform") && StringContainsString(Line, "location")) {
            //printf("[Line %d] Found a uniform definition: [%s]\n", LineNumber, Line);
            char* LocationIDString = EatWhiteSpace(FindFirstOf(Line, '=')+1);
            size_t LocationIDStringLength = (FindFirstOf(LocationIDString, ')') - LocationIDString);
            int LocationID = ReadIntFromString(LocationIDString, LocationIDStringLength);
            if (LocationID != CurrLocation) {
                printf("!!!!! Location mismatch!\n  Expected %d, found %d\n  Line: %s !!!!!\n", CurrLocation, LocationID, Line);
                Success = false;
            }

            char* UniformTypeString = EatWhiteSpace(FindFirstOf(Line, 'm')+1);
            size_t UniformTypeStringLength = (FindFirstWhitespace(UniformTypeString) - UniformTypeString);
            int LocationSize = 1;
            if (IsCustomType(UniformTypeString, UniformTypeStringLength)) {
                // do some extra processing
                LocationSize = GetCustomSize(UniformTypeString, UniformTypeStringLength, &Success);
            }
            int LocationCount = 1;
            if (IsMoreThanOneUniform(Line)) {
                // do some extra processing
                LocationCount = CountUniforms(Line);
            }
            int LocationAdvance = LocationSize * LocationCount;

            char* UniformName = UniformTypeString + UniformTypeStringLength;
            UniformName = FindFirstWhitespace(UniformName)+1;
            size_t UniformNameLength = FindNameLength(UniformName);

            uniform_definition Uniform = {};
            memcpy(Uniform.Type, UniformTypeString, UniformTypeStringLength);
            memcpy(Uniform.Name, UniformName, UniformNameLength);
            Uniform.Count = LocationCount;
            Uniform.Location = LocationID;
            Uniform.Size = LocationSize;
            if (LocationSize == 1) {
                Uniform.SimpleType = true;
            } else {
                Uniform.SimpleType = false;
                // define that
            }
            Shader.Uniforms.push_back(Uniform);

            CurrLocation += LocationAdvance;
        }

        // advance
        Line = NextLine;
        LineNumber++;
    } while (BytesRead);

    ShaderList.push_back(Shader);

    return Success;
}

static void AddToErrorList(char* cFileName) {
    strcpy(FailedFiles[CurrentFileIndex], cFileName);
    CurrentFileIndex++;
}

static void OpenShaderFile(char* FullPath, char* cFileName) {
    printf("Processing Shader: %-20s [%s]\n", cFileName, FullPath);

    HANDLE FileHandle = CreateFileA(FullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (FileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER FileSizeBytes;
        if (GetFileSizeEx(FileHandle, &FileSizeBytes)) {

            // use FileSizeBytes + 1 to allow a null-terminator to be added
            FileSizeBytes.QuadPart++;
            LPVOID Buffer = VirtualAlloc(0, FileSizeBytes.QuadPart,
                                         MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            DWORD BytesRead;
            if (Buffer) {
                if (ReadFile(FileHandle, Buffer, (DWORD)FileSizeBytes.QuadPart, &BytesRead, NULL)) {
                    ((char*)Buffer)[BytesRead] = 0;
                    // read the entire file into a buffer
                    if (!ProcessShaderFile((char*)Buffer, BytesRead, cFileName)) {
                        printf("Error processing file\n");
                        AddToErrorList(cFileName);
                    }
                } else {
                    printf("Error reading file\n");
                    AddToErrorList(cFileName);
                }
                VirtualFree(Buffer, 0, MEM_RELEASE);
            } else {
                printf("Error allocating buffer\n");
                AddToErrorList(cFileName);
            }
        } else {
            printf("Error getting file size\n");
            AddToErrorList(cFileName);
        }
        CloseHandle(FileHandle);
    } else {
        printf("Error creating file handle\n");
        AddToErrorList(cFileName);
    }
}

static void 
ProcessCommandLine(int argc, char** argv, char* PathToShaderDir, char* OutputCodePath) {
    if (argc == 1) {
        // run with no args
        char DefaultPathToShaders[] = "Game/run_tree/Data/Shaders/*";
        char DefaultPathToOutputCode[] = "Tools/shader_tool/out/shaders_generated.cpp";
        strcpy(PathToShaderDir, DefaultPathToShaders);
        strcpy(OutputCodePath, DefaultPathToOutputCode);
    } else if (argc == 3) {
        // Tools/shader_tool/unity_build/shader_tool.exe Game/run_tree/Data/Shaders/ Game/src/ShaderSrc/shaders_generated.cpp
        // Tools/shader_tool/unity_build/shader_tool.exe Game/run_tree/Data/Shaders/ Game/src/ShaderSrc/shaders_generated.cpp
        strcpy(PathToShaderDir, argv[1]);
        strcpy(OutputCodePath, argv[2]);

        size_t path_length = StringLength(PathToShaderDir);
        if (PathToShaderDir[path_length] != '*') {
            PathToShaderDir[path_length+1] = '*';
        }
    } else {
        // idk man
        printf("Incorrect args passed, shutting down...\n");
        PathToShaderDir = nullptr;
        OutputCodePath = nullptr;
    }
}

int main(int argc, char** argv) {
    system("cd");
    //HINSTANCE Instance = GetModuleHandleA(NULL);
    //LPSTR CommandLine = GetCommandLineA();
    //printf("Run with command line: [%s]\n", CommandLine);
    printf("Command line args:\n");
    for (int n = 0; n < argc; n++) {
        printf(" argv[%d] = %s\n", n, argv[n]);
    }
    char PathToShaders[256] = { 0 };
    char OutputCodePath[256] = { 0 };
    ProcessCommandLine(argc, argv, PathToShaders, OutputCodePath);
    if ((!PathToShaders[0]) || (!OutputCodePath[0])) {
        return -1;
    }
    printf("PathToShaders = %s\n", PathToShaders);
    printf("OutputCodePath = %s\n", OutputCodePath); 

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileA(PathToShaders, &ffd);

#if 0
    OpenShaderFile("W:/Rohin/Game/run_tree/Data/Shaders/test.glsl", "test.glsl", PathToOutputCode);
#else
    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        } else {
            LARGE_INTEGER filesize;
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
            char* Extension = FindLastOf(ffd.cFileName, '.');
            if (StringMatch(Extension, ".glsl")) {
                char FullPath[MAX_PATH] = { 0 };
                int n = 0;
                size_t path_length = StringLength(PathToShaders);
                for (n = 0; n < path_length; n++) {
                    FullPath[n] = PathToShaders[n];
                }
                size_t name_len = StringLength(ffd.cFileName)+1;
                for (int i = 0; i < name_len; i++) {
                    FullPath[n++] = ffd.cFileName[i];
                }

                OpenShaderFile(FullPath, ffd.cFileName);
                printf("================================================\n");
                printf("\n");
            }
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
#endif

    if (CurrentFileIndex > 0) {
        printf("\n\n\n!!!!! Some files failed !!!!!!\n");
        for (int n = 0; n < CurrentFileIndex; n++) {
            printf("Failed file: %s\n", FailedFiles[n]);
        }
    } else {
        printf("\n Success! 0 files failed. \n");

        HANDLE FileHandle = CreateFileA(OutputCodePath, 
                                        GENERIC_WRITE, FILE_SHARE_READ, NULL, 
                                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (FileHandle) {

            printf("Generating c++ code into %s\n\n", OutputCodePath);

            char DateString[256] = { 0 };
            GetDateFormatA(LOCALE_USER_DEFAULT, DATE_LONGDATE, NULL, NULL, DateString, sizeof(DateString));

            char EXEFilename[256] = { 0 };
            GetModuleFileNameA(NULL, EXEFilename, sizeof(EXEFilename));

            char HeaderBuffer[1024] = {0};
            DWORD HeaderBytesWritten = 0;
            DWORD ActBytesWritten;

            HeaderBytesWritten += wsprintfA(HeaderBuffer, "// Code generated by %s on %s\n", EXEFilename, DateString);
            WriteFile(FileHandle, HeaderBuffer, HeaderBytesWritten, &ActBytesWritten, NULL);

            for (int n = 0; n < ShaderList.size(); n++) {
                shader_definition Shader = ShaderList[n];

                printf("%s...", Shader.Name);
                char* period = FindFirstOf(Shader.Name, '.');
                *period = 0;

                char ShaderStructBuffer[4096] = { 0 };
                DWORD bytesWritten = 0;
                bytesWritten += wsprintfA(ShaderStructBuffer, "struct shader_%s {\n    uint32 Handle;\n\n", Shader.Name);
                for (int u = 0; u < Shader.Uniforms.size(); u++) {
                    uniform_definition Uniform = Shader.Uniforms[u];

                    if (Uniform.Count > 1) {
                        bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "    ShaderUniform_%s %s[%d];\n", Uniform.Type, Uniform.Name, Uniform.Count);
                        //printf("    ShaderUniform_%s %s[%d];\n", Uniform.Type, Uniform.Name, Uniform.Count);
                    } else {
                        bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "    ShaderUniform_%s %s;\n", Uniform.Type, Uniform.Name);
                        //printf("      ShaderUniform_%s %s;\n", Uniform.Type, Uniform.Name);
                    }
                }

                // write init function
                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "\n    void InitShaderLocs() {\n");
                for (int u = 0; u < Shader.Uniforms.size(); u++) {
                    uniform_definition Uniform = Shader.Uniforms[u];

                    if (Uniform.Count > 1) {
                        bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        for (int n = 0; n < %d; n++) {\n", Uniform.Count);
                        if (Uniform.SimpleType) {
                            bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Location = %d+n;\n", Uniform.Name, Uniform.Location);
                        } else {
                            if (StringMatch(Uniform.Type, "Light")) {
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Position.Location = %d+(n*%d);\n",  Uniform.Name, Uniform.Location+0, Uniform.Size);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Direction.Location = %d+(n*%d);\n", Uniform.Name, Uniform.Location+1, Uniform.Size);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Color.Location = %d+(n*%d);\n",     Uniform.Name, Uniform.Location+2, Uniform.Size);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Strength.Location = %d+(n*%d);\n",  Uniform.Name, Uniform.Location+3, Uniform.Size);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Inner.Location = %d+(n*%d);\n",     Uniform.Name, Uniform.Location+4, Uniform.Size);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].Outer.Location = %d+(n*%d);\n",     Uniform.Name, Uniform.Location+5, Uniform.Size);
                            } else {
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "            %s[n].{}.Location = %d+n;\n", Uniform.Name, Uniform.Location);
                            }
                        }
                        bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        }\n", Uniform.Count);
                        //bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "    ShaderUniform_%s %s[%d];\n", Uniform.Type, Uniform.Name, Uniform.Count);
                    } else {
                        if (Uniform.SimpleType) {
                            bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Location = %d;\n", Uniform.Name, Uniform.Location);
                        } else {
                            if (StringMatch(Uniform.Type, "Light")) {
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Position.Location = %d;\n",  Uniform.Name, Uniform.Location+0);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Direction.Location = %d;\n", Uniform.Name, Uniform.Location+1);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Color.Location = %d;\n",     Uniform.Name, Uniform.Location+2);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Strength.Location = %d;\n",  Uniform.Name, Uniform.Location+3);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Inner.Location = %d;\n",     Uniform.Name, Uniform.Location+4);
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.Outer.Location = %d;\n",     Uniform.Name, Uniform.Location+5);
                            } else {
                                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "        %s.{}.Location = %d;\n", Uniform.Name, Uniform.Location);
                            }
                        }
                    }
                }
                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "    }\n");

                bytesWritten += wsprintfA(ShaderStructBuffer+bytesWritten, "};\n");
                //printf("};\n");

                WriteFile(FileHandle, ShaderStructBuffer, bytesWritten, &ActBytesWritten, NULL);
                if (ActBytesWritten != bytesWritten) {
                    printf("whyyyy\n");
                }

                printf("Done.\n");
            }

            CloseHandle(FileHandle);
        } else {
            printf("!!!!! Failed to open output file!\n !!!!!");
        }
    }

    system("pause");
    return 0;
}