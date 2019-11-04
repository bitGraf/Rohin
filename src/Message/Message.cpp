#include "Message.hpp"

Message::Message() : 
    type(0), 
    data{0} 
{}

Message::Message(Type _type) : 
    type(_type), 
    data{0} 
{}

Message::Message(Type _type, std::string _text) :
    type(_type),
    text(_text), 
    data{0}
{}