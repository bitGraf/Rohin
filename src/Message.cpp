#include "Message.hpp"

Message::Message() {
    type = Type::empty;
    text = "";
}

Message::Message(Type _type) {
    type = _type;
    text = "";
}

Message::Message(Type _type, std::string _text) {
    type = _type;
    text = _text;
}

Message::~Message() {

}