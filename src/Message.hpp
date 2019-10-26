#ifndef __MESSAGE__
#define __MESSAGE__

#include <string>

class Message {
public:
    enum class Type {
        empty,
        log,
        coreSystemCreate,
        inputEvent,
        windowEvent,
        gameEvent,
        renderEvent
    };

    Message();
    Message(Type _type);
    Message(Type _type, std::string _text);
    ~Message();

    /* Data */
    Type type;
    std::string text;
    int subtype;
};

#endif