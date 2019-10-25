#ifndef __MESSAGE__
#define __MESSAGE__

#include <string>

class Message {
public:
    enum Type {
        empty = 0,
        standard,
        quit,
        log
    };

    Message();
    Message(Type _type);
    Message(Type _type, std::string _text);
    ~Message();

    Type type;
    std::string text;
};

#endif