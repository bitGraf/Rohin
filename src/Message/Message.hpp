#ifndef __MESSAGE__
#define __MESSAGE__

#include <string>
#include "DataTypes.hpp"

class Message {
public:
    typedef u32 Type;

    Message();
    Message(Type _type);
    Message(Type _type, std::string _text);

    /* Data */
    Type type;
    std::string text;
    u8 data[8];
};

#endif