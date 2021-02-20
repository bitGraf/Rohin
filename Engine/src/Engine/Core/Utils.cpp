#include "enpch.hpp"
#include "Utils.hpp"

u32 hash_djb2(unsigned char* str) {
    u32 hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

stringID operator"" _sid(const char *input, size_t s) {
    return hash_djb2((unsigned char*)input);
}

char* StripComments(char* inputBuf, size_t inputBufSize, size_t& newBufSize) {
    bool erasing = false;

    for (size_t n = 0; n < inputBufSize; n++) {
        if (inputBuf[n] == '/' && inputBuf[n + 1] == '*') {
            erasing = true;
        }

        if (erasing && inputBuf[n] == '*' && inputBuf[n + 1] == '/') {
            erasing = false;
            inputBuf[n] = '\7';
            n++;
            inputBuf[n] = '\7';
        }

        if (erasing) {
            inputBuf[n] = '\7';
        }
    }

    std::string cleanData(inputBuf, inputBufSize);
    cleanData.erase(std::remove(cleanData.begin(), cleanData.end(), '\7'), cleanData.end());

    char* tmp = (char*)malloc(cleanData.size() + 1);
    memcpy(tmp, cleanData.data(), cleanData.size());
    tmp[cleanData.size()] = 0;

    std::vector<char*> lines;
    lines.reserve(100);
    char* ptr;
    ptr = strtok(tmp, "\r\n");
    while (ptr != NULL) {
        if (ptr[0] != '#') {
            // find first nonwhitespace character
            size_t off = 0;

            while (ptr[off] == ' ')
                off++;

            // find any trailing comments
            if (stringContainsChar(ptr + off, '#')) {
                // theres a trailing comment on this line
                // find where it is and mark it as the end
                size_t eol = strlen(ptr);
                size_t end = off;
                while (end < eol && ptr[end] != '#') {
                    end++;
                }
                if (ptr[end - 1] == ' ') {
                    end--;
                }
                ptr[end] = 0;
                lines.push_back(ptr + off);
                ptr += eol;
            }
            else {
                // Add the whole line
                lines.push_back(ptr + off);
            }
        }

        ptr = strtok(NULL, "\r\n");
    }

    size_t bufLength = 0, loc = 0;
    for (int n = 0; n < lines.size(); n++) {
        bufLength += strlen(lines[n]);
    }

    char* newBuffer = (char*)malloc(bufLength + 1);
    for (int n = 0; n < lines.size(); n++) {
        size_t len = strlen(lines[n]);
        memcpy(newBuffer + loc, lines[n], len);
        loc += len;
    }
    newBuffer[bufLength] = 0;

    // temp free for now
    free(tmp);

    bool inQuotes = false;
    size_t numSpaces = 0;
    for (size_t n = 0; n < strlen(newBuffer); n++) {
        if (newBuffer[n] == '\"') {
            inQuotes = !inQuotes;
        }

        if (!inQuotes) {
            if (newBuffer[n] == ' ' ||
                newBuffer[n] == '\t'
                )
                numSpaces++;
        }
    }

    // there are X spaces in the buffer
    if (numSpaces > 0) {
        // realloc a new buffer
        bufLength -= numSpaces;
        char* spacelessBuf = (char*)malloc(bufLength + 1);
        size_t loc = 0;
        inQuotes = false;
        for (size_t n = 0; n < strlen(newBuffer); n++) {
            if (newBuffer[n] == '\"') {
                inQuotes = !inQuotes;
            }

            if (inQuotes) {
                spacelessBuf[loc++] = newBuffer[n];
            } else if (newBuffer[n] != ' ' && newBuffer[n] != '\t') {
                spacelessBuf[loc++] = newBuffer[n];
            }
        }
        spacelessBuf[bufLength] = 0;
        free(newBuffer);

        newBufSize = bufLength + 1;
        return spacelessBuf;
    }
    else {
        // no whitespace to remove
        newBufSize = bufLength + 1;
        return newBuffer;
    }
}


bool stringContainsChar(char* str, char ch) {
    for (int n = 0; n < strlen(str); n++) {
        if (str[n] == ch)
            return true;
    }

    return false;
}

size_t findChar(char* buffer, char ch) {
    for (int n = 0; n < strlen(buffer); n++) {
        if (buffer[n] == ch)
            return n;
    }
    return strlen(buffer);
}