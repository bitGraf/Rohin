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


MultiData DataNode::getData(std::string key) {
    MultiData blank;
    if (data.find(key) == data.end()) {
        Console::logError("DataNode [%s] tried to get data [%s] but could not find one...",
            name.c_str(), key.c_str());
        return blank;
    }
    return data[key];
}

DataNode DataNode::getChild(std::string key) {
    DataNode blank;
    if (children.find(key) == children.end()) {
        Console::logError("DataNode [%s] tried to get child [%s] but could not find one...",
            name.c_str(), key.c_str());
        return blank;
    }
    return children[key];
}

MultiData DataNode::getDataFromPath(std::string path) {
    std::string delimiter = ".";

    std::vector<std::string> tokens;

    size_t pos = 0;
    std::string token;
    while ((pos = path.find(delimiter)) != std::string::npos) {
        token = path.substr(0, pos);
        tokens.push_back(token);
        path.erase(0, pos + delimiter.length());
    }
    tokens.push_back(path);

    if (tokens.size() == 1) {
        return getData(token);
    }
    else if (tokens.size() == 2) {
        return getChild(tokens[0]).getData(tokens[1]);
    }
    else {
        Console::logError("Error getting data from path");
        return MultiData();
    }
}

void DataNode::CreateAsRoot(char* buffer, size_t bufSize) {
    name = "root";

    DataNode newNode;

    bool node = true;
    char* token = strtok(buffer, "{}");
    while (token != NULL) {
        if (node) {
            // clean out the shared newNode variable
            newNode.children.clear();
            newNode.data.clear();

            size_t nodeNameLength = findChar(token, ':'); // strlen(token) - 2;
            newNode.name = std::string(token, nodeNameLength);
        }
        else {
            if (stringContainsChar(token, '{')) {
                // need to be go on layer deeper
                // currently not possible though...
            }
            else {
                // split string into multiple multiData blocks
                size_t nameStart = 0, nameEnd = 0, dataStart = 0, dataEnd = 0;
                std::string currName;
                std::string currData;

                size_t len = strlen(token);
                for (int n = 0; n < len; n++) {
                    if (token[n] == ':') {
                        nameEnd = n;
                        dataStart = nameEnd + 1;

                        currName = std::string(token + nameStart, nameEnd - nameStart);
                    }

                    if (dataStart > 0 && (n == len - 1 || token[n] == ',')) {
                        dataEnd = n;
                        nameStart = dataEnd + 1;

                        currData = std::string(token + dataStart, dataEnd - dataStart);

                        // TODO: currently store everything as a string, need to decode
                        newNode.data[currName] = MultiData(currData);
                    }
                }
                children[newNode.name] = newNode;
            }
        }

        node = !node;
        token = strtok(NULL, "{}");
    }

    decodeMultiDataStrings();

    bool donezo = true;
}

void DataNode::decodeMultiDataStrings() {
    for (auto datum : data) {
        if (datum.second.asString().at(0) == '\"') {
            // starts with ", is a string
            size_t len = datum.second.asString().size();
            if (datum.second.asString().at(len-1) == '\"') {
                // check that it ends in a " as well
                data[datum.first] = MultiData(std::string(datum.second.asString(), 1, len - 2));
            }
        }
        else if (datum.second.asString().at(0) == 't' ||
            datum.second.asString().at(0) == 'T') {
            // starts with t, is true
            data[datum.first] = MultiData(true);
        }
        else if (datum.second.asString().at(0) == 'f' ||
            datum.second.asString().at(0) == 'F') {
            // starts with f, is false
            data[datum.first] = MultiData(false);
        }
        else if (datum.second.asString().at(0) == '[') {
            // starts with [, is a vector
            size_t len = datum.second.asString().size();
            if (datum.second.asString().at(len - 1) == ']') {
                // check that it ends in a " as well
                std::string s(datum.second.asString(), 1, len - 2);
                std::stringstream ss(s);
                math::vec4 v; char c;
                ss >> v.x >> c >> v.y >> c >> v.z >> c >> v.w;
                data[datum.first] = MultiData(v);
            }
        }
        else  {
            // its a number (either int or float)
            // assume its supposed to be a float, 
            // then set both int and float values
            float val = std::stof(datum.second.asString());
            data[datum.first] = MultiData(val);
            data[datum.first].intVal = (int)val;
        }
    }

    for (auto child : children) {
        child.second.decodeMultiDataStrings();
        children[child.first] = child.second;
    }
}