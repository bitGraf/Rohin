#ifndef __MESSAGE__
#define __MESSAGE__

#include <string>

enum class MessageType {
    empty,
    standard,
    quit
};

class Message {
public:
    Message();
    Message(MessageType _type);
    Message(MessageType _type, std::string _text);
    ~Message();

//private:
    MessageType type;
    std::string text;
};

#endif