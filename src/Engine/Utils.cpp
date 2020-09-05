#include "Utils.hpp"

std::string getNextString(std::istringstream& iss) {
    std::string str;
    std::getline(iss, str, '"');
    std::getline(iss, str, '"');

    return str;
}

math::scalar getNextFloat(std::istringstream& iss) {
    math::scalar v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v;
    std::getline(iss, garb, '"');

    return v;
}

math::vec2 getNextVec2(std::istringstream& iss) {
    math::vec2 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y;
    std::getline(iss, garb, '"');

    return v;
}

math::vec3 getNextVec3(std::istringstream& iss) {
    math::vec3 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z;
    std::getline(iss, garb, '"');

    return v;
}

math::vec4 getNextVec4(std::istringstream& iss) {
    math::vec4 v;
    std::string garb;
    std::getline(iss, garb, '"');
    iss >> v.x >> v.y >> v.z >> v.w;
    std::getline(iss, garb, '"');

    return v;
}

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

        if (erasing && inputBuf[n] == '/' && inputBuf[n - 1] == '\7') {
            erasing = false;
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

            lines.push_back(ptr+off);
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
    
    // send outputs
    newBufSize = bufLength + 1;
    return newBuffer;
}


KVH::KVH() {
    numChildren = 0;
    children = nullptr;
}

KVH::~KVH() {
}

void KVH::Destroy() {
    // recursively destroy children from the bottom up
    if (numChildren > 0 && children) {
        for (int n = 0; n < numChildren; n++) {
            if (children[n]) {
                children[n]->Destroy();
                free(children[n]);
                children[n] = 0;
            }
        }
        numChildren = 0;
    }

    free(children);
    children = 0;
}

void KVH::CreateAsRoot(char* buffer, size_t bufferSize) {
    AddChild(2);

    children[0]->AddChild(3);
    children[1]->AddChild(1);
}

void KVH::AddChild(u32 num) {
    children = (KVH**)realloc(children, (numChildren+num) * sizeof(KVH*));

    for (int n = numChildren; n < numChildren + num; n++) {
        // initialize the new children
        children[n] = (KVH*)malloc(sizeof(KVH));
        
        children[n]->numChildren = 0;
        children[n]->children = 0;
    }

    numChildren += num;
}